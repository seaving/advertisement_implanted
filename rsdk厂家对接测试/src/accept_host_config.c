#include "includes.h"

int check_domain_accept(char *host, int len)
{
	if (host == NULL)
	{
		return 0;
	}

	int accept_flag = 0;
	int i = 0;
	int host_len = len;
	if (host_len <= 0)
	{
		return 0;
	}
	
	for (i = 0; ahconfig[i].host != NULL; i ++)
	{
		//if (ahconfig[i].len == len)
		{
			if (isStrnexit(host, host_len, ahconfig[i].host, ahconfig[i].len))
			{
				accept_flag = 1;
				break;
			}
		}
	}
	if (ahconfig[i].host == NULL)
	{
		accept_flag = 0;
	}

	return accept_flag;
}

void init_host_config()
{
	int i;
	for (i = 0; ahconfig[i].host != NULL; i ++)
	{
		ahconfig[i].len = strlen(ahconfig[i].host);
	}
}

