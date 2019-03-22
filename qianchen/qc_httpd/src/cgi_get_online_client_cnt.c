#include "cgi_get_online_client_cnt.h"


int get_online_client_cnt_at_br_lan()
{
	return get_online_client_cnt(LAN_DEV);
}

int get_online_client_cnt_at_br_guest()
{
	return get_online_client_cnt(LAN_GUEST_DEV);
}

int cgi_get_online_client_cnt(struct evhttp_request *req, const t_http_server *http_server)
{
	int ret = -1;
		
	json_object *my_object = json_object_new_object();
	if (! my_object) return -1;

	int auth_online_client_cnt = get_online_client_cnt_at_br_lan();
	int no_auth_online_client_cnt = get_online_client_cnt_at_br_guest();
	int online_client_cnt = auth_online_client_cnt + no_auth_online_client_cnt;

	json_object_object_add(my_object, "res", json_object_new_string("success"));
	json_object_object_add(my_object, "online_client_cnt", json_object_new_int(online_client_cnt));
	json_object_object_add(my_object, "auth_online_client_cnt", json_object_new_int(auth_online_client_cnt));
	json_object_object_add(my_object, "no_auth_online_client_cnt", json_object_new_int(no_auth_online_client_cnt));
	
	LOG_HL_INFO("JSON -->: %s\n", json_object_to_json_string(my_object));
	
	evhttpd_send_200_response(req, "application/json;charset=UTF-8", json_object_to_json_string(my_object));
	json_object_put(my_object);

	return ret;
}



