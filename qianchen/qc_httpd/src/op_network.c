#include "includes.h"

//第一个%s是结点，第二个是属性，第三个是属性值
#define CMD_UCI_NETWORK_GET		"uci get network.%s.%s"
#define CMD_UCI_NETWORK_SET		"uci set network.%s.%s=%s"

#define CMD_UCI_NETWORK_DEL		"uci del network.%s.%s"

#define CMD_UBUS_NETWORK_GET	"ubus call network.interface.%s status"

int del_network_section(char *dev_name, char *section)
{
	char cmd[255];
	EXECUTE_CMD(cmd, CMD_UCI_NETWORK_DEL, dev_name, section);
	return 0;
}

int write_del_network_section_cmd_shell(char *dev_name, char *section)
{
	char cmd[255];
	WRITE_EXECUTE_CMD_TO_FILE(cmd, CMD_UCI_NETWORK_DEL, dev_name, section);
	return 0;
}

//---------------------------------------------
void _get_network_ipaddr_call_(char *buf, void *ipaddr)
{
	if (ipaddr && buf)
	{
		memcpy((char *)ipaddr, buf, strlen(buf));
	}
}

int get_network_ipaddr(char *dev_name, char *ipaddr)
{
	char cmd[255] = {0};
	snprintf(cmd, 255, CMD_UCI_NETWORK_GET, dev_name, "ipaddr");
	if (popen_cmd(cmd, _get_network_ipaddr_call_, ipaddr) < 0)
	{
		return -1;
	}
	
	LOG_WARN_INFO("%s ipaddr: %s\n", dev_name, ipaddr);

	return 0;
}

int set_network_ipaddr(char *dev_name, char *ipaddr)
{
	char cmd[255];
	EXECUTE_CMD(cmd, CMD_UCI_NETWORK_SET, dev_name, "ipaddr", ipaddr);
	return 0;
}

int write_set_network_ipaddr_cmd_shell(char *dev_name, char *ipaddr)
{
	char cmd[255];
	WRITE_EXECUTE_CMD_TO_FILE(cmd, CMD_UCI_NETWORK_SET, dev_name, "ipaddr", ipaddr);
	return 0;
}

//---------------------------------------------
void _get_network_mask_call_(char *buf, void *mask)
{
	if (mask && buf)
	{
		memcpy((char *)mask, buf, strlen(buf));
	}
}

int get_network_mask(char *dev_name, char *mask)
{
	char cmd[255] = {0};
	snprintf(cmd, 255, CMD_UCI_NETWORK_GET, dev_name, "netmask");

	if (popen_cmd(cmd, _get_network_mask_call_, mask) < 0)
	{
		return -1;
	}
	
	LOG_WARN_INFO("%s mask: %s\n", dev_name, mask);

	return 0;
}

int set_network_mask(char *dev_name, char *mask)
{
	char cmd[255];
	EXECUTE_CMD(cmd, CMD_UCI_NETWORK_SET, dev_name, "netmask", mask);
	return 0;
}

int write_set_network_mask_cmd_shell(char *dev_name, char *mask)
{
	char cmd[255];
	WRITE_EXECUTE_CMD_TO_FILE(cmd, CMD_UCI_NETWORK_SET, dev_name, "netmask", mask);
	return 0;
}

//---------------------------------------------
void _get_network_proto_call_(char *buf, void *proto)
{
	if (proto && buf)
	{
		memcpy((char *)proto, buf, strlen(buf));
	}
}

int get_network_proto(char *dev_name, char *proto)
{
	char cmd[255] = {0};
	snprintf(cmd, 255, CMD_UCI_NETWORK_GET, dev_name, "proto");

	if (popen_cmd(cmd, _get_network_proto_call_, proto) < 0)
	{
		return -1;
	}
	
	LOG_WARN_INFO("%s proto: %s\n", dev_name, proto);

	return 0;
}

int set_network_proto(char *dev_name, char *proto)
{
	char cmd[255];
	EXECUTE_CMD(cmd, CMD_UCI_NETWORK_SET, dev_name, "proto", proto);
	return 0;
}

int write_set_network_proto_cmd_shell(char *dev_name, char *proto)
{
	char cmd[255];
	WRITE_EXECUTE_CMD_TO_FILE(cmd, CMD_UCI_NETWORK_SET, dev_name, "proto", proto);
	return 0;
}

//---------------------------------------------
void _get_network_dns_call_(char *buf, void *dns)
{
	if (dns && buf)
	{
		memcpy((char *)dns, buf, strlen(buf));
	}
}

int get_network_dns(char *dev_name, char *dns)
{
	char cmd[255] = {0};
	snprintf(cmd, 255, CMD_UCI_NETWORK_GET, dev_name, "dns");

	if (popen_cmd(cmd, _get_network_proto_call_, dns) < 0)
	{
		return -1;
	}
	
	LOG_WARN_INFO("%s dns: %s\n", dev_name, dns);

	return 0;
}

