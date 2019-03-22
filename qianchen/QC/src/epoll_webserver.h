#ifndef __EPOLL_WEBSEVER_H__
#define __EPOLL_WEBSEVER_H__

#include "webserverinfo.h"

int epoll_add_to_webserver(int webserverfd, phoneinfo_t *phone, bool isAccept);
int epoll_create_webserver();
int epoll_wait_webserver(int webserver_epollfd);
void *webserver_thread(void *arg);
int webserver_thread_start();
int epoll_mod_to_webserver(webserverinfo_t *webserver);

#endif
