#include "includes.h"

int MonitorProcess()
{
	FILE *fp = NULL;

	char cmd[255];
	sprintf(cmd, "ps | grep \"mproxy_arm\" | wc -l");
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

	//我们在终端执行ps | grep \"mproxy_arm\" | wc -l时候打印出来的数据
	//和popen执行的不一样，popen执行的要多出1
	if (atoi(buf) <= 2)
	{
		return -1;
	}
	return 0;
}

int init_firewall()
{
#if 1
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
	init_firewall();
	system("killall mproxy_arm");
	if ((access(FILE_PATH"/mproxy_arm", F_OK)) == 0)
	{
		system(FILE_PATH"/mproxy_arm &");
		return 0;
	}
	return -1;
}

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

	while (1)
	{
		cur_time = GetSystemTime_Sec();
		if (download_flag || abs(cur_time - old_time) > UPDATE_TIME)
		{
			download_flag = 0;
			old_time = cur_time;
			int ret = RequestUpdate();
			if (ret > 0)
			{
				LOG_SAVE("update success, restart program .\n");
				if (start_local_program() < 0)
				{
					//启动失败，不存在该程序
					LOG_ERROR_INFO("local program isnot exsiting !\n");
				
					//清除版本号
					memset(version, 0, sizeof(version));
					
					//启动下载
					download_flag = 1;
				}
				sleep(1);
			}
			else if (ret == 0)
			{
				//无须更新
				download_flag = 0;
			}
			else
			{
				//更新失败
				download_flag = 1;
			}
		}
		if (MonitorProcess() < 0)
		{
			LOG_SAVE("mproxy_arm process is exit!\n");

			//启动程序
			if (start_local_program() < 0)
			{
				//启动失败，不存在该程序
				LOG_ERROR_INFO("local program isnot exsiting !\n");

				//清除版本号
				memset(version, 0, sizeof(version));

				//启动下载
				download_flag = 1;
			}
			sleep(1);
		}
		sleep(3);
	}

	return 0;
}



