#ifndef __TRAFFIC_H__
#define __TRAFFIC_H__

#include "usr_uthash.h"


#define DEBUG_ON_TR		0



#define TRAFFIC_DIR		"/tmp/traffic"

//由于ICMP和IGMP协议没有端口号，这里定义一个端口号用于hash表寻址
#define ICMP_PORT	99999999
#define IGMP_PORT	99999998

typedef enum
{
	PROTO_UDP,
	PROTO_TCP,
	PROTO_ICMP,
	PROTO_IGMP,
	PROTO_UNKOWN
} proto_t;

typedef enum
{
	UPLOAD,
	DOWNLOAD,
	NUKOWN
} dir_t;

typedef struct
{
	proto_t proto;				//协议
	unsigned int port;			//端口
	unsigned int traffic_up;	//上行
	unsigned int traffic_down;	//下行
	unsigned int traffic_cnt;	//流量
} traffic_port_t;

typedef struct
{
	char *cap_dev;
	char *phone_mac;
	char *ip;
	char *domain;
	hashtable_t *ht_port;
} traffic_t;

int traffic_init();
int traffic_add(char *cap_dev, char *mac, char *ip, unsigned int port, proto_t proto, unsigned int traffic, dir_t dir);
int traffic_hash_data_to_file(bool isHashFull);
void traffic_hash_data_to_file_lock();
void traffic_hash_data_to_file_unlock();
hashtable_t **traffic_ht_head();
traffic_t *create_traffic_node(char *cap_dev, char *phone_mac, char *ip, char *domain);
int traffic_hash_add(char *cap_dev, char *ip, char *phone_mac, char *domain);
char *get_cap_dev_name();

#endif


