#ifndef __DUP_FILE_H__
#define __DUP_FILE_H__

#include "time.h"
#include "debug.h"

#define LOG_SAVE_ON		0

#if LOG_SAVE_ON
#define LOG_SAVE(first...) { \
    int year, mon, data, hour, min, sec; \
    GetAllSystemTime(&year, &mon, &data, &hour, &min, &sec, NULL); \
	printf("[%d%d%d%d%d%d%d%d%d%d%d%d]", year/10, year%10, mon/10, mon%10, \
			data/10, data%10, hour/10, hour%10, min/10, min%10, sec/10, sec%10); \
	printf(first); \
	fflush(stdout); \
}
#else
#define LOG_SAVE(first...) LOG_NORMAL_INFO(first);
#endif

int dup_file();
int dup_close();
int dup_remove();

#endif
