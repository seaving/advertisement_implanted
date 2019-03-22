#include "cgi_get_info.h"


int get_mac(char *mac)
{
	return get_local_mac(mac, get_netdev_wan_name());
}

int get_wan_ip(char *ip)
{
	 get_dev_ip(ip, get_netdev_wan_name());
	 return 0;
}

int get_wan_first_dns(char * dns)
{
	get_dns(dns, 1);
	return 0;
}

int get_wan_second_dns(char * dns)
{
	get_dns(dns, 2);
	return 0;
}

int get_reboot_time(char *time, int time_size)
{
	if (read_file("/etc/app/reboot_time", time, time_size) <= 0)
	{
		char *p = utf8_encode("Î´ÉèÖÃ");
		if (p)
		{
			memset(time, 0, time_size);
			memcpy(time, p, strlen(p));
			utf8_encode_free(p);
		}
	}

	return strlen(time);
}

int cgi_get_info(struct evhttp_request *req, const t_http_server *http_server)
{
	int ret = -1;
	
	json_object *my_object = json_object_new_object();
	if (! my_object) return -1;

	char model[64] = {0};
	get_model(model, 64);

	char version[64] = {0};
	get_firmware_version(version);

	if (strlen(info.wan_info.wan_mac) <= 0)
	{
		get_mac(info.wan_info.wan_mac);
	}
	
	if (strlen(info.wireless_info.auth_2g_ssid) <= 0)
	{
		get_2g_auth_ssid(info.wireless_info.auth_2g_ssid);
	}
	
	if (strlen(info.wireless_info.auth_5g_ssid) <= 0)
	{
		get_5g_auth_ssid(info.wireless_info.auth_5g_ssid);
	}
	
	if (strlen(info.wireless_info.no_auth_2g_ssid) <= 0)
	{
		get_2g_not_auth_ssid(info.wireless_info.no_auth_2g_ssid);
	}
	
	if (strlen(info.wireless_info.no_auth_5g_ssid) <= 0)
	{
		get_5g_not_auth_ssid(info.wireless_info.no_auth_5g_ssid);
	}


	int auth_assigned_count = get_auth_dhcp_assigned_count();
	int not_auth_assigned_count = get_not_auth_dhcp_assigned_count();

	if (info.dhcp_info.auth_dhcp_max <= 0)
	{
		info.dhcp_info.auth_dhcp_max = get_auth_dhcp_max_count();
	}

	if (info.dhcp_info.no_auth_dhcp_max <= 0)
	{
		info.dhcp_info.no_auth_dhcp_max = get_not_auth_dhcp_max_count();
	}
	
	char wan_ip[32] = {0};
	get_wan_ip(wan_ip);

	char dns_first[32] = {0};
	get_wan_first_dns(dns_first);
	
	char dns_second[32] = {0};
	get_wan_second_dns(dns_second);

	if (strlen(info.wan_info.protocol) <= 0)
	{
		get_wan_protocol(info.wan_info.protocol);
	}
	
	char reboot_time[33] = {0};
	get_reboot_time(reboot_time, 32);
	
	json_object_object_add(my_object, "res", json_object_new_string("success"));
	json_object_object_add(my_object, "model", json_object_new_string(model));
	json_object_object_add(my_object, "version", json_object_new_string(version));
	json_object_object_add(my_object, "mac", json_object_new_string(info.wan_info.wan_mac));
	json_object_object_add(my_object, "auth_ssid_2g", json_object_new_string(info.wireless_info.auth_2g_ssid));
	json_object_object_add(my_object, "auth_ssid_5g", json_object_new_string(info.wireless_info.auth_5g_ssid));
	json_object_object_add(my_object, "not_auth_ssid_2g", json_object_new_string(info.wireless_info.no_auth_2g_ssid));
	json_object_object_add(my_object, "not_auth_ssid_5g", json_object_new_string(info.wireless_info.no_auth_5g_ssid));
	json_object_object_add(my_object, "auth_assigned_count", json_object_new_int(auth_assigned_count));
	json_object_object_add(my_object, "not_auth_assigned_count", json_object_new_int(not_auth_assigned_count));
	json_object_object_add(my_object, "auth_max_count", json_object_new_int(info.dhcp_info.auth_dhcp_max));
	json_object_object_add(my_object, "not_auth_max_count", json_object_new_int(info.dhcp_info.no_auth_dhcp_max));
	json_object_object_add(my_object, "wan_ip", json_object_new_string(wan_ip));
	json_object_object_add(my_object, "dns_first", json_object_new_string(dns_first));
	json_object_object_add(my_object, "dns_second", json_object_new_string(dns_second));
	json_object_object_add(my_object, "wan_protocol", json_object_new_string(info.wan_info.protocol));
	json_object_object_add(my_object, "reboot_time", json_object_new_string(reboot_time));
	
	LOG_HL_INFO("JSON -->: %s\n", json_object_to_json_string(my_object));

	evhttpd_send_200_response(req, "application/json;charset=UTF-8", json_object_to_json_string(my_object));
	json_object_put(my_object);

	return ret;
}

