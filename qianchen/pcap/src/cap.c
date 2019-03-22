#include "includes.h"

#define SNAP_LEN 			1518

#define CAP_THREAD_COUNT_MAX	1

struct cap_thread_info _cap_thread_info[CAP_THREAD_COUNT_MAX];

//static char _cap_dev_name[15] = {0};
static pthread_mutex_t cap_run_lock = PTHREAD_MUTEX_INITIALIZER;
volatile int g_traffic_time_cnt = 0;

static int _cap(struct cap_thread_info *cti)
{
	char errbuf[PCAP_ERRBUF_SIZE];
		
	bpf_u_int32 mask;
	bpf_u_int32 net;

	int num_packets = -1;

	char *dev = libpcap_lookupdev(cti->name, errbuf);
	if (! dev)
		return -1;

	nl_log("package dump for net device: %s .\n", dev);

	libpcap_lookupnet(dev, &net, &mask, errbuf);

	cti->handle = libpcap_open_live(dev, SNAP_LEN, 1, 0, errbuf);
	if (! cti->handle)
		return -1;

	if (libpcap_datalink(cti->handle) != DLT_EN10MB)
		return -1;

	cap_call_fun_arg_t arg;
	memset(&arg, 0, sizeof(cap_call_fun_arg_t));
	memcpy(arg.cap_dev, dev, strlen(dev));
	arg.eth_type = true;//strcmp(dev, LAN_DEV) ? false : true;
	libpcap_loop(cti->handle, num_packets, packet_process, (char *)&arg);
	libpcap_exit(cti->handle, NULL);
	cti->handle = NULL;

	memset(cti, 0, sizeof(struct cap_thread_info));
	nl_log("cap exit!!!!!!!!!!\n");

	return 0;
}

static void *_cap_thread(void *arg)
{
	pthread_detach(pthread_self());
	if (arg)
	{
		struct cap_thread_info *cti = (struct cap_thread_info *)arg;
		_cap(cti);
	}
	
	return NULL;
}

static void _cap_init(char *dev)
{
	char ip[32] = {0};
	get_dev_ip(ip, dev);
	if (strlen(ip) > 7)
	{
		create_cap_sub_process(dev);
		sleep(1);
	}
}

void cap_init()
{
	memset(_cap_thread_info, 0, sizeof(struct cap_thread_info) * CAP_THREAD_COUNT_MAX);
	
	int i = 0;
	for (i = 0; i < CAP_THREAD_COUNT_MAX; i ++)
	{
		char dev[20] = {0};
		snprintf(dev, 19, WAN_DEV);
		_cap_init(dev);
	}
}

#if 1
int create_cap_thread(struct cap_thread_info *cti)
{
	pthread_t tdp;
	if (pthread_create(&tdp, NULL, _cap_thread, cti) != 0)
	{
		return -1;
	}
	
	return 0;
}

int kill_cap_thread(char *cap_dev)
{
	pthread_mutex_lock(&cap_run_lock);
	
	int i, j = 0;
	for (i = 0; i < CAP_THREAD_COUNT_MAX; i ++)
	{
		if (strcmp(_cap_thread_info[i].name, cap_dev) == 0)
		{
			VAL_LOG("kill pid %d capdev: %s\n", _cap_thread_info[i].thid, _cap_thread_info[i].name);
			libpcap_breakloop(_cap_thread_info[i].handle);
			while (_cap_thread_info[i].handle) sleep(1);
			j = i;

			memset(&_cap_thread_info[i], 0, sizeof(struct cap_thread_info));

			sleep(2);
			
			break;
		}
	}

	pthread_mutex_unlock(&cap_run_lock);
	
	return j;
}

static void signal_pro(int sign)
{
	traffic_hash_data_to_file_lock();
	dns_data_to_file_lock();
	
	VAL_LOG("^^^^^^^^^^^^Recv signal to save traffic\n");
	traffic_hash_data_to_file(true);
	g_traffic_time_cnt = 0;
	
	VAL_LOG("^^^^^^^^^^^^Recv signal to save dns\n");
	dns_buf_save(true);
	g_dns_time_cnt = 0;
	
	dns_data_to_file_unlock();
	traffic_hash_data_to_file_unlock();

	VAL_LOG("^^^^^^^^^^^^Recv signal to EXIT\n");

	exit(0);
}

extern bool run_begin;
void main_loop()
{
	signal(SIGTERM, signal_pro);
	
	//进程自动回收
	signal(SIGCHLD, SIG_IGN);
	signal(SIGPIPE, SIG_IGN);

	//cap_log_init();

	time_init();

	//create_upload_thread();

	//while (! run_begin) sleep(1);

	memset(_cap_thread_info, 0, sizeof(struct cap_thread_info) * CAP_THREAD_COUNT_MAX);

	//初始化流量相关文件和变量
	traffic_init();

	//预先启动cap线程
	cap_init();

	//启动网卡监控线程
	//create_net_dev_monitor_thread();

	while (1)
	{	
		sleep(1);
	}
}

char *get_cap_dev_name(cap_call_fun_arg_t *arg)
{
	return arg ? arg->cap_dev : NULL;
}

int create_cap_sub_process(char *cap_dev)
{
	int i = 0;
	kill_cap_thread(cap_dev);

	pthread_mutex_lock(&cap_run_lock);
	
	for (i = 0; i < CAP_THREAD_COUNT_MAX; i ++)
	{
		if (_cap_thread_info[i].handle == NULL
		&& strlen(_cap_thread_info[i].name) <= 0)
		{
			snprintf(_cap_thread_info[i].name, sizeof(_cap_thread_info[i].name), "%s", cap_dev);
			if (create_cap_thread(&_cap_thread_info[i]) != 0)
				memset(&_cap_thread_info[i], 0, sizeof(struct cap_thread_info));
			pthread_mutex_unlock(&cap_run_lock);
			return 0;
		}
	}

	pthread_mutex_unlock(&cap_run_lock);
	
	return -2;

}

#endif

