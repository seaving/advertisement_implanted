#include "includes.h"

#define ACCEPT_MARK				1000

#define QC_DOG_AUTH_MARK_CHAIN	"qc_auth_mark"
#define QC_DOG_REDIRECT_CHAIN	"qc_redirect"
#define QC_DOG_DISABLE_CHAIN	"qc_disable_network"
#define QC_DOG_MPROXY_CHAIN		"qc_mproxy"

int ip_to_int(char *ip, unsigned int *nip)
{
	char strIp[32] = {0};
	char *pIp = strIp;
	memcpy(pIp, ip, strlen(ip));
	LOG_WARN_INFO("%s  %s\n", ip, pIp);
	
	unsigned char __ip0 = 0;
	unsigned char __ip1 = 0;
	unsigned char __ip2 = 0;
	unsigned char __ip3 = 0;
	char *p = strchr(pIp, '.');
	if (p)
	{
		*p = 0;
		__ip0 = (unsigned char)atoi(pIp);
		LOG_WARN_INFO("%d\n", __ip0);
		pIp = p + 1;
		p = strchr(pIp, '.');
		if (p)
		{
			*p = 0;
			__ip1 = (unsigned char)atoi(pIp);
			LOG_WARN_INFO("%d\n", __ip1);
			pIp = p + 1;
			p = strchr(pIp, '.');
			if (p)
			{
				*p = 0;
				__ip2 = (unsigned char)atoi(pIp);
				LOG_WARN_INFO("%d\n", __ip2);
				pIp = p + 1;
				__ip3 = (unsigned char)atoi(pIp);
				LOG_WARN_INFO("%d\n", __ip3);

				*nip = (unsigned int)((unsigned int)__ip0 << 24) 
						| (unsigned int)((unsigned int)__ip1 << 16) 
						| (unsigned int)((unsigned int)__ip2 << 8) 
						| (unsigned int)((unsigned int)__ip3 << 0);
				
				return 0;
			}
		}
	}

	return -1;
}

void get_segment(char *ipseg, char *dev)
{
	char ip[32] = {0};	
	char mask[32] = {0};
	
	get_dev_ip(ip, dev);
	get_dev_mask(mask, dev);
	
	unsigned int nip = 0;
	if (ip_to_int(ip, &nip) < 0)
	{
		LOG_ERROR_INFO("ip_to_int ERROR!\n");
		return ;
	}
	unsigned int nmask = 0;
	if (ip_to_int(mask, &nmask) < 0)
	{
		LOG_ERROR_INFO("ip_to_int ERROR!\n");
		return ;
	}

	int seg = nip & nmask;
	sprintf(ipseg, "%d.%d.%d.%d", 
		(unsigned char)(seg >> 24 & 0xff), 
		(unsigned char)(seg >> 16 & 0xff), 
		(unsigned char)(seg >> 8 & 0xff), 
		(unsigned char)(seg >> 0 & 0xff));
	
	LOG_NORMAL_INFO("segment: %s\n", ipseg);
}

void get_segment2(char *ipseg, char *dev)
{	
	char ip[32] = {0};
	char mask[32] = {0};
	
	get_dev_ip(ip, dev);
	get_dev_mask(mask, dev);
	
	unsigned int nip = 0;
	if (ip_to_int(ip, &nip) < 0)
	{
		LOG_ERROR_INFO("ip_to_int ERROR!\n");
		return ;
	}
	
	unsigned int nmask = 0;
	if (ip_to_int(mask, &nmask) < 0)
	{
		LOG_ERROR_INFO("ip_to_int ERROR!\n");
		return ;
	}

	int i;
	for (i = 0; i < 32; i ++)
	{
		if ((nmask << i) & 0x80000000)
			continue;
		break;
	}
	
	int seg = nip & nmask;
	sprintf(ipseg, "%d.%d.%d.%d/%d", 
		(unsigned char)(seg >> 24 & 0xff), 
		(unsigned char)(seg >> 16 & 0xff), 
		(unsigned char)(seg >> 8 & 0xff), 
		(unsigned char)(seg >> 0 & 0xff), i);
	LOG_NORMAL_INFO("segment2: %s\n", ipseg);
}

//重定向到mproxy
void fw_redirect_mproxy()
{
	char cmd[255];
	char ip[32] = {0};
	get_dev_ip(ip, GET_LAN_NAME);
	
	//QC_DOG_MPROXY_CHAIN
	EXECUTE_CMD(cmd, "iptables -t nat -A %s -w -m mark --mark %d -p tcp --dport 80 -j DNAT --to %s:%d", QC_DOG_MPROXY_CHAIN, ACCEPT_MARK, ip, MPROXY_ARM_PORT);
	EXECUTE_CMD(cmd, "iptables -t nat -A %s -w -m mark --mark %d -p tcp --dport 8080 -j DNAT --to %s:%d", QC_DOG_MPROXY_CHAIN, ACCEPT_MARK, ip, MPROXY_ARM_PORT);
}

//清除重定向mproxy
void fw_reflush_mproxy()
{
	char cmd[255];
	EXECUTE_CMD(cmd, "iptables -t nat -F %s -w", QC_DOG_MPROXY_CHAIN);	
}

