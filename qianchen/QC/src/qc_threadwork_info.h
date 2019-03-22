
#ifndef __QC_THREADWORK_H__
#define __QC_THREADWORK_H__

/**枚举，描述workfun，建议枚举定义的时候枚举常量需要命名得体,
保证能看明白workfun的含义
注意:取值范围 0-255
*/
typedef enum _work_index_
{
	E_WORK_PROCESS_REQUEST = 0,
	E_WORK_PROCESS_RESPONSE,
	E_END = 256
} work_index_t;

typedef void *(*work_fun)(void *arg);
typedef struct _work_info_
{
	work_index_t index;		//work的描述
	work_fun fun;			//work的实现
	//void *arg;
} work_info_t;

typedef struct _work_
{
	short offset;			//work_info的索引
	int workinfo_size;	//work_info_t的size
	short workinfo_cnt;	//work_info_t的数量
	work_info_t *work_info;
} work_t;


/**初始化一个线程池work缓存*/
bool qc_threadwork_init(short work_cnt);

/**增加一个work到缓存中*/
bool qc_threadwork_add(work_fun fun, work_index_t index);

/**从缓存中获取对应的work来执行*/
bool qc_threadwork_call(work_index_t index, void *arg);

#endif

