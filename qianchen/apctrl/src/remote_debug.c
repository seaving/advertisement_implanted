#include "includes.h"

/*************************************
	远程控制，利用ssh反向隧道实现
route:
cmd:
	ssh -fCNR <port_server_1>:localhost:22 user_server@IP_server
说明:
	port_server_1: 服务器上的端口，用于与路由器的22端口版定
	user_server: 服务器上ssh帐户
	IP_server: 服务器IP
例子:
	ssh -fCNR 8989:localhost:22 root@120.77.149.125

server:
cmd:
	ssh -fCNL *:<port_server_2>:localhost:<port_server_1> localhost
说明:
	port_server_2: 服务器上本地转发端口与外网通信，并将数据转发到port_server_1
	* : 表示接收任意机器访问
例子:
	ssh -fCNL *:9090:localhost:8989 localhost

控制者:
cmd:
	ssh -p <port_server_2> user_route:IP_server
说明:
	user_route: 路由器上ssh帐户
例子:
	ssh -p 9090 root@120.77.149.125
	
	
**************************************/


int router_ssh_to_server(char *server_ip, int server_port, char *user_server)
{
	char cmd_buf[255] = {0};
	EXECUTE_CMD(cmd_buf, "ssh -fCNR %d:localhost:22 %s@%s", server_port, user_server, server_ip);
	return 0;
}


