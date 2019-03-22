#ifndef __EVHTTPD_FILTER_H__
#define __EVHTTPD_FILTER_H__

#include "common.h"

typedef enum
{
	E_SESSION_INVALID = 0,
	E_SESSION_OK
} session_t;

typedef enum
{
	E_PASSWD_INVALID = 0,
	E_NOT_PASSWD_LOGIN,
	E_PASSWD_OK
} passwd_t;

/**
* 是否不经过session过滤器的path
*/
typedef struct
{
	char *path;
} filter_list;

session_t evhttpd_filter_session(struct evhttp_request *req);
passwd_t evhttpd_filter_password(struct evhttp_request *req);

#endif


