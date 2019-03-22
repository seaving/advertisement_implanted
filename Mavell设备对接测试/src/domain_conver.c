#include "includes.h"


int domain_conver_url(int client_socket, char * url)
{
	char *c_url;
	char *p = straddr(url, "http://");
	if (!p)
	{
		p = straddr(url, "https://");
		if (p)
		{
			p += strlen("https://");
		}
	}
	else
	{
		p += strlen("http://");
	}
	if (!p)
	{
		p = url;
	}

	int len = strlen(p);
	char s_url[len + 1];
	memcpy(s_url, p, len);
	s_url[len] = 0;
	if (s_url[len - 1] == '/')
	{
		s_url[len - 1] = 0;
	}
	c_url = get_conver_url(s_url);
	if (c_url)
	{
		LOG_HL_INFO("conver url : %s ---> %s\n", url, c_url);
		web_302_jump(client_socket, c_url);
	}
	else
	{
		return -1;
	}
	
	return 0;
}

