#ifndef __SOCKET_H__
#define __SOCKET_H__

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

int socket_setfd_noblock(int sockfd);
int socket_setfd_block(int sockfd);
int socket_read(int sock, char *buf, int readlen, unsigned char time_out);
int socket_send(int sock, char *buf, int len, unsigned char time_out);
int socket_close(int sockfd);

int get_addr_info(char *hostname, char *ipaddr);
int get_dev_ip(char *ip, char *dev);

#endif

