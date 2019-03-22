#include "includes.h"


int fhconfig_cmp(char *host, int len)
{
	int i = 0;
	for (i = 0; fhconfig[i].filter_host != NULL; i ++)
	{
		if (isStrnexit(host, len, fhconfig[i].filter_host, fhconfig[i].len))
		{
			return 0;
		}
	}

	return -1;
}

void init_fhconfig()
{
	int i = 0;
	for (i = 0; fhconfig[i].filter_host != NULL; i ++)
	{
		fhconfig[i].len = strlen(fhconfig[i].filter_host);
	}
}

