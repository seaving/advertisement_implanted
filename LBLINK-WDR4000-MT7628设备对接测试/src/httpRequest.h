#ifndef __HTTP_REQUEST_H__
#define __HTTP_REQUEST_H__

#include "datainfo.h"

typedef struct 
{
	data_t *host;
	data_t *uri;
	data_t *method;
	data_t *port;
} request_t;


int free_request(request_t *request);
request_t * init_request();


#endif

