#ifndef __MY_SOCKET_H__
#define __MY_SOCKET_H__	1


typedef enum
{
	E_EAGIN = 0,
	E_ERROR,
	E_EINTR,
	E_NORMAL
} socketstate_t;



int socket_setfd_noblock(int sockfd);
int socket_setfd_block(int sockfd);
int socket_listen(char *bindip, int listen_port, int listen_counts);
int socket_setarg(int socket);
int socket_ET_accept(int listenfd, int epollfd);
int socket_read(int sock, char *buf, int readlen, socketstate_t *state);
int socket_send(int sock, char *buf, int len);
int socket_close_client(int sockfd);
int socket_close(int sockfd);
#endif
