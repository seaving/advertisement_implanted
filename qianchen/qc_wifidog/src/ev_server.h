#ifndef __EV_SERVER_H__
#define __EV_SERVER_H__
#if 1

#include "common.h"


extern struct event_base *g_evhttp_base;

int server_listen_port();

void *_create_http_redirect_server_(void *arg);
#endif

#endif


