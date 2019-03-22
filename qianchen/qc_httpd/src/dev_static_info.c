#include "dev_static_info.h"

info_t info;

int get_2g_auth_ssid(char *ssid)
{
	int i = 0, ret = -1;
	for (i = 0; i < 2; i ++)
	{
		char device[15] = {0};
		char hwmode[15] = {0};
		char wifi_iface[64] = {0};
		snprintf(wifi_iface, 64, "default_radio%d", i);
		get_wifi_device(wifi_iface, device);
		char *p = strstr(device, "disable_");
		if (p)
		{
			p = device + strlen("disable_");
		}
		else
			p = device;
		
		get_wifi_hwmode(p, hwmode);
		if (strchr(hwmode, 'a'))
		{
			//5g
			continue;
		}

		get_wifi_ssid(wifi_iface, ssid);
		ret = 0;
		break;
	}
	
	return ret;
}

int get_5g_auth_ssid(char *ssid)
{
	int i = 0, ret = -1;
	for (i = 0; i < 2; i ++)
	{
		char device[15] = {0};
		char hwmode[15] = {0};
		char wifi_iface[64] = {0};
		snprintf(wifi_iface, 64, "default_radio%d", i);
		get_wifi_device(wifi_iface, device);
		char *p = strstr(device, "disable_");
		if (p)
		{
			p = device + strlen("disable_");
		}
		else
			p = device;
		
		get_wifi_hwmode(p, hwmode);

		if (! strchr(hwmode, 'a'))
		{
			//2g
			continue;
		}

		get_wifi_ssid(wifi_iface, ssid);
		ret = 0;
		break;
	}
	
	return ret;
}

int get_2g_not_auth_ssid(char *ssid)
{
	int i = 0, ret = -1;
	for (i = 0; i < 2; i ++)
	{
		char device[15] = {0};
		char hwmode[15] = {0};
		char wifi_iface[64] = {0};
		snprintf(wifi_iface, 64, "guest_radio%d", i);
		get_wifi_device(wifi_iface, device);
		char *p = strstr(device, "disable_");
		if (p)
		{
			p = device + strlen("disable_");
		}
		else
			p = device;
		
		get_wifi_hwmode(p, hwmode);

		if (strchr(hwmode, 'a'))
		{
			//5g
			continue;
		}
		
		get_wifi_ssid(wifi_iface, ssid);
		ret = 0;
		break;
	}
	
	return ret;
}

int get_5g_not_auth_ssid(char *ssid)
{
	int i = 0, ret = -1;
	for (i = 0; i < 2; i ++)
	{
		char device[15] = {0};
		char hwmode[15] = {0};
		char wifi_iface[64] = {0};
		snprintf(wifi_iface, 64, "guest_radio%d", i);
		get_wifi_device(wifi_iface, device);
		char *p = strstr(device, "disable_");
		if (p)
		{
			p = device + strlen("disable_");
		}
		else
			p = device;
		
		get_wifi_hwmode(p, hwmode);

		if (! strchr(hwmode, 'a'))
		{
			//2g
			continue;
		}

		get_wifi_ssid(wifi_iface, ssid);
		ret = 0;
		break;
	}
	
	return ret;
}

int get_2g_not_auth_password(char *password)
{
	int i = 0, ret = -1;
	for (i = 0; i < 2; i ++)
	{
		char device[15] = {0};
		char hwmode[15] = {0};
		char wifi_iface[64] = {0};
		snprintf(wifi_iface, 64, "guest_radio%d", i);
		get_wifi_device(wifi_iface, device);
		char *p = strstr(device, "disable_");
		if (p)
		{
			p = device + strlen("disable_");
		}
		else
			p = device;
		
		get_wifi_hwmode(p, hwmode);

		if (strchr(hwmode, 'a'))
		{
			//5g
			continue;
		}

		get_wifi_key(wifi_iface, password);
		ret = 0;
		break;
	}
	
	return ret;
}

