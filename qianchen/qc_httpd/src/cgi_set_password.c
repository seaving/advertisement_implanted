#include "cgi_set_password.h"


int cgi_set_password(struct evhttp_request *req, const t_http_server *http_server)
{
	LOG_WARN_INFO("set_password\n");

	char *source_password = evhttp_get_post_parm(req, "source_password");
	char *new_password = evhttp_get_post_parm(req, "new_password");
	char *repeat_password = evhttp_get_post_parm(req, "repeat_password");

	if (! source_password
	|| ! new_password
	|| ! repeat_password
	|| strlen(source_password) > 16
	|| strlen(new_password) > 16
	|| strlen(repeat_password) > 16) {
		free_malloc(source_password);
		free_malloc(new_password);
		free_malloc(repeat_password);
		return -1;
	}

	char *result = "error";

	if (passwd_check(source_password) == true
	&& strcmp(new_password, repeat_password) == 0)
	{
		LOG_NORMAL_INFO("password set success .\n");
		set_passwd(new_password);
		s_cookie_password_set(new_password);
		uci_commit();
		result = "success";
	}

	json_object *my_object = json_object_new_object();
	if (! my_object) return -1;
	json_object_object_add(my_object, "res", json_object_new_string(result));
	LOG_HL_INFO("JSON -->: %s\n", json_object_to_json_string(my_object));
	evhttpd_send_200_response(req, "application/json;charset=UTF-8", json_object_to_json_string(my_object));
	json_object_put(my_object);

	free_malloc(source_password);
	free_malloc(new_password);
	free_malloc(repeat_password);

	return 0;
}



