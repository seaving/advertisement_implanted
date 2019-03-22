#ifndef __MAP_URL_H__
#define __MAP_URL_H__

#include "common.h"
#include "evhttp_server.h"

//uri: httpÇëÇóÍ·µÄ path
//context: post context
typedef int (*cgi_fun)(struct evhttp_request *req, const t_http_server *http_server);

typedef struct _map_url_
{
	char *map_name;
	char *real_path;
	cgi_fun fun;
} map_url_t;


int evhttp_map_url_service(struct evhttp_request *req, const t_http_server *http_server, const char *path);


#endif

