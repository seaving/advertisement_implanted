#ifndef __ATTACK_H__
#define __ATTACK_H__	1

#include "webserverinfo.h"

int attack_static(webserverinfo_t *webserver);
int accept_static(webserverinfo_t *webserver);

int attack_chunked(webserverinfo_t *webserver);
int accept_chunked(webserverinfo_t *webserver);

int process_static(webserverinfo_t *webserver);
int process_chunked(webserverinfo_t *webserver);
int default_forward(webserverinfo_t *webserver);

#endif

