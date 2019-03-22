#ifndef __USR_SOCKET_H__
#define __USR_SOCKET_H__

#include "common.h"
#include "mySocket.h"

typedef struct sockaddr SA;
typedef enum 
{
	E_CONNECT_BLOCK = 0,
	E_CONNECT_NOBLOCK
}e_connectmode;

int connect_noblock(int clientfd, struct sockaddr_in *serveraddr);
int open_clientfd(char *bind_ip, char *hostname, int port);
int open_clientfd_noblock(char *bindip, char *hostname, int port);
int connect_host_block(char *hostname, int port);
int connect_host_noblock(char *hostname, int port);
int connecthost(char *hostname, int port, e_connectmode mode);

ssize_t readline(int fd, void *usrbuf, size_t maxlen);

#endif

