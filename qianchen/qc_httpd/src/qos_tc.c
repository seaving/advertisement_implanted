#include "includes.h"


#define LOCAL_PACKAGE_MARK		1000
#define FORWARD_PACKAGE_MARK	1001


/**
限速模型参考前辰LEDE开发文档
***/
//--------------------------------------------------------------------
//初始化tc限速模型
void _init_tc_model(char *dev, int max_speed_bps)
{
	char cmd[255] = {0};

	//root节点
	EXECUTE_CMD(cmd, "tc qdisc add dev %s root handle 1: htb default 2", dev);

	// 本机不限速通道
	EXECUTE_CMD(cmd, "tc class add dev %s parent 1: classid 1:1 htb rate"
				" 10000000mbps ceil 10000000mbps prio 1 burst 20k", dev);

	//非本机不限速通道
	EXECUTE_CMD(cmd, "tc class add dev %s parent 1: classid 1:2 htb rate"
				" %dbps ceil %dbps prio 1 burst 20k", dev, max_speed_bps, max_speed_bps);


	//各种限制速率通道
	//... 动态添加


	//filter 匹配本机通道
	EXECUTE_CMD(cmd, "tc filter add dev %s parent 1: protocol ip prio 2 "
				"u32 match mark %d 0xffff flowid 1:1", dev, LOCAL_PACKAGE_MARK);
	//filter 匹配非本机不限速通道
	EXECUTE_CMD(cmd, "tc filter add dev %s parent 1: protocol ip prio 2 "
				"u32 match mark %d 0xffff flowid 1:2", dev, FORWARD_PACKAGE_MARK);

	//filter 各种限制速率通道
	//... 动态添加

}

//创建速率限速class
void _new_limit_class(char *dev, char *classid, int set_speed_limit)
{
	char cmd[255] = {0};
	EXECUTE_CMD(cmd, "tc class add dev %s parent 1: classid %s htb rate"
				" %dmbps ceil %dmbps prio 1 burst 20k", 
				dev, classid, set_speed_limit, set_speed_limit);

}

//创建速率限速filter
void _new_limit_filter(char *dev, int mark, char *flowid)
{
	char cmd[255] = {0};
	EXECUTE_CMD(cmd, "tc filter add dev %s parent 1: protocol ip prio 2 "
				"u32 match mark %d 0xffff flowid %s", dev, mark, flowid);
}

void new_tc_speed_limit_rule(char *dev, char *classid, int set_speed_limit, int mark)
{
	_new_limit_class(dev, classid, set_speed_limit);
	_new_limit_filter(dev, mark, classid);
}
//--------------------------------------------------------------------

void destroy_tc_model(char *dev)
{
	char cmd[256];
	EXECUTE_CMD(cmd, "tc qdisc del dev %s root", dev);
}

void qos_init_tc_model(int forward_max_speed)
{
	destroy_ingress();
	destroy_tc_model(get_netdev_wan_name());
	destroy_tc_model("ifb0");
	
	init_ingress();
	_init_tc_model(get_netdev_wan_name(), forward_max_speed);
	_init_tc_model("ifb0", forward_max_speed);
}

void qos_new_tc_speed_limit_rule(char *classid, int set_speed_limit, int mark)
{
	new_tc_speed_limit_rule(get_netdev_wan_name(), classid, set_speed_limit, mark);
	new_tc_speed_limit_rule("ifb0", classid, set_speed_limit, mark);
}

//-----------------------------------------------------------------
void _get_traffic_count_call_(char *buf, void *traffic)
{
	if (traffic && buf)
	{
		memcpy((char *)traffic, buf, strlen(buf));
	}
}

/*unsigned long long _get_traffic_count(char *dev)
{
	char cmd[255] = {0};
	snprintf(cmd, 255, "tc -s qdisc ls dev %s | grep \"Sent\" | "
			"head -n 1 | tail -n 1 | awk -F \" \" '{printf $2}'", dev);
	char traffic[255] = {0};
	if (popen_cmd(cmd, _get_traffic_count_call_, traffic) < 0)
	{
		return -1;
	}
	
	if (isNumber(traffic))
	{
		return atoll(traffic);
	}

	return 0;
}*/

unsigned long long _get_traffic_rx_conut(char *dev)
{
	char cmd[255] = {0};
	snprintf(cmd, 255, "ifconfig %s | grep \"RX bytes:\" | awk -F \":\" '{print $2}' | awk -F \" \" '{print $1}'", dev);
	char traffic[255] = {0};
	if (popen_cmd(cmd, _get_traffic_count_call_, traffic) < 0)
	{
		return -1;
	}
	
	if (isNumber(traffic))
	{
		return atoll(traffic);
	}
	
	return 0;
}

unsigned long long _get_traffic_tx_conut(char *dev)
{
	char cmd[255] = {0};
	snprintf(cmd, 255, "ifconfig %s | grep \"TX bytes:\" | awk -F \":\" '{print $3}' | awk -F \" \" '{print $1}'", dev);
	char traffic[255] = {0};
	if (popen_cmd(cmd, _get_traffic_count_call_, traffic) < 0)
	{
		return -1;
	}
	
	if (isNumber(traffic))
	{
		return atoll(traffic);
	}
	
	return 0;
}

unsigned long long get_traffic_upload_total_count()
{
	return _get_traffic_tx_conut(get_netdev_wan_name());
}

unsigned long long get_traffic_download_total_count()
{
	return _get_traffic_rx_conut(get_netdev_wan_name());
}

unsigned long long get_traffic_total_count()
{
	return get_traffic_upload_total_count() +
		get_traffic_download_total_count();
}

int get_network_speed(int *upload_speed, int *download_speed, int *total_speed)
{
	static unsigned long last_ms = 0;
	static unsigned long long last_traffic_upload_total_count = 0;
	static unsigned long long last_traffic_download_total_count = 0;
	static unsigned long long last_traffic_total_count = 0;

	unsigned long cur_ms = get_system_clock_ms();
	if (cur_ms <= 0)
	{
		return -1;
	}
	
	unsigned long long diff_traffic = 0;
	unsigned long long diff_ms = 0;

	unsigned long long cur_traffic_upload_total_count = get_traffic_upload_total_count();
	unsigned long long cur_traffic_download_total_count = get_traffic_download_total_count();
	unsigned long long cur_traffic_total_count = cur_traffic_upload_total_count + cur_traffic_download_total_count;
	
	diff_traffic = cur_traffic_upload_total_count - last_traffic_upload_total_count;
	diff_ms = cur_ms - last_ms;
	if (upload_speed) *upload_speed = diff_traffic / diff_ms * 1000;
	last_traffic_upload_total_count = cur_traffic_upload_total_count;

	diff_traffic = cur_traffic_download_total_count - last_traffic_download_total_count;
	diff_ms = cur_ms - last_ms;
	if (download_speed) *download_speed = diff_traffic / diff_ms * 1000;
	last_traffic_download_total_count = cur_traffic_download_total_count;

	diff_traffic = cur_traffic_total_count - last_traffic_total_count;
	diff_ms = cur_ms - last_ms;
	if (total_speed) *total_speed = diff_traffic / diff_ms * 1000;
	last_traffic_total_count = cur_traffic_total_count;

	last_ms = cur_ms;

	return 0;
}



