
#include "includes.h"



bool qc_pthread_create(thread_t *thread, thread_attr_t *attr, start_routine thread_fun, void *arg)
{
	if (pthread_create(thread, attr, thread_fun, arg) != 0)
	{
		LOG_PERROR_INFO("pthread_create failt!");
		return false;
	}
	return true;
}

bool qc_pthread_mutex_trylock(mutex_t *mutex)
{
	if (pthread_mutex_trylock(mutex) != 0)
	{
		return false;
	}
	return true;
}