int get_5g_not_auth_password(char *password)
{
	int i = 0, ret = -1;
	for (i = 0; i < 2; i ++)
	{
		char device[15] = {0};
		char hwmode[15] = {0};
		char wifi_iface[64] = {0};
		snprintf(wifi_iface, 64, "guest_radio%d", i);
		get_wifi_device(wifi_iface, device);
		char *p = strstr(device, "disable_");
		if (p)
		{
			p = device + strlen("disable_");
		}
		else
			p = device;
		
		get_wifi_hwmode(p, hwmode);

		if (! strchr(hwmode, 'a'))
		{
			//2g
			continue;
		}

		get_wifi_key(wifi_iface, password);
		ret = 0;
		break;
	}
	
	return ret;
}

bool get_not_auth_disable()
{
	int i = 0, err = 0;
	for (i = 0; i < 2; i ++)
	{
		char device[15] = {0};
		char wifi_iface[64] = {0};
		snprintf(wifi_iface, 64, "guest_radio%d", i);
		get_wifi_device(wifi_iface, device);
		char radio[10] = {0};
		sprintf(radio, "radio%d", i);
		if (strcmp(device, radio))
		{
			err ++;
		}
	}
	
	return err >= 2 ? true : false;
}

bool get_not_auth_hidden()
{
	int i = 0, r = 0;
	for (i = 0; i < 2; i ++)
	{
		char wifi_iface[64] = {0};
		snprintf(wifi_iface, 64, "guest_radio%d", i);
		if (get_wifi_hidden(wifi_iface) != 0)
		{
			r ++;
		}
	}
	
	return r >= 2 ? false : true;
}

int set_not_auth_hidden(bool isHidden)
{
	set_wifi_hidden("guest_radio0", isHidden ? "1" : "0");
	set_wifi_hidden("guest_radio1", isHidden ? "1" : "0");
	return 0;
}

int set_not_auth_disable()
{
	set_wifi_device("guest_radio0", "disable_radio0");
	set_wifi_device("guest_radio1", "disable_radio1");
	return 0;
}

int set_not_auth_enable()
{
	set_wifi_device("guest_radio0", "radio0");
	set_wifi_device("guest_radio1", "radio1");
	return 0;
}

int get_auth_dhcp_assigned_count()
{
	return get_dhcp_assigned_count(LAN_DEV);
}

int get_not_auth_dhcp_assigned_count()
{
	return get_dhcp_assigned_count(LAN_GUEST_DEV);
}

int get_auth_dhcp_max_count()
{
	return get_dhcp_ippool_limit("lan");
}

int get_not_auth_dhcp_max_count()
{
	return get_dhcp_ippool_limit("guest");
}

bool save_struct_to_file()
{
	FILE *fp = NULL;

	unlink("/tmp/static_info");
	
	fp = fopen("/tmp/static_info", "wb");
	if (! fp) return false;
	
	fwrite(&info, sizeof(info_t), 1, fp); // д1????
	fclose(fp);
	
	return true;
}

bool read_struct_from_file()
{
	FILE *fp = NULL;

	fp = fopen("/tmp/static_info", "rb");
	if (! fp) return false;
	
	fread(&info, sizeof(info_t), 1, fp); // ??1????
	fclose(fp);

	return true;
}

void dev_static_info_init()
{
	memset(&info, 0, sizeof(info_t));
	
	if (! read_struct_from_file())
	{
		get_local_mac(info.wan_info.wan_mac, get_netdev_wan_name());
		get_network_proto("wan", info.wan_info.protocol);
		
		get_2g_auth_ssid(info.wireless_info.auth_2g_ssid);	
		get_5g_auth_ssid(info.wireless_info.auth_5g_ssid);	
		get_2g_not_auth_ssid(info.wireless_info.no_auth_2g_ssid);
		get_5g_not_auth_ssid(info.wireless_info.no_auth_5g_ssid);
	
		info.dhcp_info.auth_dhcp_max = get_auth_dhcp_max_count();
		info.dhcp_info.no_auth_dhcp_max = get_not_auth_dhcp_max_count();
	}

	save_struct_to_file();
}




