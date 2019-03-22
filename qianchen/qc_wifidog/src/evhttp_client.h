
#ifndef __EVHTTP_CLIENT_H__
#define __EVHTTP_CLIENT_H__

#include "common.h"

#define HTTP_CONTENT_TYPE_URL_ENCODED   "application/x-www-form-urlencoded"
#define HTTP_CONTENT_TYPE_FORM_DATA     "multipart/form-data"
#define HTTP_CONTENT_TYPE_TEXT_PLAIN    "text/plain"

#define REQUEST_POST_FLAG               2  
#define REQUEST_GET_FLAG                3

typedef struct evhttp_cli_requset
{
	struct event_base *evbase;
	int method;
	char *url;
	char *post_data;
} evhttp_cli_requset_t;

void print_uri_parts_info(const struct evhttp_uri * http_uri);

#endif

