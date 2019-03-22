#ifndef __CGI_WIRELESS_H__
#define __CGI_WIRELESS_H__


#include "includes.h"



int cgi_get_wlan_info(struct evhttp_request *req, const t_http_server *http_server);

int cgi_set_wlan(struct evhttp_request *req, const t_http_server *http_server);


#endif

