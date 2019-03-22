#include "cgi_qos.h"

static bool set_qos_flag = false;

int get_qos_switch(char *qos_switch)
{
	get_user_config_value("qos_switch", qos_switch);
	if (strlen(qos_switch) <= 0
	|| qos_switch[0] == '0')
	{
		sprintf(qos_switch, "off");
	}
	else if (qos_switch[0] == '1')
	{
		sprintf(qos_switch, "on");
	}
	
	return 0;
}

int get_qos_type(char *qos_type)
{
	get_user_config_value("qos_type", qos_type);
	if (strlen(qos_type) <= 0)
	{
		sprintf(qos_type, "auto");
	}
	
	return 0;
}

int get_qos_disable_p2p_switch(char *disable_p2p_switch)
{
	get_user_config_value("qos_disable_p2p_switch", disable_p2p_switch);
	if (strlen(disable_p2p_switch) <= 0
	|| disable_p2p_switch[0] == '0')
	{
		sprintf(disable_p2p_switch, "off");
	}
	else if (disable_p2p_switch[0] == '1')
	{
		sprintf(disable_p2p_switch, "on");
	}
	
	return 0;
}

int get_qos_bandwidth_allocation(char *bandwidth_allocation)
{
	get_user_config_value("qos_bandwidth_allocation", bandwidth_allocation);
	if (strlen(bandwidth_allocation) <= 0)
	{
		sprintf(bandwidth_allocation, "race");
	}
	
	return 0;
}

int get_qos_tx()
{
	char qos_tx[15] = {0};
	get_user_config_value("qos_tx", qos_tx);
	if (strlen(qos_tx) <= 0)
	{
		return 0;
	}

	if (isNumber(qos_tx))
	{
		return atoi(qos_tx);
	}
	
	return 0;
}

int get_qos_rx()
{
	char qos_rx[15] = {0};
	get_user_config_value("qos_rx", qos_rx);
	if (strlen(qos_rx) <= 0)
	{
		return 0;
	}

	if (isNumber(qos_rx))
	{
		return atoi(qos_rx);
	}
	
	return 0;
}

int get_qos_station_max_tx()
{
	char qos_station_max_tx[15] = {0};
	get_user_config_value("qos_station_max_tx", qos_station_max_tx);
	if (strlen(qos_station_max_tx) <= 0)
	{
		return 0;
	}

	if (isNumber(qos_station_max_tx))
	{
		return atoi(qos_station_max_tx);
	}
	
	return 0;
}

int get_qos_station_max_rx()
{
	char qos_station_max_rx[15] = {0};
	get_user_config_value("qos_station_max_rx", qos_station_max_rx);
	if (strlen(qos_station_max_rx) <= 0)
	{
		return 0;
	}

	if (isNumber(qos_station_max_rx))
	{
		return atoi(qos_station_max_rx);
	}
	
	return 0;
}

int cgi_get_qos_info(struct evhttp_request *req, const t_http_server *http_server)
{
	int ret = -1;
	
	json_object *my_object = json_object_new_object();
	if (! my_object) return -1;

	char qos_switch[5] = {0};
	get_qos_switch(qos_switch);

	int qos_tx = get_qos_tx();
	int qos_rx = get_qos_rx();

	char qos_type[15] = {0};
	get_qos_type(qos_type);

	char qos_disable_p2p_switch[5] = {0};
	get_qos_disable_p2p_switch(qos_disable_p2p_switch);

	char qos_bandwidth_allocation[15] = {0};
	get_qos_bandwidth_allocation(qos_bandwidth_allocation);

	int qos_station_max_tx = get_qos_station_max_tx();
	int qos_station_max_rx = get_qos_station_max_rx();

	json_object_object_add(my_object, "res", json_object_new_string("success"));
	json_object_object_add(my_object, "tips_flag", json_object_new_boolean(set_qos_flag));
	json_object_object_add(my_object, "qos_switch", json_object_new_string(qos_switch));
	json_object_object_add(my_object, "qos_tx", json_object_new_int(qos_tx));
	json_object_object_add(my_object, "qos_rx", json_object_new_int(qos_rx));
	json_object_object_add(my_object, "qos_type", json_object_new_string(qos_type));
	json_object_object_add(my_object, "qos_disable_p2p_switch", json_object_new_string(qos_disable_p2p_switch));
	json_object_object_add(my_object, "qos_bandwidth_allocation", json_object_new_string(qos_bandwidth_allocation));
	json_object_object_add(my_object, "qos_station_max_tx", json_object_new_int(qos_station_max_tx));
	json_object_object_add(my_object, "qos_station_max_rx", json_object_new_int(qos_station_max_rx));

	LOG_HL_INFO("JSON -->: %s\n", json_object_to_json_string(my_object));
	evhttpd_send_200_response(req, "application/json;charset=UTF-8", json_object_to_json_string(my_object));
	json_object_put(my_object);

	return ret;
}

