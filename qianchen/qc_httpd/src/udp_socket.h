#ifndef __UDP_SOCKET_H__
#define __UDP_SOCKET_H__

#include "common.h"

#define UDP_BROADCAST	"0.0.0.0"

int open_udp_client(char *ser_ip, int port, struct sockaddr_in *server);
int open_udp_client_s(char *ser_ip, int ser_port, struct sockaddr_in *server, int my_port);

int open_udp_server(int port);

int udp_recv(int fd, char *buf, int buf_size, struct sockaddr_in *from);

int udp_send(int fd, char *buf, int data_len, struct sockaddr_in *to);

int udp_select_recv(int fd, char *buf, int buf_size, struct sockaddr_in *from, int timeout_sec);

int udp_send_to(int fd, char *buf, int data_len, char *to_ip, int to_port);

#endif


