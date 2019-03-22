#ifndef __IPTABLES_H__
#define __IPTABLES_H__

void bwlist_ip_drop(char *ip);

void bwlist_domain_drop(char *domain);

void bwlist_redirect(unsigned short port, char *toip, unsigned short toport);

void bwlist_refresh(char *table, char *chain);

void bwlist_ip_accept(char *ip);


#endif

