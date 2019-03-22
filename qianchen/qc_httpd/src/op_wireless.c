#include "includes.h"

//第一个%s是节点名称(wifi-iface),第二个为option属性, 第三个%s为属性值
#define CMD_UCI_WIRELESS_SET		"uci set wireless.%s.%s=%s"
#define CMD_UCI_WIRELESS_GET		"uci get wireless.%s.%s"

#define CMD_UCI_WIRELESS_DEL		"uci del wireless.%s.%s"

//---------------------------------------------
//ssid
void _get_wifi_ssid_call_(char *buf, void *ssid)
{
	if (ssid && buf)
	{
		memcpy((char *)ssid, buf, strlen(buf));
	}
}

int get_wifi_ssid(char *wifi_iface, char *ssid)
{
	char cmd[256];
	snprintf(cmd, 255, CMD_UCI_WIRELESS_GET, wifi_iface, "ssid");
	if (popen_cmd(cmd, _get_wifi_ssid_call_, ssid) < 0)
	{
		return -1;
	}
	
	LOG_WARN_INFO("ssid: %s\n", ssid);

	return 0;
}

int set_wifi_ssid(char *wifi_iface, char *ssid)
{
	char cmd[255];
	EXECUTE_CMD(cmd, CMD_UCI_WIRELESS_SET, wifi_iface, "ssid", ssid);
	return 0;
}
//---------------------------------------------
//device
void _get_wifi_device_call_(char *buf, void *device)
{
	if (device && buf)
	{
		memcpy((char *)device, buf, strlen(buf));
	}
}

int get_wifi_device(char *wifi_iface, char *device)
{
	char cmd[256];
	snprintf(cmd, 255, CMD_UCI_WIRELESS_GET, wifi_iface, "device");
	if (popen_cmd(cmd, _get_wifi_device_call_, device) < 0)
	{
		return -1;
	}
	
	LOG_WARN_INFO("ssid: %s\n", device);

	return 0;
}

int set_wifi_device(char *wifi_iface, char *device)
{
	char cmd[256];
	EXECUTE_CMD(cmd, CMD_UCI_WIRELESS_SET, wifi_iface, "device", device);
	return 0;
}
//---------------------------------------------
//无线功率
void _get_wifi_txpower_call_(char *buf, void *txpower)
{
	if (txpower && buf)
	{
		memcpy((char *)txpower, buf, strlen(buf));
	}
}

int get_wifi_txpower(char *wifi_iface)
{
	char txpower[10] = {0};
	char cmd[256];
	snprintf(cmd, 255, CMD_UCI_WIRELESS_GET, wifi_iface, "txpower");

	if (popen_cmd(cmd, _get_wifi_txpower_call_, txpower) < 0)
	{
		return -1;
	}
	
	LOG_WARN_INFO("txpower: %s\n", txpower);

	return atoi(txpower);
}

int set_wifi_txpower(char *wifi_iface, char *txpower)
{
	char cmd[255];
	EXECUTE_CMD(cmd, CMD_UCI_WIRELESS_SET, wifi_iface, "txpower", txpower);
	return 0;
}
//---------------------------------------------
//无线信道
void _get_wifi_channel_call_(char *buf, void *channel)
{
	if (channel && buf)
	{
		memcpy((char *)channel, buf, strlen(buf));
	}
}

int get_wifi_channel(char *wifi_iface, char *channel)
{
	char cmd[256];
	snprintf(cmd, 255, CMD_UCI_WIRELESS_GET, wifi_iface, "channel");

	if (popen_cmd(cmd, _get_wifi_channel_call_, channel) < 0)
	{
		return -1;
	}
	
	LOG_WARN_INFO("channel: %s\n", channel);

	return 0;
}

int set_wifi_channel(char *wifi_iface, char *channel)
{
	char cmd[255];
	EXECUTE_CMD(cmd, CMD_UCI_WIRELESS_SET, wifi_iface, "channel", channel);
	return 0;
}
//---------------------------------------------
//无线模式
void _get_wifi_mode_call_(char *buf, void *mode)
{
	if (mode && buf)
	{
		memcpy((char *)mode, buf, strlen(buf));
	}
}

