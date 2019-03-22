#include "includes.h"

//尽量取合适的值，取大了主要靠时间触发上传
//取小了，DNS之类的缓存就会被清掉，而浏览器
//不会继续DNS ，因为浏览器已经有缓存
#define TRAFFIC_NODE_MAX	1024 * 10
#define TRAFFIC_PORT_NODE_MAX	200

#define SWITCH_PROTOCOL(proto)	(proto == PROTO_UDP ? "UDP" : (proto == PROTO_TCP ? "TCP" : (proto == PROTO_ICMP ? "ICMP" : (proto == PROTO_IGMP ? "IGMP" : "UNKOWN"))))

#if 0
#define kb_conver(traffic, str)	{ \
	if (0 <= traffic && traffic < 1024) \
	{ \
		sprintf(str, "%d Byte", traffic); \
	}\
	else if (1024 <= traffic && traffic < 1024 * 1024) \
	{ \
		sprintf(str, "%0.3f KB", (float)((float)traffic / (float)1024)); \
	}\
	else if (1024 * 1024 <= traffic && traffic < 1024 * 1024 * 1024)\
	{ \
		sprintf(str, "%0.4f MB", (float)((float)traffic / (float)(1024 * 1024))); \
	}\
	else \
	{ \
		sprintf(str, "%0.5f GB", (float)((float)traffic / (float)(1024 * 1024 * 1024))); \
	}\
}
#endif

#define kb_conver(traffic, str)	{ \
	sprintf(str, "%d", traffic); \
}

#define HT_KEY(htkey, ip, mac) { \
	sprintf(htkey, "%s_%s", ip, mac ? mac : "*"); \
}

extern volatile int g_traffic_time_cnt;

//static volatile int _traffic_total_count = 0;

static hashtable_t *_trafficHead = NULL;
static pthread_mutex_t _traffic_hash_lock = PTHREAD_MUTEX_INITIALIZER;

static char _traffic_gmt_start[164] = {0};
static char _traffic_gmt_end[164] = {0};

static char _traffic_file[128] = {0};
static unsigned int _traffic_file_create_time = 0;
static unsigned int _file_index = 0;
int create_traffic_file(char *cap_dev)
{
	if (strlen(_traffic_file) <= 0)
	{
		sprintf(_traffic_file, TRAFFIC_DIR"/.%s_tr-%ld-%d-%d_%s", cap_dev ? cap_dev : "null", GetTimestrip_us(), _file_index, rand(), get_sime_sn());
		_file_index ++;
		_traffic_file_create_time = SYSTEM_SEC;
	}
	
	int traffic_fd = open(_traffic_file, O_CREAT | O_RDWR | O_TRUNC, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH);
	if (traffic_fd < 0)
		perror("open error.\n");

	return traffic_fd;
}

