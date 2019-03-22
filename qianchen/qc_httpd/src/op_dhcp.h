#ifndef __OP_DHCP_H__
#define __OP_DHCP_H__

int get_dhcp_switch(char *dhcp_dev);

int get_dhcp_leasetime(char *dhcp_dev);

int get_dhcp_ippool_start(char *dhcp_dev);

int get_dhcp_ippool_limit(char *dhcp_dev);

int set_dhcp_ippool(char *dhcp_dev, char *start, char *limit);
int set_dhcp_leasetime(char *dhcp_dev, char *leasetime);
int set_dhcp_disable(char *dhcp_dev);
int set_dhcp_enable(char *dhcp_dev);
int set_dhcp_start(char *dhcp_dev, char *start);
int set_dhcp_limit(char *dhcp_dev, char *limit);

int get_dhcp_assigned_count(char *dhcp_dev);

#endif


