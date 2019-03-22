#include "includes.h"



static pthread_mutex_t mut = PTHREAD_MUTEX_INITIALIZER;
static pthread_cond_t cond = PTHREAD_COND_INITIALIZER;

void monitor_wait_cond()
{
	pthread_cond_wait(&cond, &mut);
}

void monitor_send_cond()
{
	pthread_mutex_lock(&mut);
	pthread_cond_signal(&cond);
	pthread_mutex_unlock(&mut);
}

int getPid(char *pro)
{
	FILE *fp = NULL;

	char cmd[255];
	sprintf(cmd, "ps | grep \"%s\"", pro);
	LOG_HL_INFO("%s\n", cmd);
	fp = popen(cmd, "r");
	if (NULL == fp)	
	{  
		LOG_PERROR_INFO("popen error!");  
		return -1;
	}

	char buf[512] = {0};
	while(fgets(buf, 255, fp) != NULL)
	{
		LOG_HL_INFO("%s", buf);
		if (isStrexit(buf, "grep"))
			continue;

		char tmp[255] = {0};
		if (awk(buf, " ", 0, tmp) >= 0)
		{	
			pclose(fp);
			return atoi(tmp);
		}
		
		//break;
	}
	pclose(fp);
	return -1;
}

int MonitorProcess(int pid)
{	
	char path[64] = {0};
	sprintf(path, "/proc/%d", pid);
	DIR* dir = opendir(path);
	if (dir == NULL)
	{
		LOG_ERROR_INFO("PID: %d , was exit !\n", pid);
		return -1;
	}
	closedir(dir);
	dir = NULL;
	return 0;
}

int monitor_runflag = 0;
void wait_monitor_exit()
{
	//1表示监控程序还在监控中
	if (monitor_runflag == 1)
	{
		//该标记是监控程序的for条件
		//清空为0，for退出
		monitor_runflag = 0;

		//2表示for退出了
		//等待监控退出
		while (monitor_runflag != 2)
		{
			sleep(1);
		}		
	}
}

int restart_flag = 0;
void *monitor_pid(void *arg)
{
	while (1)
	{
		monitor_wait_cond();

		//先获取进程的pid
		int pid = getPid(MONITOR_PROGRAM);
		if (pid < 0)
		{
			//获取失败，说明程序未跑起来
			LOG_ERROR_INFO("getPid failt .. !\n");

			//设置标记位，1表示需要重新跑程序, 0表示不需要重新跑程序
			restart_flag = 1;

			//回到monitor_wait_cond，待主线程启动监控命令
			continue;
		}

		LOG_WARN_INFO("Begin monitor ... \n");
		monitor_runflag = 1;

		//开始监控线程状态
		for ( ; monitor_runflag && MonitorProcess(pid) >= 0; )
			sleep(3);
		
		monitor_runflag = 2;
		
		LOG_SAVE(MONITOR_PROGRAM" exit !!!\n");

		//走到这里说明进程退出，需要重新设标记，告知主线程重新跑程序
		restart_flag = 1;
	}

	return NULL;
}

int create_monitor_pid_thread()
{
	pthread_t thread;
	if (pthread_create(&thread, NULL, monitor_pid, NULL) != 0)
	{
		LOG_PERROR_INFO("pthread_create error.");
		return -1;
	}
	return 0;
}

