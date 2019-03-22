
#include "includes.h"

void serverinfo();
void usege();
void wait_init_netdev_finish();
void wait_get_jscode_finish();
void wait_create_server_finish();
void wait_init_start_finish();
void wait_init_devNo();

int mitm_start()
{
	init_pid_file();

	//写入pid
	save_pid("mproxy_parent");
	
	//开机运行的一段初始化
	wait_init_start_finish();

	while (1)
	{
		//初始化防火墙
		//iptables_del_proxy();

		//初始化WAN/LAN口
		wait_init_netdev_finish();

		/**请求渠道所属渠道*/
		wait_init_devNo();

		/**当需要更新只能重启程序的时候进行更新,在misc中做了每隔6小时重启程序*/
		wait_get_jscode_finish();

		//创建服务器
		wait_create_server_finish();

		//输出服务器信息
		//serverinfo();
		
		//主线程轮询
		misc(NULL);

		//由于强迫症，在此延时10秒
		sleep(10);

		//走到这里说明程序异常退出
		_exit_(-1);
		
		//return -1;
	}

	return 0;
}
/*
//存在问题
void serverinfo()
{
	char proxy_ip[20] = {0};
	int proxy_port = 0;
	get_socket_name(get_listenfd(), proxy_ip, &proxy_port);
	LOG_HL_INFO("------------------------------------\n");
	LOG_HL_INFO("|		  Mitm Proxy Info			 \n");
	LOG_HL_INFO("|	IP        : %s\n", proxy_ip);
	LOG_HL_INFO("|	PORT	  : %d\n", proxy_port);
	LOG_HL_INFO("|	MAC 	  : %s\n", get_netdev_lan_mac());
	LOG_HL_INFO("------------------------------------\n");
}
*/
void usege()
{
	printf(COLOR_NONE_C);
	printf(COLOR_GREEN "----------------------------------------\n");
	printf(COLOR_GREEN "|       PROGRAM INFO\n");
	printf(COLOR_GREEN "| NAME      : %s\n", MODULE_NAME);
	printf(COLOR_GREEN "| VERSION   : %s\n", VERSION);
	printf(COLOR_GREEN "| PLATFORMS : %s\n", PLATFORMS);
	printf(COLOR_GREEN "| devNo		: %s\n", GET_DEV_NO);
	printf(COLOR_GREEN "----------------------------------------\n");
	printf(COLOR_NONE_C);
	fflush(stdout);
}

void wait_init_start_finish()
{
	//父进程信号处理
	signal_task_exit();
	signal_sigpipe();

	init_host_config();
	init_uri_config();
	init_fhconfig();

	if (dev_share_init() < 0)
		exit(-1);
	
	InitMsgQueue();

	//输出程序版本信息
	usege();

}

void wait_init_netdev_finish()
{
	char ip[32] = {0};
	while (get_dev_ip(ip, GET_WAN_NAME) != 0)
	{
		LOG_ERROR_INFO("init_netdev %s is failt!\n", GET_WAN_NAME);
		sleep(3);
	}

	LOG_HL_INFO("[%s] ip: %s\n", GET_WAN_NAME, ip);
}

void wait_get_jscode_finish()
{
	while (get_js_code_from_server() < 0)
	{
		LOG_ERROR_INFO("get_js_code_from_server is failt!\n");
		sleep(10);
	}
}

void wait_create_server_finish()
{
	char ip[32] = {0};
	get_dev_ip(ip, GET_LAN_NAME);

	if (create_server(ip, MITM_PORT, MAX_LISTEN) < 0)
	{
		LOG_ERROR_INFO("create_server failt!\n");
		_exit_(-1);
	}
}

void wait_init_devNo()
{
	while (strlen(GET_DEV_NO) <= 0)
		sleep(20);
}