int cgi_set_qos(struct evhttp_request *req, const t_http_server *http_server)
{
	int ret = -1;
	
	json_object *my_object = json_object_new_object();
	if (! my_object) return -1;

	char *qos_switch = evhttp_get_post_parm(req, "qos_switch");
	char *qos_tx_str = evhttp_get_post_parm(req, "qos_tx");
	char *qos_rx_str = evhttp_get_post_parm(req, "qos_rx");
	char *qos_type = evhttp_get_post_parm(req, "qos_type");
	char *qos_disabled_p2p_switch = evhttp_get_post_parm(req, "qos_disabled_p2p_switch");
	char *qos_bandwidth_allocation = evhttp_get_post_parm(req, "qos_bandwidth_allocation");
	char *qos_station_max_tx_str = evhttp_get_post_parm(req, "qos_station_max_tx");
	char *qos_station_max_rx_str = evhttp_get_post_parm(req, "qos_station_max_rx");
	
	if ((qos_switch && strlen(qos_switch) > 3)
	|| (qos_tx_str && strlen(qos_tx_str) > 10)
	|| (qos_rx_str && strlen(qos_rx_str) > 10)
	|| (qos_type && strlen(qos_type) > 10)
	|| (qos_disabled_p2p_switch && strlen(qos_disabled_p2p_switch) > 3)
	|| (qos_bandwidth_allocation && strlen(qos_bandwidth_allocation) > 10)
	|| (qos_station_max_tx_str && strlen(qos_station_max_tx_str) > 10)
	|| (qos_station_max_rx_str && strlen(qos_station_max_rx_str) > 10))
	{
		goto done;
	}

	LOG_WARN_INFO(
		"\nqos_switch: %s\n"
		"qos_tx_str: %s\n"
		"qos_rx_str: %s\n"
		"qos_type: %s\n"
		"qos_disabled_p2p_switch: %s\n"
		"qos_bandwidth_allocation: %s\n"
		"qos_station_max_tx_str: %s\n"
		"qos_station_max_rx_str: %s\n",
		qos_switch, qos_tx_str, qos_rx_str, 
		qos_type, qos_disabled_p2p_switch, 
		qos_bandwidth_allocation, 
		qos_station_max_tx_str, qos_station_max_rx_str
	);

	char *res = "error";
	if (strcmp(qos_switch, "off") == 0)
	{
		set_user_config_value("qos_switch", "off");
		uci_commit();
		res = "success";
		set_qos_flag = true;

	}
	else if (strcmp(qos_switch, "on") == 0)
	{
		if ((! qos_tx_str || (qos_tx_str && strlen(qos_tx_str) <= 0) 
			|| (qos_tx_str && strlen(qos_tx_str) > 0 
				&& isNumber(qos_tx_str) 
				&& (1 <= atoi(qos_tx_str) && atoi(qos_tx_str) <= 1000)))
		&& (qos_rx_str && strlen(qos_rx_str) > 0 
				&& isNumber(qos_rx_str) 
				&& (4 <= atoi(qos_rx_str) && atoi(qos_rx_str) <= 1000))
		&& (qos_type && strlen(qos_type) > 0 
			&& (strcmp(qos_type, "auto") == 0 || strcmp(qos_type, "custom") == 0))
		&& (qos_disabled_p2p_switch && strlen(qos_disabled_p2p_switch) > 0 
			&& (strcmp(qos_disabled_p2p_switch, "on") == 0 || strcmp(qos_disabled_p2p_switch, "off") == 0))
		&& (qos_bandwidth_allocation && strlen(qos_bandwidth_allocation) > 0 
			&& (strcmp(qos_bandwidth_allocation, "race") == 0
				|| strcmp(qos_bandwidth_allocation, "dynamic") == 0
				|| strcmp(qos_bandwidth_allocation, "config") == 0))
		&& (! qos_station_max_tx_str || (qos_station_max_tx_str && strlen(qos_station_max_tx_str) <= 0) 
			|| (qos_station_max_tx_str && strlen(qos_station_max_tx_str) > 0 
				&& isNumber(qos_station_max_tx_str) 
				&& (0 <= atoi(qos_station_max_tx_str) && atoi(qos_station_max_tx_str) <= 1024 * 1000)))
		&& (qos_station_max_rx_str && strlen(qos_station_max_rx_str) > 0 
				&& isNumber(qos_station_max_rx_str) 
				&& (0 <= atoi(qos_station_max_rx_str) && atoi(qos_station_max_rx_str) <= 1024 * 1000)))
		{
			set_user_config_value("qos_switch", qos_switch);
			set_user_config_value("qos_tx", qos_tx_str);
			set_user_config_value("qos_rx", qos_rx_str);
			set_user_config_value("qos_type", qos_type);
			set_user_config_value("qos_disabled_p2p_switch", qos_disabled_p2p_switch);
			set_user_config_value("qos_bandwidth_allocation", qos_bandwidth_allocation);
			set_user_config_value("qos_station_max_tx", qos_station_max_tx_str);
			set_user_config_value("qos_station_max_rx", qos_station_max_rx_str);
			uci_commit();
			res = "success";
			set_qos_flag = true;
		}
	}

	json_object_object_add(my_object, "res", json_object_new_string(res));
	json_object_object_add(my_object, "tips_flag", json_object_new_boolean(set_qos_flag));
	json_object_object_add(my_object, "msg", json_object_new_string(""));
	
	LOG_HL_INFO("JSON -->: %s\n", json_object_to_json_string(my_object));

	evhttpd_send_200_response(req, "application/json;charset=UTF-8", json_object_to_json_string(my_object));
done:
	json_object_put(my_object);

	free_malloc(qos_switch);
	free_malloc(qos_tx_str);
	free_malloc(qos_rx_str);
	free_malloc(qos_type);
	free_malloc(qos_disabled_p2p_switch);
	free_malloc(qos_bandwidth_allocation);
	free_malloc(qos_station_max_tx_str);
	free_malloc(qos_station_max_rx_str);

	return ret;	
}