int traffic_hash_data_to_file(bool isHashFull)
{
	int portHeadCount = 0;
	int trafficHeadCount = 0;
	int traffic_total_count = 0;
	
	hashtable_t *traffic_current = NULL, *port_current = NULL;
	{
		int /*i = 0,*/ j = 0;
		bool upflg = false;

		//判断是否存在数据，不存在则退出
		if (! _trafficHead || uthash_count(&_trafficHead) <= 0)
			return 0;
		
		int traffic_fd = create_traffic_file(_trafficHead ? (((traffic_t *)(_trafficHead->value))->cap_dev) : "null");
			
		//生成报表的title
		char title[1024] = {0};
		//if (get_file_size(_traffic_file) <= 0)
		{
			sprintf(title, "+---------------+-------------------+-------------------+---------+---------+-------------------+-------------------+-------------------+---------------------------------------\n");
			write(traffic_fd, title, strlen(title));
		#if DEBUG_ON_TR
			printf("--- %s", title);
		#endif
			sprintf(title, "|%15s|%19s|%19s|%9s|%9s|%19s|%19s|%19s|\t%s\n", 
							"CAP_DEV","PHONE_MAC", "REQ_IP", "PROTOCOL", "REQ_PORT", "CUR_UP_TRF", "CUR_DOWN_TRF", "CUR_ALL_TRF", "DOMAIN");
			write(traffic_fd, title, strlen(title));
		#if DEBUG_ON_TR
			printf("--- %s", title);
		#endif
			sprintf(title, "+---------------+-------------------+-------------------+---------+---------+-------------------+-------------------+-------------------+---------------------------------------\n");
			write(traffic_fd, title, strlen(title));
		#if DEBUG_ON_TR
			printf("--- %s", title);
		#endif
		}

		//判断是否已经满了TRAFFIC_FILE_MAX_SIZE或者判断该文件的生命时间是否超过
		if (get_file_size(_traffic_file) >= TRAFFIC_FILE_MAX_SIZE
		|| (SYSTEM_SEC - _traffic_file_create_time) >= TRAFFIC_FILE_LIFE_TIME
		|| isHashFull)
		{
			upflg = true;
		}

		//遍历IP相关的hash表得到port的哈希表
		for (traffic_current = _trafficHead; traffic_current != NULL; )
		{
			traffic_t *tr = (traffic_t *)(traffic_current->value);

			//VAL_LOG("######## %s %s \n", tr->ip, tr->domain ? tr->domain : " ");
			
			//遍历port
			for (port_current = tr->ht_port; port_current != NULL; )
			{
				traffic_port_t *port_traffic = (traffic_port_t *)(port_current->value);
				if (port_traffic)
				{
					char tmp[64], tmp_up[64], tmp_down[64];
					kb_conver(port_traffic->traffic_cnt, tmp);
					kb_conver(port_traffic->traffic_up, tmp_up);
					kb_conver(port_traffic->traffic_down, tmp_down);

					//统计总流量
					traffic_total_count += port_traffic->traffic_cnt;
					
					char data[1024] = {0};
					sprintf(data, "%16s%20s%20s%10s%10d%20s%20s%20s\t%s\n", 
									tr->cap_dev, 
									tr->phone_mac ? tr->phone_mac : "ff:ff:ff:ff:ff:ff", 
									tr->ip, 
									//port_traffic->proto == PROTO_TCP ? "TCP" : "UDP", 
									SWITCH_PROTOCOL(port_traffic->proto), 
									port_traffic->port, 
									tmp_up, 
									tmp_down, 
									tmp, 
									tr->domain ? tr->domain : " ");

					write(traffic_fd, data, strlen(data));
				#if DEBUG_ON_TR
					printf("--- %s", data);
				#endif
					j ++;
				}
				
				//删除当前结点
				hashtable_t *port_tmp = (hashtable_t *)port_current;
				port_current = (hashtable_t *)(port_current->hh.next);
				if (upflg)
				{
					uthash_delete(&(tr->ht_port), &port_tmp);
				}
				portHeadCount = uthash_count(&(tr->ht_port));
			}
			
			hashtable_t *traffic_tmp = (hashtable_t *)traffic_current;
			traffic_current = (hashtable_t *)(traffic_current->hh.next);

			//删除当前结点
			if (upflg)
			{
				tr->ht_port = NULL;
				uthash_delete(&_trafficHead, &traffic_tmp);
			}
			
			trafficHeadCount = uthash_count(&_trafficHead);
		}
		
		//判断是否已经满了TRAFFIC_FILE_MAX_SIZE或者判断该文件的生命时间是否超过
		{
			//if (upflg)
			{
				//生成尾部title
				sprintf(title, "+---------------+-------------------+-------------------+---------+---------+-------------------+-------------------+-------------------+---------------------------------------\n");
				write(traffic_fd, title, strlen(title));
			#if DEBUG_ON_TR
				printf("--- %s", title);
			#endif
			
				char total_tmp[64] = {0};
				sprintf(total_tmp, "total: ");
				kb_conver(traffic_total_count, total_tmp+strlen(total_tmp));
				total_tmp[strlen(total_tmp)] = '\n';
			#if DEBUG_ON_TR
				printf("--- %s", total_tmp);
			#endif
				write(traffic_fd, total_tmp, strlen(total_tmp));
				
				//记录起始时间
				check_gmt_time("Start", _traffic_gmt_start, sizeof(_traffic_gmt_start), (SYSTEM_SEC - _traffic_file_create_time));

			#if DEBUG_ON_TR
				printf("--- %s", _traffic_gmt_start);
			#endif
				write(traffic_fd, _traffic_gmt_start, strlen(_traffic_gmt_start));
				
				//记录结束时间
				gmt_time("End", _traffic_gmt_end, sizeof(_traffic_gmt_end));
			#if DEBUG_ON_TR
				printf("--- %s", _traffic_gmt_end);
			#endif
				write(traffic_fd, _traffic_gmt_end, strlen(_traffic_gmt_end));
				
				sprintf(title, "+-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------\n");
				write(traffic_fd, title, strlen(title));
			#if DEBUG_ON_TR
				printf("--- %s", title);
			#endif

				if (upflg)
				{
					//记录起始时间
					gmt_time("Start", _traffic_gmt_start, sizeof(_traffic_gmt_start));

					//把临时保存的文件更名，这样就上传线程便可以识别进行上传
					char file_tmp[128];
					sprintf(file_tmp, "%s", _traffic_file);
					char *p = strchr(file_tmp, '.');
					if (p)
						*p = '0';
					if (rename(_traffic_file, file_tmp) != 0)
					{
						char cmd[256] = {0};
						snprintf(cmd, 255, "mv %s %s", _traffic_file, file_tmp);
						system(cmd);
						unlink(_traffic_file);
					}
					
					//清掉缓存，待下次重新生成文件记录
					memset(_traffic_file, 0, sizeof(_traffic_file));
					_traffic_file_create_time = 0;
					_trafficHead = NULL;
				}
			}
		}
		
		close(traffic_fd);
	}

	VAL_LOG("*****************************\n");
	VAL_LOG("* trhead: %d, phead: %d *\n", trafficHeadCount, portHeadCount);
	VAL_LOG("*****************************\n");
	
	return 0;
}

