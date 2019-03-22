
#include "includes.h"

#define CMD_PORT_FORWARD_COUNT		"cat /etc/config/port_forward | wc -l"
#define CMD_GET_PORT_FORWARD_LIST	"cat /etc/config/port_forward | head -n %d | tail -n 1"
#define CMD_ADD_PORT_FORWARD		"echo \"%s %s %s:%s\" >> /etc/config/port_forward"
#define CMD_IS_EXIST_PORT_FORWARD	"cat /etc/config/port_forward | awk -F \" \" '{print $1}' | grep \"%s\" | wc -l"
#define CMD_DELETE_PORT_FORWARD		"sed -i '/%s/d' /etc/config/port_forward"

void _get_port_forward_total_call_(char *buf, void *val)
{
	if (val && buf)
	{
		memcpy((char *)val, buf, strlen(buf));
	}
}

int get_port_forward_total()
{
	char total[20] = {0};
	if (popen_cmd(CMD_PORT_FORWARD_COUNT, _get_port_forward_total_call_, total) < 0)
	{
		return -1;
	}

	if (isNumber(total))
	{
		return atoi(total);
	}
	
	return 0;
}

void _get_port_forward_call_(char *buf, void *val)
{
	if (val && buf)
	{
		memcpy((char *)val, buf, strlen(buf));
	}
}

int get_port_forward(int offset, char *port_forward)
{
	char cmd[255] = {0};
	snprintf(cmd, 255, CMD_GET_PORT_FORWARD_LIST, offset);

	if (popen_cmd(cmd, _get_port_forward_call_, port_forward) < 0)
	{
		return -1;
	}
	
	return 0;
}

void _is_exist_port_forward_call_(char *buf, void *val)
{
	if (val && buf)
	{
		memcpy((char *)val, buf, strlen(buf));
	}
}

bool is_exist_port_forward(char *name)
{
	char cmd[255] = {0};
	snprintf(cmd, 255, CMD_IS_EXIST_PORT_FORWARD, name);

	char cnt[20] = {0};
	if (popen_cmd(cmd, _is_exist_port_forward_call_, cnt) < 0)
	{
		return false;
	}

	if (isNumber(cnt)
	&& atoi(cnt) > 0)
	{
		return true;
	}
	
	return false;
}

int add_port_forward(char *name, char *src_port, char *dst_port, char *dst_ip)
{
	if (is_exist_port_forward(name))
		return 1;

	char cmd[255] = {0};
	EXECUTE_CMD(cmd, CMD_ADD_PORT_FORWARD, name, src_port, dst_ip, dst_port);

	return 0;
}

void delete_port_forward(char *name)
{
	char cmd[255] = {0};
	EXECUTE_CMD(cmd, CMD_DELETE_PORT_FORWARD, name);
}

