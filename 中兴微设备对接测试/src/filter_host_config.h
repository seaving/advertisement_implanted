#ifndef __FILTER_HOST_CONFIG_H__
#define __FILTER_HOST_CONFIG_H__	1

typedef struct FILTER_HOST_CONFIG_s
{
	char *filter_host;
	int len;
}filter_host_conf;

int fhconfig_cmp(char *host, int len);
void init_fhconfig();

#endif

