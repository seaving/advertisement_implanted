#ifndef __QC_THREAD_WORK_CONF_H__
#define __QC_THREAD_WORK_CONF_H__


/**直接封装了qc_threadwork_init和qc_threadwork_add，
如果有work需要配置，直接在此接口中增加就可以了*/
bool qc_threadwork_config();

#define qc_threadwork_conf	qc_threadwork_config()

#endif

