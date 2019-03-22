#ifndef __CGI_QOS_H__
#define __CGI_QOS_H__

#include "includes.h"

int cgi_get_qos_info(struct evhttp_request *req, const t_http_server *http_server);
int cgi_set_qos(struct evhttp_request *req, const t_http_server *http_server);



#endif


