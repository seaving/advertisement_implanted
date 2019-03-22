
#ifndef __QC_THREAD_POOL_INFO_H__
#define __QC_THREAD_POOL_INFO_H__


#include "qc_threadwork_info.h"
#include "qc_pthread.h"

typedef struct _pipe_
{
	int read;
	int write;
} pipe_t;


typedef struct _thread_pool_
{
	pipe_t *pipe;			//主线程通过pipe发送命令到子线程
	thread_t *thread_id;	//记录线程ID
	short pool_cnt;			//总共线程池中的线程数量
	short offset;				//为thread_id做索引
	int witch_wirte;		//为pipe发送的时候做索引
} threadpool_t;

bool qc_threadpool_init(short pool_cnt);
void qc_threadpool_destroy();

bool qc_threadpool_pipe_send_cmd(char *com_data, int data_len, work_index_t work_index);
int qc_thread_pipe_read_cmd(int witch_read, char *databuf, int read_len);
bool qc_threadpool_add(thread_t id);
bool qc_threadpool_check(int witch, thread_t id);


#endif

