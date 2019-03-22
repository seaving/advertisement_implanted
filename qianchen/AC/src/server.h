#ifndef __SERVER_H__
#define __SERVER_H__


//功能：管理连接，网络读写，任务分发

#include "includes.h"

#define CONN_READ_TIMEOUT 	(300)
#define CONN_WRITE_TIMEOUT 	(300)

typedef struct _SERVER SERVER;

typedef struct _WORKER
{
	pthread_t thread_id;
	uint16 ind;
	int sfd;
	struct event_base *base;
	CONN_LIST *conns;
	struct event notify_event;
	int notified_rfd;
	int notifed_wfd;
	LOG_QUEUE *qlog;
	SERVER *server;
} WORKER;

typedef struct _WORKER_ARRAY {
	uint16 size;
	SERVER *server;
	WORKER array[0];
} WORKER_ARRAY;

struct _SERVER {
	int server_fd;
	int port;
	struct event_base *base;
	struct event listen_event;
	WORKER_ARRAY *workers;
	LOG_QUEUE *qlog;
	int ret;
	uint16 start_worker_num;
	uint64 conn_count;
	pthread_mutex_t start_lock;//start_worker_num同步;cond wait队列同步
	pthread_cond_t start_cond;
};

SERVER* init_server(int port, uint16 workernum, uint32 connnum, int read_timeout, int write_timeout);
WORKER_ARRAY* init_workers(SERVER *server, uint16 workernum, uint32 connnum, int read_timeout, int write_timeout);
void* start_server(void* arg);
int start_workers(WORKER_ARRAY* workers);

#endif

