
#ifndef __MONITOR_H__
#define __MONITOR_H__	1

#define MONITOR_PROGRAM		"qcdog"

int create_monitor_pid_thread();

void monitor_send_cond();

void monitor_wait_cond();

void wait_monitor_exit();

#endif




