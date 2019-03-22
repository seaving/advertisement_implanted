#include "includes.h"


static threadpool_t *thpool = NULL;


bool qc_threadpool_init(short pool_cnt)
{
	thpool = (threadpool_t *)calloc(sizeof(threadpool_t), 1);
	if (! thpool)
	{
		goto _error_;
	}

	thpool->pipe = (pipe_t *)calloc(sizeof(pipe_t), pool_cnt);
	if (! thpool)
	{
		goto _error_;
	}

	thpool->thread_id = (thread_t *)calloc(sizeof(int), pool_cnt);
	if (! thpool)
	{
		goto _error_;
	}

	short i;
	for (i = 0; i < pool_cnt; i ++)
	{
		if (! qc_pipe_create(&thpool->pipe[i].read, &thpool->pipe[i].write))
		{
			goto _error_;
		}
	}

	thpool->pool_cnt = pool_cnt;

	return true;

_error_:

	qc_threadpool_destroy();

	return false;
}

void qc_threadpool_destroy()
{
	short i;

	if (thpool)
	{
		for (i = 0; i < thpool->pool_cnt; i ++)
		{
			qc_pipe_destroy(thpool->pipe[i].read, thpool->pipe[i].write);
		}
		
		free(thpool->pipe);

		free(thpool->thread_id);

		free(thpool);
	}
}

bool qc_threadpool_add(thread_t id)
{
	if (thpool->offset >= thpool->pool_cnt)
	{
		LOG_ERROR_INFO("thread pool count is full!\n");
		return false;
	}
	
	thpool->thread_id[thpool->offset ++] = id;

	return true;
}

bool qc_threadpool_check(int witch, thread_t id)
{
	return thpool->thread_id[witch] == id;
}

static int qc_threadpool_pipe_write(char *databuf, int data_len)
{
	int len = qc_pipe_write(thpool->pipe[thpool->witch_wirte ++].write, databuf, data_len);
	if (thpool->witch_wirte >= thpool->pool_cnt)
	{
		thpool->witch_wirte = 0;
	}
	
	return len;
}

static int qc_threadpool_pipe_read(char *readbuf, int read_len, int witch)
{
	return qc_pipe_read(thpool->pipe[witch].read, readbuf, read_len);
}

//通信协议的打包
static int qc_com_frame_package(char *com_data, int data_len, work_index_t work_index, char *databuf)
{
	int offset = 0;
	databuf[offset ++] = work_index;
	memcpy(&databuf[offset], com_data, data_len);
	return data_len + 1;
}

//管道发送
bool qc_threadpool_pipe_send_cmd(char *com_data, int data_len, work_index_t work_index)
{
	char databuf[data_len + 2];
	int len = qc_com_frame_package(com_data, data_len, work_index, databuf);
	
	int ret = qc_threadpool_pipe_write(databuf, len);
	if (ret != len)
	{	LOG_ERROR_INFO("pipe write error!\n");
		return false;
	}

	return true;
}

//基于管道获取任务
int qc_thread_pipe_read_cmd(int witch_read, char *databuf, int read_len)
{
	int len = qc_threadpool_pipe_read(databuf, read_len, witch_read);
	if (len <= 0)
	{
		return -1;
	}

	return len;
}


