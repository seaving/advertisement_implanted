#include "cgi_ping.h"

int cgi_ping(struct evhttp_request *req, const t_http_server *http_server)
{
	int ret = -1;
	
	char *msg = NULL;
	
	json_object *my_object = json_object_new_object();
	if (! my_object) return -1;

	char *domain = evhttp_get_post_parm(req, "domain");
	char *count_str = evhttp_get_post_parm(req, "count");
	char *size_str = evhttp_get_post_parm(req, "size");
	LOG_WARN_INFO(
		"\ndomain: %s\n"
		"count: %s\n"
		"size: %s\n",
		domain, count_str, size_str
	);

	if (! domain || strlen(domain) <= 0 || strlen(domain) > 255
	|| ! count_str || strlen(count_str) <= 0 || ! isNumber(count_str)
	|| ! size_str || strlen(size_str) <= 0 || ! isNumber(size_str))
	{
		goto done;
	}

	int count = atoi(count_str);
	int size = atoi(size_str);

	char *result = "success";

	if (! (0 < count && count <= 20)
	|| ! (4 <= size && size <= 1472))
	{
		goto done;
	}

	ping(count, size, domain);

	int time_cnt = 0;
	for (time_cnt = 0; ping_result_size() < 5 && time_cnt < 5; time_cnt ++)
	{
		sleep(1);
	}

	bool ping_finished = false;
	if (ping_result_size() < 5)
	{
		result = "error";
		msg = utf8_encode("请检查域名或者网络是否异常");
		ping_finished = true;
	}

	json_object_object_add(my_object, "res", json_object_new_string(result));
	json_object_object_add(my_object, "msg", json_object_new_string(msg ? msg : ""));
	json_object_object_add(my_object, "ping_finished", json_object_new_boolean(ping_finished));

	LOG_HL_INFO("JSON -->: %s\n", json_object_to_json_string(my_object));
	evhttpd_send_200_response(req, "application/json;charset=UTF-8", json_object_to_json_string(my_object));

done:
	json_object_put(my_object);

	free_malloc(domain);
	free_malloc(count_str);
	free_malloc(size_str);
	utf8_encode_free(msg);
	
	return ret;
}

int cgi_get_result(struct evhttp_request *req, const t_http_server *http_server)
{
	int ret = -1;
	
	char *msg = NULL;
	
	json_object *my_object = json_object_new_object();
	if (! my_object) return -1;

	char *result = "success";

	bool ping_finished = false;
	char *ping_result = get_result();
	if (! ping_result)
	{
		msg = utf8_encode("获取ping结果失败!");
		result = "error";
		ping_finished = true;
	}
	else
	{
		ping_finished = is_ping_finished(ping_result);
	}
	
	json_object_object_add(my_object, "res", json_object_new_string(result));
	json_object_object_add(my_object, "msg", json_object_new_string(msg ? msg : ""));
	json_object_object_add(my_object, "ping_result", json_object_new_string(ping_result ? ping_result : ""));
	json_object_object_add(my_object, "ping_finished", json_object_new_boolean(ping_finished));

	LOG_HL_INFO("JSON -->: %s\n", json_object_to_json_string(my_object));
	evhttpd_send_200_response(req, "application/json;charset=UTF-8", json_object_to_json_string(my_object));

	json_object_put(my_object);

	free_malloc(ping_result);
	utf8_encode_free(msg);
	
	return ret;
}

int cgi_stop_ping(struct evhttp_request *req, const t_http_server *http_server)
{
	int ret = -1;
		
	json_object *my_object = json_object_new_object();
	if (! my_object) return -1;

	char *result = "success";

	bool ping_finished = true;
	stop_ping();
	
	json_object_object_add(my_object, "res", json_object_new_string(result));
	json_object_object_add(my_object, "msg", json_object_new_string(""));
	json_object_object_add(my_object, "ping_finished", json_object_new_boolean(ping_finished));

	LOG_HL_INFO("JSON -->: %s\n", json_object_to_json_string(my_object));
	evhttpd_send_200_response(req, "application/json;charset=UTF-8", json_object_to_json_string(my_object));

	json_object_put(my_object);

	return ret;
}




