#include "includes.h"
 
static tpool_t *tpool = NULL;
 
/* 工作者线程函数, 从任务链表中取出任务并执行 */
static void* thread_routine(void *arg)
{
    tpool_work_t *work;

	LOG_NORMAL_INFO("thread:%d begin running ...\n", (unsigned int)pthread_self());
    
	for ( ; ; )
    {
        /* 如果线程池没有被销毁且没有任务要执行，则等待 */
        pthread_mutex_lock(&tpool->queue_lock);
        while(!tpool->queue_head && !tpool->shutdown)
        {
            pthread_cond_wait(&tpool->queue_ready, &tpool->queue_lock);
        }
        if (tpool->shutdown)
        {
            pthread_mutex_unlock(&tpool->queue_lock);
            pthread_exit(NULL);
        }

		//开始取任务
		//数量加1
		tpool->working_counts ++;

        //取任务执行
        work = tpool->queue_head;
        if (! work)
        {
         	//数量减1
        	tpool->working_counts --;

        	pthread_mutex_unlock(&tpool->queue_lock);
        	
        	continue;
        }
        
		tpool->queue_head = tpool->queue_head->next;
		pthread_mutex_unlock(&tpool->queue_lock);
		work->routine(work->arg);
		free(work);
		work = NULL;

        //数量减1
        tpool->working_counts --;
    }

    return NULL;   
}
 
/*
 * 创建线程池 
 */
int tpool_create(int max_thr_num)
{
    int i;
 
    tpool = malloc(sizeof(tpool_t));
    if (!tpool)
    {
        LOG_PERROR_INFO("malloc failed");
        return -1;
    }
    
    /* 初始化 */
    tpool->max_thr_num = max_thr_num;
    tpool->working_counts = 0;
    tpool->shutdown = 0;
    tpool->queue_head = NULL;
    if (pthread_mutex_init(&tpool->queue_lock, NULL) != 0)
    {
        LOG_PERROR_INFO("pthread_mutex_init failed.");
        free(tpool);
        return -1;
    }
    if (pthread_mutex_init(&tpool->put_lock, NULL) != 0)
    {
        LOG_PERROR_INFO("pthread_mutex_init failed.");
        free(tpool);
        return -1;
    }
    if (pthread_cond_init(&tpool->queue_ready, NULL) != 0)
    {
		LOG_PERROR_INFO("pthread_cond_init failed.");
        free(tpool);
        return -1;
    }
    
    /* 创建工作者线程 */
    tpool->thr_id = malloc(max_thr_num * sizeof(pthread_t));
    if (!tpool->thr_id)
    {
        LOG_PERROR_INFO("malloc failed");
        free(tpool);
        return -1;
    }
    for (i = 0; i < max_thr_num; i ++)
    {
        if (pthread_create(&tpool->thr_id[i], NULL, thread_routine, NULL) != 0)
        {
			LOG_PERROR_INFO("pthread_create failed");
			free(tpool->thr_id);
			free(tpool);
			return -1;
        }
    }
 
    return 0;
}
 
/* 销毁线程池 */
int tpool_destroy()
{
    int i;
    tpool_work_t *member;
 
    if (tpool->shutdown)
    {
        return 0;
    }
    tpool->shutdown = 1;
 
    /* 通知所有正在等待的线程 */
    pthread_mutex_lock(&tpool->queue_lock);
    pthread_cond_broadcast(&tpool->queue_ready);
    pthread_mutex_unlock(&tpool->queue_lock);
    for (i = 0; i < tpool->max_thr_num; i ++)
    {
        pthread_join(tpool->thr_id[i], NULL);
    }
    free(tpool->thr_id);
 
    while(tpool->queue_head)
    {
        member = tpool->queue_head;
        tpool->queue_head = tpool->queue_head->next;
        free(member);
    }
 
    pthread_mutex_destroy(&tpool->queue_lock);
    pthread_mutex_destroy(&tpool->put_lock);
    pthread_cond_destroy(&tpool->queue_ready);
 
    free(tpool);

    return 0;
}
 
/* 向线程池添加任务 */
int tpool_add_work(void*(*routine)(void*), void *arg)
{
    tpool_work_t *work, *member;

	if (tpool->working_counts >= tpool->max_thr_num)
	{
		//返回-2表示没有空闲线程
		printf("threads are all busy now! run_counts:%d .\n", tpool->working_counts);
		return -2;
	}
    
    if (!routine)
    {
        LOG_ERROR_INFO("Invalid argument\n");
        return -1;
    }
    
    work = malloc(sizeof(tpool_work_t));
    if (!work)
    {
        LOG_PERROR_INFO("malloc failed");
        return -1;
    }
    work->routine = routine;
    work->arg = arg;
    work->next = NULL;
 
    pthread_mutex_lock(&tpool->queue_lock);
    member = tpool->queue_head;
    if (!member)
    {
        tpool->queue_head = work;
    } 
    else
    {
        while (member->next)
        {
            member = member->next;
        }
        member->next = work;
    }
    
    /* 通知工作者线程，有新任务添加 */
    pthread_mutex_unlock(&tpool->queue_lock);
 	pthread_cond_signal(&tpool->queue_ready);
 	
    return 0;    
}

int display_worker_counts()
{
	int now_time_sec = 0;
	static int old_time_sec = -1;
	static int diff_cnt = -1;
	GetAllSystemTime(NULL, NULL, NULL, NULL, NULL, &now_time_sec, NULL);
	if (now_time_sec != old_time_sec)
	{
		diff_cnt ++;
		old_time_sec = now_time_sec;
	}
	else
	{
		return 0;
	}

	if (diff_cnt > 1)
	{
		diff_cnt = 0;
		
		PRINTF("\n");
		PRINTF("--------------------------------\n");
		PRINTF("- there are %d worker thread .\n", tpool->working_counts);
		PRINTF("--------------------------------\n");
		PRINTF("\n");
	}
	
	return 0;
}