int get_wifi_mode(char *wifi_iface, char *mode)
{
	char cmd[256];
	snprintf(cmd, 255, CMD_UCI_WIRELESS_GET, wifi_iface, "mode");

	if (popen_cmd(cmd, _get_wifi_mode_call_, mode) < 0)
	{
		return -1;
	}
	
	LOG_WARN_INFO("mode: %s\n", mode);

	return 0;
}

int set_wifi_mode(char *wifi_iface, char *mode)
{
	char cmd[255];
	EXECUTE_CMD(cmd, CMD_UCI_WIRELESS_SET, wifi_iface, "mode", mode);
	return 0;
}
//---------------------------------------------
//encryption
void _get_wifi_encryption_call_(char *buf, void *encryption)
{
	if (encryption && buf)
	{
		memcpy((char *)encryption, buf, strlen(buf));
	}
}

int get_wifi_encryption(char *wifi_iface, char *encryption)
{
	char cmd[256];
	snprintf(cmd, 255, CMD_UCI_WIRELESS_GET, wifi_iface, "encryption");

	if (popen_cmd(cmd, _get_wifi_encryption_call_, encryption) < 0)
	{
		return -1;
	}
	
	LOG_WARN_INFO("encryption: %s\n", encryption);

	return 0;
}

int set_wifi_encryption(char *wifi_iface, char *encryption)
{
	char cmd[255];
	EXECUTE_CMD(cmd, CMD_UCI_WIRELESS_SET, wifi_iface, "encryption", encryption);
	return 0;
}
//---------------------------------------------
//key(密码)
void _get_wifi_key_call_(char *buf, void *key)
{
	if (key && buf)
	{
		memcpy((char *)key, buf, strlen(buf));
	}
}

int get_wifi_key(char *wifi_iface, char *key)
{
	char cmd[256];
	snprintf(cmd, 255, CMD_UCI_WIRELESS_GET, wifi_iface, "key");

	if (popen_cmd(cmd, _get_wifi_key_call_, key) < 0)
	{
		return -1;
	}
	
	LOG_WARN_INFO("key: %s\n", key);

	return 0;
}

int set_wifi_key(char *wifi_iface, char *key)
{
	char cmd[255];
	EXECUTE_CMD(cmd, CMD_UCI_WIRELESS_SET, wifi_iface, "key", key);
	return 0;
}
//---------------------------------------------
//无线开关
void _get_wifi_disabled_call_(char *buf, void *disabled)
{
	if (disabled && buf)
	{
		memcpy((char *)disabled, buf, strlen(buf));
	}
}

int get_wifi_disabled(char *wifi_iface)
{
	char disabled[10] = {0};
	char cmd[256];
	snprintf(cmd, 255, CMD_UCI_WIRELESS_GET, wifi_iface, "disabled");
	if (popen_cmd(cmd, _get_wifi_disabled_call_, disabled) < 0)
	{
		return -1;
	}
	
	if (isNumber(disabled))
	{
		return atoi(disabled);
	}

	return 0;
}

int set_wifi_disabled(char *wifi_iface, char *disabled)
{
	char cmd[255];
	EXECUTE_CMD(cmd, CMD_UCI_WIRELESS_SET, wifi_iface, "disabled", disabled);
	return 0;
}
//---------------------------------------------
//无线带宽
void _get_wifi_htmode_call_(char *buf, void *htmode)
{
	if (htmode && buf)
	{
		memcpy((char *)htmode, buf, strlen(buf));
	}
}

int get_wifi_htmode(char *wifi_iface, char *htmode)
{
	char cmd[256];
	snprintf(cmd, 255, CMD_UCI_WIRELESS_GET, wifi_iface, "htmode");

	if (popen_cmd(cmd, _get_wifi_htmode_call_, htmode) < 0)
	{
		return -1;
	}
	
	LOG_WARN_INFO("htmode: %s\n", htmode);

	return 0;
}