hashtable_t **traffic_ht_head()
{
	return &_trafficHead;
}

int traffic_init()
{
	_trafficHead = NULL;

	srand(GetTimestrip_us());
	mkdir(TRAFFIC_DIR, 0777);

	gmt_time("Start", _traffic_gmt_start, sizeof(_traffic_gmt_start));
	dns_init();

	return 0;
}

void free_port_traffic_node(void *node)
{
	if ((traffic_port_t *)node)
	{
		free (node);
	}
}

void free_traffic_node(void *node)
{
	if ((traffic_t *)node)
	{
		if (((traffic_t *)node)->phone_mac)
			free (((traffic_t *)node)->phone_mac);
		if (((traffic_t *)node)->ip)
			free (((traffic_t *)node)->ip);
		if (((traffic_t *)node)->domain)
			free (((traffic_t *)node)->domain);
		if (((traffic_t *)node)->cap_dev)
			free (((traffic_t *)node)->cap_dev);
		free(node);
	}
}

traffic_port_t *create_port_traffic_node(unsigned int port, proto_t proto, unsigned int traffic)
{
	traffic_port_t *port_traffic = (traffic_port_t *)calloc(sizeof(traffic_port_t), 1);
	if (! port_traffic)
		return NULL;

	port_traffic->port = port;
	port_traffic->proto = proto;
	port_traffic->traffic_cnt = traffic;

	return port_traffic;
}

traffic_t *create_traffic_node(char *cap_dev, char *phone_mac, char *ip, char *domain)
{
	traffic_t *tr = (traffic_t *)calloc(sizeof(traffic_t), 1);
	if (! tr)
	{
		VAL_PERROR("calloc error.");
		return NULL;
	}
	
	tr->ip = calloc(1, strlen(ip) + 1);
	if (! tr->ip)
	{
		VAL_PERROR("calloc error.");
		free(tr);
		return NULL;
	}

	if (phone_mac)
	{
		tr->phone_mac = calloc(1, strlen(phone_mac) + 1);
		if (! tr->phone_mac)
		{
			VAL_PERROR("calloc error.");
			free(tr->ip);
			free(tr);
			return NULL;
		}
		
		memcpy(tr->phone_mac, phone_mac, strlen(phone_mac));
	}

	memcpy(tr->ip, ip, strlen(ip));

	if (domain)
	{
		tr->domain = calloc(1, strlen(domain) + 1);
		if (! tr->domain)
		{
			VAL_PERROR("calloc error.");
			free(tr->ip);
			if (tr->phone_mac) free(tr->phone_mac);
			free(tr);
			return NULL;
		}
		
		memcpy(tr->domain, domain, strlen(domain));
	}

	if (cap_dev)
	{
		tr->cap_dev = calloc(1, strlen(cap_dev) + 1);
		if (! tr->cap_dev)
		{
			VAL_PERROR("calloc error.");
			free(tr->ip);
			if (tr->phone_mac) free(tr->phone_mac);
			if (tr->domain) free(tr->domain);
			free(tr);
			return NULL;
		}

		memcpy(tr->cap_dev, cap_dev, strlen(cap_dev));
	}
	
	
	tr->ht_port = NULL;
	
	return tr;
}

void traffic_hash_data_to_file_lock()
{
	pthread_mutex_lock(&_traffic_hash_lock);
}

void traffic_hash_data_to_file_unlock()
{
	pthread_mutex_unlock(&_traffic_hash_lock);
}

