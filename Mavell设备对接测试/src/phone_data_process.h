#ifndef __PHONE_DATA_PROCESS_H__
#define __PHONE_DATA_PROCESS_H__	1

#include "common.h"


int phone_data_process(struct epoll_event event);
int create_phone_data_process_thread(struct epoll_event event);

#endif
