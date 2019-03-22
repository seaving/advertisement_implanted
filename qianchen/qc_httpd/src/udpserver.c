#include "includes.h"

void *create_udp_server(void *arg)
{
	pthread_detach(pthread_self());
	
	int usfd = open_udp_server(AC_SERVER_PORT);
	if (usfd < 0)
		return NULL;

	struct sockaddr_in client;

	for ( ; ; )
	{
		char buf[RECV_SIZE_MAX + 1] = {0};
		int ret = udp_select_recv(usfd, buf, RECV_SIZE_MAX, &client, 3);
		if (ret < 0)
			break;
		else if (ret == 0)
			continue;
		else
		{
			//处理数据
			ac_process(usfd, &client, buf, ret);
		}
	}

	close(usfd);
	
    return NULL;
}

int create_udp_server_thread()
{
	pthread_t tdp;
	return pthread_create(&tdp, NULL, create_udp_server, NULL);
}

