#include "includes.h"

data_t * init_data()
{
	data_t *data = calloc(1, sizeof(data_t));
	if (! data)
	{
		LOG_PERROR_INFO("calloc error.");
		return NULL;
	}

	return data;
}

int calloc_buf(data_t *data, int bufsize)
{
	if (! data)
		return -1;
	if (data->buf)
		return 0;
		
	data->buf = calloc(1, bufsize);
	if (! data->buf)
	{
		LOG_PERROR_INFO("calloc error.");
		return -1;		
	}
	data->datalen = 0;
	data->bufsize = bufsize;

	return 0;
}

int free_data(data_t *data)
{
	if (! data)
		return -1;

	if (data->buf)
		free (data->buf);
		
	data->buf = NULL;
	data->bufsize = 0;
	data->datalen = 0;
	
	free (data);
	data = NULL;

	return 0;
}

