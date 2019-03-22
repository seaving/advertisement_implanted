#include "includes.h"

int upload_dev_info(char *devNo)
{
	int ret = -1;
	
	get_dev_id(devNo);

	char mac[33] = {0};
	get_local_mac(mac, get_netdev_wan_name());

	char model[65] = {0};
	get_model(model, 64);

	char fwv[65] = {0};
	get_firmware_version(fwv);

	char fwid[129] = {0};
	get_firmware_id(fwid);

	char gccv[65] = {0};
	get_gcc_version(gccv);

	char pv[65] = {0};
	get_plugin_version(pv);

	LOG_WARN_INFO(
		"\n"
		"devNo: %s\n"
		"fwv: %s\n"
		"gccv: %s\n"
		"fwid: %s\n"
		"pv: %s\n"
		"model: %s\n"
		"mac: %s\n",
		devNo, fwv, gccv, fwid, pv, model, mac
	);

	json_object *my_object = json_object_new_object();
	if (! my_object) return -1;

	json_object_object_add(my_object, "devNo", json_object_new_string(devNo));
	json_object_object_add(my_object, "fwv", json_object_new_string(fwv));
	json_object_object_add(my_object, "gccv", json_object_new_string(gccv));
	json_object_object_add(my_object, "fwid", json_object_new_string(fwid));
	json_object_object_add(my_object, "pv", json_object_new_string(pv));
	json_object_object_add(my_object, "model", json_object_new_string(model));
	json_object_object_add(my_object, "mac", json_object_new_string(mac));

	char *json_str = (char *) json_object_to_json_string(my_object);
	LOG_HL_INFO("JSON -->: %s\n", json_str);

	char response[513] = {0};
	http_send_post_data(SERVER_HOST, "/api/router/infomation", 80, json_str, "application/json", response, 512);
	LOG_NORMAL_INFO("---- %s\n", response);
	json_object_put(my_object);

	json_object *response_json = json_tokener_parse(response);
	if (is_error(response_json))
	{
		return -1;
	}

	const char *code = "";
	const char *data = "";
	const char *json_data = NULL;
	const char *token = "";
	
	json_object *object_code;
	if (json_object_object_get_ex(response_json, "code", &object_code))
	{
		code = json_object_get_string(object_code);
	}
	
	json_object *object_data;
	if (json_object_object_get_ex(response_json, "data", &object_data))
	{
		data = json_object_get_string(object_data);
		json_data = json_object_to_json_string(object_data);
	}

	json_object *object_token;
	if (json_object_object_get_ex(response_json, "token", &object_token))
	{
		token = json_object_get_string(object_token);
	}

	LOG_NORMAL_INFO("code: %s, data: %s, token: %s\n", code, data, token);

	if (strcmp(code, "10000") == 0)
	{
	
		char md5[33] = {0};
		CalMd5ByCopy((char *) (json_data ? json_data : ""), strlen(json_data), DATA_TOKEN_KEY, DATA_TOKEN_KEY_LEN, md5);
	
		if (strcmp(md5, token) == 0 && 0 < strlen(data) && strlen(data) <= 32)
		{
			ret = 0;
			memcpy(devNo, data, strlen(data));
		}
		else
		{
			LOG_ERROR_INFO("cal_md5: %s, net_md5: %s, data_len: %d\n", md5, token, strlen(data));
		}
	}

	json_object_put(response_json);

	return ret;
}

int request_dev_id_from_server(char *devNo)
{
	return upload_dev_info(devNo);
}

void *request_dev_id_from_server_work(void *arg)
{
	pthread_detach(pthread_self());

	for ( ; ; )
	{
		char devNo[33] = {0};
		get_dev_id(devNo);
		if (0 < strlen(devNo) && strlen(devNo) <= 32)
		{
			break;
		}

		request_dev_id_from_server(devNo);
		if (0 < strlen(devNo) && strlen(devNo) <= 32)
		{
			set_dev_id(devNo);
			break;
		}

		sleep(5);
	}

	return NULL;
}

void request_dev_id_from_server_thread()
{
	pthread_t thd;
	pthread_create(&thd, NULL, request_dev_id_from_server_work, NULL);
}

