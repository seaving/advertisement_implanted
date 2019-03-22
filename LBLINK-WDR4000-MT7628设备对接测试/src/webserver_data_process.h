#ifndef __WEBSERVER_DATA_PROCESS_H__
#define __WEBSERVER_DATA_PROCESS_H__

#include "common.h"

int webserver_data_process(struct epoll_event event);
int create_webserver_data_process_thread(struct epoll_event event);

#endif
