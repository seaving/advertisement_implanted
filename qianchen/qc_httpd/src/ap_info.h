#ifndef __AP_INFO_H__
#define __AP_INFO_H__
#if 0
typedef struct _ap_wireless_info_
{
	char *device;			//接口名称 'radio0'...
	char *hwmode;			//wifi频道: '11a': 5G  '11g': 2.4G 提供频道切换，但是不提供hwmode设置
	char *htmode;			//频宽 HT20  HT40  (iwinfo wlan0 htmodelist)
	int txpower;			//功率 (iwinfo wlan0 txpowerlist)
	char *channel;			//通道 (1,6,11 相邻设备不重叠) (iwinfo wlan0 freqlist)
	char disabled;			//关闭无线功能 '0'表示开启，'1'表示关闭

	char *network;			//连网模式，'lan'局域网，'wan'外网
	char *mode;				//工作模式，'ap'，'sta'
	char *key;				//密码
	char *ssid;				//ssid wifi名称
	char *encryption;		//加密方式 psk
	char hidden;			//隐藏ssid '0'表示不隐藏，'1'表示隐藏
} ap_wireless_info_t;

/**运行在AC+AP模式下的AP需要做中继
* 中继开启需要network配置文件:
* 1. config interface 'lan'结点屏蔽ipaddr,netmask,ip6assingn,dns,gw等静态配置
* 2. config interface 'lan'增加option proto 'dhcp'
* 3. config interface 'wan'结点屏蔽
* 4. config interface 'wan6'结点屏蔽
*/
/*typedef struct _ap_network_info_
{
	
} ap_network_info_t;
*/

/**运行在AC+AP模式下的AP需要做中继
* 中继开启需要dhcp配置文件:
* 1. config dhcp 'lan'的结点需要屏蔽
* 2. config dhcp 'wan'结点修改interface 'wan'为'lan', config dhcp 'wan'修改为'lan'
*/
/*typedef struct _ap_dhcp_info_
{
	
} ap_dncp_info_t;
*/

typedef struct _ap_base_info_
{
	char *ip;				//设备分配的ip
	char *mac;				//设备mac

	char *dev_name;			//设备名称，由用户自己在网页配置中命名
	char *address;			//设备安放地址，由用户自己在网页中命名
	
} ap_base_info_t;

typedef struct _ap_
{
	int update_time;		//最近一次的更新时间

	ap_wireless_info_t *wireless_5g;	//5G频段
	ap_wireless_info_t *wireless_2g;	//2.4G频段

	_ap_base_info_ *base_info;			//基础配置

} ap_t;
#endif
#endif


