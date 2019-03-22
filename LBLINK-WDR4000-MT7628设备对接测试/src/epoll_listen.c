#include "includes.h"

int g_connect_count = 0;

int epoll_create_listen(int listenfd)
{
	int epollfd;

	socket_setfd_noblock(listenfd);

	//创建epoll句柄,并加入监听套接字
	epollfd = epoll_create1(0);
    if (epollfd <= 0)
    {
		LOG_PERROR_INFO("epoll_create error.");
		return -1;
    }

	struct epoll_event ev;
	ev.data.fd = listenfd;
	ev.events = EPOLLIN | EPOLLET;
	int ret = epoll_ctl(epollfd, EPOLL_CTL_ADD, listenfd, &ev);
	if (ret < 0)
	{
		LOG_PERROR_INFO("epoll_ctl EPOLL_CTL_ADD error.");
	}
	return epollfd;
}

int accept_phone(int listenfd, struct sockaddr *in_addr, socklen_t *in_len)
{
	int ret = -1;

	while (1)
	{
		int sock = accept(listenfd, in_addr, in_len);
		if (sock < 0)
		{
			if ((errno == EAGAIN) || (errno == EWOULDBLOCK))
			{
				//处理完所有监听套接字
				break;
			}
			else if (errno == EINTR)
			{
				continue;
			}
			else
			{
				LOG_PERROR_INFO("accept error.");
				return -1;
			}
		}
		else if (sock == 0)
		{
			LOG_PERROR_INFO("accept fd == 0.");
			continue;
		}

		if (g_connect_count >= MAX_CLIENT)
		{
			LOG_ERROR_INFO("listen counts is more than MAX_LISTEN, Drop the socket !\n");
			socket_close(sock);
			
			//如果超过了的话，重新启动程序，释放所有的clientsocket
			//为了保证能让大家上网，此刻做出牺牲
			_exit_(-1);
			
			//continue;
		}
		
		//平均分配clientepoll
		ret = epoll_add_to_phone(sock);
		if (ret < 0)
		{
			LOG_ERROR_INFO("add clientfd to phone_epoll error!\n");
			socket_close(sock);
			continue;
		}
		g_connect_count ++;
		
		LOG_HL_INFO("add clientfd to phone: %d, g_connect_count: %d\n", sock, g_connect_count);
	}

	return 0;
}

int epoll_wait_listen(int epollfd, int listenfd)
{
	struct epoll_event event = {0};

	struct sockaddr in_addr;
	socklen_t in_len;

	int ret = -1;
	
    in_len = sizeof(in_addr);

	for ( ; ; )
	{
		LOG_NORMAL_INFO("************* listen begin ************* \n");
		int is_work = epoll_wait(epollfd, &event, 1, -1);
		if (is_work > 0)
		{			
			if (process_getlock(g_sprolock, getpid()) < 0)
				continue;
			ret = accept_phone(listenfd, &in_addr, &in_len);
			if (ret < 0)
			{
				LOG_ERROR_INFO("accept phone error!\n");
				process_unlock(g_sprolock);
				return -1;
			}
			process_unlock(g_sprolock);
		}
	}

	return 0;
}

void *listen_thread(void *arg)
{
	pthread_detach(pthread_self());
	
	if (! arg)
		return (void *)0;
		
	int listenfd = *((int *)arg);
	free(arg);

	int ret = epoll_create_listen(listenfd);
	if (ret < 0)
		return (void *)0;
		
	epoll_wait_listen(ret, listenfd);

	return (void *)0;
}

int listen_thread_start(int listenfd)
{
	int *arg = calloc(1, sizeof(int));
	if (! arg)
	{
		LOG_PERROR_INFO("calloc error.");
		return -1;
	}
	*arg = listenfd;
	pthread_t Thlisten;
	if (pthread_create(&Thlisten, NULL, listen_thread, arg) != 0)
	{
		LOG_PERROR_INFO("phtread_create error.");
		free(arg);
		return -1;
	}
	
	return 0;
}

int destroy_listen_epoll()
{
	return 0;
}

int display_client_counts()
{
	int now_time_sec = 0;
	static int old_time_sec = -1;
	static int diff_cnt = -1;
	GetAllSystemTime(NULL, NULL, NULL, NULL, NULL, &now_time_sec, NULL);
	if (now_time_sec != old_time_sec)
	{
		diff_cnt ++;
		old_time_sec = now_time_sec;
	}
	else
	{
		return 0;
	}

	if (diff_cnt > 1)
	{
		diff_cnt = 0;

		PRINTF("\n");
		PRINTF("------------------------------------------\n");
		PRINTF("- there are [ %d ] client connection .\n", g_connect_count);
		PRINTF("------------------------------------------\n");
		PRINTF("\n");
	}
	
	return 0;
}


