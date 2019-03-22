#include "includes.h"

static bool set_wan_flag = false;

int cgi_get_wan_protocol(struct evhttp_request *req, const t_http_server *http_server)
{
	int ret = -1;
	
	json_object *my_object = json_object_new_object();
	if (! my_object) return -1;

	char proto[15] = {0};
	get_wan_protocol(proto);
	
	json_object_object_add(my_object, "res", json_object_new_string("success"));
	json_object_object_add(my_object, "wan_protocol", json_object_new_string(proto));
	
	LOG_HL_INFO("JSON -->: %s\n", json_object_to_json_string(my_object));

	evhttpd_send_200_response(req, "application/json;charset=UTF-8", json_object_to_json_string(my_object));
	json_object_put(my_object);

	return ret;	
}

int get_wan_dns_by_uci(char *dns_first, char *dns_second)
{
	char dns_list[65] = {0};
	get_network_dns("wan", dns_list);

	int i = 0, j = 0, k = 0;
	for (i = 0; i < strlen(dns_list); i ++)
	{
		if (dns_list[i] != ' ')
		{
			break;
		}
	}

	for ( ; i < strlen(dns_list); i ++)
	{
		if (dns_list[i] == ' ')
		{
			break;
		}
		
		dns_first[j ++] = dns_list[i];
	}

	for ( ; i < strlen(dns_list); i ++)
	{
		if (dns_list[i] != ' ')
		{
			break;
		}
	}

	for ( ; i < strlen(dns_list); i ++)
	{
		if (dns_list[i] == ' ')
		{
			break;
		}
		
		dns_second[k ++] = dns_list[i];
	}

	return 0;
}

int cgi_get_wan_info(struct evhttp_request *req, const t_http_server *http_server)
{
	int ret = -1;
	
	json_object *my_object = json_object_new_object();
	if (! my_object) return -1;

	char proto[15] = {0};
	get_wan_protocol(proto);

	int mtu = get_network_mtu("wan");

	char dns_first[32] = {0};
	char dns_second[32] = {0};
	get_wan_dns_by_uci(dns_first, dns_second);

	char wan_ip[32] = {0};
	get_network_ipaddr("wan", wan_ip);

	char wan_mask[32] = {0};
	get_network_mask("wan", wan_mask);

	char wan_gw[32] = {0};
	get_wan_gateway(wan_gw);

	char username[64] = {0};
	get_wan_username(username);

	char password[64] = {0};
	get_wan_password(password);
	
	
	json_object_object_add(my_object, "res", json_object_new_string("success"));
	json_object_object_add(my_object, "tips_flag", json_object_new_boolean(set_wan_flag));
	json_object_object_add(my_object, "wan_protocol", json_object_new_string(proto));
	if (mtu <= 0)
		json_object_object_add(my_object, "mtu", json_object_new_string(""));
	else
		json_object_object_add(my_object, "mtu", json_object_new_int(mtu));
	json_object_object_add(my_object, "dns_first", json_object_new_string(dns_first));
	json_object_object_add(my_object, "dns_second", json_object_new_string(dns_second));
	json_object_object_add(my_object, "wan_ip", json_object_new_string(wan_ip));
	json_object_object_add(my_object, "wan_mask", json_object_new_string(wan_mask));
	json_object_object_add(my_object, "wan_gw", json_object_new_string(wan_gw));
	json_object_object_add(my_object, "username", json_object_new_string(username));
	json_object_object_add(my_object, "password", json_object_new_string(password));
	
	LOG_HL_INFO("JSON -->: %s\n", json_object_to_json_string(my_object));

	evhttpd_send_200_response(req, "application/json;charset=UTF-8", json_object_to_json_string(my_object));
	json_object_put(my_object);

	return ret;
}

bool set_wan_dns(char *dns_first, char *dns_second)
{
	if (dns_first)
	{
		if (strlen(dns_first) > 0)
		{
			if (! is_ip_str(dns_first))
				return false;
		}
	}
	
	if (dns_second)
	{
		if (strlen(dns_second) > 0)
		{
			if (! is_ip_str(dns_second))
				return false;
		}
	}

	if ((dns_first == NULL || strlen(dns_first) <= 0)
	&& (dns_second == NULL || strlen(dns_second) <= 0))
	{
		del_network_section("wan", "dns");
		return true;
	}

	char dns_list[65] = {0};
	snprintf(dns_list, 65, "\"%s %s\"", dns_first, dns_second);
	
	set_network_dns("wan", dns_list);

	return true;
}

bool set_wan_mtu(char *strmtu)
{
	if (! isNumber(strmtu))
	{
		return false;
	}
	
	int mtu = -1;

	mtu = atoi(strmtu);
	
	if (mtu > 0)
	{
		if (1 <= mtu && mtu <= 1500)
		{
			char strmtu[10] = {0};
			set_network_mtu("wan", strmtu);
			return true;
		}

		return false;
	}

	del_network_section("wan", "mtu");
	return true;
}

int clear_section()
{
	del_network_section("wan", "ipaddr");
	del_network_section("wan", "netmask");
	del_network_section("wan", "gateway");
	del_network_section("wan", "dns");
	del_network_section("wan", "username");
	del_network_section("wan", "password");
	del_network_section("wan", "mtu");

	return 0;
}

