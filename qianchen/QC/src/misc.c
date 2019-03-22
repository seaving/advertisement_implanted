#include "includes.h"

int monitor_wifi_state();
void com_server();
void reboot_program();
int CheckDDRSizeWarn();


void *misc(void *arg)
{
	pthread_detach(pthread_self());

	while (1)
	{
		/**监控内存使用情况*/
		CheckDDRSizeWarn();
		
		/**监控wan口的ip情况*/
		if (monitor_wifi_state(get_listenfd()) < 0)
		{
			break;
		}

		/**communication with server*/
		com_server();

		/**reboot program by mitm_monitor.sh*/
		reboot_program();
		
		/**monitor server sub process running state*/
		monitor_subprocess();
		
		sleep(1);
	}

	return (void *)0;
}

int monitor_wifi_state()
{
	char wan_ip[20] = {0};
	int changed = 0;
	static int change_cnt = 0;
	
	int timeout = 10;
	int timecmp = 0;
	int now_time_sec = 0;
	static int old_time_sec = 0;

	GetAllSystemTime(NULL, NULL, NULL, NULL, NULL, &now_time_sec, NULL);
	if (now_time_sec >= old_time_sec)
	{
		timecmp = now_time_sec - old_time_sec;
	}
	else {
		timecmp = 60 - old_time_sec + now_time_sec;
	}

	if (timecmp < timeout)
	{
		return 0;
	}

	old_time_sec = now_time_sec;

	memset(wan_ip, 0, 20);
	if (get_dev_ip(wan_ip, GET_WAN_NAME) < 0)
	{
		LOG_WARN_INFO("get wlan0 device ip error!!!\n");
		changed = 1;		
	}
	else
	{
		char ip[32] = {0};
		get_dev_ip(ip, GET_WAN_NAME);
		if (strcmp(wan_ip, ip))
		{
			LOG_WARN_INFO("[%s] state was changed!!! new:%s old:%s\n", GET_WAN_NAME, wan_ip, ip);
			changed = 1;
		}
	}

	if (changed)
	{
		change_cnt ++;
		printf(COLOR_RED "[%s] check ip error ... %d cnt!\n" COLOR_NONE_C, GET_WAN_NAME, change_cnt);
	}
	else
	{
		change_cnt = 0;
	}
	if (change_cnt > 5)
	{
		printf(COLOR_RED "[%s] check ip continuous error too many times!\n" COLOR_NONE_C, GET_WAN_NAME);
		change_cnt = 0;

		//退出程序,待脚本重新启动
		LOG_WARN_INFO("exit mitm-attack program... \n");
		_exit_(-1);
	}
	LOG_HL_INFO("wifi is connecting...OK\n");
	return 0;
}

void com_server()
{
	int now_time_hour = 0;
	static int old_time_hour = -1;
	static int diff_cnt = -1;
	GetAllSystemTime(NULL, NULL, NULL, &now_time_hour, NULL, NULL, NULL);
	if (now_time_hour != old_time_hour)
	{
		diff_cnt ++;
		old_time_hour = now_time_hour;
	}
	else
	{
		return;
	}
	
	if (!(diff_cnt % 2))
	{
		//request_domain_conver_list();
	}
	//if (!(diff_cnt % 2))
	//{
		//RequestUpdate();
	//}

	if (diff_cnt > 24)
	{
		diff_cnt = 0;
	}
}

void reboot_program()
{
	int now_time_hour = 0;
	static int old_time_hour = -1;
	static int diff_cnt = -1;
	GetAllSystemTime(NULL, NULL, NULL, &now_time_hour, NULL, NULL, NULL);
	if (now_time_hour != old_time_hour)
	{
		diff_cnt ++;
		old_time_hour = now_time_hour;
	}
	else
	{
		return;
	}
	
	if (diff_cnt >= 6)
	{
		//每个6小时重新运行程序
		printf(COLOR_YELLOW "it time to restart program...\n" COLOR_NONE_C);
		_exit_(0);
	}

	if (diff_cnt > 24)
	{
		diff_cnt = 0;
	}
}

void _exit_(int ret)
{
	//iptables_del_proxy();
	free_adjs();
	epoll_server_exit();
	exit(ret);
}

#if 1
long getFreeDDRSize()
{
	struct sysinfo s_info;

	int error;
	error = sysinfo(&s_info);
	if (error)
		return -1;
	return (long)s_info.freeram;
}

int CheckDDRSizeWarn()
{
	long size = getFreeDDRSize();
	//printf("free ddr size: %d\n", size);
	if (size < 0)
		return -1;
	if (size < 1024 * 1024 * 1)
	{
		printf("ddr size < 1MB, %ld, exit!\n", size);
		fflush(stdout);
		_exit_(-1);
		return -2;
	}
	
	return 0;
}

#endif

void init_pid_file()
{
	system("rm -rf /tmp/qcrun");
}

int save_pid(char *pname)
{
	char *diroot = "/tmp/qcrun";
	char subdir[255] = {0};
	int pid = getpid();
	char strpid[65] = {0};
	sprintf(strpid, "%d", pid);
	sprintf(subdir, "%s/%s", diroot, pname);
    mkdir(diroot, 0777);
    mkdir(subdir, 0777);
    char file[128];
    sprintf(file, "%s/%s", subdir, strpid);
	int fd = open(file, O_CREAT | O_RDWR | O_TRUNC, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH);
	if (fd < 0)
	{
		LOG_PERROR_INFO("open %s error.", file);
		return -1;
	}
	
	write(fd, strpid, strlen(strpid));
	close(fd);
	return 0;
}


