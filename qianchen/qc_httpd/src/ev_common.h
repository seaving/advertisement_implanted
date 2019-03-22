
#ifndef __EV_COMMON_H__
#define __EV_COMMON_H__

#include "common.h"

void evhttp_gw_reply_302_redirect(struct evhttp_request *req, const char *redirect_url);

void evhttp_gw_reply_302_redirect_with_cookie(struct evhttp_request *req, const char *redirect_url, const char *cookie);

char *evhttp_get_parm(struct evhttp_request *req, const char *key);

char *evhttp_get_post_parm(struct evhttp_request *req, const char *key);

char *evhttpd_parse_parma_by_uri(char *uri, char *key);

int evhttpd_send_200_response(struct evhttp_request *req, char *type, const char *fmt, ...);

#endif


