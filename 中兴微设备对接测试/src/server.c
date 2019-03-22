#include "includes.h"

sprocess_lock *g_sprolock;					//进程锁
int g_listenfd = -1;
int g_listen_port = 60000;

int server_init();

int create_server(char * bindip, int listen_port, int listen_counts)
{
	int listenfd = socket_listen(bindip, listen_port, listen_counts);
	if (listenfd < 0)
	{
		return -1;
	}
	g_listenfd = listenfd;
	g_listen_port = listen_port;
	
	//创建进程锁
    g_sprolock = process_spinlock_create(sizeof(sprocess_lock));
    if (g_sprolock == (sprocess_lock *)-1)
    {
        LOG_ERROR_INFO("process lock create error, exit.!\n");
        return -1;
    }

#if 1	
    //创建子进程
    int ret = process_CreateSub();
	if (1 == ret)
    {
    	//子进程
        //初始化信号处理
        signal_init();
        LOG_NORMAL_INFO("child process running...\n");
    }
    else
    {
    	//父进程
    	close(listenfd);
    	return ret;
    }
#else
	signal_init();
#endif
	//--------子进程执行内容-------------------------------

#if __SOCKET_MONITOR__	
	socket_monitor_init();
#endif

	if (server_init() < 0)
	{
		LOG_ERROR_INFO("server init failt! exit program.\n");
		socket_close(listenfd);
		_exit_(-1);
	}

//WIFI认证使用到的结构体初始化
#if _AUTH_ON_
	auth_struct_init();
#endif
//WIFI认证初始化
#if _AUTH_ON_
	auth_firewall_init();
#endif
#if _AUTH_ON_
	check_validTime_thread();
#endif

	//防火墙重定向
	iptables_webview_proxy(get_netdev_lan_ip(), get_listen_port());
	sleep(5);

	printf("********** Init Ok **********\n");

	while (1)
	{
		display_worker_counts();
		display_client_counts();
		sleep(1);
	}

	return 0;
}

int server_init()
{
	if (tpool_create(THREAD_NUM) < 0)
	{
		LOG_ERROR_INFO("create thread pool failt!\n");
		return -1;
	}

	if (listen_thread_start(g_listenfd) < 0)
	{
		return -1;
	}
	if (phone_thread_start() < 0)
	{
		return -1;
	}
	if (webserver_thread_start() < 0)
	{
		return -1;
	}
	
	return 0;
}

int epoll_server_exit()
{	
	signal_exit(SIGTERM);

	//外加的一个退出
	char killall[125];
	sprintf(killall, "killall %s", MODULE_NAME);
	system(killall);

	int timecnt = 0;
	while (1)
	{
		sleep(1);
		timecnt ++;
		if (timecnt > 5)
		{
			//5秒内还未退出
			LOG_WARN_INFO("KILLALL %s failt! now reboot system.\n", MODULE_NAME);
			iptables_del_proxy();
			system("reboot -f");
		}
	}
	
	return 0;
}

int get_listenfd()
{
	return g_listenfd;
}

int get_listen_port()
{
	return g_listen_port;
}