int set_wifi_htmode(char *wifi_iface, char *htmode)
{
	char cmd[255];
	EXECUTE_CMD(cmd, CMD_UCI_WIRELESS_SET, wifi_iface, "htmode", htmode);
	return 0;
}
//---------------------------------------------
//无线带宽
void _get_wifi_hwmode_call_(char *buf, void *hwmode)
{
	if (hwmode && buf)
	{
		memcpy((char *)hwmode, buf, strlen(buf));
	}
}

int get_wifi_hwmode(char *wifi_iface, char *hwmode)
{
	char cmd[256];
	snprintf(cmd, 255, CMD_UCI_WIRELESS_GET, wifi_iface, "hwmode");

	if (popen_cmd(cmd, _get_wifi_hwmode_call_, hwmode) < 0)
	{
		return -1;
	}
	
	LOG_WARN_INFO("hwmode: %s\n", hwmode);

	return 0;
}

int set_wifi_hwmode(char *wifi_iface, char *hwmode)
{
	char cmd[255];
	EXECUTE_CMD(cmd, CMD_UCI_WIRELESS_SET, wifi_iface, "hwmode", hwmode);
	return 0;
}
//---------------------------------------------
//无线隐藏
void _get_wifi_hidden_call_(char *buf, void *hidden)
{
	if (hidden && buf)
	{
		memcpy((char *)hidden, buf, strlen(buf));
	}
}

int get_wifi_hidden(char *wifi_iface)
{
	char cmd[256];
	snprintf(cmd, 255, CMD_UCI_WIRELESS_GET, wifi_iface, "hidden");

	char hidden[5] = {0};
	if (popen_cmd(cmd, _get_wifi_hidden_call_, hidden) < 0)
	{
		return -1;
	}
	
	if (isNumber(hidden))
	{
		return atoi(hidden);
	}

	return 0;
}

int set_wifi_hidden(char *wifi_iface, char *hidden)
{
	char cmd[255];
	EXECUTE_CMD(cmd, CMD_UCI_WIRELESS_SET, wifi_iface, "hidden", hidden);
	return 0;
}
//---------------------------------------------
int get_2g_wifi_iface(char *wifi_iface)
{
	int i = 0;
	for (i = 0; i < 2; i ++)
	{
		char device[15] = {0};
		char hwmode[15] = {0};
		sprintf(wifi_iface, "default_radio%d", i);
		get_wifi_device(wifi_iface, device);
		get_wifi_hwmode(device, hwmode);
		if (strchr(hwmode, 'a'))
		{
			//5g
			continue;
		}

		return 0;
	}
	
	return -1;
}

int get_2g_wifi_device(char *wifi_device)
{
	int i = 0;
	for (i = 0; i < 2; i ++)
	{
		char device[15] = {0};
		char hwmode[15] = {0};
		char wifi_iface[64] = {0};
		snprintf(wifi_iface, 64, "default_radio%d", i);
		get_wifi_device(wifi_iface, device);
		get_wifi_hwmode(device, hwmode);
		if (strchr(hwmode, 'a'))
		{
			//5g
			continue;
		}

		sprintf(wifi_device, "%s", device);

		return 0;
	}
	
	return -1;
}

int get_guest_2g_wifi_iface(char *wifi_iface)
{
	int i = 0;
	for (i = 0; i < 2; i ++)
	{
		char device[15] = {0};
		char hwmode[15] = {0};
		sprintf(wifi_iface, "guest_radio%d", i);
		get_wifi_device(wifi_iface, device);
		get_wifi_hwmode(device, hwmode);
		if (strchr(hwmode, 'a'))
		{
			//5g
			continue;
		}

		return 0;
	}
	
	return -1;
}

