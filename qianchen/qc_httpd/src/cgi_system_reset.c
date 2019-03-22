#include "cgi_system_reset.h"

#define CMD_SYSTEM_RESET	"tar -xzvf /etc/app/backup_config.tar.gz -C /etc/config/"

static bool system_reset_flag = false;

void system_reset()
{
	char cmd[255] = {0};
	EXECUTE_CMD(cmd, CMD_SYSTEM_RESET);
}

int cgi_system_reset(struct evhttp_request *req, const t_http_server *http_server)
{
	int ret = -1;
	
	json_object *my_object = json_object_new_object();
	if (! my_object) return -1;

	char *result = "success";
	system_reset();
	system_reset_flag = true;
	
	json_object_object_add(my_object, "res", json_object_new_string(result));
	json_object_object_add(my_object, "tips_flag", json_object_new_boolean(system_reset_flag));
	json_object_object_add(my_object, "msg", json_object_new_string(""));
	
	LOG_HL_INFO("JSON -->: %s\n", json_object_to_json_string(my_object));
	evhttpd_send_200_response(req, "application/json;charset=UTF-8", json_object_to_json_string(my_object));

	json_object_put(my_object);
	
	return ret;
}



