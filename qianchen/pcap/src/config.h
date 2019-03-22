#ifndef __CONFIG_H__
#define __CONFIG_H__


//从hash表中取出数据保存到文件的时间间隔 秒
#define SAVE_FILE_FROM_HASH_TABLE_TIME_OUT		5

//每个报表文件的大小 字节
#define TRAFFIC_FILE_MAX_SIZE		1024 * 1024

//文件的生命周期 秒
#define TRAFFIC_FILE_LIFE_TIME		60 * 60

//DNS缓存文件大小 字节
#define DNS_FILE_MAX_SIZE		1024 * 1024

//文件的生命周期 秒
#define DNS_FILE_LIFE_TIME		60 * 60


#define WAN_DEV		"eth0.2"
#define LAN_DEV		"br-lan"


#endif


