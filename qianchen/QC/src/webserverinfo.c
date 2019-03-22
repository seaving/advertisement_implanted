#include "includes.h"


webserverinfo_t * init_webserver()
{
	webserverinfo_t *webserver = calloc(1, sizeof(webserverinfo_t));
	if (! webserver)
	{
		LOG_PERROR_INFO("calloc error.");
		return NULL;
	}

	webserver->respons = init_data();
	if (! webserver->respons)
	{
		LOG_ERROR_INFO("init data error!\n");
		free (webserver);
		return NULL;
	}
		
	webserver->swap_buf = init_data();
	if (! webserver->swap_buf)
	{
		LOG_ERROR_INFO("init data error!\n");
		free (webserver->respons);
		free (webserver);
		return NULL;
	}

	if (calloc_buf(webserver->swap_buf, MAXBUF+1) < 0)
	{
		LOG_ERROR_INFO("webserver swap buf calloc error!\n");
		free (webserver->respons);
		free (webserver->swap_buf);
		free (webserver);

		return NULL;
	}

	webserver->opt = E_READ_RESPONES_HEADER;
	webserver->mitm = E_ACCEPT;
	webserver->jsin_flag = 1;
	webserver->policy_flag = 1;
	return webserver;
}

int free_webserver_member(webserverinfo_t *webserver)
{
	if (! webserver)
		return -1;

	if (webserver->respons)
		free_data(webserver->respons);

	if (webserver->swap_buf)
		free_data(webserver->swap_buf);

	webserver->phonefd = -1;
	webserver->serverfd = -1;
	webserver->swap_buf = NULL;
	webserver->respons = NULL;
	webserver->processflag = 1;
	
	return 0;
}

int free_webserver(webserverinfo_t *webserver)
{
	if (! webserver)
		return -1;

	if (webserver->respons)
		free_data(webserver->respons);

	if (webserver->swap_buf)
		free_data(webserver->swap_buf);

	webserver->phonefd = -1;
	webserver->serverfd = -1;
	webserver->swap_buf = NULL;
	webserver->respons = NULL;

	free (webserver);
	webserver = NULL;
	
	return 0;

}
