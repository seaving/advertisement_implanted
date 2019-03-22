#include "cgi_not_auth.h"

static bool set_not_auth_flag = false;

int get_guest_start_ip(char *ipaddr)
{
	char mask[32] = {0};
	get_network_mask("guest", mask);
	char ip[32] = {0};
	get_network_ipaddr("guest", ip);
	unsigned int segment = get_network_segment_int(ip, mask);
	unsigned int start = get_dhcp_ippool_start("guest");
	unsigned int sub_ip = segment + start;
	int_ip_to_str(sub_ip, ipaddr);
	return 0;
}

int set_not_auth_2g_ssid(char *ssid)
{
	if (ssid && strlen(ssid) > 0)
	{
		char wifi_iface[64] = {0};

		get_guest_2g_wifi_iface(wifi_iface);

		set_wifi_ssid(wifi_iface, ssid);
	}
	
	return 0;
}

int set_not_auth_5g_ssid(char *ssid)
{
	if (ssid && strlen(ssid) > 0)
	{
		char wifi_iface[64] = {0};

		get_guest_5g_wifi_iface(wifi_iface);

		set_wifi_ssid(wifi_iface, ssid);
	}
	
	return 0;
}

int set_not_auth_2g_password(char *password)
{
	if (password && strlen(password) > 0)
	{
		char wifi_iface[64] = {0};

		get_guest_2g_wifi_iface(wifi_iface);

		set_wifi_encryption(wifi_iface, "psk2+ccmp");
		set_wifi_key(wifi_iface, password);
	}
	
	return 0;
}

int set_not_auth_5g_password(char *password)
{
	if (password && strlen(password) > 0)
	{
		char wifi_iface[64] = {0};

		get_guest_5g_wifi_iface(wifi_iface);

		set_wifi_encryption(wifi_iface, "psk2+ccmp");
		set_wifi_key(wifi_iface, password);
	}
	
	return 0;
}

int cgi_get_not_auth_info(struct evhttp_request *req, const t_http_server *http_server)
{
	int ret = -1;
	
	json_object *my_object = json_object_new_object();
	if (! my_object) return -1;

	char *not_auth_switch = get_not_auth_disable() ? "off" : "on";

	char not_auth_2g_ssid[33] = {0};
	get_2g_not_auth_ssid(not_auth_2g_ssid);

	char not_auth_2g_password[33] = {0};
	get_2g_not_auth_password(not_auth_2g_password);

	char not_auth_5g_ssid[33] = {0};
	get_5g_not_auth_ssid(not_auth_5g_ssid);
	
	char not_auth_5g_password[33] = {0};
	get_5g_not_auth_password(not_auth_5g_password);

	char *not_auth_ssid_hidden = get_not_auth_hidden() ? "off" : "on";

	char not_auth_ip[32] = {0};
	get_network_ipaddr("guest", not_auth_ip);

	char not_auth_mask[32] = {0};
	get_network_mask("guest", not_auth_mask);

	char *not_auth_dhcp_switch =  get_dhcp_switch("guest") == 1 ? "on" : "off";

	char start_ip[32] = {0};
	get_guest_start_ip(start_ip);
	int limit = get_dhcp_ippool_limit("guest");
	int leasetime = get_dhcp_leasetime("guest");

	json_object_object_add(my_object, "res", json_object_new_string("success"));
	json_object_object_add(my_object, "msg", json_object_new_string(""));
	json_object_object_add(my_object, "tips_flag", json_object_new_boolean(set_not_auth_flag));
	json_object_object_add(my_object, "not_auth_switch", json_object_new_string(not_auth_switch));
	json_object_object_add(my_object, "not_auth_2g_ssid", json_object_new_string(not_auth_2g_ssid));
	json_object_object_add(my_object, "not_auth_2g_password", json_object_new_string(not_auth_2g_password));
	json_object_object_add(my_object, "not_auth_5g_ssid", json_object_new_string(not_auth_5g_ssid));
	json_object_object_add(my_object, "not_auth_5g_password", json_object_new_string(not_auth_5g_password));
	json_object_object_add(my_object, "not_auth_ssid_hidden", json_object_new_string(not_auth_ssid_hidden));
	json_object_object_add(my_object, "not_auth_ip", json_object_new_string(not_auth_ip));
	json_object_object_add(my_object, "not_auth_mask", json_object_new_string(not_auth_mask));
	json_object_object_add(my_object, "not_auth_dhcp_switch", json_object_new_string(not_auth_dhcp_switch));
	json_object_object_add(my_object, "start_ip", json_object_new_string(start_ip));
	json_object_object_add(my_object, "not_auth_dhcp_max_count", json_object_new_int(limit));
	json_object_object_add(my_object, "not_auth_leasetime", json_object_new_int(leasetime));
	
	LOG_HL_INFO("JSON -->: %s\n", json_object_to_json_string(my_object));
	evhttpd_send_200_response(req, "application/json;charset=UTF-8", json_object_to_json_string(my_object));

	if (my_object) json_object_put(my_object);

	return ret;
}