//完成静态退则的初始化
void fw_init()
{
	char ipseg[32] = {0};
	char ip[32] = {0};
	char cmd[255];

	//删除
	EXECUTE_CMD(cmd, "iptables -t mangle -F PREROUTING -w");
	EXECUTE_CMD(cmd, "iptables -t mangle -F FORWARD -w");
	EXECUTE_CMD(cmd, "iptables -t nat -F PREROUTING -w");
	EXECUTE_CMD(cmd, "iptables -t nat -F %s -w", QC_DOG_REDIRECT_CHAIN);

	EXECUTE_CMD(cmd, "/etc/init.d/firewall restart");

	//创建新链
	EXECUTE_CMD(cmd, "iptables -t mangle -X %s -w", QC_DOG_AUTH_MARK_CHAIN);
	EXECUTE_CMD(cmd, "iptables -t mangle -N %s -w", QC_DOG_AUTH_MARK_CHAIN);
	EXECUTE_CMD(cmd, "iptables -t mangle -X %s -w", QC_DOG_DISABLE_CHAIN);
	EXECUTE_CMD(cmd, "iptables -t mangle -N %s -w", QC_DOG_DISABLE_CHAIN);
	EXECUTE_CMD(cmd, "iptables -t nat -X %s -w", QC_DOG_REDIRECT_CHAIN);
	EXECUTE_CMD(cmd, "iptables -t nat -N %s -w", QC_DOG_REDIRECT_CHAIN);
	EXECUTE_CMD(cmd, "iptables -t nat -X %s -w", QC_DOG_MPROXY_CHAIN);
	EXECUTE_CMD(cmd, "iptables -t nat -N %s -w", QC_DOG_MPROXY_CHAIN);

	//加入到表中
	get_dev_ip(ip, "br-lan");
	get_segment2(ipseg, "br-lan");
	EXECUTE_CMD(cmd, "iptables -t mangle -I PREROUTING -w -s %s ! -d %s -j %s", ipseg, ip, QC_DOG_AUTH_MARK_CHAIN);
	EXECUTE_CMD(cmd, "iptables -t mangle -I FORWARD -w -s %s ! -d %s -j %s", ipseg, ip, QC_DOG_DISABLE_CHAIN);
	EXECUTE_CMD(cmd, "iptables -t nat -I PREROUTING -w -s %s ! -d %s -j %s", ipseg, ip, QC_DOG_REDIRECT_CHAIN);

	//在新建的链中创建静态规则
	EXECUTE_CMD(cmd, "iptables -t mangle -F %s -w", QC_DOG_AUTH_MARK_CHAIN);
	EXECUTE_CMD(cmd, "iptables -t mangle -F %s -w", QC_DOG_DISABLE_CHAIN);
	EXECUTE_CMD(cmd, "iptables -t nat -F %s -w", QC_DOG_REDIRECT_CHAIN);

	//EXECUTE_CMD(cmd, "iptables -t mangle -A %s -w -m mark --mark %d -j ACCEPT", QC_DOG_DISABLE_CHAIN, ACCEPT_MARK);
	//EXECUTE_CMD(cmd, "iptables -t mangle -A %s -w -p tcp --dport 53 -j ACCEPT", QC_DOG_DISABLE_CHAIN);
	//EXECUTE_CMD(cmd, "iptables -t mangle -A %s -w -p udp --dport 53 -j ACCEPT", QC_DOG_DISABLE_CHAIN);
	//EXECUTE_CMD(cmd, "iptables -t mangle -A %s -w -d %s -j ACCEPT", QC_DOG_DISABLE_CHAIN, AUTH_SERVER_HOST); //认证服务器放行
	//EXECUTE_CMD(cmd, "iptables -t mangle -A %s -w -j DROP", QC_DOG_DISABLE_CHAIN);

	EXECUTE_CMD(cmd, "iptables -t nat -A %s -w -m mark --mark %d -p tcp -j %s", QC_DOG_REDIRECT_CHAIN, ACCEPT_MARK, QC_DOG_MPROXY_CHAIN);	
	//EXECUTE_CMD(cmd, "iptables -t nat -A %s -w -m mark --mark %d -j ACCEPT", QC_DOG_REDIRECT_CHAIN, ACCEPT_MARK);
	//EXECUTE_CMD(cmd, "iptables -t nat -A %s -w -d %s -j ACCEPT", QC_DOG_REDIRECT_CHAIN, AUTH_SERVER_HOST); //认证服务器放行
	//EXECUTE_CMD(cmd, "iptables -t nat -A %s -w -p tcp --dport 80 -j DNAT --to %s:%d", QC_DOG_REDIRECT_CHAIN, get_netdev_lan_ip(), server_listen_port());
	//EXECUTE_CMD(cmd, "iptables -t nat -A %s -w -p tcp --dport 8080 -j DNAT --to %s:%d", QC_DOG_REDIRECT_CHAIN, get_netdev_lan_ip(), server_listen_port());
	//EXECUTE_CMD(cmd, "iptables -t nat -A %s -w -p tcp --dport 443 -j DNAT --to %s:%d", QC_DOG_REDIRECT_CHAIN, get_netdev_lan_ip(), ssl_server_listen_port());

	//QC_DOG_MPROXY_CHAIN
	fw_redirect_mproxy();
	
	EXECUTE_CMD(cmd, "iptables -t mangle -A %s -w -j MARK --set-mark %d", QC_DOG_AUTH_MARK_CHAIN, ACCEPT_MARK);
}

//设置mark
void fw_mark_set(char *ip, char *mac)
{
	char cmd[255];
	EXECUTE_CMD(cmd, "iptables -t mangle -A %s -w -s %s -m mac --mac-source %s -j MARK --set-mark %d", QC_DOG_AUTH_MARK_CHAIN, ip, mac, ACCEPT_MARK);
}

//删除规则
void fw_del_rule(int num)
{
	char cmd[255];
	EXECUTE_CMD(cmd, "iptables -t mangle -D %s %d -w", QC_DOG_AUTH_MARK_CHAIN, num);
}

