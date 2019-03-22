#ifndef __S_CLIENT_LIST_H__
#define __S_CLIENT_LIST_H__

typedef struct s_cli_list
{
	struct sockaddr_in client;
	char ip[32];
	char mac[32];
	int sockfd;
} sclist_t;

#endif

