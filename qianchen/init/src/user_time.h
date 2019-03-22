#ifndef __USER_TIME_H__
#define __USER_TIME_H__


int CheckTimeSync(void);

void time_init();

unsigned int get_system_clock_sec(void);

unsigned long get_system_clock_ms(void);

#endif





