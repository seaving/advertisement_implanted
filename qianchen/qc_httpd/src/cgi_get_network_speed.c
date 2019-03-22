#include "includes.h"

int cgi_get_network_speed(struct evhttp_request *req, const t_http_server *http_server)
{
	int ret = -1;
	
	json_object *my_object = json_object_new_object();
	if (! my_object) return -1;

	int upload_speed = 0;
	int download_speed = 0;
	int total_speed = 0;
	get_network_speed(&upload_speed, &download_speed, &total_speed);

	unsigned long long traffic_total_count = 0;
	traffic_total_count = get_traffic_total_count();
	char traffic_total_count_str[255] = {0};
	snprintf(traffic_total_count_str, 255, "%llu", traffic_total_count);

	json_object_object_add(my_object, "res", json_object_new_string("success"));
	json_object_object_add(my_object, "upload_speed", json_object_new_int(upload_speed));
	json_object_object_add(my_object, "download_speed", json_object_new_int(download_speed));
	json_object_object_add(my_object, "total_speed", json_object_new_int(total_speed));
	json_object_object_add(my_object, "traffic_total_count", json_object_new_string(traffic_total_count_str));

	
	LOG_HL_INFO("JSON -->: %s\n", json_object_to_json_string(my_object));

	evhttpd_send_200_response(req, "application/json;charset=UTF-8", json_object_to_json_string(my_object));
	json_object_put(my_object);

	return ret;	
}

