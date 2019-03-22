#include "includes.h"

#if 1
int main(int args, char *argv[])
{
	signal(SIGPIPE, SIG_IGN);

	common_setup();
	
	time_init();

	//初始化共享内存
	dev_share_init();

	//初始化消息队列
	InitMsgQueue();
	
	auth_client_init();

	while (strlen(GET_LAN_NAME) <= 0) sleep(10);
	while (strlen(GET_DEV_NO) <= 0) sleep(10);

	create_server();

	//设置防火墙初始化
	fw_init();

	create_auth_client_scan_thread();
	
	for ( ; ; )
	{
	    // 消息处理
	    char recvbuf[MAX_MESSAGE_LEN + 1] = {0};
		int ret = RecvFwSetMsg(recvbuf);
		if (ret > 0)
		{
            // 消息处理
            LOG_NORMAL_INFO("recv data, len = %d\n", ret);
			OnMessage((unsigned char *)recvbuf, ret);
		}
	}
	
	return 0;
}

#endif



