#include "includes.h"

int monitor_wifi_state();
void com_server();
void reboot_program();


void *misc(void *arg)
{
	pthread_detach(pthread_self());

	while (1)
	{
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
	if (get_ip(wan_ip, get_netdev_wan_name()) < 0)
	{
		LOG_WARN_INFO("get wlan0 device ip error!!!\n");
		changed = 1;		
	}
	else
	{
		if (strcmp(wan_ip, get_netdev_wan_ip()))
		{
			LOG_WARN_INFO("[%s] state was changed!!! new:%s old:%s\n", get_netdev_wan_name(), wan_ip, get_netdev_wan_ip());
			changed = 1;
		}
	}

	if (changed)
	{
		change_cnt ++;
		printf(COLOR_RED "[%s] check ip error ... %d cnt!\n" COLOR_NONE_C, get_netdev_wan_name(), change_cnt);
	}
	else
	{
		change_cnt = 0;
	}
	if (change_cnt > 5)
	{
		printf(COLOR_RED "[%s] check ip continuous error too many times!\n" COLOR_NONE_C, get_netdev_wan_name());
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
		//_exit_(-1);
	}

	if (diff_cnt > 24)
	{
		diff_cnt = 0;
	}
}

void _exit_(int ret)
{
	iptables_del_proxy();
	free_js();
	epoll_server_exit();
	exit(ret);
}

