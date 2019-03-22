#include "server.h"

void safe_close(int fd)
{
	int ret = -1;
	
	if (fd > 0)
	{
		ret = close(fd);
		while (ret != 0)
		{
			if (errno != EINTR || errno == EBADF)
				break;
			ret = close(fd);
		}
	}
}

static int open_server_socket(const char *ip, short port, int backlog)
{
	int fd = -1;
	fd = socket(AF_INET, SOCK_STREAM, 0);
	if (fd < 0)
	{
		LOG_PERROR_INFO("socket create error !\n");
		return -1;
	}

	unsigned long non_blocking = 1;
	if (ioctl(fd, FIONBIO, &non_blocking) != 0)
	{
		LOG_PERROR_INFO("cannot set nonblocking !\n");
		safe_close(fd);
		return -1;
	}
	
	int flag_reuseaddr = 1;
	setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, (char *) &flag_reuseaddr, sizeof(flag_reuseaddr));

	struct sockaddr_in addr;
	socklen_t addrlen = sizeof(addr);
	memset(&addr, 0, addrlen);
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = (ip == NULL ? INADDR_ANY : inet_addr(ip));
	addr.sin_port = htons(port);

	if (bind(fd, (const struct sockaddr *) &addr, addrlen) != 0)
	{
		LOG_PERROR_INFO("cannot bind, port: %d !\n", port);
		safe_close(fd);
		return -1;
	}

	if (listen(fd, backlog) != 0)
	{
		LOG_PERROR_INFO("cannot listen, port: %d !\n", port);
		safe_close(fd);
		return -1;
	}

	return fd;
}

static int accept_client(int fd, struct sockaddr_in *s_in)
{
	int cfd = -1;
	
	for ( ; ; )
	{
		socklen_t len = sizeof(struct sockaddr_in);
		cfd = accept(fd, (struct sockaddr *) s_in, &len);
		if (cfd < 0)
		{
			if (errno == EINTR)
				continue;
			LOG_PERROR_INFO("cannot accpet !\n");
			break;
		}
		
		unsigned long non_blocking = 1;
		if (ioctl(cfd, FIONBIO, &non_blocking) != 0)
		{
			LOG_PERROR_INFO("cannot set nonblocking, client_fd: %d !\n", cfd);
			safe_close(cfd);
			cfd = -1;
		}
		
		return cfd;
	}
	
	return -1;
}

static void accept_action(int fd, short event, void *arg)
{
	SERVER *s = (SERVER*) arg;
	struct sockaddr_in s_in;
	
	int cfd = accept_client(fd, &s_in);
	if (cfd == -1)
	{
		LOG_ERROR_INFO("client accept error !\n");
		return ;
	}

	int retry = 0;
	for (retry = 0 ; retry < (s->workers->size / 2); retry ++)
	{
		WORKER* w = &s->workers->array[(s->conn_count++) % s->workers->size];
		CONN *citem;
		
		//johan: conns is a one-way queue; its out-way is owned by the server and its in-way is owned by one worker.
		GET_FREE_CONN(w->conns, citem);
		if (citem == NULL)
			continue;
		
		citem->fd = cfd;
		citem->cip = *(uint32 *) &s_in.sin_addr;
		citem->cport = (uint16) s_in.sin_port;
		
		//notify worker to enable cfd's read and write
		//处理客户端
		
		return;
	
	}

	LOG_ERROR_INFO("workers are too busy !\n");
	safe_close(cfd);
}

static void receive_notify_action(int fd, short event, void *arg)
{
	WORKER *w = (WORKER*) arg;
	uint32 ind;
	if (read(fd, &ind, 4) != 4)
	{
		slog_err_t_w(w->qlog, "notify_pipe read error, errno: %d %m", errno);
		return;
	}
	
	bufferevent_setfd(w->conns->list[ind].bufev, w->conns->list[ind].fd);
	bufferevent_enable(w->conns->list[ind].bufev, EV_READ | EV_WRITE);
}

static void *start_worker(void *arg) {
	WORKER *w = (WORKER*) arg;
	SERVER *s = w->server;
	pthread_mutex_lock(&s->start_lock);
	++s->start_worker_num;
	pthread_cond_signal(&s->start_cond);
	pthread_mutex_unlock(&s->start_lock);
	event_base_loop(w->base, 0);
	LOG_ERROR_INFO("start_worker error, thread_id: %lu\n", w->thread_id);
	return NULL;
}

