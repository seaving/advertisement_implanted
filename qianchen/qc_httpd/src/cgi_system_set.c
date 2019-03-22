
#include "cgi_system_set.h"

int cgi_reboot_system(struct evhttp_request *req, const t_http_server *http_server)
{
	int ret = -1;
	
	json_object *my_object = json_object_new_object();
	if (! my_object) return -1;

	json_object_object_add(my_object, "res", json_object_new_string("success"));
	LOG_HL_INFO("JSON -->: %s\n", json_object_to_json_string(my_object));
	evhttpd_send_200_response(req, "application/json;charset=UTF-8", json_object_to_json_string(my_object));
	json_object_put(my_object);

	char cmd[32] = {0};
	EXECUTE_CMD(cmd, "reboot");

	return ret;

}

