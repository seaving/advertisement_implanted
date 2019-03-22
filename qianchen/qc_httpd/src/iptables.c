#include "includes.h"



void bwlist_ip_drop(char *ip)
{
	char cmd_buf[512] = {0};
	EXECUTE_CMD(cmd_buf, 
	"iptables -t filter -I FORWARD -i %s -w -p tcp -d %s -j DROP", "ap0", ip);
}

void bwlist_domain_drop(char *domain)
{
	char cmd_buf[1024] = {0};
	EXECUTE_CMD(cmd_buf, 
	"iptables -t filter -I FORWARD -i %s -w -m string --string \"%s\" --algo kmp -j DROP", "ap0", domain);
}

void bwlist_redirect(unsigned short port, char *toip, unsigned short toport)
{
	char cmd_buf[512] = {0};
	EXECUTE_CMD(cmd_buf, 
	"iptables -t nat -I PREROUTING -i %s -w -p tcp --dport %d -j DNAT --to %s:%d", 
	"ap0", port, toip, toport);
}

void bwlist_ip_accept(char *ip)
{
	char cmd_buf[512] = {0};
	EXECUTE_CMD(cmd_buf, 
	"iptables -t nat -I PREROUTING -i %s -w -p tcp -d %s -j ACCEPT", "ap0", ip);	
}

void bwlist_refresh(char *table, char *chain)
{
	char cmd_buf[255] = {0};
	EXECUTE_CMD(cmd_buf, "iptables -t %s -F %s", table, chain);
}

