#include "includes.h"

int init_firewall()
{
#if 0
	//之前是做了认证建立了防火墙
	FILE *fp = NULL;

	char cmd[255];
	sprintf(cmd, "iptables -t mangle -nL seaving | wc -l");
	LOG_HL_INFO("%s\n", cmd);
	fp = popen(cmd, "r");
	if (NULL == fp)	
	{  
		LOG_PERROR_INFO("popen error!");  
		return -1;
	}

	char buf[20] = {0};
	while(fgets(buf, 20, fp) != NULL)
	{
		LOG_HL_INFO("%s", buf);
		//break;
	}
	pclose(fp);
	int i;
	for (i = 0; buf[i] != '\0'; i ++)
	{
		if (buf[i] == '\r'
		|| buf[i] == '\n')
		{
			buf[i] = '\0';
			break;
		}
	}	
	if (atoi(buf) <= 0)
	{
		return -1;
	}
#endif
	system("/etc/init.d/firewall restart");
	return 0;
}

int start_local_program()
{
	//初始化防火墙,目的是为了尽快让用户能恢复上网
	init_firewall();

	//kill掉老程序
	system("killall "MONITOR_PROGRAM);

	//检测是否存在程序
	if ((access(FILE_PATH"/"MONITOR_PROGRAM, F_OK)) == 0)
	{
		//执行程序
		system(FILE_PATH"/"MONITOR_PROGRAM" &");
		return 0;
	}

	//未检测到有程序存在
	return -1;
}

extern int restart_flag;
int main(void)
{
	int cur_time = 0;
	int old_time = 0;
	int download_flag = 0;
	
	signal(SIGPIPE, SIG_IGN);

	//初始化WAN/LAN口
	while (init_netdev() < 0)
	{
		LOG_ERROR_INFO("init_netdev is failt!\n");
		sleep(3);
	}
	LOG_HL_INFO("[%s] ip: %s\n", get_netdev_wan_name(), get_netdev_wan_ip());

	//同步时间
	int year = 0;
	while (year < 16)
	{
		LOG_WARN_INFO("wait system time sync ...\n");
		GetSystemTime(&year, NULL, NULL, NULL, NULL);
		sleep(3);
	}

//时间同步成功,重新创建记录后面的日志
#if LOG_SAVE_ON
	dup_file();
#endif

	//监控线程
	create_monitor_pid_thread();

	while (1)
	{
		cur_time = GetSystemTime_Sec();
		//每个一个小时更新一次，或者更新标记开启也进行更新
		if (/*download_flag || */abs(cur_time - old_time) > 1 * _1_HOUR_)
		{
			//不管是不是由时间触发还是由download_flag触发
			//都要对download_flag清空
			download_flag = 0;

			//赋值当前时间,便于下次计数
			old_time = cur_time;

			//开始请求更新，返回0表示无须更新，1表示更新成功，<0表示更新出错
			int ret = RequestUpdate();
			if (ret > 0)
			{
				//更新成功，准备重新启动新的更新包
				LOG_SAVE("update success, restart program .\n");
				restart_flag = 1;
				sleep(1);
			}
			else if (ret == 0)
			{
				//无须更新,等1小时候再次更新
				download_flag = 0;
			}
			else
			{
				//更新失败,隔小段时间间隔再次去请求
				download_flag = 1;
			}
		}

		//更新成功后，需要kill掉老程序，重新运行新程序
		if (restart_flag)
		{
			//只运行一次
			restart_flag = 0;

			//等待监控线程退出监控模式
			wait_monitor_exit();
			
			LOG_SAVE("mproxy_arm process restart ... \n");

			//启动程序
			if (start_local_program() < 0)
			{
				//启动失败，不存在该程序
				LOG_ERROR_INFO("local program isnot exsiting !\n");

				//启动失败后，清除版本号，版本号用于后台对比，如果未清空
				//可能造成后台认为路由器已经是最新程序，不会下发更新包
				memset(version, 0, sizeof(version));

				//启动下载
				download_flag = 1;
			}
			else
			{
				//开始监控
				monitor_send_cond();
			}
			
			sleep(1);
		}
		
		sleep(3);
	}

	return 0;
}



