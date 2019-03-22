#ifndef __QC_PTHREAD_H__
#define __QC_PTHREAD_H__

/*****************************************************************************
Copyright: 2016-2027, Qianchen Tech. Co., Ltd.
File name: qc_pthread.h
Description: pthread库函数接口封装声明
Author: Xiwen.cai
Version: v0.1
Date: 2017.03.16
History: 
*****************************************************************************/
#include <pthread.h>
#include <stdbool.h>

typedef pthread_t thread_t;
typedef pthread_attr_t thread_attr_t;

typedef pthread_mutex_t mutex_t;
typedef pthread_mutexattr_t mutexattr_t;

typedef void *(*start_routine)(void *);

#define qc_pthread_detach							pthread_detach(pthread_self())
#define qc_pthread_mutex_init(pmutex, pmutexattr) 	pthread_mutex_init(pmutex, pmutexattr)
#define qc_pthread_mutex_lock(pmutex) 				pthread_mutex_lock(pmutex)
#define qc_pthread_mutex_unlock(pmutex) 			pthread_mutex_unlock(pmutex)
#define qc_pthread_mutex_destroy(pmutex) 			pthread_mutex_destroy(pmutex)


bool qc_pthread_create(thread_t *thread, thread_attr_t *attr, start_routine thread_fun, void *arg);
bool qc_pthread_mutex_trylock(mutex_t *mutex);

#endif

