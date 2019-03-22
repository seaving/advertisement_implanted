#include "includes.h"

#define ACCEPT_MARK				1000

#define QC_DOG_MPROXY_CHAIN		"qc_mproxy"


void fw_redirect_mproxy()
{
	char cmd[255];

	char lan_ip[33] = {0};
	get_dev_ip(lan_ip, GET_LAN_NAME);

	//QC_DOG_MPROXY_CHAIN
	EXECUTE_CMD(cmd, "iptables -t nat -A %s -w -m mark --mark %d -p tcp --dport 80 -j DNAT --to %s:%d", QC_DOG_MPROXY_CHAIN, ACCEPT_MARK, lan_ip, MITM_PORT);
	EXECUTE_CMD(cmd, "iptables -t nat -A %s -w -m mark --mark %d -p tcp --dport 8080 -j DNAT --to %s:%d", QC_DOG_MPROXY_CHAIN, ACCEPT_MARK, lan_ip, MITM_PORT);
}

//清除重定向mproxy
void fw_reflush_mproxy()
{
	char cmd[255];
	EXECUTE_CMD(cmd, "iptables -t nat -F %s -w", QC_DOG_MPROXY_CHAIN);	
}

