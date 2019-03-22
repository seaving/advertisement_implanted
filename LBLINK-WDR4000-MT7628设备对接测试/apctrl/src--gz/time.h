#ifndef __TIME_H__#define __TIME_H__
#define DEBUG_TIME	0
#define CONVERTSEC(H, M, S)	(H * 3600 + M * 60 + S)void GetSystemTime(int *Year, int *Mon, int *Data, int *Hour, int *Min);void GetAllSystemTime(int *Year, int *Mon, int *Data, int *Hour, int *Min, int *Sec, int *Weekday);int GetSystemTime_Sec();char * weekday(int day);char * month(int month);

long GetTimestrip_us();#endif


