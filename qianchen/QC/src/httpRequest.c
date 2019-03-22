#include "includes.h"

request_t * init_request()
{
	request_t * request = calloc(1, sizeof(request_t));
	if (! request)
	{
		LOG_PERROR_INFO("calloc error.");
		return NULL;
	}
	request->host = init_data();
	if (! request->host)
	{
		LOG_ERROR_INFO("request->host init error!\n");
		free_request(request);
		return NULL;
	}
	request->uri = init_data();
	if (! request->uri)
	{
		LOG_ERROR_INFO("request->uri init error!\n");
		free_request(request);
		return NULL;
	}	
	request->method = init_data();
	if (! request->method)
	{
		LOG_ERROR_INFO("request->method init error!\n");
		free_request(request);
		return NULL;
	}
	request->port = init_data();
	if (! request->port)
	{
		LOG_ERROR_INFO("request->port init error!\n");
		free_request(request);
		return NULL;
	}	
	return request;
}

int free_request(request_t *request)
{
	if (! request)
		return -1;

	free_data(request->host);
	free_data(request->uri);
	free_data(request->method);
	free_data(request->port);
	free_data(request->userAgent);
	free (request);
	request = NULL;
	
	return 0;
}

