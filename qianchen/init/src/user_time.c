#include "includes.h"

static long _start_sec = 0;
static long _start_ms = 0;

void time_init()
{
	struct timespec start_time;
	clock_gettime(CLOCK_BOOTTIME, &start_time);
	_start_sec = (start_time.tv_sec + (start_time.tv_nsec / (1000 * 1000 * 1000)));
	_start_ms = (start_time.tv_sec * 1000 + (start_time.tv_nsec / (1000 * 1000)));
}

unsigned int get_system_clock_sec(void)
{
	struct timespec now;
	long sec = 0;
	long current = 0;

	clock_gettime(CLOCK_BOOTTIME, &now);
	current = (now.tv_sec + (now.tv_nsec/(1000 * 1000 * 1000)));
	sec = current - _start_sec;
	return (unsigned int)sec;
}

unsigned long get_system_clock_ms(void)
{
	struct timespec now;
	long ms = 0;
	long current = 0;

	clock_gettime(CLOCK_BOOTTIME, &now);
	current = (now.tv_sec * 1000 + (now.tv_nsec/(1000 * 1000)));
	ms = current - _start_ms;
	return (unsigned long) ms;
}

/*
*******************************
*	fun: 
*	arg:	
*	return:
*	time:
*	author:Seaving Cai
*******************************
*/
int CheckTimeSync(void)
{
	int Year;
	int Mon;
	int Data;
	int Hour;
	int Min;
	
	GetSystemTime(&Year, &Mon, &Data, &Hour, &Min);

	if (Year >= 16)
	{
		return 0;
	}

	return -1;
	
}






















