/**
init 模块: 
负责软件更新
负责启动其他程序: ssh(消息转发)，qchttp(web服务)

init和qchttpd模块不存在共享内存，只有消息队列

*/

#include "includes.h"

void start_program(char *program)
{
	char cmd[255];
	EXECUTE_CMD(cmd, "%s &", program);
}

int main()
{
	signal(SIGPIPE, SIG_IGN);

	time_init();

	init_netdev();

	//初始化消息队列
	InitMsgQueue();

	//开始启用其他进程
	start_program("killall qchttpd;/etc/app/qchttpd");

	//获取devNo
	request_dev_id_from_server_thread();

	//创建update线程
	create_update_thread();
	
    while (1)
	{
	    // 消息处理
	    char recvbuf[MAX_MESSAGE_LEN + 1]= {0};
		int ret = RecvInitMsg(recvbuf);
		if (ret > 0)
		{
            // 消息处理
			OnMessage((unsigned char *)recvbuf, ret);
		}
    }
	
	return 0;
}

