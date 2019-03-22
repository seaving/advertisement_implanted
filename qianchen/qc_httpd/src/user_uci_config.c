#include "includes.h"

#define CMD_UCI_USER_CONFIG_GET		"uci get user_config.@user_config[0].%s"
#define CMD_UCI_USER_CONFIG_SET		"uci set user_config.@user_config[0].%s=%s"

//---------------------------------------------------------
//开关
void _get_user_config_value_call_(char *buf, void *val)
{
	if (val && buf)
	{
		memcpy((char *)val, buf, strlen(buf));
	}
}

int get_user_config_value(char *key, char *value)
{
	char cmd[255] = {0};
	snprintf(cmd, 255, CMD_UCI_USER_CONFIG_GET, key);
	if (popen_cmd(cmd, _get_user_config_value_call_, value) < 0)
	{
		return -1;
	}

	return 0;
}

//通过修改interface来禁用
int set_user_config_value(char *key, char *value)
{
	char cmd[255];
	EXECUTE_CMD(cmd, CMD_UCI_USER_CONFIG_SET, key, value);
	return 0;
}



