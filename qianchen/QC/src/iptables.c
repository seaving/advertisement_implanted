#include "includes.h"

#if 0
int iptables_del_proxy()
{
	system("/etc/init.d/firewall restart");
	return 0;
}

int iptables_accept_root()
{
	char iptables_cmd[250] = {0};
	
	memset(iptables_cmd, 0, 250);
	sprintf(iptables_cmd, 
	"iptables -t nat -I PREROUTING -w -p tcp -i %s -d %s -j ACCEPT", get_netdev_lan_name(), get_netdev_lan_ip());

	LOG_HL_INFO("%s\n", iptables_cmd);
	
	system(iptables_cmd);
	return 0;
}

int iptables_redirect(int port, char *toip, int toport)
{
	char iptables_cmd[250] = {0};

	memset(iptables_cmd, 0, 250);
	sprintf(iptables_cmd, 
	"iptables -t nat -I PREROUTING -i %s -w -p tcp --dport %d -j DNAT --to %s:%d", 
	get_netdev_lan_name(), port, toip, toport);

	LOG_HL_INFO("%s\n", iptables_cmd);
	
	system(iptables_cmd);
	return 0;
}

int iptables_webview_proxy(char *proxy_ip, int proxy_port)
{
	iptables_redirect(80, proxy_ip, proxy_port);
	iptables_redirect(8080, proxy_ip, proxy_port);
	iptables_accept_root(); // 放行本身去往192.168.1.1的数据
	return 0;
}
#endif

