#ifndef __CGI_CLIENT_MANAGER_H__
#define __CGI_CLIENT_MANAGER_H__

#include "includes.h"

int cgi_get_client_list(struct evhttp_request *req, const t_http_server *http_server);
int cgi_disconnect_client(struct evhttp_request *req, const t_http_server *http_server);
int cgi_set_black_client(struct evhttp_request *req, const t_http_server *http_server);
int cgi_get_black_list(struct evhttp_request *req, const t_http_server *http_server);
int cgi_delete_black_name(struct evhttp_request *req, const t_http_server *http_server);

#endif


