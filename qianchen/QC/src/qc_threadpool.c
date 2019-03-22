#include "includes.h"



//线程池中的子线程
void *qc_subthread_loop(void *arg)
{
	qc_pthread_detach;

	if (! arg)
	{
		return NULL;
	}

	short witch = *((short *)arg);
	free(arg);

	char readbuf[10];

	sleep(1);
	
	if (! qc_threadpool_check(witch, pthread_self()))
	{
		LOG_ERROR_INFO("not self!\n");
		exit(-1);
	}

	while (1)
	{
		int len = qc_thread_pipe_read_cmd(witch, readbuf, 10);
		if (len <= 0)
		{
			//error
			break;
		}

		//process
		qc_threadwork_call((work_index_t)readbuf[0], &readbuf[1]);
	}

	return NULL;
}

//创建线程池
bool qc_threadpool_create(short pool_cnt)
{
	if (! qc_threadpool_init(pool_cnt))
	{
		goto _error_;
	}

	short i;
	for (i = 0; i < pool_cnt; i ++)
	{		
		thread_t thid;
		short *arg = calloc(sizeof(short), 1);
		if (! arg)
		{
			goto _error_;
		}

		*arg = i;
		if (! qc_pthread_create(&thid, NULL, qc_subthread_loop, arg))
		{
			free(arg);
			goto _error_;
		}

		if (! qc_threadpool_add(thid))
		{
			goto _error_;
		}
	}

	return true;
	
_error_:

	qc_threadpool_destroy();
	
	return false;
}


