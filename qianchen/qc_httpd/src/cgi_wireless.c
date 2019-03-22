#include "cgi_wireless.h"

static bool set_wlan_2g_flag = false;
static bool set_wlan_5g_flag = false;

json_object* get_wlan_2g_info()
{
	json_object *my_object = json_object_new_object();
	if (! my_object) return NULL;

	char wifi_iface[64] = {0};
	char wifi_device[64] = {0};

	get_2g_wifi_iface(wifi_iface);
	get_2g_wifi_device(wifi_device);
	if (strlen(wifi_iface) <= 0
	|| strlen(wifi_device) <= 0)
	{
		json_object_put(my_object);
		return NULL;
	}

	char *wifi_switch = "on";
	if (get_wifi_disabled(wifi_device) == 1)
	{
		wifi_switch = "off";
	}

	char ssid[SSID_LEN] = {0};
	get_wifi_ssid(wifi_iface, ssid);

	char encryption[33] = {0};
	get_wifi_encryption(wifi_iface, encryption);
	
	char password[33] = {0};
	if (strlen(encryption) > 0 && strcmp(encryption, "none") != 0)
	{
		get_wifi_key(wifi_iface, password);
	}

	char channel[15] = {10};
	get_wifi_channel(wifi_device, channel);
	
	char htmode[15] = {0};
	get_wifi_htmode(wifi_device, htmode);
	
	char auth_2g_signal_min[10] = {0};
	get_signal_min(e_auth_2g_signal_min, auth_2g_signal_min);
	if (strlen(auth_2g_signal_min) <= 0)
	{
		sprintf(auth_2g_signal_min, "%s", "0");
	}

	char *ssid_hidden = "off";
	if (get_wifi_hidden(wifi_iface) == 1)
	{
		ssid_hidden = "on";
	}
	
	json_object_object_add(my_object, "fre", json_object_new_string("2g"));
	json_object_object_add(my_object, "tips_flag", json_object_new_boolean(set_wlan_2g_flag));
	json_object_object_add(my_object, "auth_wifi_2g_switch", json_object_new_string(wifi_switch));
	json_object_object_add(my_object, "auth_2g_ssid", json_object_new_string(ssid));
	json_object_object_add(my_object, "auth_2g_password", json_object_new_string(password));
	json_object_object_add(my_object, "auth_2g_channel", json_object_new_string(channel));
	json_object_object_add(my_object, "auth_2g_bandwidth", json_object_new_string(htmode));
	json_object_object_add(my_object, "auth_2g_signal_min", json_object_new_string(auth_2g_signal_min));
	json_object_object_add(my_object, "auth_2g_ssid_hidden", json_object_new_string(ssid_hidden));

	//LOG_HL_INFO("JSON -->: %s\n", json_object_to_json_string(my_object));

	return my_object;
}

json_object* get_wlan_5g_info()
{
	json_object *my_object = json_object_new_object();
	if (! my_object) return NULL;

	char wifi_iface[64] = {0};
	char wifi_device[64] = {0};

	get_5g_wifi_iface(wifi_iface);
	get_5g_wifi_device(wifi_device);
	if (strlen(wifi_iface) <= 0
	|| strlen(wifi_device) <= 0)
	{
		json_object_put(my_object);
		return NULL;
	}

	char *wifi_switch = "on";
	if (get_wifi_disabled(wifi_device) == 1)
	{
		wifi_switch = "off";
	}

	char ssid[SSID_LEN] = {0};
	get_wifi_ssid(wifi_iface, ssid);

	char encryption[33] = {0};
	get_wifi_encryption(wifi_iface, encryption);
	
	char password[33] = {0};
	if (strlen(encryption) > 0 && strcmp(encryption, "none") != 0)
	{
		get_wifi_key(wifi_iface, password);
	}

	char channel[15] = {0};
	get_wifi_channel(wifi_device, channel);

	char htmode[15] = {0};
	get_wifi_htmode(wifi_device, htmode);
	
	char auth_5g_signal_min[10] = {0};
	get_signal_min(e_auth_5g_signal_min, auth_5g_signal_min);
	if (strlen(auth_5g_signal_min) <= 0)
	{
		sprintf(auth_5g_signal_min, "%s", "0");
	}

	char *ssid_hidden = "off";
	if (get_wifi_hidden(wifi_iface) == 1)
	{
		ssid_hidden = "on";
	}
	
	json_object_object_add(my_object, "fre", json_object_new_string("5g"));
	json_object_object_add(my_object, "tips_flag", json_object_new_boolean(set_wlan_5g_flag));
	json_object_object_add(my_object, "auth_wifi_5g_switch", json_object_new_string(wifi_switch));
	json_object_object_add(my_object, "auth_5g_ssid", json_object_new_string(ssid));
	json_object_object_add(my_object, "auth_5g_password", json_object_new_string(password));
	json_object_object_add(my_object, "auth_5g_channel", json_object_new_string(channel));
	json_object_object_add(my_object, "auth_5g_bandwidth", json_object_new_string(htmode));
	json_object_object_add(my_object, "auth_5g_signal_min", json_object_new_string(auth_5g_signal_min));
	json_object_object_add(my_object, "auth_5g_ssid_hidden", json_object_new_string(ssid_hidden));

	//LOG_HL_INFO("JSON -->: %s\n", json_object_to_json_string(my_object));

	return my_object;
}

