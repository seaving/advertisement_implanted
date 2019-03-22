#ifndef __SOCKET_H__
#define __SOCKET_H__

#define	MAXLINE	 8192  /* max text line length */
#define MAXBUF   8192  /* max I/O buffer size */

int read_header(int fd, char * buffer, int buffer_len, int *read_end);
int readline(int fd, void *usrbuf, int maxlen);
int open_clientfd_noblock(char *bindip, char *hostname, int port);
int socket_read(int sockfd, char *buf, int len);
int socket_send(int sockfd, char *buf, int len);

int get_local_mac(char *mac, char *dev);
int get_addr_info(char *hostname, char *ipaddr);

int get_dev_ip(char *ip, char *dev);

#endif