int get_guest_2g_wifi_device(char *wifi_device)
{
	int i = 0;
	for (i = 0; i < 2; i ++)
	{
		char device[15] = {0};
		char hwmode[15] = {0};
		char wifi_iface[64] = {0};
		snprintf(wifi_iface, 64, "guest_radio%d", i);
		get_wifi_device(wifi_iface, device);
		get_wifi_hwmode(device, hwmode);
		if (strchr(hwmode, 'a'))
		{
			//5g
			continue;
		}

		sprintf(wifi_device, "%s", device);

		return 0;
	}
	
	return -1;
}

int get_5g_wifi_iface(char *wifi_iface)
{
	int i = 0;
	for (i = 0; i < 2; i ++)
	{
		char device[15] = {0};
		char hwmode[15] = {0};
		sprintf(wifi_iface, "default_radio%d", i);
		get_wifi_device(wifi_iface, device);
		get_wifi_hwmode(device, hwmode);
		if (! strchr(hwmode, 'a'))
		{
			//5g
			continue;
		}

		return 0;
	}
	
	return -1;
}

int get_5g_wifi_device(char *wifi_device)
{
	int i = 0;
	for (i = 0; i < 2; i ++)
	{
		char device[15] = {0};
		char hwmode[15] = {0};
		char wifi_iface[64] = {0};
		snprintf(wifi_iface, 64, "default_radio%d", i);
		get_wifi_device(wifi_iface, device);
		get_wifi_hwmode(device, hwmode);
		if (! strchr(hwmode, 'a'))
		{
			//5g
			continue;
		}

		sprintf(wifi_device, "%s", device);

		return 0;
	}
	
	return -1;
}

int get_guest_5g_wifi_iface(char *wifi_iface)
{
	int i = 0;
	for (i = 0; i < 2; i ++)
	{
		char device[15] = {0};
		char hwmode[15] = {0};
		sprintf(wifi_iface, "guest_radio%d", i);
		get_wifi_device(wifi_iface, device);
		get_wifi_hwmode(device, hwmode);
		if (! strchr(hwmode, 'a'))
		{
			//5g
			continue;
		}

		return 0;
	}
	
	return -1;
}

int get_guest_5g_wifi_device(char *wifi_device)
{
	int i = 0;
	for (i = 0; i < 2; i ++)
	{
		char device[15] = {0};
		char hwmode[15] = {0};
		char wifi_iface[64] = {0};
		snprintf(wifi_iface, 64, "guest_radio%d", i);
		get_wifi_device(wifi_iface, device);
		get_wifi_hwmode(device, hwmode);
		if (! strchr(hwmode, 'a'))
		{
			//5g
			continue;
		}

		sprintf(wifi_device, "%s", device);

		return 0;
	}
	
	return -1;
}

int get_signal_min(node_name_t node, char *signal)
{	
	char *node_name = "";
	switch (node)
	{
		case e_auth_2g_signal_min:
			node_name = "auth_2g_signal_min";
			break;
		case e_auth_5g_signal_min:
			node_name = "auth_5g_signal_min";
			break;
		case e_not_auth_2g_signal_min:
			node_name = "not_auth_2g_signal_min";
			break;
		case e_not_auth_5g_signal_min:
			node_name = "not_auth_5g_signal_min";
			break;
		default:
			return -1;
	}

	get_user_config_value(node_name, signal);

	return 0;
}

int set_signal_min(node_name_t node, char *signal_min)
{
	create_file("/etc/config/signal_min");

	char *node_name = "";
	switch (node)
	{
		case e_auth_2g_signal_min:
			node_name = "auth_2g_signal_min";
			break;
		case e_auth_5g_signal_min:
			node_name = "auth_5g_signal_min";
			break;
		case e_not_auth_2g_signal_min:
			node_name = "not_auth_2g_signal_min";
			break;
		case e_not_auth_5g_signal_min:
			node_name = "not_auth_5g_signal_min";
			break;
		default:
			return -1;
	}

	set_user_config_value(node_name, signal_min);
	return 0;
}

int del_wifi_section(char *wifi_dev, char *section)
{
	char cmd[255];
	EXECUTE_CMD(cmd, CMD_UCI_WIRELESS_DEL, wifi_dev, section);
	return 0;
}

