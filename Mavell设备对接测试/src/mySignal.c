#include "includes.h"

void signal_exit(int signum)
{     
    int i;
    //发送给子进程退出信号
    for (i = 0; i < PROCESS_NUM; i ++)
    {
    	if (g_ProcessArr[i] > 0)
    	{
        	printf("kill pid:%d\n", g_ProcessArr[i]);
      		kill(g_ProcessArr[i], SIGTERM);
		}
    }
  //pause();
	exit(0);
}


int signal_init()
{
    //忽略该信号，对已关闭的socket两次write会产生该信号
    //如果不加此句，程序遇到此信号 会奔溃
    signal_task_exit();
	signal_sigpipe();
    return 0;
}