int set_network_dns(char *dev_name, char *dns)
{
	char cmd[255];
	EXECUTE_CMD(cmd, CMD_UCI_NETWORK_SET, dev_name, "dns", dns);
	return 0;
}

int write_set_network_dns_cmd_shell(char *dev_name, char *dns)
{
	char cmd[255];
	WRITE_EXECUTE_CMD_TO_FILE(cmd, CMD_UCI_NETWORK_SET, dev_name, "dns", dns);
	return 0;
}

//---------------------------------------------
void _get_network_mtu_call_(char *buf, void *mtu)
{
	if (mtu && buf)
	{
		memcpy((char *)mtu, buf, strlen(buf));
	}
}

int get_network_mtu(char *dev_name)
{
	char mtu[10] = {0};
	char cmd[255] = {0};
	snprintf(cmd, 255, CMD_UCI_NETWORK_GET, dev_name, "mtu");

	if (popen_cmd(cmd, _get_network_mtu_call_, mtu) < 0)
	{
		return -1;
	}
	
	if (isNumber(mtu))
	{
		return atoi(mtu);
	}
	
	return 0;
}

int set_network_mtu(char *dev_name, char *mtu)
{
	char cmd[255];
	EXECUTE_CMD(cmd, CMD_UCI_NETWORK_SET, dev_name, "mtu", mtu);
	return 0;
}

int write_set_network_mtu_cmd_shell(char *dev_name, char *mtu)
{
	char cmd[255];
	WRITE_EXECUTE_CMD_TO_FILE(cmd, CMD_UCI_NETWORK_SET, dev_name, "mtu", mtu);
	return 0;
}

//---------------------------------------------
void _get_wan_gateway_call_(char *buf, void *gw)
{
	if (gw && buf)
	{
		memcpy((char *)gw, buf, strlen(buf));
	}
}

int get_wan_gateway(char *gw)
{
	char cmd[255] = {0};
	snprintf(cmd, 255, CMD_UBUS_NETWORK_GET" | grep nexthop | grep -oE '([0-9]{1,3}.){3}.[0-9]{1,3}'", "wan");

	if (popen_cmd(cmd, _get_wan_gateway_call_, gw) < 0)
	{
		return -1;
	}
	
	return 0;
}

int set_network_gateway(char *dev_name, char *gateway)
{
	char cmd[255];
	EXECUTE_CMD(cmd, CMD_UCI_NETWORK_SET, dev_name, "gateway", gateway);
	return 0;
}

int write_set_network_gateway_cmd_shell(char *dev_name, char *gateway)
{
	char cmd[255];
	WRITE_EXECUTE_CMD_TO_FILE(cmd, CMD_UCI_NETWORK_SET, dev_name, "gateway", gateway);
	return 0;
}

//---------------------------------------------
//wan username
void _get_wan_username_call_(char *buf, void *username)
{
	if (username && buf)
	{
		memcpy((char *)username, buf, strlen(buf));
	}
}

int get_wan_username(char *username)
{
	char cmd[255] = {0};
	snprintf(cmd, 255, CMD_UCI_NETWORK_GET, "wan", "username");

	if (popen_cmd(cmd, _get_wan_username_call_, username) < 0)
	{
		return -1;
	}
	
	LOG_WARN_INFO("wan username: %s\n", username);

	return 0;
}

int set_wan_username(char *username)
{
	char cmd[255];
	EXECUTE_CMD(cmd, CMD_UCI_NETWORK_SET, "wan", "username", username);
	return 0;
}

int write_set_wan_username_cmd_shell(char *username)
{
	char cmd[255];
	WRITE_EXECUTE_CMD_TO_FILE(cmd, CMD_UCI_NETWORK_SET, "wan", "username", username);
	return 0;
}

//---------------------------------------------
//wan password
void _get_wan_password_call_(char *buf, void *password)
{
	if (password && buf)
	{
		memcpy((char *)password, buf, strlen(buf));
	}
}

int get_wan_password(char *password)
{
	char cmd[255] = {0};
	snprintf(cmd, 255, CMD_UCI_NETWORK_GET, "wan", "password");

	if (popen_cmd(cmd, _get_wan_password_call_, password) < 0)
	{
		return -1;
	}
	
	LOG_WARN_INFO("wan password: %s\n", password);

	return 0;
}

int set_wan_password(char *password)
{
	char cmd[255];
	EXECUTE_CMD(cmd, CMD_UCI_NETWORK_SET, "wan", "password", password);
	return 0;
}

int write_set_wan_password_cmd_shell(char *password)
{
	char cmd[255];
	WRITE_EXECUTE_CMD_TO_FILE(cmd, CMD_UCI_NETWORK_SET, "wan", "password", password);
	return 0;
}

//---------------------------------------------
int get_wan_protocol(char * proto)
{
	get_network_proto("wan", proto);
	return 0;
}