//为dns解析接口提供hash操作
int traffic_hash_add(char *cap_dev, char *ip, char *phone_mac, char *domain)
{
	int domain_len = domain ? strlen(domain) : 0;
	if (! domain || domain_len <= 0)
		return -1;

	traffic_hash_data_to_file_lock();

	//通过ip:mac寻找到是否存在traffic_t
	char htkey[64] = {0};
	HT_KEY(htkey, ip, phone_mac);
	traffic_t* tr = (traffic_t *)uthash_find(&_trafficHead, htkey);
	if (! tr)
	{
		//创建IP相关结点
		tr = create_traffic_node(cap_dev, phone_mac, ip, NULL);
		if (! tr)
		{
			goto done;
		}

		if (domain)
		{
			tr->domain = calloc(1, domain_len + 1);
			if (! tr->domain)
			{
				VAL_PERROR("calloc error.");
				goto done;
			}

			memcpy(tr->domain, domain, domain_len);
		}

		//把端口hash表头加入到IP哈希表中
		if (uthash_put(&_trafficHead, htkey, tr, free_traffic_node) < 0)
		{
			free_traffic_node(tr);
			goto done;
		}
	}
	else
	{
		if (! tr->domain)
		{
			tr->domain = calloc(1, domain_len + 1);
			if (! tr->domain)
			{
				VAL_PERROR("calloc error.");
				goto done;
			}

			memcpy(tr->domain, domain, domain_len);
		}
	}

done:
	traffic_hash_data_to_file_unlock();

	return 0;
}

int traffic_add(char *cap_dev, char *phone_mac, char *ip, unsigned int port, proto_t proto, unsigned int traffic, dir_t dir)
{
	return 0;
	
	if (traffic <= 0)
		return 0;
	traffic_hash_data_to_file_lock();
	char strport[20] = {0};
	sprintf(strport, "%d", port);

	//通过ip:mac寻找到是否存在traffic_t
	char htkey[64];
	HT_KEY(htkey, ip, phone_mac);
	traffic_t* tr = (traffic_t *)uthash_find(&_trafficHead, htkey);
	if (! tr)
	{
		//创建IP相关结点
		tr = create_traffic_node(cap_dev, phone_mac, ip, NULL);
		if (! tr)
		{
			goto done;
		}

		//把端口hash表头加入到IP哈希表中
		if (uthash_put(&_trafficHead, htkey, tr, free_traffic_node) < 0)
		{
			free_traffic_node(tr);
			goto done;
		}
	}

	//通过portHead找到porttraffic
	traffic_port_t* port_traffic = (traffic_port_t *)uthash_find(&tr->ht_port, strport);
	if (! port_traffic)
	{
		//创建端口相关结点
		port_traffic = create_port_traffic_node(port, proto, 0);
		if (! port_traffic)
		{
			goto done;
		}

		//把端口结点插入到端口hash表中
		if (uthash_put(&tr->ht_port, strport, port_traffic, free_port_traffic_node) < 0)
		{
			free_port_traffic_node(port_traffic);
			goto done;
		}
	}

	if (port_traffic)
	{
		//上下行流量
		if (dir == UPLOAD)
			port_traffic->traffic_up += traffic;
		else if (dir == DOWNLOAD)
			port_traffic->traffic_down += traffic;
		else
			VAL_LOG("UNKOWN PACKAGE DIR TYPE!\n");
			
		//总流量
		port_traffic->traffic_cnt += traffic;
	}

	tr = (traffic_t *)uthash_find(&_trafficHead, htkey);
	if (! tr)
	{
		VAL_LOG("uthash_find error by %s\n", htkey);
		return -1;
	}
	
	port_traffic = (traffic_port_t *)uthash_find(&tr->ht_port, strport);
	if (! port_traffic)
	{
		VAL_LOG("uthash_find error by strport: %s!\n", strport);
		return -1;
	}

#if 1
	char tmp[64], tmp_up[64], tmp_down[64];
	kb_conver(port_traffic->traffic_cnt, tmp);
	kb_conver(port_traffic->traffic_up, tmp_up);
	kb_conver(port_traffic->traffic_down, tmp_down);
	//VAL_LOG("++++++++ u: %d, d: %d, t: %d\n", port_traffic->traffic_up, port_traffic->traffic_down, port_traffic->traffic_cnt);
	VAL_LOG(">>>>>%s %s %s %s %d %s %s %s %s\n", 
				cap_dev, 
				phone_mac ? phone_mac : "-", 
				tr->ip, 
				SWITCH_PROTOCOL(port_traffic->proto), 
				port_traffic->port, 
				tmp_up, 
				tmp_down, 
				tmp,
				tr->domain ? tr->domain : " ");
#endif
#if 1
	if (uthash_count(&_trafficHead) >= TRAFFIC_NODE_MAX
	|| uthash_count(&tr->ht_port) >= TRAFFIC_PORT_NODE_MAX)
	{
		//VAL_LOG("HASH FULL , BEGIN TO UPLOAD FILE\n");
		g_traffic_time_cnt = 0;
		traffic_hash_data_to_file(true);
		g_traffic_time_cnt = 0;
	}
#endif

done:
	traffic_hash_data_to_file_unlock();
	return 0;
}

