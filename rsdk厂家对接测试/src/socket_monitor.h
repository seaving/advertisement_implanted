#ifndef __SOCKET_MONITOR_H__
#define __SOCKET_MONITOR_H__	1

#define __SOCKET_MONITOR__	0

#if __SOCKET_MONITOR__

#include "phoneinfo.h"

typedef struct SocketMonitor
{
	phoneinfo_t *phone;
	int start;
	int validTime;
	int flag;
	pthread_mutex_t lock;
	struct SocketMonitor *next;
	struct SocketMonitor *pre;
} SocketMonitor_t;


int add_socket_monitor(phoneinfo_t *phone);
void socket_monitor_init();
int activity_socket_monitor(phoneinfo_t *phone);
#endif
#endif

