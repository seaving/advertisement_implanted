#ifndef __USR_SOCKET_H__
#define __USR_SOCKET_H__	1

#include "mySocket.h"

typedef struct sockaddr SA;
typedef enum 
{
	E_CONNECT_BLOCK = 0,
	E_CONNECT_NOBLOCK
}e_connectmode;
#if 1
#define SEND_DATA(socketfd, buf, len) { \
	if (socket_send(socketfd, buf, len) != len) \
	{ \
		LOG_ERROR_INFO("send data error!!!\n"); \
		return -1; \
	} \
}
#define SEND_DATA_GOTO(socketfd, buf, len, ret) { \
	if (socket_send(socketfd, buf, len) != len) \
	{ \
		LOG_ERROR_INFO("send data error!!!\n"); \
		ret = -1; \
		goto _return_; \
	} \
}
#else
#define SEND_DATA(socketfd, buf, len)	send(socketfd, buf, len, 0)
#endif
#define READ_DATA(socketfd, buf, readlen, ret, state) { \
	ret = socket_read(socketfd, buf, readlen, &state); \
	if (ret <= 0) \
	{ \
		if (state == E_EAGIN) \
		{ \
			return 0; \
		} \
		if (state == E_ERROR) \
		{ \
			LOG_ERROR_INFO("socket read error! ret = %d\n", ret); \
			return -1; \
		} \
	} \
}
#define READ_DATA_GOTO(socketfd, buf, readlen, ret, state) { \
	ret = socket_read(socketfd, buf, readlen, &state); \
	if (ret <= 0) \
	{ \
		if (state == E_EAGIN) \
			return 0; \
		if (state == E_ERROR) \
		{ \
			LOG_ERROR_INFO("socket read error! ret = %d\n", ret); \
			goto _return_; \
		} \
	} \
}

int connect_noblock(int clientfd, struct sockaddr_in *serveraddr);
int open_clientfd(char *bind_ip, char *hostname, int port);
int open_clientfd_noblock(char *bindip, char *hostname, int port);
int connect_host_block(char *hostname, int port);
int connect_host_noblock(char *hostname, int port);
int connecthost(char *hostname, int port, e_connectmode mode);

ssize_t readline(int fd, void *usrbuf, size_t maxlen, socketstate_t *state);

#endif

