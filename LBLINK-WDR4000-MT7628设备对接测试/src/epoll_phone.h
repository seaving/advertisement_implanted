#ifndef __EPOLL_PHONE_H__
#define __EPOLL_PHONE_H__

#include "phoneinfo.h"

int epoll_add_to_phone(int phonefd);
void *phone_thread(void *arg);

int epoll_create_phone();
int phone_thread_start();
int epoll_mod_to_phone(phoneinfo_t *phone);

#endif

