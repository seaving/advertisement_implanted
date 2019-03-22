#include "includes.h"

int main()
{

	SERVER *server = init_server(SERVER_PORT, 
							WORKER_NUM, 
							CONN_NUM, 
							SOCKET_TIMEOUT_S,
						SOCKET_TIMEOUT_S);
	if (server == NULL)
		return -1;

	start_server((void*) server);
	
	return 0;
}


