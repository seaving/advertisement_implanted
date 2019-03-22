#ifndef __HTTP_REQUEST_H__
#define __HTTP_REQUEST_H__

#define HEADER_PATH_LEN		255
#define HEADER_COOKIE_LEN	255
#define POST_CONTENT_LEN	255

#if 0
typedef enum
{
	E_GET = 0,
	E_POST,
	E_UNKOWN
} eMethod_t;


int read_method(int phonefd, char *buf);
int read_url(int phonefd, char *buf);
int analysis_post(int phonefd);
int analysis_get(int phonefd);

eMethod_t httpmethod_cmp(char *method);
#endif
#endif


