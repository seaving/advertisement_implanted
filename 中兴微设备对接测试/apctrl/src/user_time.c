#include "includes.h"

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






















