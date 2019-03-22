#ifndef __EV_HTTPD_SERVER_H__
#define __EV_HTTPD_SERVER_H__

typedef struct _http_server_t
{
	char 			*base_path;
	unsigned short	gw_http_port;
	char 			gw[32];
} t_http_server;

int evhttpd_create_server_thread();
void evhttpd_redirect_login(struct evhttp_request *req, const t_http_server *http_server);

#endif




