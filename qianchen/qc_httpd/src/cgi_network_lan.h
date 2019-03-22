#ifndef __CGI_NETWORK_LAN_H__
#define __CGI_NETWORK_LAN_H__

#include "includes.h"

int cgi_get_lan_info(struct evhttp_request *req, const t_http_server *http_server);
int cgi_set_lan(struct evhttp_request *req, const t_http_server *http_server);


#endif

