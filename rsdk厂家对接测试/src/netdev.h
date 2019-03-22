#ifndef __NETDEV_H__
#define __NETDEV_H__	1

int init_netdev();
char * get_netdev_wan_segment();
char * get_netdev_wan_ip();
char * get_netdev_wan_mac();
char * get_netdev_wan_name();
char * get_netdev_lan_segment();
char * get_netdev_lan_ip();
char * get_netdev_lan_mac();
char * get_netdev_lan_name();
char * get_router_gw();

int get_peer_name(int own_socket, char *ip, int *port);
int get_socket_name(int own_socket, char *ip, int *port);
int get_peer_mac(int sockfd, char *mac);
int getpeermac_by_ip(char *ipaddr, char *mac);
int get_local_mac(char *mac,  char *dev);

int get_ip(char *ip, char *dev);
int get_addr_info(char *hostname, char *ipaddr);

#endif

