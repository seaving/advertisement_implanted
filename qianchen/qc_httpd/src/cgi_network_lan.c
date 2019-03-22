#include "cgi_network_lan.h"

static bool set_lan_flag = false;

int get_lan_quarantine_switch()
{
	char buf[5] = {0};
	get_user_config_value("lan_quarantine", buf);
	if (buf[0] == '0')
	{
		return 0;
	}
	else if (buf[0] == '1')
	{
		return 1;
	}

	return 0;
}

int set_lan_quarantine_switch(bool isEnable)
{
	if (isEnable)
	{
		set_user_config_value("lan_quarantine", "1");
	}
	else
	{
		set_user_config_value("lan_quarantine", "0");
	}

	return 0;
}

int get_start_ip(char *ipaddr)
{
	char mask[32] = {0};
	get_network_mask("lan", mask);
	char ip[32] = {0};
	get_network_ipaddr("lan", ip);
	unsigned int segment = get_network_segment_int(ip, mask);
	unsigned int start = get_dhcp_ippool_start("lan");
	unsigned int sub_ip = segment + start;
	int_ip_to_str(sub_ip, ipaddr);
	return 0;
}

int cgi_get_lan_info(struct evhttp_request *req, const t_http_server *http_server)
{
	int ret = -1;
	
	json_object *my_object = json_object_new_object();
	if (! my_object) return -1;

	char ip[32] = {0};
	get_network_ipaddr("lan", ip);

	char mask[32] = {0};
	get_network_mask("lan", mask);

	char *dhcp_switch = "off";
	if (get_dhcp_switch("lan") > 0)
	{
		dhcp_switch = "on";
	}

	char start_ip[32] = {0};
	get_start_ip(start_ip);
	int limit = get_dhcp_ippool_limit("lan");
	int leasetime = get_dhcp_leasetime("lan");

	char *lan_quarantine = "off";
	if (get_lan_quarantine_switch() > 0)
	{
		lan_quarantine = "on";
	}
	
	json_object_object_add(my_object, "res", json_object_new_string("success"));
	json_object_object_add(my_object, "tips_flag", json_object_new_boolean(set_lan_flag));
	json_object_object_add(my_object, "ip", json_object_new_string(ip));
	json_object_object_add(my_object, "mask", json_object_new_string(mask));
	json_object_object_add(my_object, "dhcp_switch", json_object_new_string(dhcp_switch));
	json_object_object_add(my_object, "start_ip", json_object_new_string(start_ip));
	json_object_object_add(my_object, "auth_dhcp_max_count", json_object_new_int(limit));
	json_object_object_add(my_object, "auth_leasetime", json_object_new_int(leasetime));
	json_object_object_add(my_object, "lan_quarantine", json_object_new_string(lan_quarantine));
	
	LOG_HL_INFO("JSON -->: %s\n", json_object_to_json_string(my_object));
	evhttpd_send_200_response(req, "application/json;charset=UTF-8", json_object_to_json_string(my_object));
	json_object_put(my_object);

	return ret;
}

