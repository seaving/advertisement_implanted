
#include "cgi_client_manager.h"


json_object* get_client_list_info_json(char *br_dev, int page, int limit)
{
	json_object *json_array = json_object_new_array();
	if (! json_array)
	{
		return NULL;
	}

	int len = 0, i = 0;
	station_info_t *station_info = get_client_list_info(br_dev, page, limit, &len);
	if (! station_info)
	{
		json_object_put(json_array);
		return NULL;
	}
	
	for (i = 0; i < len; i ++)
	{		
		json_object *my_object = json_object_new_object();
		if (! my_object) break;
		
		station_info_t *tmp = station_info + i;

		json_object_object_add(my_object, "name", json_object_new_string(tmp->name));		
		json_object_object_add(my_object, "mac", json_object_new_string(tmp->mac));
		json_object_object_add(my_object, "ip", json_object_new_string(tmp->ip));
		char traffic_str[33] = {0};
		snprintf(traffic_str, 32, "%llu", (tmp->rx + tmp->tx) / (1024 * 1024));
		json_object_object_add(my_object, "traffic", json_object_new_string(traffic_str));
		json_object_object_add(my_object, "auth_state", json_object_new_int(tmp->auth_status));
		json_object_object_add(my_object, "signal", json_object_new_int(tmp->signal));
		json_object_object_add(my_object, "black", json_object_new_boolean(tmp->black));
		json_object_array_add(json_array, my_object);
	}
	
	free (station_info);
	
	return json_array;
}

int cgi_get_client_list(struct evhttp_request *req, const t_http_server *http_server)
{
	int ret = -1;
	
	json_object *my_object = json_object_new_object();
	if (! my_object) return -1;
	json_object *json_array = NULL;

	char *br_dev = evhttp_get_post_parm(req, "br_dev");
	char *page = evhttp_get_post_parm(req, "page");
	char *limit = evhttp_get_post_parm(req, "limit");
	LOG_WARN_INFO(
		"\nbr_dev: %s\n"
		"page: %s\n"
		"limit: %s\n",
		br_dev, page, limit
	);

	if (! br_dev || ! page || ! limit 
	|| (strcmp(br_dev, "br-lan") && strcmp(br_dev, "br-guest"))
	|| ! isNumber(page) || ! isNumber(limit))
	{
		goto done;
	}

	int count = get_online_client_cnt(br_dev);

	json_array = get_client_list_info_json(br_dev, atoi(page), atoi(limit));
	if (! json_array)
	{
		goto done;
	}

	json_object_object_add(my_object, "code", json_object_new_string("0"));
	json_object_object_add(my_object, "msg", json_object_new_string(""));
	json_object_object_add(my_object, "count", json_object_new_int(count));
	json_object_object_add(my_object, "data", json_array);

	LOG_HL_INFO("JSON -->: %s\n", json_object_to_json_string(my_object));
	evhttpd_send_200_response(req, "application/json;charset=UTF-8", json_object_to_json_string(my_object));

done:
	if (json_array) json_object_put(json_array);
	json_object_put(my_object);

	free_malloc(br_dev);
	free_malloc(page);
	free_malloc(limit);

	return ret;
}

int cgi_disconnect_client(struct evhttp_request *req, const t_http_server *http_server)
{
	int ret = -1;
	
	json_object *my_object = json_object_new_object();
	if (! my_object) return -1;

	char *mac = evhttp_get_post_parm(req, "mac");
	LOG_WARN_INFO(
		"\nmac: %s\n",
		mac
	);

	if (! mac || strlen(mac) <= 0 || strlen(mac) > 20)
	{
		goto done;
	}

	char *result = "success";
	
	disconnect_wifi_client(mac);

	json_object_object_add(my_object, "res", json_object_new_string(result));
	json_object_object_add(my_object, "msg", json_object_new_string(""));

	LOG_HL_INFO("JSON -->: %s\n", json_object_to_json_string(my_object));
	evhttpd_send_200_response(req, "application/json;charset=UTF-8", json_object_to_json_string(my_object));

done:
	json_object_put(my_object);

	free_malloc((mac));
	return ret;
}

