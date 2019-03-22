#include "cgi_bond.h"

int request_bond_code(char *bond_code)
{
	char devNo[33] = {0};
	get_dev_id(devNo);
	
	char fwv[65] = {0};
	get_firmware_version(fwv);
	
	char gccv[65] = {0};
	get_gcc_version(gccv);
	
	char fwid[129] = {0};
	get_firmware_id(fwid);
	
	char pv[65] = {0};
	get_plugin_version(pv);

	char model[65] = {0};
	get_model(model, 64);

	LOG_WARN_INFO(
		"\n"
		"devNo: %s\n"
		"fwv: %s\n"
		"gccv: %s\n"
		"fwid: %s\n"
		"pv: %s\n"
		"model: %s\n",
		devNo, fwv, gccv, fwid, pv, model
	);
	
	json_object *my_object = json_object_new_object();
	if (! my_object) return -1;

	json_object_object_add(my_object, "devNo", json_object_new_string(devNo));
	json_object_object_add(my_object, "fwv", json_object_new_string(fwv));
	json_object_object_add(my_object, "gccv", json_object_new_string(gccv));
	json_object_object_add(my_object, "fwid", json_object_new_string(fwid));
	json_object_object_add(my_object, "pv", json_object_new_string(pv));
	json_object_object_add(my_object, "model", json_object_new_string(model));

	char *json_str = (char *) json_object_to_json_string(my_object);
	LOG_HL_INFO("JSON -->: %s\n", json_str);
	
	char response[513] = {0};
	http_send_post_data(SERVER_HOST, "/api/router/binding", 80, json_str, "application/json", response, 512);
	LOG_NORMAL_INFO("---- %s\n", response);
	json_object_put(my_object);

	json_object *response_json = json_tokener_parse(response);
	if (response_json == NULL
	|| is_error(response_json))
	{
		return -1;
	}

	int ret = -1;
	const char *code = "";
	const char *data = "";

	json_object *object_code = NULL;
	if (json_object_object_get_ex(response_json, "code", &object_code))
	{
		code = json_object_get_string(object_code);
	}

	json_object *object_data = NULL;
	if (json_object_object_get_ex(response_json, "data", &object_data))
	{
		data = json_object_get_string(object_data);
	}

	if (strcmp(code, "10000") == 0
	&& 0 < strlen(data) && strlen(data) <= 32)
	{
		memcpy(bond_code, data, strlen(data));
		ret = 0;
	}
	
	json_object_put(response_json);
	
	return ret;
}

int cgi_get_bond_code(struct evhttp_request *req, const t_http_server *http_server)
{
	int ret = -1;
	
	json_object *my_object = json_object_new_object();
	if (! my_object) return -1;

	char *result = "success";
	
	char code[33] = {0};
	request_bond_code(code);

	if (strlen(code) <= 0)
	{
		result = "error";
	}
	
	json_object_object_add(my_object, "res", json_object_new_string(result));
	json_object_object_add(my_object, "msg", json_object_new_string(""));
	json_object_object_add(my_object, "code", json_object_new_string(code));
	
	LOG_HL_INFO("JSON -->: %s\n", json_object_to_json_string(my_object));
	evhttpd_send_200_response(req, "application/json;charset=UTF-8", json_object_to_json_string(my_object));

	json_object_put(my_object);
	
	return ret;
}



