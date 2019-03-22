#ifndef __WEBSERVERINFO_H__
#define __WEBSERVERINFO_H__	1

#include "phoneinfo.h"

//typedef struct phoneinfo phoneinfo_t;

typedef enum
{
	E_READ_RESPONES_HEADER = 0,
	E_READ_CONTEXT,
	E_WEBSERVER_FINISHED
} webserver_opt_t;

typedef enum
{
	E_ACCEPT = 0,
	E_ATTACK
} mitm_t;

typedef enum
{
	E_STATIC = 0,
	E_CHUNKED,
	E_LOCATION,
	E_UNKOWN_PAGE
} page_t;

typedef struct webserverinfo
{
	int phonefd;
	int serverfd;
	data_t *swap_buf;
	data_t *respons;
	webserver_opt_t opt;
	mitm_t mitm;
	int phone_epollfd;
	int server_epollfd;
	page_t pagetype;
	int tmp;
	int flag;
	int jsin_flag;
	int policy_flag;
	struct phoneinfo/*phoneinfo_t*/ *phone;
	int time;
	int processflag;
} webserverinfo_t;

webserverinfo_t * init_webserver();
int free_webserver(webserverinfo_t *webserver);
int free_webserver_member(webserverinfo_t *webserver);

#endif
