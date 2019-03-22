#ifndef __THREAD_POOL_H__
#define __THREAD_POOL_H__	1
 
#include <pthread.h>
 
/* 要执行的任务链表 */
typedef struct tpool_work {
	void*				(*routine)(void*);		 /* 任务函数 */
	void				*arg;					 /* 传入任务函数的参数 */
	struct tpool_work	*next;					  
}tpool_work_t;
 
typedef struct tpools {
	int 			shutdown;					 /* 线程池是否销毁 */
	int 			max_thr_num;				/* 最大线程数 */
	int 			working_counts;				/* 正在执行任务的线程数量 */
	pthread_t		*thr_id;					/* 线程ID数组 */
	tpool_work_t	*queue_head;				/* 线程链表 */
	pthread_mutex_t queue_lock;					/* 线程等信号锁 */ 				   
	pthread_cond_t	queue_ready;
	pthread_mutex_t put_lock;					/* 放置/取任务锁 */
}tpool_t;
 
/*
 * @brief	  创建线程池 
 * @param	  max_thr_num 最大线程数
 * @return	   0: 成功 其他: 失败  
 */
int tpool_create(int max_thr_num);
 
/*
 * @brief	  销毁线程池 
 */
int tpool_destroy();
 
/*
 * @brief	  向线程池中添加任务
 * @param	 routine 任务函数指针
 * @param	  arg 任务函数参数
 * @return	   0: 成功 其他:失败 
 */
int tpool_add_work(void*(*routine)(void*), void *arg);

int display_worker_counts();

#endif


