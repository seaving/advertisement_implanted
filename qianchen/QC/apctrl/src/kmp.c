#include "includes.h"

void get_nextval(char * ptn, int ptn_len, int * nextval)
{
	int i, j;

	nextval[0] = -1;
	for (i = 1; i < ptn_len; i ++)
	{
		j = nextval[i - 1];
		while (ptn[j + 1] != ptn[i] && (j >= 0))
		{
			j = nextval[j];
		}
		if (ptn[i] == ptn[j + 1])
		{
			nextval[i] = j + 1;
		}
		else
		{
			nextval[i] = -1;
		}
	}
}

int kmp_search(char * src, int src_len, char * patn, int patn_len, int const * nextval)
{
	int s_i = 0, p_i = 0;

	while (s_i < src_len && p_i < patn_len)
	{
		if (src[s_i] == patn[p_i])
		{
			s_i ++;
			p_i ++;
		}
		else
		{
			if (p_i == 0)
			{
				s_i++;
			}
			else
			{
				p_i = nextval[p_i - 1] + 1;
			}
		}
	}
	return (p_i == patn_len) ? (s_i - patn_len) : -1;
}

