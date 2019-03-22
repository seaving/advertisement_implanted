#ifndef __MAP_URL_CONFIG_H__
#define __MAP_URL_CONFIG_H__

#include "map_url.h"
#include "cgi_set_password.h"
#include "cgi_logout.h"
#include "cgi_get_info.h"
#include "cgi_get_online_client_cnt.h"
#include "cgi_get_network_speed.h"
#include "cgi_network_wan.h"
#include "cgi_system_set.h"
#include "cgi_network_lan.h"
#include "cgi_wireless.h"
#include "cgi_qos.h"
#include "cgi_client_manager.h"
#include "cgi_not_auth.h"
#include "cgi_port_forward.h"
#include "cgi_ping.h"
#include "cgi_system_reset.h"
#include "cgi_bond.h"

map_url_t urlMap[] = {
	{"/setpwd", NULL, cgi_set_password},
	{"/logout", NULL, cgi_logout},
	{"/getInfo", NULL, cgi_get_info},
	{"/getOnlineCnt", NULL, cgi_get_online_client_cnt},
	{"/getNetworkSpeed", NULL, cgi_get_network_speed},
	{"/getWanProtocol", NULL, cgi_get_wan_protocol},
	{"/getWanInfo", NULL, cgi_get_wan_info},
	{"/setWan", NULL, cgi_set_wan},
	{"/getLanInfo", NULL, cgi_get_lan_info},
	{"/setLan", NULL, cgi_set_lan},
	{"/getWlanInfo", NULL, cgi_get_wlan_info},
	{"/setWlan", NULL, cgi_set_wlan},
	{"/getQoSInfo", NULL, cgi_get_qos_info},
	{"/setQoS", NULL, cgi_set_qos},
	{"/getOnlineClientList", NULL, cgi_get_client_list},
	{"/disconnect", NULL, cgi_disconnect_client},
	{"/getBlackList", NULL, cgi_get_black_list},
	{"/setBlack", NULL, cgi_set_black_client},
	{"/deleteBlack", NULL, cgi_delete_black_name},
	{"/getNotAuthInfo", NULL, cgi_get_not_auth_info},
	{"/setNotAuth", NULL, cgi_set_not_auth},
	{"/getNotAuthInfo", NULL, cgi_get_not_auth_info},
	{"/getPortForwardList", NULL, cgi_get_port_forward_list},
	{"/setPortForward", NULL, cgi_set_port_forward},
	{"/deletePortForward", NULL, cgi_delete_port_forward},
	{"/ping", NULL, cgi_ping},
	{"/getResult", NULL, cgi_get_result},
	{"/stopPing", NULL, cgi_stop_ping},
	{"/systemReset", NULL, cgi_system_reset},
	{"/bond", NULL, cgi_get_bond_code},
	{"/reboot", NULL, cgi_reboot_system},
	{NULL, NULL, NULL} //½áÊøÌõ¼þ
};


#endif


