#ifndef __HTTP_H__
#define __HTTP_H__

#include "common.h"

#define MAXBUF	8192


typedef enum
{
	E_GET = 0,
	E_POST,
	E_UNKOWN
} eMethod_t;

void struct_http_header(char * method, char * path, char * host, int port, char *data, char * header_buff);
bool getRequestParma(char *data, char *parma_name, char *value, int value_size);
int analysis_host_uri_port(char *header, int header_len, char *host, int host_size, char *uri, int uri_size);
int url_add_param(char *param_name, char *param_arg, char *addr);

int web_302_jump(int client_sock, char * to_url);
int read_method(int phonefd, char *buf);
int read_url(int phonefd, char *buf);

eMethod_t httpmethod_cmp(char *method);

#endif


