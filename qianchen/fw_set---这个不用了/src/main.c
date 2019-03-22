/**
fw_set 模块: 
负责防火墙的设置
*/

#include "includes.h"

int main()
{
	signal(SIGPIPE, SIG_IGN);

	//初始化共享内存
	dev_share_init();

	//初始化消息队列
	InitMsgQueue();

	//初始化防火墙
	fw_init();
	
    while (1)
	{
	    // 消息处理
	    // 消息的类型有:其他模块对调度中心、手柄查询、设置的响应
	    char recvbuf[MAX_MESSAGE_LEN + 1]= {0};
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