int cgi_set_black_client(struct evhttp_request *req, const t_http_server *http_server)
{
	int ret = -1;
	
	json_object *my_object = json_object_new_object();
	if (! my_object) return -1;
	char *name = evhttp_get_post_parm(req, "name");
	char *mac = evhttp_get_post_parm(req, "mac");
	LOG_WARN_INFO(
		"\nname: %s\n"
		"mac: %s\n",
		name, mac
	);

	if (! name || strlen(name) <= 0)
	{
		free_malloc(name);
		
		name = calloc(1, 2);
		if (! name)
		{
			goto done;
		}
	}

	if ((name && strlen(name) > 64)
	|| ! mac || strlen(mac) <= 0 || strlen(mac) > 20)
	{
		goto done;
	}

	char *result = "success";
	
	disconnect_wifi_client(mac);
	//写入黑名单文件
	add_black_name(name, mac);
	
	json_object_object_add(my_object, "res", json_object_new_string(result));
	json_object_object_add(my_object, "msg", json_object_new_string(""));

	LOG_HL_INFO("JSON -->: %s\n", json_object_to_json_string(my_object));
	evhttpd_send_200_response(req, "application/json;charset=UTF-8", json_object_to_json_string(my_object));

done:
	json_object_put(my_object);
	
	free_malloc(name);
	free_malloc(mac);
	
	return ret;
}

int cgi_get_black_list(struct evhttp_request *req, const t_http_server *http_server)
{
	int ret = -1;
	
	json_object *my_object = json_object_new_object();
	if (! my_object) return -1;
	json_object *json_array = NULL;

	char *page = evhttp_get_post_parm(req, "page");
	char *limit = evhttp_get_post_parm(req, "limit");
	LOG_WARN_INFO(
		"\n"
		"page: %s\n"
		"limit: %s\n",
		page, limit
	);

	if (! page || ! limit 
	|| ! isNumber(page) || ! isNumber(limit))
	{
		goto done;
	}
	
	int count = get_black_name_total();

	json_array = get_black_name_list(atoi(page), atoi(limit));
	if (! json_array)
	{
		goto done;
	}

	json_object_object_add(my_object, "code", json_object_new_string("0"));
	json_object_object_add(my_object, "msg", json_object_new_string(""));
	json_object_object_add(my_object, "count", json_object_new_int(count));
	json_object_object_add(my_object, "data", json_array);	
	
	LOG_HL_INFO("JSON -->: %s\n", json_object_to_json_string(my_object));
	evhttpd_send_200_response(req, "application/json;charset=UTF-8", json_object_to_json_string(my_object));

done:
	if (json_array) json_object_put(json_array);
	if (my_object) json_object_put(my_object);

	free_malloc(page);
	free_malloc(limit);
	
	return ret;
}

int cgi_delete_black_name(struct evhttp_request *req, const t_http_server *http_server)
{
	int ret = -1;
	
	json_object *my_object = json_object_new_object();
	if (! my_object) return -1;

	char *mac = evhttp_get_post_parm(req, "mac");
	LOG_WARN_INFO(
		"\nmac: %s\n",
		mac
	);

	if (! mac || strlen(mac) <= 0 || strlen(mac) > 20)
	{
		goto done;
	}

	char *result = "success";
	
	//恢复防火墙
	
	//从文件中删除
	delete_black_name(mac);
	
	json_object_object_add(my_object, "res", json_object_new_string(result));
	json_object_object_add(my_object, "msg", json_object_new_string(""));

	LOG_HL_INFO("JSON -->: %s\n", json_object_to_json_string(my_object));
	evhttpd_send_200_response(req, "application/json;charset=UTF-8", json_object_to_json_string(my_object));

done:
	json_object_put(my_object);

	free_malloc(mac);
	return ret;
}