int cgi_set_not_auth(struct evhttp_request *req, const t_http_server *http_server)
{
	int ret = -1;
	char *res = "error";
	char *msg = "";
	
	char *utf8_msg = NULL;
	
	json_object *my_object = json_object_new_object();
	if (! my_object) return -1;
	
	char *not_auth_switch = evhttp_get_post_parm(req, "not_auth_switch");
	char *not_auth_2g_ssid = evhttp_get_post_parm(req, "not_auth_2g_ssid");
	char *not_auth_2g_password = evhttp_get_post_parm(req, "not_auth_2g_password");
	char *not_auth_5g_ssid = evhttp_get_post_parm(req, "not_auth_5g_ssid");
	char *not_auth_5g_password = evhttp_get_post_parm(req, "not_auth_5g_password");
	char *not_auth_ssid_hidden = evhttp_get_post_parm(req, "not_auth_ssid_hidden");
	char *not_auth_ip = evhttp_get_post_parm(req, "not_auth_ip");
	char *not_auth_mask = evhttp_get_post_parm(req, "not_auth_mask");
	char *not_auth_dhcp_switch = evhttp_get_post_parm(req, "not_auth_dhcp_switch");
	char *start_ip = evhttp_get_post_parm(req, "start_ip");
	char *limit = evhttp_get_post_parm(req, "limit");
	char *leasetime = evhttp_get_post_parm(req, "leasetime");

	
	if ((not_auth_switch && strlen(not_auth_switch) > 3)
	|| (not_auth_2g_ssid && strlen(not_auth_2g_ssid) > 32)
	|| (not_auth_2g_password && strlen(not_auth_2g_password) > 16)
	|| (not_auth_5g_ssid && strlen(not_auth_5g_ssid) > 32)
	|| (not_auth_5g_password && strlen(not_auth_5g_password) > 16)
	|| (not_auth_ssid_hidden && strlen(not_auth_ssid_hidden) > 3)
	|| (not_auth_ip && strlen(not_auth_ip) > 32)
	|| (not_auth_mask && strlen(not_auth_mask) > 32)
	|| (not_auth_dhcp_switch && strlen(not_auth_dhcp_switch) > 3)
	|| (start_ip && strlen(start_ip) > 32)
	|| ((limit && strlen(limit) > 10) || (! isNumber(limit)))
	|| ((leasetime && strlen(leasetime) > 3) || (! isNumber(leasetime))))
	{
		goto done;
	}

	LOG_WARN_INFO(
		"\n"
		"not_auth_switch: %s\n"
		"not_auth_2g_ssid: %s\n"
		"not_auth_2g_password: %s\n"
		"not_auth_5g_ssid: %s\n"
		"not_auth_5g_password: %s\n"
		"not_auth_ssid_hidden: %s\n"
		"not_auth_ip: %s\n"
		"not_auth_mask: %s\n"
		"not_auth_dhcp_switch: %s\n"
		"start_ip: %s\n"
		"limit: %s\n"
		"leasetime: %s\n",
		not_auth_switch, not_auth_2g_ssid, 
		not_auth_2g_password, not_auth_5g_ssid, 
		not_auth_5g_password, not_auth_ssid_hidden,
		not_auth_ip, not_auth_mask, not_auth_dhcp_switch,
		start_ip, limit, leasetime
	);

	if (not_auth_switch && strcmp(not_auth_switch, "off") == 0)
	{
		set_not_auth_disable();
		uci_commit();

		set_not_auth_flag = true;
		res = "success";
	}
	else if (
			not_auth_switch && strcmp(not_auth_switch, "on") == 0
		&&	not_auth_2g_ssid && strlen(not_auth_2g_ssid) <= 32
		&&	(! not_auth_2g_password 
			|| (not_auth_2g_password 
				&& strlen(not_auth_2g_password) <= 16))
		&&	(! not_auth_5g_ssid
			|| (not_auth_5g_ssid 
				&& strlen(not_auth_5g_ssid) <= 32))
		&&	(! not_auth_5g_password
			|| (not_auth_5g_password 
				&& strlen(not_auth_5g_password) <= 16))
		&&	not_auth_ssid_hidden && strlen(not_auth_ssid_hidden) <= 3
		&&	not_auth_ip && strlen(not_auth_ip) <= 32 && is_ip_str(not_auth_ip)
		&&	(not_auth_mask && strlen(not_auth_mask) <= 32 
			&& (msg = "掩码设置错误，请重新设置")
			&& is_mask_valid(not_auth_mask) && (msg = ""))
		&& 	not_auth_dhcp_switch && strlen(not_auth_dhcp_switch) <= 3
		&&	((strcmp(not_auth_dhcp_switch, "off") == 0) 
				|| ((strcmp(not_auth_dhcp_switch, "on") == 0) && (start_ip && strlen(start_ip) <= 32 && is_ip_str(start_ip)
					&& (msg = "起始IP与本机IP不在同一网段，请根据掩码重新设置")
					&& is_same_segment(start_ip, start_ip, not_auth_mask) && (msg = ""))))
		&&	((strcmp(not_auth_dhcp_switch, "off") == 0)
				|| ((strcmp(not_auth_dhcp_switch, "on") == 0) && (! limit || (limit && strlen(limit) <= 10 
					&& isNumber(limit) 
					&& 0 < atoi(limit) && atoi(limit) < 65535))))
		&&	((strcmp(not_auth_dhcp_switch, "off") == 0) 
				|| ((strcmp(not_auth_dhcp_switch, "on") == 0) && (! leasetime || (leasetime && strlen(leasetime) <= 2 
					&& isNumber(limit) 
					&& 0 < atoi(limit) && atoi(limit) < 25)))))
	{
		set_not_auth_enable();

		set_not_auth_2g_ssid(not_auth_2g_ssid);
		set_not_auth_2g_password(not_auth_2g_password);
		set_not_auth_5g_ssid(not_auth_5g_ssid);
		set_not_auth_5g_password(not_auth_5g_password);

		if (strcmp(not_auth_ssid_hidden, "on") == 0)
		{
			set_not_auth_hidden(true);
		}
		else if (strcmp(not_auth_ssid_hidden, "off") == 0)
		{
			set_not_auth_hidden(false);
		}
		
		set_network_ipaddr("guest", not_auth_ip);
		set_network_mask("guest", not_auth_mask);

		if (strcmp(not_auth_dhcp_switch, "on") == 0)
		{
			set_dhcp_enable("guest");
			
			unsigned int start = get_sub_ip_int(start_ip, not_auth_mask);
			if (is_sub_ip_valid(start, not_auth_mask))
			{
				char start_str[32] = {0};
				snprintf(start_str, 32, "%u", start);
				set_dhcp_start("guest", start_str);
			}

			if (limit && strlen(limit) > 0)
			{
				unsigned int limit_cnt = atoi(limit);
				if (0 < limit_cnt && limit_cnt <= 65535)
				{
					char limit_str[32] = {0};
					snprintf(limit_str, 32, "%u", limit_cnt);
					set_dhcp_limit("guest", limit_str);
				}
			}

			if (leasetime && strlen(leasetime) > 0)
			{
				unsigned int leasetime_cnt = atoi(leasetime);
				if (0 < leasetime_cnt && leasetime_cnt <= 24)
				{
					char leasetime_str[32] = {0};
					snprintf(leasetime_str, 32, "%u", leasetime_cnt);
					set_dhcp_leasetime("guest", leasetime_str);
				}
			}
		}
		else if (strcmp(not_auth_dhcp_switch, "off") == 0)
		{
			set_dhcp_disable("guest");
		}
		
		uci_commit();

		set_not_auth_flag = true;
		res = "success";
	}

	if (strlen(msg) > 0) utf8_msg = utf8_encode(msg);
	
	json_object_object_add(my_object, "res", json_object_new_string(res));
	json_object_object_add(my_object, "tips_flag", json_object_new_boolean(set_not_auth_flag));
	json_object_object_add(my_object, "msg", json_object_new_string(utf8_msg));
	LOG_HL_INFO("JSON -->: %s\n", json_object_to_json_string(my_object));

	evhttpd_send_200_response(req, "application/json;charset=UTF-8", json_object_to_json_string(my_object));
done:
	json_object_put(my_object);
	
	free_malloc(not_auth_switch);
	free_malloc(not_auth_2g_ssid);
	free_malloc(not_auth_2g_password);
	free_malloc(not_auth_5g_ssid);
	free_malloc(not_auth_5g_password);
	free_malloc(not_auth_ssid_hidden);
	free_malloc(not_auth_ip);
	free_malloc(not_auth_mask);
	free_malloc(not_auth_dhcp_switch);
	free_malloc(start_ip);
	free_malloc(limit);
	free_malloc(leasetime);

	utf8_encode_free(utf8_msg);

	return ret;
}


