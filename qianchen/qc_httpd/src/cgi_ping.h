#ifndef __CGI_PING_H__
#define __CGI_PING_H__

#include "includes.h"

int cgi_ping(struct evhttp_request *req, const t_http_server *http_server);
int cgi_get_result(struct evhttp_request *req, const t_http_server *http_server);
int cgi_stop_ping(struct evhttp_request *req, const t_http_server *http_server);

#endif


