#ifndef __JSINJECT_H__
#define __JSINJECT_H__


int find_http_body(char * psrc, int src_len);


int js_inject2(int sockfd, char **psrc, int *srclen, char *url, int url_len, 
	char *title, int title_len, char *keyword, int keyword_len, char jsin_flag, char *policy_flag);
int js_inject3(int sockfd, char **psrc, int *srclen, char *url, int url_len, 
	char *title, int title_len, char *keyword, int keyword_len, char jsin_flag, char *policy_flag);

#endif

