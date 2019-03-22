#include "includes.h"

#define EPOLL_SERVER_THREAD_NUM		1
static int g_webserver_index = 0;
static int g_webserver_epollfd[EPOLL_SERVER_THREAD_NUM];
static pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;
int epoll_add_to_webserver(int webserverfd, phoneinfo_t *phone, int isAccept)
{
	webserverinfo_t *webserver = init_webserver();
	if (! webserver || ! phone)
		return -1;

	socket_setfd_noblock(webserverfd);

	webserver->serverfd = webserverfd;
	webserver->phonefd = phone->fd;
	webserver->mitm = isAccept ? E_ACCEPT : E_ATTACK;
	webserver->phone_epollfd = phone->epoll_fd;
	webserver->phone = phone;
	phone->webserver = webserver;
	struct epoll_event ev;
	ev.data.ptr = webserver;
	ev.events = EPOLLIN | EPOLLET | EPOLLONESHOT;
	
	pthread_mutex_lock(&lock);
	webserver->server_epollfd = g_webserver_epollfd[g_webserver_index];
	phone->epoll_serverfd = webserver->server_epollfd;
	LOG_WARN_INFO("add server to epollfd: %d\n", g_webserver_epollfd[g_webserver_index]);
	webserver->server_epollfd = g_webserver_epollfd[g_webserver_index];
	int ret = epoll_ctl(g_webserver_epollfd[g_webserver_index], EPOLL_CTL_ADD, webserverfd, &ev);
	if (ret < 0)
	{
		LOG_PERROR_INFO("epoll_ctl EPOLL_CTL_ADD error.");
		pthread_mutex_unlock(&lock);
		return -1;
	}
	g_webserver_index ++;
	if (g_webserver_index >= EPOLL_SERVER_THREAD_NUM)
	{
		g_webserver_index = 0;
	}

	pthread_mutex_unlock(&lock);
	
	return 0;
}

int epoll_mod_to_webserver(webserverinfo_t *webserver)
{
	if (! webserver)
		return -1;

	struct epoll_event ev;
	ev.data.ptr = webserver;
	ev.events = EPOLLIN | EPOLLET | EPOLLONESHOT;

	socket_setfd_noblock(webserver->serverfd);
	int ret = epoll_ctl(webserver->server_epollfd, EPOLL_CTL_MOD, webserver->serverfd, &ev);
	if (ret < 0)
	{
		LOG_PERROR_INFO("epoll_ctl (fd:%d) EPOLL_CTL_MOD error.", webserver->serverfd);
		return -1;
	}	

	return 0;
}

int epoll_create_webserver()
{
	int i;
	for (i = 0; i < EPOLL_SERVER_THREAD_NUM; i ++)
	{
		g_webserver_epollfd[i] = epoll_create1(0);
		if (g_webserver_epollfd[i] < 0)
		{
			LOG_PERROR_INFO("epoll_create1 error.");
			return -1;
		}
	}

	return 0;
}

int epoll_wait_webserver(int webserver_epollfd)
{
	struct epoll_event events[MAX_EVENTS];
	int fds;
	int i;
	
	while (1)
	{
		//通常需要超时处理
		fds = epoll_wait(webserver_epollfd, events, MAX_EVENTS, -1);
		if (fds < 0)
		{
			//高级别中断
			if (errno == EINTR)
				continue;
			LOG_PERROR_INFO("epoll_wait error");
			break;
		}
		for (i = 0; i < fds; i ++)
		{
		   	//处理event
		   	if (! events[i].data.ptr)
		   		continue;

		   	//不做EPOLLIN判断，不管是什么事件都走进去
		   	//这样通过里面的read或者send来判断是否socket错误
		   	//从而释放资源
		   	//if (events[i].events & EPOLLIN)
		   	{
		   		//PRINTF("[WEBSERVER] fds: %d, ---> process begin.\n", fds);
		   		create_webserver_data_process_thread(events[i]);
				//webserver_data_process(events[i]);
			}			
		}
		//usleep(1000);
	}

	return 0;
}

void *webserver_thread(void *arg)
{
	pthread_detach(pthread_self());
	
	if (! arg)
		return (void *)0;

	int webserver_epollfd = *((int *)arg);
	free(arg);

	epoll_wait_webserver(webserver_epollfd);

	return (void *)0;
}

int webserver_thread_start()
{
	int ret = epoll_create_webserver();
	if (ret < 0)
	{
		LOG_ERROR_INFO("can't create webserver epoll thread!\n");
		return -1;
	}

	int i;
	for (i = 0; i < EPOLL_SERVER_THREAD_NUM; i ++)
	{
		pthread_t thread;
		int *arg = calloc(1, sizeof(int));
		if (! arg)
		{
			LOG_PERROR_INFO("calloc error.");
			return -1;
		}
		*arg = g_webserver_epollfd[i];
		if (pthread_create(&thread, NULL, webserver_thread, arg) != 0)
		{
			LOG_PERROR_INFO("pthread_create error.")
			free(arg);
			return -1;
		}
	}
	
	return 0;
}

