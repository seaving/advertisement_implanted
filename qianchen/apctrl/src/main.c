/**
apctrl 模块: 
负责软件更新
负责启动其他程序: ssh(消息转发)，qchttp(web服务)

init和qchttpd模块不存在共享内存，只有消息队列

init开机运行:
下载包，解压包，运行脚本

脚本工作:
运行 apctrl以及其他的程序

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
	
	//初始化设备相关共享内存
	if (dev_share_init() < 0)
		exit(-1);

	//初始化消息队列
	InitMsgQueue();

    while (1)
	{
	    // 消息处理
	    // 消息的类型有:其他模块对调度中心、手柄查询、设置的响应
	    char recvbuf[MAX_MESSAGE_LEN + 1]= {0};
		int ret = RecvApctrlMsg(recvbuf);
		if (ret > 0)
		{
            // 消息处理
			OnMessage((unsigned char *)recvbuf, ret);
		}
    }
	
	return 0;
}

