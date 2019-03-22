
#include "includes.h"

int check_uri_accept(char *uri, int len)
{
	if (uri == NULL)
	{
		return 0;
	}

	int accept_flag = 0;
	int i = 0;
	int uri_len = len;
	if (uri_len <= 0)
	{
		return 0;
	}
	
	for (i = 0; auconfig[i].uri != NULL; i ++)
	{
		//if (auconfig[i].len == len)
		{
			if (isStrnexit(uri, uri_len, auconfig[i].uri, auconfig[i].len))
			{
				accept_flag = 1;
				break;
			}
		}
	}
	if (auconfig[i].uri == NULL)
	{
		accept_flag = 0;
	}

	return accept_flag;
}

void init_uri_config()
{
	int i;
	for (i = 0; auconfig[i].uri != NULL; i ++)
	{
		auconfig[i].len = strlen(auconfig[i].uri);
	}
}

