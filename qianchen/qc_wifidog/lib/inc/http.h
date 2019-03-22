#ifndef __HTTP_H__
#define __HTTP_H__

typedef enum
{
	E_GET = 0,
	E_POST,
	E_UNKOWN
} eMethod_t;

int web_302_jump(int client_sock, char * to_url);
int read_method(int phonefd, char *buf);
int read_url(int phonefd, char *buf);
int analysis_post(int phonefd, char *header);
int analysis_get(int phonefd, char *header);

eMethod_t httpmethod_cmp(char *method);

#endif


