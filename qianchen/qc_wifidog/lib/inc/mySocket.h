#ifndef __MY_SOCKET_H__
#define __MY_SOCKET_H__

#define LAN_DEV	"ap0"
#define WAN_DEV	"ccmni0"

#define localhost	"127.0.0.1"

int socket_setfd_noblock(int sockfd);
int socket_setfd_block(int sockfd);
int socket_listen(char *bindip, int listen_port, int listen_counts);
int socket_setarg(int socket);
int socket_read(int sock, char *buf, int readlen, unsigned char time_out);
int socket_send(int sock, char *buf, int len, unsigned char time_out);
int socket_close_client(int sockfd);
int socket_close(int sockfd);

int get_addr_info(char *hostname, char *ipaddr);
int get_dev_ip(char *ip, char *dev);
int get_peer_mac(int sockfd, char *mac);
int get_local_mac(char *mac,  char *dev);

#endif