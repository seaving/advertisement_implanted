#ifndef __CGI_NOT_AUTH_H__
#define __CGI_NOT_AUTH_H__

#include "includes.h"

int cgi_get_not_auth_info(struct evhttp_request *req, const t_http_server *http_server);

int cgi_set_not_auth(struct evhttp_request *req, const t_http_server *http_server);

#endif

