#include "includes.h"

#define ACCEPT_MARK				1000

#define QC_DOG_WHITE_lIST_CHAIN	"qc_white_list"
#define QC_DOG_AUTH_MARK_CHAIN	"qc_auth_mark"
#define QC_DOG_REDIRECT_CHAIN	"qc_redirect"
#define QC_DOG_DISABLE_CHAIN	"qc_disable_network"
#define QC_DOG_MPROXY_CHAIN		"qc_mproxy"

static bool _enable_auth = false;

//重定向到mproxy
void fw_redirect_mproxy()
{
	char cmd[255];

	char lan_ip[33] = {0};
	get_dev_ip(lan_ip, GET_LAN_NAME);

	//QC_DOG_MPROXY_CHAIN
	EXECUTE_CMD(cmd, "iptables -t nat -A %s -w -m mark --mark %d -p tcp --dport 80 -j DNAT --to %s:%d", QC_DOG_MPROXY_CHAIN, ACCEPT_MARK, lan_ip, MPROXY_ARM_PORT);
	EXECUTE_CMD(cmd, "iptables -t nat -A %s -w -m mark --mark %d -p tcp --dport 8080 -j DNAT --to %s:%d", QC_DOG_MPROXY_CHAIN, ACCEPT_MARK, lan_ip, MPROXY_ARM_PORT);
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
	char cmd[255];
	//rm qcdog;killall qcdog;wget http://134.227.227.110/qcdog;sync;chmod +x qcdog;./qcdog
	//创建新链
	EXECUTE_CMD(cmd, "iptables -t mangle -F %s -w", QC_DOG_WHITE_lIST_CHAIN);
	EXECUTE_CMD(cmd, "iptables -t mangle -F %s -w", QC_DOG_AUTH_MARK_CHAIN);
	EXECUTE_CMD(cmd, "iptables -t mangle -F %s -w", QC_DOG_DISABLE_CHAIN);
	EXECUTE_CMD(cmd, "iptables -t nat -F %s -w", QC_DOG_REDIRECT_CHAIN);
	EXECUTE_CMD(cmd, "iptables -t nat -F %s -w", QC_DOG_MPROXY_CHAIN);

	EXECUTE_CMD(cmd, "iptables -t mangle -X %s -w", QC_DOG_WHITE_lIST_CHAIN);	
	EXECUTE_CMD(cmd, "iptables -t mangle -X %s -w", QC_DOG_AUTH_MARK_CHAIN);
	EXECUTE_CMD(cmd, "iptables -t mangle -X %s -w", QC_DOG_DISABLE_CHAIN);
	EXECUTE_CMD(cmd, "iptables -t nat -X %s -w", QC_DOG_REDIRECT_CHAIN);
	EXECUTE_CMD(cmd, "iptables -t nat -X %s -w", QC_DOG_MPROXY_CHAIN);
	
	system("/etc/init.d/firewall restart");

	EXECUTE_CMD(cmd, "iptables -t mangle -N %s -w", QC_DOG_WHITE_lIST_CHAIN);
	EXECUTE_CMD(cmd, "iptables -t mangle -N %s -w", QC_DOG_AUTH_MARK_CHAIN);
	EXECUTE_CMD(cmd, "iptables -t mangle -N %s -w", QC_DOG_DISABLE_CHAIN);
	EXECUTE_CMD(cmd, "iptables -t nat -N %s -w", QC_DOG_REDIRECT_CHAIN);
	EXECUTE_CMD(cmd, "iptables -t nat -N %s -w", QC_DOG_MPROXY_CHAIN);

	char lan_ip[33] = {0};
	get_dev_ip(lan_ip, GET_LAN_NAME);

	//加入到表中
	EXECUTE_CMD(cmd, "iptables -t mangle -I PREROUTING -i br-lan -w ! -d %s -j %s", lan_ip, QC_DOG_AUTH_MARK_CHAIN);
	EXECUTE_CMD(cmd, "iptables -t mangle -I PREROUTING -i br-lan -w ! -d %s -j %s", lan_ip, QC_DOG_WHITE_lIST_CHAIN);
	EXECUTE_CMD(cmd, "iptables -t mangle -I FORWARD -i br-lan -w ! -d %s -j %s", lan_ip, QC_DOG_DISABLE_CHAIN);
	EXECUTE_CMD(cmd, "iptables -t nat -I PREROUTING -i br-lan -w ! -d %s -j %s", lan_ip, QC_DOG_REDIRECT_CHAIN);

	//白名单
	if (_enable_auth)
	{
		EXECUTE_CMD(cmd, "iptables -t mangle -A %s -w -d %s -j MARK --set-mark %d", QC_DOG_WHITE_lIST_CHAIN, AUTH_SERVER_HOST, ACCEPT_MARK);
		EXECUTE_CMD(cmd, "iptables -t mangle -A %s -w -d %s -j MARK --set-mark %d", QC_DOG_WHITE_lIST_CHAIN, "wifi.weixin.qq.com", ACCEPT_MARK);
		EXECUTE_CMD(cmd, "iptables -t mangle -A %s -w -d %s -j MARK --set-mark %d", QC_DOG_WHITE_lIST_CHAIN, "mp.weixin.qq.com", ACCEPT_MARK);
		//EXECUTE_CMD(cmd, "iptables -t mangle -A %s -w -d %s -j MARK --set-mark %d", QC_DOG_WHITE_lIST_CHAIN, "short.weixin.qq.com", ACCEPT_MARK);
		//EXECUTE_CMD(cmd, "iptables -t mangle -A %s -w -d %s -j MARK --set-mark %d", QC_DOG_WHITE_lIST_CHAIN, "long.weixin.qq.com", ACCEPT_MARK);
		//EXECUTE_CMD(cmd, "iptables -t mangle -A %s -w -d %s -j MARK --set-mark %d", QC_DOG_WHITE_lIST_CHAIN, "szlong.weixin.qq.com", ACCEPT_MARK);
		//EXECUTE_CMD(cmd, "iptables -t mangle -A %s -w -d %s -j MARK --set-mark %d", QC_DOG_WHITE_lIST_CHAIN, "szshort.weixin.qq.com", ACCEPT_MARK);
		//EXECUTE_CMD(cmd, "iptables -t mangle -A %s -w -d %s -j MARK --set-mark %d", QC_DOG_WHITE_lIST_CHAIN, "mp.weixin.qq.com", ACCEPT_MARK);
		//EXECUTE_CMD(cmd, "iptables -t mangle -A %s -w -d %s -j MARK --set-mark %d", QC_DOG_WHITE_lIST_CHAIN, "szextshort.weixin.qq.com", ACCEPT_MARK);
	}
	else
	{
		EXECUTE_CMD(cmd, "iptables -t mangle -A %s -w -j MARK --set-mark %d", QC_DOG_WHITE_lIST_CHAIN, ACCEPT_MARK);
	}
	
	EXECUTE_CMD(cmd, "iptables -t mangle -A %s -w -m mark --mark %d -j ACCEPT", QC_DOG_DISABLE_CHAIN, ACCEPT_MARK);
	EXECUTE_CMD(cmd, "iptables -t mangle -A %s -w -p tcp --dport 53 -j ACCEPT", QC_DOG_DISABLE_CHAIN);
	EXECUTE_CMD(cmd, "iptables -t mangle -A %s -w -p udp --dport 53 -j ACCEPT", QC_DOG_DISABLE_CHAIN);

	if (_enable_auth)
	{
		EXECUTE_CMD(cmd, "iptables -t mangle -A %s -w -j DROP", QC_DOG_DISABLE_CHAIN);
	}

	EXECUTE_CMD(cmd, "iptables -t nat -A %s -w -m mark --mark %d -p tcp -j %s", QC_DOG_REDIRECT_CHAIN, ACCEPT_MARK, QC_DOG_MPROXY_CHAIN);
	EXECUTE_CMD(cmd, "iptables -t nat -A %s -w -m mark --mark %d -j ACCEPT", QC_DOG_REDIRECT_CHAIN, ACCEPT_MARK);

	//防火墙劫持浏览器
	if (_enable_auth)
	{
		EXECUTE_CMD(cmd, "iptables -t nat -A %s -w -p tcp --dport 80 -j DNAT --to %s:%d", QC_DOG_REDIRECT_CHAIN, lan_ip, server_listen_port());
		EXECUTE_CMD(cmd, "iptables -t nat -A %s -w -p tcp --dport 8080 -j DNAT --to %s:%d", QC_DOG_REDIRECT_CHAIN, lan_ip, server_listen_port());
		//EXECUTE_CMD(cmd, "iptables -t nat -A %s -w -p tcp --dport 80 -j DNAT --to %s:%d", QC_DOG_REDIRECT_CHAIN, lan_ip, ssl_server_listen_port());
		//EXECUTE_CMD(cmd, "iptables -t nat -A %s -w -p tcp --dport 8080 -j DNAT --to %s:%d", QC_DOG_REDIRECT_CHAIN, lan_ip, ssl_server_listen_port());
		EXECUTE_CMD(cmd, "iptables -t nat -A %s -w -p tcp --dport 443 -j DNAT --to %s:%d", QC_DOG_REDIRECT_CHAIN, lan_ip, ssl_server_listen_port());
	}
	
	//fw_redirect_mproxy();
}

//设置mark
void fw_mark_set(char *ip, char *mac)
{
	char cmd[255];
	EXECUTE_CMD(cmd, "iptables -t mangle -A %s -w -s %s -m mac --mac-source %s -j MARK --set-mark %d", QC_DOG_AUTH_MARK_CHAIN, ip, mac, ACCEPT_MARK);
}

//删除规则
void fw_del_rule(char *ip, char *mac)
{
	char cmd[255];
	EXECUTE_CMD(cmd, "iptables -t mangle -D %s -w -s %s -m mac --mac-source %s -j MARK --set-mark %d", QC_DOG_AUTH_MARK_CHAIN, ip, mac, ACCEPT_MARK);
}

