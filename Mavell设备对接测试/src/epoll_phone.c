#include "includes.h"

#define EPOLL_PHONE_THREAD_NUM	1
static int g_phone_epollfd[EPOLL_PHONE_THREAD_NUM];
static int g_phone_index = 0;

int epoll_add_to_phone(int phonefd)
{
	phoneinfo_t *phone = init_phoneinfo();
	if (! phone)
		return -1;

	socket_setfd_noblock(phonefd);

	phone->fd = phonefd;
	phone->epoll_fd = g_phone_epollfd[g_phone_index];
	
	struct epoll_event ev;
	ev.data.ptr = phone;
	ev.events = EPOLLIN | EPOLLET | EPOLLONESHOT;
	LOG_WARN_INFO("add phone to epollfd: %d\n", g_phone_epollfd[g_phone_index]);
	int ret = epoll_ctl(g_phone_epollfd[g_phone_index], EPOLL_CTL_ADD, phonefd, &ev);
	if (ret < 0)
	{
		LOG_PERROR_INFO("epoll_ctl EPOLL_CTL_ADD error.");
		return -1;
	}
	g_phone_index ++;
	if (g_phone_index >= EPOLL_PHONE_THREAD_NUM)
	{
		g_phone_index = 0;
	}

#if __SOCKET_MONITOR__
	add_socket_monitor(phone);
#endif

	return 0;
}

int epoll_mod_to_phone(phoneinfo_t *phone)
{
	if (! phone)
		return -1;
		
	struct epoll_event ev;
	ev.data.ptr = phone;
	ev.events = EPOLLIN | EPOLLET | EPOLLONESHOT;
	socket_setfd_noblock(phone->fd);
	int ret = epoll_ctl(phone->epoll_fd, EPOLL_CTL_MOD, phone->fd, &ev);
	if (ret < 0)
	{
		LOG_PERROR_INFO("epoll_ctl (fd:%d) EPOLL_CTL_MOD error.", phone->fd);
		return -1;
	}
	return 0;
}

int epoll_create_phone()
{
	int i;
	for (i = 0; i < EPOLL_PHONE_THREAD_NUM; i ++)
	{
		//g_phone_epollfd[i] = epoll_create1(0);	------->>> 中兴微方案的 内核较低，不支持该函数
		g_phone_epollfd[i] = epoll_create(MAX_EVENTS);
		if (g_phone_epollfd[i] < 0)
		{
			LOG_PERROR_INFO("epoll_create1 error.");
			return -1;
		}
	}

	return 0;
}

int epoll_wait_phone(int phone_epollfd)
{
	struct epoll_event events[MAX_EVENTS];
	int fds;
	int i;
	
	while (1)
	{
		//通常需要超时处理
		fds = epoll_wait(phone_epollfd, events, MAX_EVENTS, -1);
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
		   		//PRINTF("[PHONE] fds: %d, ---> process begin.\n", fds);
		   		create_phone_data_process_thread(events[i]);
		   		//phone_data_process(events[i]);
			}
		}
	}

	return 0;
}

void *phone_thread(void *arg)
{
	pthread_detach(pthread_self());
	
	if (! arg)
		return (void *)0;

	int phone_epollfd = *((int *)arg);
	free(arg);

	epoll_wait_phone(phone_epollfd);

	return (void *)0;
}

int phone_thread_start()
{
	int ret = epoll_create_phone();
	if (ret < 0)
	{
		LOG_ERROR_INFO("can't create phone epoll thread!\n");
		return -1;
	}

	int i;
	for (i = 0; i < EPOLL_PHONE_THREAD_NUM; i ++)
	{
		pthread_t thread;
		int *arg = calloc(1, sizeof(int));
		if (! arg)
		{
			LOG_PERROR_INFO("calloc error.");
			return -1;
		}
		*arg = g_phone_epollfd[i];
		if (pthread_create(&thread, NULL, phone_thread, arg) != 0)
		{
			LOG_PERROR_INFO("pthread_create error.")
			free(arg);
			return -1;
		}
	}
	
	return 0;
}

