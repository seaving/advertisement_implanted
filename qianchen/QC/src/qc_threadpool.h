#ifndef __QC_THREAD_POOL_H__
#define __QC_THREAD_POOL_H__

//#define _QC_THREAD_POOL_

/**不能太大，一个进程最大一般能打开1024个文件，
现在取值为100，加上0，1，2三个描述符，总共加上pipe就已经
占用了203个描述符*/
#define POOL_MAX_THRED	100

/**尽量不要使用该接口，使用下面定义的宏*/
bool qc_threadpool_create(short pool_cnt);


#define qc_threadpool_conf	{ \
	if (! qc_threadpool_create(POOL_MAX_THRED)) \
	{ \
		exit(-1); \
	} \
}

#endif
