#ifndef __CGI_PORT_FORWARD_H__
#define __CGI_PORT_FORWARD_H__

#include "includes.h"

int cgi_get_port_forward_list(struct evhttp_request *req, const t_http_server *http_server);

int cgi_set_port_forward(struct evhttp_request *req, const t_http_server *http_server);

int cgi_delete_port_forward(struct evhttp_request *req, const t_http_server *http_server);


#endif


