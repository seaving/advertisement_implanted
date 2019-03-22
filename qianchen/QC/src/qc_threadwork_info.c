#include "includes.h"

static work_t *work = NULL;

bool qc_threadwork_init(short work_cnt)
{
	work = calloc(sizeof(work_t), 1);
	if (! work)
	{
		goto _error_;
	}
	
	work->work_info = calloc(sizeof(work_info_t), work_cnt);
	if (! work->work_info)
	{
		goto _error_;
	}

	work->workinfo_cnt = work_cnt;
	work->offset = 0;
	work->workinfo_size = sizeof(work_info_t);
	return true;

_error_:
	if (work)
	{
		free(work->work_info);
		free(work);
	}
	return false;
}

bool qc_threadwork_add(work_fun fun, work_index_t index)
{
	if (! work)
	{
		LOG_ERROR_INFO("you need qc_threadwork_init first!\n");
		return false;
	}

	if (work->offset >= work->workinfo_cnt)
	{
		LOG_ERROR_INFO("work array was full! can't add function!\n");
		return false;
	}

	if (index != work->offset)
	{
		LOG_ERROR_INFO("work info index wrong! work->offset is %d\n", work->offset);
		return false;
	}

	//work->work_info[work->offset].arg = NULL;
	work->work_info[work->offset].index = index;
	work->work_info[work->offset].fun = fun;
	
	work->offset ++;

	return true;
}

bool qc_threadwork_call(work_index_t index, void *arg)
{
	if (! work)
	{
		LOG_ERROR_INFO("you need qc_threadwork_init first!\n");
		return false;
	}

	if (index >= work->offset)
	{
		LOG_ERROR_INFO("false: array range!\n");
		return false;
	}

	work_info_t *workinfo = NULL;
	workinfo = &work->work_info[index];
	if (! workinfo)
	{
		LOG_ERROR_INFO("false: not found work!\n");
		return false;
	}

	if (workinfo->index != index)
	{
		LOG_ERROR_INFO("index error!\n");
		return false;
	}

	workinfo->fun(arg);

	return true;
}


