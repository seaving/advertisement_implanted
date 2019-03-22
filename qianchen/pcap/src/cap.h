#ifndef __CAP_H__
#define __CAP_H__

#include "common.h"

typedef struct
{
	bool eth_type;
	char cap_dev[32];
} cap_call_fun_arg_t;

struct cap_thread_info
{
	int thid;
	char name[32];
	pcap_t *handle;
} cti_t;

extern volatile int g_traffic_time_cnt;

int create_cap_thread(struct cap_thread_info *cti);
int kill_cap_thread(char *cap_dev);

void cap_init();
int create_cap_sub_process(char *cap_dev);

void main_loop();

#endif




