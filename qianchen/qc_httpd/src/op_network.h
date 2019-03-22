#ifndef __NETWORK_OP_H__
#define __NETWORK_OP_H__

int get_network_ipaddr(char *dev_name, char *ipaddr);
int get_network_mask(char *dev_name, char *mask);
int get_network_proto(char *dev_name, char *proto);
int get_wan_username(char *username);
int get_wan_password(char *password);
int get_network_mtu(char *dev_name);
int get_wan_gateway(char *gw);
int get_network_dns(char *dev_name, char *dns);

int get_wan_protocol(char * proto);

int set_network_ipaddr(char *dev_name, char *ipaddr);
int set_network_mask(char *dev_name, char *mask);
int set_network_proto(char *dev_name, char *proto);
int set_wan_username(char *username);
int set_wan_password(char *password);
int set_network_mtu(char *dev_name, char *mtu);
int set_network_gateway(char *dev_name, char *gateway);
int set_network_dns(char *dev_name, char *dns);

int del_network_section(char *dev_name, char *section);

int write_set_network_ipaddr_cmd_shell(char *dev_name, char *ipaddr);
int write_set_network_mask_cmd_shell(char *dev_name, char *mask);
int write_set_network_proto_cmd_shell(char *dev_name, char *proto);
int write_set_wan_username_cmd_shell(char *username);
int write_set_wan_password_cmd_shell(char *password);
int write_set_network_mtu_cmd_shell(char *dev_name, char *mtu);
int write_set_network_gateway_cmd_shell(char *dev_name, char *gateway);
int write_set_network_dns_cmd_shell(char *dev_name, char *dns);
int write_del_network_section_cmd_shell(char *dev_name, char *section);

#endif

