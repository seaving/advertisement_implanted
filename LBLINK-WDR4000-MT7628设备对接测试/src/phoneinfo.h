#ifndef __PHONE_INFO_H__
#define __PHONE_INFO_H__

#include "webserverinfo.h"
#include "socket_monitor.h"

typedef struct webserverinfo webserverinfo_t;

typedef enum
{
	E_READ_METHOD = 0,
	E_READ_REQUEST,
	E_READ_POST_CONTEXT,
	E_PHONE_FINISHED
} phone_option_t;

typedef enum
{
	E_GET = 0,
	E_POST,
	E_HEAD,
	E_CONNECT,
	E_OPTIONS
} method_t;

typedef struct phoneinfo
{
	int fd;
	data_t *swap_buf;
	request_t *request;
	phone_option_t opt;
	method_t method;
	int epoll_fd;
	int tmp;
	int serverfd;
	int epoll_serverfd;
	webserverinfo_t *webserver;
	int time;
	int flag;
	int processflag;
#if __SOCKET_MONITOR__
	SocketMonitor_t *SM;
	int monitorflag;	
#endif
} phoneinfo_t;

phoneinfo_t * init_phoneinfo();
int free_phoneinfo(phoneinfo_t *phone);
int httpmethod_cmp(phoneinfo_t *phone);
int free_phoneinfo_member(phoneinfo_t *phone);

#endif

