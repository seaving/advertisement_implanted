#ifndef __IPTABLES_H__
#define __IPTABLES_H__	1


int iptables_del_proxy();
int iptables_webview_proxy(char *proxy_ip, int proxy_port);

#endif

