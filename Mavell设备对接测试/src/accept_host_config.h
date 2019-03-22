#ifndef __ACCEPT_HOST_CONFIG_H__
#define __ACCEPT_HOST_CONFIG_H__	1
typedef struct {
	char *host;
	int len;
}accept_host_conf;

int check_domain_accept(char *host, int len);
void init_host_config();

#endif

