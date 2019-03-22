#ifndef __PROCESS_H__
#define __PROCESS_H__	1

#include "config.h"

extern int g_ProcessArr[PROCESS_NUM];

int process_CreateSub();
int monitor_subprocess();

#endif