int cgi_get_wlan_info(struct evhttp_request *req, const t_http_server *http_server)
{
	int ret = -1;
	
	json_object *my_object = json_object_new_object();
	if (! my_object) return -1;
	json_object *json_array = json_object_new_array();
	if (! json_array)
	{
		json_object_put(my_object);
		return -1;
	}

	json_object *json_object_2g = get_wlan_2g_info();
	if (json_object_2g)
		json_object_array_add(json_array, json_object_2g);

	json_object *json_object_5g = get_wlan_5g_info();
	if (json_object_5g)
		json_object_array_add(json_array, json_object_5g);

	json_object_object_add(my_object, "res", json_object_new_string("success"));
	json_object_object_add(my_object, "msg", json_object_new_string(""));
	json_object_object_add(my_object, "data", json_array);
	
	LOG_HL_INFO("JSON -->: %s\n", json_object_to_json_string(my_object));
	evhttpd_send_200_response(req, "application/json;charset=UTF-8", json_object_to_json_string(my_object));

	if (json_object_2g) json_object_put(json_object_2g);
	if (json_object_5g) json_object_put(json_object_5g);
	if (json_array) json_object_put(json_array);
	if (my_object) json_object_put(my_object);

	return ret;
}

int set_wlan_2g(struct evhttp_request *req, const t_http_server *http_server)
{
	char *res = "error";
		
	json_object *my_object = json_object_new_object();
	if (! my_object) return -1;

	char wifi_iface[64] = {0};
	char wifi_device[64] = {0};

	get_2g_wifi_iface(wifi_iface);
	get_2g_wifi_device(wifi_device);
	if (strlen(wifi_iface) <= 0
	|| strlen(wifi_device) <= 0)
	{
		char *utf8_msg = utf8_encode("该设备不支持2G无线功能!");
		json_object_object_add(my_object, "res", json_object_new_string(res));
		json_object_object_add(my_object, "msg", json_object_new_string(utf8_msg));
		LOG_HL_INFO("JSON -->: %s\n", json_object_to_json_string(my_object));
		
		evhttpd_send_200_response(req, "application/json;charset=UTF-8", json_object_to_json_string(my_object));
		json_object_put(my_object);
		free_malloc(utf8_msg);

		return -1;
	}

	char *auth_wifi_2g_switch = evhttp_get_post_parm(req, "auth_wifi_2g_switch");
	char *auth_2g_ssid = evhttp_get_post_parm(req, "auth_2g_ssid");
	char *auth_2g_password = evhttp_get_post_parm(req, "auth_2g_password");
	char *auth_2g_channel = evhttp_get_post_parm(req, "auth_2g_channel");
	char *auth_2g_bandwidth = evhttp_get_post_parm(req, "auth_2g_bandwidth");
	char *auth_2g_signal_min = evhttp_get_post_parm(req, "auth_2g_signal_min");
	char *auth_2g_ssid_hidden = evhttp_get_post_parm(req, "auth_2g_ssid_hidden");

	if ((auth_wifi_2g_switch && strlen(auth_wifi_2g_switch) > 3)
	|| (auth_2g_ssid && strlen(auth_2g_ssid) > 32)
	|| (auth_2g_password && strlen(auth_2g_password) > 16)
	|| (auth_2g_channel && strlen(auth_2g_channel) > 4)
	|| (auth_2g_bandwidth && strlen(auth_2g_bandwidth) > 5)
	|| (auth_2g_signal_min && strlen(auth_2g_signal_min) > 5)
	|| (auth_2g_ssid_hidden && strlen(auth_2g_ssid_hidden) > 3))
	{
		goto done;
	}

	LOG_WARN_INFO(
		"\n"
		"auth_wifi_2g_switch: %s\n"
		"auth_2g_ssid: %s\n"
		"auth_2g_password: %s\n"
		"auth_2g_bandwidth: %s\n"
		"auth_2g_signal_min: %s\n"
		"auth_2g_ssid_hidden: %s\n"
		"auth_2g_channel: %s\n",
		auth_wifi_2g_switch, auth_2g_ssid, 
		auth_2g_password, auth_2g_bandwidth, 
		auth_2g_signal_min, auth_2g_ssid_hidden,
		auth_2g_channel
	);
	
	if (auth_2g_ssid && strlen(auth_2g_ssid) > 0 && strlen(auth_2g_ssid) < 33
	&& auth_wifi_2g_switch && strlen(auth_wifi_2g_switch) > 0 
		&& (strcmp(auth_wifi_2g_switch, "on") == 0 || strcmp(auth_wifi_2g_switch, "off") == 0)
	&& auth_2g_ssid_hidden && strlen(auth_2g_ssid_hidden) > 0 
		&& (strcmp(auth_2g_ssid_hidden, "on") == 0 || strcmp(auth_2g_ssid_hidden, "off") == 0)
	&& auth_2g_bandwidth && strlen(auth_2g_bandwidth) > 0 
		&& (strcmp(auth_2g_bandwidth, "HT20") == 0 || strcmp(auth_2g_bandwidth, "HT40") == 0)
	&& auth_2g_channel && strlen(auth_2g_channel) > 0 
		&& (strcmp(auth_2g_channel, "auto") == 0 
			|| (isNumber(auth_2g_channel) && 1<= atoi(auth_2g_channel) && atoi(auth_2g_channel) <= 13))
	&& (auth_2g_signal_min && strlen(auth_2g_signal_min) > 0 && strlen(auth_2g_signal_min) < 5 
		&& (strcmp(auth_2g_signal_min, "0") == 0
		|| strcmp(auth_2g_signal_min, "-100") == 0
		|| strcmp(auth_2g_signal_min, "-90") == 0
		|| strcmp(auth_2g_signal_min, "-85") == 0
		|| strcmp(auth_2g_signal_min, "-80") == 0
		|| strcmp(auth_2g_signal_min, "-75") == 0
		|| strcmp(auth_2g_signal_min, "-70") == 0
		|| strcmp(auth_2g_signal_min, "-65") == 0
		|| strcmp(auth_2g_signal_min, "-60") == 0))
	&& (! auth_2g_password || (auth_2g_password && strlen(auth_2g_password) <= 0) 
		|| (auth_2g_password && strlen(auth_2g_password) > 0 && strlen(auth_2g_password) < 17)))
	{
		if (strcmp(auth_wifi_2g_switch, "on") == 0)
		{
			set_wifi_disabled(wifi_device, "0");
		}
		else if (strcmp(auth_wifi_2g_switch, "off") == 0)
		{
			set_wifi_disabled(wifi_device, "1");
		}
		
		set_wifi_ssid(wifi_iface, auth_2g_ssid);

		if (strlen(auth_2g_password) > 0)
		{
			set_wifi_encryption(wifi_iface, "psk2+ccmp");
			set_wifi_key(wifi_iface, auth_2g_password);
		}
		else
		{
			set_wifi_encryption(wifi_iface, "none");
			del_wifi_section(wifi_iface, "key");
		}

		set_wifi_channel(wifi_device, auth_2g_channel);
		
		set_wifi_htmode(wifi_device, auth_2g_bandwidth);

		if (strcmp(auth_2g_ssid_hidden, "on") == 0)
		{
			set_wifi_hidden(wifi_iface, "1");
		}
		else if (strcmp(auth_2g_ssid_hidden, "off") == 0)
		{
			set_wifi_hidden(wifi_iface, "0");
		}

		set_signal_min(e_auth_2g_signal_min, auth_2g_signal_min);
		
		uci_commit();
		set_wlan_2g_flag = true;
		res = "success";
	}

	json_object_object_add(my_object, "res", json_object_new_string(res));
	json_object_object_add(my_object, "tips_flag", json_object_new_boolean(set_wlan_2g_flag));
	json_object_object_add(my_object, "msg", json_object_new_string(""));
	LOG_HL_INFO("JSON -->: %s\n", json_object_to_json_string(my_object));

	evhttpd_send_200_response(req, "application/json;charset=UTF-8", json_object_to_json_string(my_object));
done:
	json_object_put(my_object);

	free_malloc(auth_wifi_2g_switch);
	free_malloc(auth_2g_ssid);
	free_malloc(auth_2g_password);
	free_malloc(auth_2g_channel);
	free_malloc(auth_2g_bandwidth);
	free_malloc(auth_2g_signal_min);
	free_malloc(auth_2g_ssid_hidden);

	return 0;
}

