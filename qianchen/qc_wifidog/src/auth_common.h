#ifndef __AUTH_COMMON_H__
#define __AUTH_COMMON_H__

typedef struct _http_server_t
{
	char 	*base_path;
	short	gw_http_port;
} t_http_server;

int create_server();

#endif


