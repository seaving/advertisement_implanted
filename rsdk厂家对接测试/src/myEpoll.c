#include "includes.h"

int epoll_init(int listenfd)
{
    int epollfd;
    int rv;
    struct epoll_event ev;
    
    //创建epoll句柄
	epollfd = epoll_create(MAX_EVENTS);
    if (epollfd <= 0)
    {
		LOG_PERROR_INFO("epoll_create error.");
		return -1;
    }

    ev.data.ptr = NULL;
	ev.events = EPOLLIN | EPOLLET;

	//将监听fd加入触发事件
	rv = epoll_ctl(epollfd, EPOLL_CTL_ADD, listenfd, &ev);
    if (rv < 0)
	{
		LOG_PERROR_INFO("epoll_ctrl err");
		return -1;
	}
    return epollfd;
}

int epoll_add_sockfd(int epollfd, int sockfd, int mode, void *ev_data_ptr)
{
	//设置非阻塞 及 TCP_CORK
	if (socket_setarg(sockfd) < 0 || epollfd <= 0)
	{
		return -1;
	}

    struct epoll_event ev;
    ev.data.ptr = ev_data_ptr;
	ev.events = mode | EPOLLIN | EPOLLET;
	int ret = epoll_ctl(epollfd, EPOLL_CTL_ADD, sockfd, &ev);
	if (ret < 0)
	{
		LOG_PERROR_INFO("epoll_ctl EPOLL_CTL_ADD error.");
	}
	return ret;
}

int epoll_mod_sockfd(int epollfd, int sockfd, int mode, void *ev_data_ptr)
{
    struct epoll_event ev;
    ev.data.ptr = ev_data_ptr;
	ev.events = mode | EPOLLIN | EPOLLET;
	int ret = epoll_ctl(epollfd, EPOLL_CTL_MOD, sockfd, &ev);
	if (ret < 0)
	{
		LOG_PERROR_INFO("epoll_ctl EPOLL_CTL_MOD error.");
	}
	return ret;
}

int epoll_del_sockfd(int epollfd, int sockfd)
{
	if (epollfd <= 0 || sockfd <= 0)
		return -1;
	struct epoll_event ev;
	int ret = epoll_ctl(epollfd, EPOLL_CTL_DEL, sockfd, &ev);
	return ret;
}