int cgi_set_lan(struct evhttp_request *req, const t_http_server *http_server)
{
	int ret = -1;
	
	char *utf8_msg = NULL;

	json_object *my_object = json_object_new_object();
	if (! my_object) return -1;

	char *ip = evhttp_get_post_parm(req, "ip");
	char *mask = evhttp_get_post_parm(req, "mask");
	char *dhcp_switch = evhttp_get_post_parm(req, "dhcp_switch");
	char *start_ip = evhttp_get_post_parm(req, "start_ip");
	char *auth_dhcp_max_count = evhttp_get_post_parm(req, "auth_dhcp_max_count");
	char *auth_leasetime = evhttp_get_post_parm(req, "auth_leasetime");
	char *lan_quarantine = evhttp_get_post_parm(req, "lan_quarantine");

	if ((ip && strlen(ip) > 32)
	|| (mask && strlen(mask) > 32)
	|| (dhcp_switch && strlen(dhcp_switch) > 3)
	|| (start_ip && strlen(start_ip) > 32)
	|| (auth_dhcp_max_count && strlen(auth_dhcp_max_count) > 15)
	|| (auth_leasetime && strlen(auth_leasetime) > 15)
	|| (lan_quarantine && strlen(lan_quarantine) > 5))
	{
		goto done;
	}

	LOG_WARN_INFO(
		"\n"
		"ip: %s\n"
		"mask: %s\n"
		"dhcp_switch: %s\n"
		"start_ip: %s\n"
		"auth_dhcp_max_count: %s\n"
		"auth_leasetime: %s\n"
		"lan_quarantine: %s\n",
		ip, mask, dhcp_switch, start_ip, auth_dhcp_max_count, auth_leasetime, lan_quarantine
	);

	char *res = "error";

	char *msg = "";
	
	if (ip && strlen(ip) > 0 && is_ip_str(ip)
	&& (mask && strlen(mask) > 0 
		&& (msg = "掩码设置错误，请重新设置") 
		&& is_mask_valid(mask) && (msg = ""))
	&& dhcp_switch && strlen(dhcp_switch) > 0 
		&& (strcmp(dhcp_switch, "on") == 0 || strcmp(dhcp_switch, "off") == 0)
	&& lan_quarantine && strlen(lan_quarantine) > 0 
		&& (strcmp(lan_quarantine, "on") == 0 || strcmp(lan_quarantine, "off") == 0)
	&& dhcp_switch && strlen(dhcp_switch) > 0
		&& (strcmp(dhcp_switch, "off") == 0 
			|| (start_ip && strlen(start_ip) > 0 && is_ip_str(start_ip)
				&& (msg = "起始IP与本机IP不在同一网段，请根据掩码重新设置") 
				&& is_same_segment(start_ip, ip, mask) && (msg = "")))
	&& (! auth_dhcp_max_count || (auth_dhcp_max_count && strlen(auth_dhcp_max_count) <= 0) 
		|| (auth_dhcp_max_count && strlen(auth_dhcp_max_count) > 0 && isNumber(auth_dhcp_max_count)))
	&& (! auth_leasetime || (auth_leasetime && strlen(auth_leasetime) <= 0) 
		|| (auth_leasetime && strlen(auth_leasetime) > 0 && isNumber(auth_leasetime))))
	{
		set_network_ipaddr("lan", ip);
		set_network_mask("lan", mask);
		if (dhcp_switch && strcmp(dhcp_switch, "on") == 0)
		{
			set_dhcp_enable("lan");
		}
		else if ((dhcp_switch && strcmp(dhcp_switch, "off") == 0))
		{
			set_dhcp_disable("lan");
		}
		
		if (strcmp(dhcp_switch, "on") == 0)
		{
			unsigned int start = get_sub_ip_int(start_ip, mask);
			if (is_sub_ip_valid(start, mask))
			{
				char start_str[32] = {0};
				snprintf(start_str, 32, "%u", start);
				set_dhcp_start("lan", start_str);
			}

			if (auth_dhcp_max_count && strlen(auth_dhcp_max_count) > 0)
			{
				unsigned int limit = atoi(auth_dhcp_max_count);
				if (0 < limit && limit <= 65536)
				{
					char limit_str[32] = {0};
					snprintf(limit_str, 32, "%u", limit);
					set_dhcp_limit("lan", limit_str);
				}
			}

			if (auth_leasetime && strlen(auth_leasetime) > 0)
			{
				unsigned int leasetime = atoi(auth_leasetime);
				if (0 < leasetime && leasetime <= 24)
				{
					char leasetime_str[32] = {0};
					snprintf(leasetime_str, 32, "%u", leasetime);
					set_dhcp_leasetime("lan", leasetime_str);
				}
			}
		}

		if (lan_quarantine && strcmp(lan_quarantine, "on") == 0)
		{
			set_lan_quarantine_switch(true);
		}
		else if (lan_quarantine && strcmp(lan_quarantine, "off") == 0)
		{
			set_lan_quarantine_switch(false);
		}
		
		uci_commit();
		res = "success";
		set_lan_flag = true;
	}
	
	if (msg && strlen(msg) > 0)
	{	
		utf8_msg = utf8_encode(msg);
	}
	
	json_object_object_add(my_object, "res", json_object_new_string(res));
	json_object_object_add(my_object, "tips_flag", json_object_new_boolean(set_lan_flag));
	json_object_object_add(my_object, "msg", json_object_new_string(utf8_msg ? utf8_msg : ""));
	LOG_HL_INFO("JSON -->: %s\n", json_object_to_json_string(my_object));

	evhttpd_send_200_response(req, "application/json;charset=UTF-8", json_object_to_json_string(my_object));
done:

	json_object_put(my_object);

	free_malloc(ip);
	free_malloc(mask);
	free_malloc(dhcp_switch);
	free_malloc(start_ip);
	free_malloc(auth_dhcp_max_count);
	free_malloc(auth_leasetime);
	free_malloc(lan_quarantine);
	utf8_encode_free(utf8_msg);

	return ret;
}

