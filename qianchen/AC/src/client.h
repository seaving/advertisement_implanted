#ifndef __CLIENT_H__
#define __CLIENT_H__

//功能：管理网络连接的数据结构和算法，供server等模块交互信息使用

#include "includes.h"


typedef struct _CONN
{
	int fd;
	struct bufferevent *bufev;
	uint32 ind;
	char in_buf[CONN_BUF_LEN];
	uint16 in_buf_len;
	char out_buf[CONN_BUF_LEN];
	uint16 out_buf_len;
	CONN *next;
	WORKER *owner;
	uint32 cip;
	uint16 cport;
	uint16 err_type;
	void *user;
} CONN;

typedef struct _CONN_LIST
{
	CONN *head;
	CONN *tail;
	CONN list[0];
} CONN_LIST;

CONN_LIST* init_conn_list(uint32 size);

#define PUT_FREE_CONN(list, item)	\
{ \
	list->tail->next = item;	\
	list->tail = item; \
}

#define	GET_FREE_CONN(list, item)	\
{ \
	if(list->head != list->tail) \
	{	\
		item = list->head;	\
		list->head = list->head->next;	\
	}
	else \
	{	\
		item = NULL; \
	} \
}

#endif

