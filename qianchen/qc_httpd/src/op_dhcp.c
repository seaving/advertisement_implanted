#include "includes.h"

//第一个%s为dhcp节点(lan,guest,wan),第二个%s为属性,第三个%s为属性值
#define CMD_UCI_DHCP_GET	"uci get dhcp.%s.%s"
#define CMD_UCI_DHCP_SET	"uci set dhcp.%s.%s=%s"


#define CMD_GET_IP_ASSIGNED_COUNT	"ip_list=`cat /proc/net/arp | grep \"%s\" | awk -F \" \" '{printf \"|\"$1}'`; " \
									"ip_list=${ip_list:1}; " \
									"[ -n \"$ip_list\" ] && { " \
									"	count=`cat /tmp/dhcp.leases | grep -E \"$ip_list\" | wc -l`; " \
									"	echo \"$count\"; " \
									" }"

//---------------------------------------------------------
//开关
void _get_dhcp_switch_call_(char *buf, void *interface)
{
	if (interface && buf)
	{
		memcpy((char *)interface, buf, strlen(buf));
	}
}

int get_dhcp_switch(char *dhcp_dev)
{
	char cmd[255] = {0};
	char interface_name[32] = {0};
	snprintf(cmd, 255, CMD_UCI_DHCP_GET, dhcp_dev, "interface");
	if (popen_cmd(cmd, _get_dhcp_switch_call_, interface_name) < 0)
	{
		return -1;
	}

	if (strcmp(dhcp_dev, "lan") == 0
	&& strcmp(interface_name, "lan") == 0)
	{
		return 1;
	}

	if (strcmp(dhcp_dev, "guest") == 0
	&& strcmp(interface_name, "guest") == 0)
	{
		return 1;
	}

	return 0;
}

//通过修改interface来禁用
int set_dhcp_disable(char *dhcp_dev)
{
	char cmd[255];
	if (strcmp(dhcp_dev, "lan") == 0)
	{
		EXECUTE_CMD(cmd, CMD_UCI_DHCP_SET, dhcp_dev, "interface", "disable_lan");
	}
	else if (strcmp(dhcp_dev, "guest") == 0)
	{
		EXECUTE_CMD(cmd, CMD_UCI_DHCP_SET, dhcp_dev, "interface", "disable_guest");
	}
	return 0;
}

int set_dhcp_enable(char *dhcp_dev)
{
	char cmd[255];
	if (strcmp(dhcp_dev, "lan") == 0)
	{
		EXECUTE_CMD(cmd, CMD_UCI_DHCP_SET, dhcp_dev, "interface", "lan");
	}
	else if (strcmp(dhcp_dev, "guest") == 0)
	{
		EXECUTE_CMD(cmd, CMD_UCI_DHCP_SET, dhcp_dev, "interface", "guest");
	}

	return 0;
}

//---------------------------------------------------------
//租期
void _get_dhcp_leasetime_call_(char *buf, void *leasetime)
{
	if (leasetime && buf)
	{
		memcpy((char *)leasetime, buf, strlen(buf));
	}
}

int get_dhcp_leasetime(char *dhcp_dev)
{
	char leasetime[20] = {0};
	char cmd[255] = {0};
	snprintf(cmd, 255, CMD_UCI_DHCP_GET, dhcp_dev, "leasetime");
	if (popen_cmd(cmd, _get_dhcp_leasetime_call_, leasetime) < 0)
	{
		return -1;
	}

	int i;
	for (i = 0; i < strlen(leasetime) && i < 2; i ++)
	{
		if (! ('0' <= leasetime[i] && leasetime[i] <= '9'))
		{
			break;
		}
	}

	leasetime[i] = 0;
	
	if (isNumber(leasetime))
	{
		return atoi(leasetime);
	}

	return 0;
}

int set_dhcp_leasetime(char *dhcp_dev, char *leasetime)
{
	char cmd[255];
	EXECUTE_CMD(cmd, CMD_UCI_DHCP_SET, dhcp_dev, "leasetime", leasetime);
	return 0;
}

//---------------------------------------------
//ip pool
void _get_dhcp_ippool_call_(char *buf, void *val)
{
	if (val && buf)
	{
		memcpy((char *)val, buf, strlen(buf));
	}
}

int get_dhcp_ippool_start(char *dhcp_dev)
{
	char start[20] = {0};
	char cmd[255] = {0};
	snprintf(cmd, 255, CMD_UCI_DHCP_GET, dhcp_dev, "start");
	if (popen_cmd(cmd, _get_dhcp_ippool_call_, start) < 0)
	{
		return -1;
	}

	if (isNumber(start))
	{
		return atoi(start);
	}
	
	return 0;
}

int get_dhcp_ippool_limit(char *dhcp_dev)
{
	char limit[20] = {0};
	char cmd[255] = {0};
	snprintf(cmd, 255, CMD_UCI_DHCP_GET, dhcp_dev, "limit");

	if (popen_cmd(cmd, _get_dhcp_ippool_call_, limit) < 0)
	{
		return -1;
	}

	if (isNumber(limit))
	{
		return atoi(limit);
	}

	return 0;
}

int set_dhcp_start(char *dhcp_dev, char *start)
{
	char cmd[255];
	EXECUTE_CMD(cmd, CMD_UCI_DHCP_SET, dhcp_dev, "start", start);
	return 0;
}

int set_dhcp_limit(char *dhcp_dev, char *limit)
{
	char cmd[255];
	EXECUTE_CMD(cmd, CMD_UCI_DHCP_SET, dhcp_dev, "limit", limit);
	return 0;
}

int set_dhcp_ippool(char *dhcp_dev, char *start, char *limit)
{
	char cmd[255];
	EXECUTE_CMD(cmd, CMD_UCI_DHCP_SET, dhcp_dev, "start", start);
	EXECUTE_CMD(cmd, CMD_UCI_DHCP_SET, dhcp_dev, "limit", limit);	
	return 0;
}
//---------------------------------------------
//ip assigned
void _get_dhcp_assigned_count_call_(char *buf, void *val)
{
	if (val && buf)
	{
		memcpy((char *)val, buf, strlen(buf));
	}
}

int get_dhcp_assigned_count(char *dhcp_dev)
{
	char count[16] = {0};
	char cmd[512] = {0};
	snprintf(cmd, 512, CMD_GET_IP_ASSIGNED_COUNT, dhcp_dev);
	if (popen_cmd(cmd, _get_dhcp_assigned_count_call_, count) < 0)
	{
		return -1;
	}

	if (isNumber(count))
	{
		return atoi(count);
	}

	return 0;
}




