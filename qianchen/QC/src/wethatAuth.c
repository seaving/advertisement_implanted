//#include "httpsclient.h"


#include "includes.h"


int check_from_wethat(char *header, int header_len)
{
	char subbuf[256];
	int len = cutstr(header, header_len, "\r\nUser-Agent:", "\r\n", subbuf, 255);
	if (len <= 0)
		return 0;

	subbuf[len] = '\0';
	
	if (isStrnexit(subbuf, len, "micromessenger", 14))
		return 1;
	return 0;
}


