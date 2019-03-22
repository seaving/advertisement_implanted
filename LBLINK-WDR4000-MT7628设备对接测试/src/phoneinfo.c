#include "includes.h"

phoneinfo_t * init_phoneinfo()
{
	phoneinfo_t *phone = calloc(1, sizeof(phoneinfo_t));
	if (! phone)
	{
		LOG_PERROR_INFO("calloc error.");
		return NULL;
	}

	phone->request = init_request();
	if (! phone->request)
	{
		free (phone);
		return NULL;
	}	
	phone->swap_buf = init_data();
	if (! phone->swap_buf)
	{
		free (phone->request);
		free (phone);
		return NULL;
	}	
	calloc_buf(phone->swap_buf, MAXBUF+1);
	phone->swap_buf->bufsize = MAXBUF + 1;
	phone->swap_buf->datalen = 0;

	phone->opt = E_READ_METHOD;
	
	return phone;
}

int httpmethod_cmp(phoneinfo_t *phone)
{
	if (! phone)
		return -1;

	if (0 == strcmp("GET", phone->request->method->buf))
	{
		phone->method = E_GET;
		return 0;
	}
	if (0 == strcmp("POST", phone->request->method->buf))
	{
		phone->method = E_POST;
		return 0;
	}
	if (0 == strcmp("HEAD", phone->request->method->buf))
	{
		phone->method = E_HEAD;
		return 0;
	}
	if (0 == strcmp("CONNECT", phone->request->method->buf))
	{
		phone->method = E_CONNECT;
		return 0;
	}
	if (0 == strcmp("OPTIONS", phone->request->method->buf))
	{
		phone->method = E_OPTIONS;
		return 0;
	}
	return -1;
}

int free_phoneinfo_member(phoneinfo_t *phone)
{
	if (! phone)
		return -1;

	if (phone->request)
		free_request(phone->request);

	if (phone->swap_buf)
		free_data(phone->swap_buf);

	phone->fd = -1;
	phone->swap_buf = NULL;
	phone->request = NULL;
	phone->processflag = 1;
	
	return 0;
}

int free_phoneinfo(phoneinfo_t *phone)
{
	if (! phone)
		return -1;

	if (phone->request)
		free_request(phone->request);

	if (phone->swap_buf)
		free_data(phone->swap_buf);

	phone->fd = -1;
	phone->swap_buf = NULL;
	phone->request = NULL;

	free (phone);
	phone = NULL;
	
	return 0;
}