SERVER* init_server(int port, uint16 workernum, uint32 connnum, int read_timeout, int write_timeout)
{
	LOG_QUEUE *lq = create_log_queue();
	if (lq == NULL)
		return NULL;

	SERVER *s = (SERVER*) calloc(sizeof(SERVER), 1);
	if (s == NULL) {
		fprintf(stderr, "init_server malloc error, errno: %d %m\n", errno);
		return NULL;
	}

	s->qlog = lq;
	s->port = port;
	s->base = event_init();
	if (s->base == NULL) {
		fprintf(stderr, "init_server event base error, errno: %m\n");
		return NULL;
	}
	s->workers = init_workers(s, workernum, connnum, read_timeout, write_timeout);
	if (s->workers == NULL)
		return NULL;

	pthread_mutex_init(&s->start_lock, NULL);
	pthread_cond_init(&s->start_cond, NULL);

	return s;
}

WORKER_ARRAY* init_workers(SERVER *server, uint16 workernum, uint32 connnum, int read_timeout,
		int write_timeout) {
	WORKER_ARRAY *workers = NULL;
	uint32 len = sizeof(WORKER_ARRAY) + sizeof(WORKER) * workernum;
	workers = (WORKER_ARRAY*) malloc(len);
	if (workers == NULL) {
		fprintf(stderr, "init_workers malloc error, errno: %d %m\n", errno);
		return NULL;
	}
	memset(workers, 0, len);
	workers->size = workernum;
	workers->server = server;
	for (int i = 0; i < workernum; i++) {
		int fds[2];
		if (pipe(fds)) {
			fprintf(stderr, "init_workers pipe error, errno: %d %m\n", errno);
			return NULL;
		}
		WORKER *w = &workers->array[i];
		LOG_QUEUE *lq = create_log_queue();
		if (lq == NULL) {
			fprintf(stderr, "init_workers qlog error, errno: %d %m\n", errno);
			return NULL;
		}
		w->qlog = lq;
		w->ind = i;
		w->notified_rfd = fds[0];
		w->notifed_wfd = fds[1];

		w->base = event_init();
		if (w->base == NULL) {
			fprintf(stderr, "init_workers event base error, errno: %d %m\n", errno);
			return NULL;
		}

		event_set(&w->notify_event, w->notified_rfd, EV_READ | EV_PERSIST, receive_notify_action, w);
		event_base_set(w->base, &w->notify_event);
		if (event_add(&w->notify_event, 0) == -1) {
			fprintf(stderr, "init_workers add event error, errno: %d %m\n", errno);
			return NULL;
		}

		CONN_LIST *lst = init_conn_list(connnum);
		if (lst == NULL) {
			fprintf(stderr, "init_workers conn_list error, errno: %d %m\n", errno);
			event_base_free(w->base);
			free(workers);
			return NULL;
		}
		w->conns = lst;
		CONN *p = lst->head;
		while (p != NULL) {
			p->bufev = bufferevent_new(-1, on_conn_read, NULL, on_conn_err, p);
			bufferevent_base_set(w->base, p->bufev);
			bufferevent_settimeout(p->bufev, read_timeout, write_timeout);
			p->owner = w;
			p = p->next;
		}

		w->server = server;
	}
	return workers;
}

void* start_server(void *arg) {
	SERVER* server = (SERVER*) arg;
	server->ret = -1;
	if (start_workers(server->workers) == -1) {
		fprintf(stderr, "start workers error, errno: %m\n");
		return (void*) server->ret;
	}
	server->server_fd = open_server_socket(NULL, server->port, 1024);
	if (server->server_fd < 0) {
		fprintf(stderr, "open server socket error, errno: %d %m\n", errno);
		return (void*) server->ret;
	}
	event_set(&server->listen_event, server->server_fd, EV_READ | EV_PERSIST, accept_action, server);
	event_base_set(server->base, &server->listen_event);
	if (event_add(&server->listen_event, 0) == -1) {
		fprintf(stderr, "start server add listen event error, errno: %d %m\n", errno);
		return (void*) server->ret;
	}
	for (int i = 0; i < server->workers->size; i++)
		server->workers->array[i].sfd = server->server_fd;

	fprintf(stdout, "--------------start server ok--------------\n");
	fflush(stdout);
	event_base_loop(server->base, 0);
	fprintf(stderr, "start server loop error, errno: %d %m\n", errno);

	if(g_stx == 0)
		g_stx = DEFAULT_STX;
	server->ret = 0;
	return (void*) server->ret;
}

int start_workers(WORKER_ARRAY* workers) {
	for (int i = 0; i < workers->size; i++) {
		WORKER *w = &workers->array[i];
		if (pthread_create(&w->thread_id, NULL, start_worker, w) != 0) {
			fprintf(stderr, "start_workers create thread error, errno: %d %m\n", errno);
			return -1;
		}
	}
	SERVER *s = workers->server;
	while (s->start_worker_num < workers->size) {
		pthread_mutex_lock(&s->start_lock);
		pthread_cond_wait(&s->start_cond, &s->start_lock);
		pthread_mutex_unlock(&s->start_lock);
	}
	return 0;
}


