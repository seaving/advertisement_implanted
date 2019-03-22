#ifndef __JSINJECT_H__
#define __JSINJECT_H__	1

#define URL_TOKEN		"ly_89cd60ed948b4=bf4a7ecee79fe"
#define URL_TOKEN_LEN	30

#define JS_HOST		" *.lynew.com "

int find_http_body(char * psrc, int src_len);

int js_inject(int sockfd, char **psrc, int *srclen, char *url, int url_len, 
	char *title, int title_len, char *keyword, int keyword_len, int jsin_flag, int *policy_flag);

#endif

