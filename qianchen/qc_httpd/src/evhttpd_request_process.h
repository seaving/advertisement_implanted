#ifndef __EVHTTPD_REQUEST_PROCESS_H__
#define __EVHTTPD_REQUEST_PROCESS_H__

#include "evhttp_server.h"


int evhttpd_get(struct evhttp_request *req, const t_http_server *http_server);
int evhttpd_post(struct evhttp_request *req, const t_http_server *http_server);
int send_html(struct evhttp_request *req, const char *docroot, const char *path);


#endif


