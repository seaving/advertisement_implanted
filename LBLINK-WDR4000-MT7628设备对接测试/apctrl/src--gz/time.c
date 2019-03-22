#include "includes.h"

static char Month[13][6] = {"No", "Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"};
static char Week[7][4] = {"Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"};

/*
*******************************
*	fun: 
*	arg:	
*	return:
*	time:
*	author:Seaving Cai
*******************************
*/
int GetSystemTime_Sec()
{
    return time((time_t*)NULL);
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
long GetTimestrip_us()
{
	struct timeval tCurr;
	gettimeofday(&tCurr, NULL);
	return (1000000L * tCurr.tv_sec + tCurr.tv_usec);
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
void GetSystemTime(int *Year, int *Mon, int *Data, int *Hour, int *Min)
{
	time_t now;          //实例化time_t结构

	struct tm *timenow;  //实例化tm结构指针

	time(&now);

    //time函数读取现在的时间(国际标准时间非北京时间)，然后传值给now
    timenow = localtime(&now);

	//分别获得 年 月 日 时  分
	if (Year != NULL)
	{
		*Year = timenow->tm_year - 100;
	}
	if (Mon != NULL)
	{
		*Mon = timenow->tm_mon + 1; // 0 --- 11
	}
	if (Data != NULL)
	{
		*Data = timenow->tm_mday;
	}
	if (Hour != NULL)
	{
		*Hour = timenow->tm_hour;
	}
	if (Min != NULL)
	{
		*Min = timenow->tm_min;
	}
	//localtime函数把从time取得的时间now换算成你电脑中的时间(就是你设置的地区)
	//asctime函数把时间转换成字符，通过printf()函数输出
    //printf("Local   time   is   %s/n",asctime(timenow));
	//Tue Jun 10 14:25:14 2014
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
void GetAllSystemTime(int *Year, int *Mon, int *Data, int *Hour, int *Min, int *Sec, int *Weekday)
{
	time_t now;          //实例化time_t结构

	struct tm *timenow;  //实例化tm结构指针

	time(&now);

    //time函数读取现在的时间(国际标准时间非北京时间)，然后传值给now
    timenow = localtime(&now);

	//分别获得 年 月 日 时  分
	if (Year != NULL)
	{
		*Year = timenow->tm_year - 100;
	}
	if (Mon != NULL)
	{
		*Mon = timenow->tm_mon + 1; // 0 --- 11
	}
	if (Data != NULL)
	{
		*Data = timenow->tm_mday;
	}
	if (Hour != NULL)
	{
		*Hour = timenow->tm_hour;
	}
	if (Min != NULL)
	{
		*Min = timenow->tm_min;
	}
	if (Sec != NULL)
	{
		*Sec = timenow->tm_sec;
	}
	if (Weekday != NULL)
	{
		*Weekday = timenow->tm_wday;
	}
}

char * weekday(int day)
{
	return Week[day];
}

char * month(int month)
{
	return Month[month];
}










