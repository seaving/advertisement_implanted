#include "includes.h"

int menu()
{
	system("clear");
	printf("1. send 'wifi enable'\n");
	printf("2. send 'wifi disable'\n");
	printf("3. send 'firewall enable'\n");
	printf("4. send 'firewall disable'\n");
	char select[3] = {0};
	scanf("%s", select);
	return atoi(select);
}

/**
AC设置AP通过广播
AC单独设置AP通过UDP
*/

int data_process(int ufd, struct sockaddr_in *from, char *buf, int len)
{
	LOG_HL_INFO("len: %d, buf: %s\n", len, buf);

	//udp_send_to(ufd, buf, len, UDP_BROADCAST, AP_CLIENT_PORT);
	//udp_send_to(ufd, buf, len, "134.227.255.255", AP_CLIENT_PORT);
	udp_send(ufd, buf, len, from);

	for ( ; ; )
	{
		char *s = NULL;
		switch (menu())
		{
			case 0: s = "wifi enable"; break;
			case 1: s = "wifi disable"; break;
			case 2: s = "firewall enable"; break;
			case 3: s = "firewall disable"; break;
			default: s = "error input"; break;
		}

		udp_send(ufd, s, strlen(s), from);
	}
	
	return 0;
}

