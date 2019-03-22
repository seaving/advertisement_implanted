#include "includes.h"

void *work1(void *arg)
{
	short i;
	PRINTF("work1 -----> ");
	for (i = 0; i < 5; i ++)
	{
		PRINTF("%02x ", ((char*)arg)[i]);
	}
	PRINTF("\n");
	return NULL;
}

void *work2(void *arg)
{
	short i;
	PRINTF("work2 -----> ");
	for (i = 0; i < 5; i ++)
	{
		PRINTF("%02x ", ((char*)arg)[i]);
	}
	PRINTF("\n");
	return NULL;
}

void *work3(void *arg)
{
	short i;
	PRINTF("work3 -----> ");
	for (i = 0; i < 5; i ++)
	{
		PRINTF("%02x ", ((char*)arg)[i]);
	}
	PRINTF("\n");
	return NULL;
}

bool qc_threadwork_config()
{
	bool ret = false;

	//-------------------------------------------------------
	//根据work数量传入参数,注意: work数量是多少就配置多少
	#define WORK_COUNT	3
	//-------------------------------------------------------
	if (qc_threadwork_init(WORK_COUNT))
	{
	//-------------------------------------------------------
	//配置work，按照格式配置就行了,注意: WORK_COUNT是多少就配置多少个work
		ret = qc_threadwork_add(callback_phone_data_process, E_WORK_PROCESS_REQUEST);
		ret = qc_threadwork_add(callback_webserver_data_process, E_WORK_PROCESS_RESPONSE);
	//-------------------------------------------------------
	}
	/**@@@@@@ end @@@@@@*/
	return ret;
}