int set_wlan_5g(struct evhttp_request *req, const t_http_server *http_server)
{
	char *res = "error";
		
	json_object *my_object = json_object_new_object();
	if (! my_object) return -1;

	char wifi_iface[64] = {0};
	char wifi_device[64] = {0};

	get_5g_wifi_iface(wifi_iface);
	get_5g_wifi_device(wifi_device);
	if (strlen(wifi_iface) <= 0
	|| strlen(wifi_device) <= 0)
	{
		char *utf8_msg = utf8_encode("该设备不支持5G无线功能!");
		json_object_object_add(my_object, "res", json_object_new_string(res));
		json_object_object_add(my_object, "msg", json_object_new_string(utf8_msg));
		LOG_HL_INFO("JSON -->: %s\n", json_object_to_json_string(my_object));
		
		evhttpd_send_200_response(req, "application/json;charset=UTF-8", json_object_to_json_string(my_object));
		json_object_put(my_object);
		free_malloc(utf8_msg);
		
		return -1;
	}

	char *auth_wifi_5g_switch = evhttp_get_post_parm(req, "auth_wifi_5g_switch");
	char *auth_5g_ssid = evhttp_get_post_parm(req, "auth_5g_ssid");
	char *auth_5g_password = evhttp_get_post_parm(req, "auth_5g_password");
	char *auth_5g_channel = evhttp_get_post_parm(req, "auth_5g_channel");
	char *auth_5g_bandwidth = evhttp_get_post_parm(req, "auth_5g_bandwidth");
	char *auth_5g_signal_min = evhttp_get_post_parm(req, "auth_5g_signal_min");
	char *auth_5g_ssid_hidden = evhttp_get_post_parm(req, "auth_5g_ssid_hidden");
	
	if ((auth_wifi_5g_switch && strlen(auth_wifi_5g_switch) > 3)
	|| (auth_5g_ssid && strlen(auth_5g_ssid) > 32)
	|| (auth_5g_password && strlen(auth_5g_password) > 16)
	|| (auth_5g_channel && strlen(auth_5g_channel) > 4)
	|| (auth_5g_bandwidth && strlen(auth_5g_bandwidth) > 5)
	|| (auth_5g_signal_min && strlen(auth_5g_signal_min) > 5)
	|| (auth_5g_ssid_hidden && strlen(auth_5g_ssid_hidden) > 3))
	{
		goto done;
	}

	LOG_WARN_INFO(
		"\n"
		"auth_wifi_5g_switch: %s\n"
		"auth_5g_ssid: %s\n"
		"auth_5g_password: %s\n"
		"auth_5g_bandwidth: %s\n"
		"auth_5g_signal_min: %s\n"
		"auth_5g_ssid_hidden: %s\n"
		"auth_5g_channel: %s\n",
		auth_wifi_5g_switch, auth_5g_ssid, 
		auth_5g_password, auth_5g_bandwidth, 
		auth_5g_signal_min, auth_5g_ssid_hidden,
		auth_5g_channel
	);
	
	if (auth_5g_ssid && strlen(auth_5g_ssid) > 0 && strlen(auth_5g_ssid) < 33
	&& auth_wifi_5g_switch && strlen(auth_wifi_5g_switch) > 0 
		&& (strcmp(auth_wifi_5g_switch, "on") == 0 || strcmp(auth_wifi_5g_switch, "off") == 0)
	&& auth_5g_ssid_hidden && strlen(auth_5g_ssid_hidden) > 0 
		&& (strcmp(auth_5g_ssid_hidden, "on") == 0 || strcmp(auth_5g_ssid_hidden, "off") == 0)
	&& auth_5g_bandwidth && strlen(auth_5g_bandwidth) > 0 
		&& (strcmp(auth_5g_bandwidth, "HT20") == 0 
			|| strcmp(auth_5g_bandwidth, "HT40") == 0
			|| strcmp(auth_5g_bandwidth, "VHT80") == 0)
	&& auth_5g_channel && strlen(auth_5g_channel) > 0 
		&& (strcmp(auth_5g_channel, "auto") == 0 
			|| (isNumber(auth_5g_channel) && 
				(atoi(auth_5g_channel) == 36
				|| atoi(auth_5g_channel) == 40
				|| atoi(auth_5g_channel) == 44
				|| atoi(auth_5g_channel) == 48
				|| atoi(auth_5g_channel) == 52
				|| atoi(auth_5g_channel) == 56
				|| atoi(auth_5g_channel) == 60
				|| atoi(auth_5g_channel) == 64
				|| atoi(auth_5g_channel) == 149
				|| atoi(auth_5g_channel) == 153
				|| atoi(auth_5g_channel) == 157
				|| atoi(auth_5g_channel) == 161
				|| atoi(auth_5g_channel) == 165)))
	&& (auth_5g_signal_min && strlen(auth_5g_signal_min) > 0 && strlen(auth_5g_signal_min) < 5 
		&& (strcmp(auth_5g_signal_min, "0") == 0
		|| strcmp(auth_5g_signal_min, "-100") == 0
		|| strcmp(auth_5g_signal_min, "-90") == 0
		|| strcmp(auth_5g_signal_min, "-85") == 0
		|| strcmp(auth_5g_signal_min, "-80") == 0
		|| strcmp(auth_5g_signal_min, "-75") == 0
		|| strcmp(auth_5g_signal_min, "-70") == 0
		|| strcmp(auth_5g_signal_min, "-65") == 0
		|| strcmp(auth_5g_signal_min, "-60") == 0))
	&& (! auth_5g_password || (auth_5g_password && strlen(auth_5g_password) <= 0) 
		|| (auth_5g_password && strlen(auth_5g_password) > 0 && strlen(auth_5g_password) < 17)))
	{
		if (strcmp(auth_wifi_5g_switch, "on") == 0)
		{
			set_wifi_disabled(wifi_device, "0");
		}
		else if (strcmp(auth_wifi_5g_switch, "off") == 0)
		{
			set_wifi_disabled(wifi_device, "1");
		}
		
		set_wifi_ssid(wifi_iface, auth_5g_ssid);

		if (strlen(auth_5g_password) > 0)
		{
			set_wifi_encryption(wifi_iface, "psk2+ccmp");
			set_wifi_key(wifi_iface, auth_5g_password);
		}
		else
		{
			set_wifi_encryption(wifi_iface, "none");
			del_wifi_section(wifi_iface, "key");
		}

		set_wifi_channel(wifi_device, auth_5g_channel);
		
		set_wifi_htmode(wifi_device, auth_5g_bandwidth);

		if (strcmp(auth_5g_ssid_hidden, "on") == 0)
		{
			set_wifi_hidden(wifi_iface, "1");
		}
		else if (strcmp(auth_5g_ssid_hidden, "off") == 0)
		{
			set_wifi_hidden(wifi_iface, "0");
		}

		set_signal_min(e_auth_2g_signal_min, auth_5g_signal_min);
		
		uci_commit();
		set_wlan_5g_flag = true;
		res = "success";
	}

	json_object_object_add(my_object, "res", json_object_new_string(res));
	json_object_object_add(my_object, "tips_flag", json_object_new_boolean(set_wlan_5g_flag));
	json_object_object_add(my_object, "msg", json_object_new_string(""));
	LOG_HL_INFO("JSON -->: %s\n", json_object_to_json_string(my_object));

	evhttpd_send_200_response(req, "application/json;charset=UTF-8", json_object_to_json_string(my_object));
done:
	json_object_put(my_object);

	free_malloc(auth_wifi_5g_switch);
	free_malloc(auth_5g_ssid);
	free_malloc(auth_5g_password);
	free_malloc(auth_5g_channel);
	free_malloc(auth_5g_bandwidth);
	free_malloc(auth_5g_signal_min);
	free_malloc(auth_5g_ssid_hidden);
	
	return -1;
}

int cgi_set_wlan(struct evhttp_request *req, const t_http_server *http_server)
{
	char *fre = evhttp_get_post_parm(req, "fre");
	if (! fre)
	{
		return -1;
	}
	
	if (strcmp(fre, "2g") == 0)
	{
		set_wlan_2g(req, http_server);
	}
	else if (strcmp(fre, "5g") == 0)
	{
		set_wlan_5g(req, http_server);
	}

	free_malloc(fre);

	return 0;
}



