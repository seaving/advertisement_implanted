#ifndef __MY_SOCKET_H__
#define __MY_SOCKET_H__


int get_dev_ip(char *ip, char *dev);
int get_peer_mac(int sockfd, char *mac);
int get_local_mac(char *mac,  char *dev);
int get_dev_mask(char *mask, char *dev);
int get_gateway(char *gw);
int get_dns(char *dns, unsigned char num);

int get_peer_name(int own_socket, char *ip, int *port);
int get_socket_name(int own_socket, char *ip, int *port);
int get_ip(char *ip, char *dev);
int get_peer_name_n(int own_socket, unsigned int *ip, int *port);


#endif
