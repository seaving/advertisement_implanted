#include "includes.h"

int create_udp_client(char *ser_ip, int ser_port)
{
	struct sockaddr_in server;
	int cfd = open_udp_client_s(ser_ip, ser_port, &server, AP_CLIENT_PORT);
    if (cfd < 0)
		return -1;

	int index = 0;

	while (1)
	{
		char buf[RECV_SIZE_MAX] = {0};

		sprintf(buf, "hello--- %d", index ++);
		udp_send(cfd, buf, strlen(buf), &server);

		struct sockaddr_in from;
		memset(&from, 0, sizeof(struct sockaddr_in));
		memset(buf, 0, RECV_SIZE_MAX);

		int ret = udp_select_recv(cfd, buf, RECV_SIZE_MAX, &from, 3);
		if (ret < 0)
		{
			LOG_ERROR_INFO("recv socket error!\n");
			break;
		}
		else if (ret == 0)
		{
			LOG_ERROR_INFO("recv ack timeout!\n");
			continue;
		}
		else
		{
			LOG_NORMAL_INFO("%s\n", buf);
			char *p = strchr(buf, ' ');
			if (p)
			{
				p ++;
				if ((index - 1) != atoi(p))
				{
					LOG_ERROR_INFO("drop package!\n");
					break;
				}
				break;
			}
			else
			{
				LOG_ERROR_INFO("ack error!\n");
				break;
			}
		}
	}

	while (1)
	{
		char buf[RECV_SIZE_MAX] = {0};

		struct sockaddr_in from;
		memset(&from, 0, sizeof(struct sockaddr_in));
		memset(buf, 0, RECV_SIZE_MAX);

		int ret = udp_select_recv(cfd, buf, RECV_SIZE_MAX, &from, 3);
		if (ret < 0)
		{
			LOG_ERROR_INFO("recv socket error!\n");
			break;
		}
		else if (ret == 0)
		{
			LOG_ERROR_INFO("recv ack timeout!\n");
			continue;
		}
		else
		{
			LOG_NORMAL_INFO("%s\n", buf);
		}
	}

	close(cfd);
	
    return 0;
}

