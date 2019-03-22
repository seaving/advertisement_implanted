#include "includes.h"

int main(int argc, char *argv[])
{
	//char ip[32] = {0};
	//while (0)
	//{
		//get_dev_ip(ip, LAN_DEV);
		//if (strlen(ip) > 0)
			//break;
		//sleep(5);
	//}

	//create_net_dev_monitor_thread();
	
#if 1
	create_udp_server();
#else
	//memset(ip, 0, sizeof(ip));
	//get_gateway(ip);
	create_udp_client("120.77.149.125", AC_SERVER_PORT);
#endif
	return 0;
}


