#ifndef __HTTP_COMMON_H__
#define __HTTP_COMMON_H__

#include "debug.h"


#define	MAXLINE	 8192  /* max text line length */
#define MAXBUF   8192  /* max I/O buffer size */

#define get_request_parma(context, parma_name, value, size) { \
	if (! getRequestParma(context, parma_name, value, size)) \
	{ \
		LOG_ERROR_INFO("parma_name: %s get failt!\n", parma_name); \
		return -1; \
	} \
}


bool getRequestParma(char *data, char *parma_name, char *value, int value_size);
int url_add_param(char *param_name, char *param_arg, char *addr);
int http_send_post_data(char *host, char *path, int port, char *data, char *content_type, char *response, int response_size);


#endif