int cgi_set_wan(struct evhttp_request *req, const t_http_server *http_server)
{
	int ret = -1;
	
	json_object *my_object = json_object_new_object();
	if (! my_object) return -1;

	char *protocol = evhttp_get_post_parm(req, "protocol");
	char *ip = evhttp_get_post_parm(req, "ip");
	char *mask = evhttp_get_post_parm(req, "mask");
	char *gateway = evhttp_get_post_parm(req, "gateway");
	char *dns_first = evhttp_get_post_parm(req, "dns_first");
	char *dns_second = evhttp_get_post_parm(req, "dns_second");
	char *mtu = evhttp_get_post_parm(req, "mtu");
	char *username = evhttp_get_post_parm(req, "username");
	char *password = evhttp_get_post_parm(req, "password");

	if ((protocol && strlen(protocol) > 10)
	|| (ip && strlen(ip) > 32)
	|| (mask && strlen(mask) > 32)
	|| (gateway && strlen(gateway) > 32)
	|| (dns_first && strlen(dns_first) > 32)
	|| (mtu && strlen(mtu) > 5)
	|| (username && strlen(username) > 32)
	|| (password && strlen(password) > 32))
	{
		goto done;
	}
	
	LOG_WARN_INFO(
		"\nprotocol: %s\n"
		"ip: %s\n"
		"mask: %s\n"
		"gateway: %s\n"
		"dns_first: %s\n"
		"dns_second: %s\n"
		"mtu: %s\n"
		"username: %s\n"
		"password: %s\n",
		protocol, ip, mask, gateway, dns_first, dns_second, mtu, username, password
	);

	char *res = "error";
	if (strcmp(protocol, "dhcp") == 0)
	{
		if ((! dns_first || (dns_first && strlen(dns_first) > 0 && is_ip_str(dns_first))
			|| (dns_first && strlen(dns_first) <= 0))
		&& (! dns_second || (dns_second && strlen(dns_second) > 0 && is_ip_str(dns_second))
			|| (dns_second && strlen(dns_second) <= 0))
		&& (! mtu || (mtu && strlen(mtu) <= 0) 
			|| (mtu && strlen(mtu) > 0 && isNumber(mtu) && (1 <= atoi(mtu) && atoi(mtu) <= 1500))))
		{
			clear_section();
			uci_commit();
			set_wan_dns(dns_first,dns_second);
			set_wan_mtu(mtu);
			set_network_proto("wan", protocol);
			uci_commit();
			res = "success";
			set_wan_flag = true;
		}
	}
	else if ((strcmp(protocol, "static") == 0))
	{
		if (ip && strlen(ip) > 0 && is_ip_str(ip)
		&& mask && strlen(mask) > 0 && is_mask_valid(mask)
		&& gateway && strlen(gateway) > 0 && is_ip_str(gateway)
		&& dns_first && strlen(dns_first) > 0 && is_ip_str(dns_first)
		&& (! dns_second || (dns_second && strlen(dns_second) > 0 && is_ip_str(dns_second))
			|| (dns_second && strlen(dns_second) <= 0))
		&& (! mtu || (mtu && strlen(mtu) <= 0) 
			|| (mtu && strlen(mtu) > 0 && isNumber(mtu) && (1 <= atoi(mtu) && atoi(mtu) <= 1500))))
		{
			clear_section();
			uci_commit();
			set_network_ipaddr("wan", ip);
			set_network_mask("wan", mask);
			set_network_gateway("wan", gateway);
			set_wan_dns(dns_first, dns_second);
			set_wan_mtu(mtu);
			set_network_proto("wan", protocol);
			uci_commit();
			res = "success";
			set_wan_flag = true;
		}
	}
	else if (strcmp(protocol, "pppoe") == 0)
	{
		if (username && (0 < strlen(username) && strlen(username) <= 32)
		&& password && (0 < strlen(password) && strlen(password) <= 32)
		&& (! dns_first || (dns_first && strlen(dns_first) > 0 && is_ip_str(dns_first))
			|| (dns_first && strlen(dns_first) <= 0))
		&& (! dns_second || (dns_second && strlen(dns_second) > 0 && is_ip_str(dns_second))
			|| (dns_second && strlen(dns_second) <= 0))
		&& (! mtu || (mtu && strlen(mtu) <= 0) 
			|| (mtu && strlen(mtu) > 0 && isNumber(mtu) && (1 <= atoi(mtu) && atoi(mtu) <= 1500))))
		{
			clear_section();
			uci_commit();

			set_wan_username(username);
			set_wan_password(password);
			set_wan_dns(dns_first, dns_second);
			set_wan_mtu(mtu);
			set_network_proto("wan", protocol);
			uci_commit();
			res = "success";
			set_wan_flag = true;
		}
	}

	json_object_object_add(my_object, "res", json_object_new_string(res));
	json_object_object_add(my_object, "tips_flag", json_object_new_boolean(set_wan_flag));
	
	LOG_HL_INFO("JSON -->: %s\n", json_object_to_json_string(my_object));

	evhttpd_send_200_response(req, "application/json;charset=UTF-8", json_object_to_json_string(my_object));
done:
	json_object_put(my_object);

	free_malloc(protocol);
	free_malloc(ip);
	free_malloc(mask);
	free_malloc(gateway);
	free_malloc(dns_first);
	free_malloc(dns_second);
	free_malloc(mtu);
	free_malloc(username);
	free_malloc(password);

	return ret;	
}

