#ifndef __MY_SOCKET_H__
#define __MY_SOCKET_H__


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
int socket_send(int sock, char *buf, int len, unsigned char time_out);
int socket_close_client(int sockfd);
int socket_close(int sockfd);

int get_peer_name(int own_socket, char *ip, int *port);
int get_socket_name(int own_socket, char *ip, int *port);
int get_peer_mac(int sockfd, char *mac);
int getpeermac_by_ip(char *ipaddr, char *mac);
int get_local_mac(char *mac,  char *dev);

int get_addr_info(char *hostname, char *ipaddr);
int get_peer_name_n(int own_socket, unsigned int *ip, int *port);
int get_dev_ip(char *ip, char *dev);
int get_peer_mac_no_semicolon(int sockfd, char *mac);

#endif
