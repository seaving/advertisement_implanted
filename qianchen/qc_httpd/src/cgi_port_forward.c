#include "cgi_port_forward.h"

json_object* get_port_forward_list(int page, int limit)
{
	json_object *json_array = json_object_new_array();
	if (! json_array)
	{
		return NULL;
	}

	int cnt = get_port_forward_total();
	limit = cnt > limit ?  limit : cnt;
	if (limit <= 0)
	{
		return NULL;
	}

	int i = 0, k = 0;
	for (i = 0; i < limit; i ++)
	{
		json_object *my_object = json_object_new_object();
		if (! my_object) break;
		
		char port_forward[255] = {0};
		get_port_forward((page - 1) * limit + i + 1, port_forward);
		if (strlen(port_forward) <= 0)
		{
			break;
		}

		char *name = NULL;
		char *src_port = NULL;
		char *dst_to = NULL;
		name = port_forward;
		int len = strlen(port_forward);
		for (k = 0; k < len; k ++)
		{
			if (port_forward[k] == ' ')
			{
				port_forward[k ++] = 0;
				break;
			}
		}

		src_port = k > len ? "" : &port_forward[k];
		for ( ; k < len; k ++)
		{
			if (port_forward[k] == ' ')
			{
				port_forward[k ++] = 0;
				break;
			}
		}

		dst_to = k > len ? "" : &port_forward[k];
		for ( ; k < len; k ++)
		{
			if (port_forward[k] == ' ')
			{
				port_forward[k ++] = 0;
				break;
			}
		}
		
		json_object_object_add(my_object, "name", json_object_new_string(name));		
		json_object_object_add(my_object, "src_port", json_object_new_string(src_port));
		json_object_object_add(my_object, "dst_to", json_object_new_string(dst_to));
		json_object_array_add(json_array, my_object);
	}

	return json_array;
}

int cgi_get_port_forward_list(struct evhttp_request *req, const t_http_server *http_server)
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
	
	int count = get_port_forward_total();
	json_array = get_port_forward_list(atoi(page), atoi(limit));
	
	json_object_object_add(my_object, "code", json_object_new_string("0"));
	json_object_object_add(my_object, "msg", json_object_new_string(""));
	json_object_object_add(my_object, "count", json_object_new_int(count));
	json_object_object_add(my_object, "data", json_array);
	
	LOG_HL_INFO("JSON -->: %s\n", json_object_to_json_string(my_object));
	evhttpd_send_200_response(req, "application/json;charset=UTF-8", json_object_to_json_string(my_object));

done:
	if (json_array) json_object_put(json_array);
	if (my_object) json_object_put(my_object);

	free_malloc(limit);
	free_malloc(page);

	return ret;
}

int cgi_set_port_forward(struct evhttp_request *req, const t_http_server *http_server)
{
	int ret = -1;
	
	char *msg = NULL;

	json_object *my_object = json_object_new_object();
	if (! my_object) return -1;
	char *name = evhttp_get_post_parm(req, "name");
	char *src_port = evhttp_get_post_parm(req, "src_port");
	char *dst_port = evhttp_get_post_parm(req, "dst_port");
	char *dst_ip = evhttp_get_post_parm(req, "dst_ip");
	LOG_WARN_INFO(
		"\nname: %s\n"
		"src_port: %s\n"
		"dst_port: %s\n"
		"dst_ip: %s\n",
		name, src_port, dst_port, dst_ip
	);

	if (! name || ! src_port 
	|| ! dst_port || ! dst_ip
	|| strlen(name) <= 0
	|| strlen(name) > 64
	|| strlen(src_port) <= 0
	|| strlen(dst_port) <= 0
	|| strlen(dst_ip) <= 0
	|| ! isNumber(src_port)
	|| ! isNumber(dst_port)
	|| ! is_ip_str(dst_ip))
	{
		goto done;
	}

	char new_name[65] = {0};
	int i, k = 0;
	for (i = 0; i < strlen(name) && k < 65; i ++)
	{
		if (name[i] != ' ')
		{
			new_name[k ++] = name[i];
		}
	}

	char *result = "success";
	
	//写入文件
	if (add_port_forward(new_name, src_port, dst_port, dst_ip) == 1)
	{
		msg = utf8_encode("已经存在该规则名称，请重新命名!");
		result = "error";
	}
	
	json_object_object_add(my_object, "res", json_object_new_string(result));
	json_object_object_add(my_object, "msg", json_object_new_string(msg ? msg: ""));

	LOG_HL_INFO("JSON -->: %s\n", json_object_to_json_string(my_object));
	evhttpd_send_200_response(req, "application/json;charset=UTF-8", json_object_to_json_string(my_object));

done:
	json_object_put(my_object);

	free_malloc(name);
	free_malloc(src_port);
	free_malloc(dst_port);
	free_malloc(dst_ip);
	utf8_encode_free(msg);
	
	return ret;
}

int cgi_delete_port_forward(struct evhttp_request *req, const t_http_server *http_server)
{
	int ret = -1;
	
	json_object *my_object = json_object_new_object();
	if (! my_object) return -1;

	char *name = evhttp_get_post_parm(req, "name");
	LOG_WARN_INFO(
		"\nname: %s\n",
		name
	);

	if (! name || strlen(name) <= 0 || strlen(name) > 64)
	{
		goto done;
	}

	char new_name[65] = {0};
	int i, k = 0;
	for (i = 0; i < strlen(name) && k < 65; i ++)
	{
		if (name[i] != ' ')
		{
			new_name[k ++] = name[i];
		}
	}

	char *result = "success";

	//删除防火墙中的规则

	//从文件中删除
	delete_port_forward(new_name);
	
	json_object_object_add(my_object, "res", json_object_new_string(result));
	json_object_object_add(my_object, "msg", json_object_new_string(""));

	LOG_HL_INFO("JSON -->: %s\n", json_object_to_json_string(my_object));
	evhttpd_send_200_response(req, "application/json;charset=UTF-8", json_object_to_json_string(my_object));

done:
	json_object_put(my_object);

	free_malloc(name);
	return ret;
}

