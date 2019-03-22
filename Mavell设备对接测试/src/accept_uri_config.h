#ifndef __ACCEPT_URI_CONFIG_H__
#define __ACCEPT_URI_CONFIG_H__	1

typedef struct {
	char *uri;
	int len;
}accept_uri_conf;


int check_uri_accept(char *host, int len);
void init_uri_config();


#endif

