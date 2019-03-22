#include "includes.h"

#define _GET_AUTH_CLIENT_HT_HEAD(HT)	&HT
#define _IS_AUTH_CLIENT_TIMEOUT(AUTH)	(((SYSTEM_SEC - AUTH->start_time) > AUTH->validity_time))

static hashtable_t *_auth_client_ht_head = NULL;
static pthread_mutex_t _auth_hash_lock = PTHREAD_MUTEX_INITIALIZER;
/*
void auth_hash_lock()
{
	LOG_ERROR_INFO("LOCK\n");
	pthread_mutex_lock(&_auth_hash_lock);
}

void auth_hash_unlock()
{
	LOG_ERROR_INFO("UNLOCK\n");
	pthread_mutex_unlock(&_auth_hash_lock);
}
*/
#define auth_hash_lock() { \
	pthread_mutex_lock(&_auth_hash_lock); \
}
	
#define auth_hash_unlock() { \
	pthread_mutex_unlock(&_auth_hash_lock); \
}

//初始化数组
void auth_client_init()
{
	_auth_client_ht_head = NULL;
}

void auth_node_init(
	auth_client_t *auth,
	char *mac,
	char *ip,
	int start_time,
	int validity_time,
	bool flag )
{
	auth->start_time = start_time;
	auth->validity_time = validity_time;
	auth->flag = flag;

	memcpy(auth->ip, ip, strlen(ip));
	memcpy(auth->mac, mac, strlen(mac));
}

auth_client_t *create_auth_node(
	char *mac,
	char *ip,
	int start_time,
	int validity_time,
	bool flag )
{
	auth_client_t *auth = calloc(sizeof(auth_client_t), 1);
	if (! auth)
	{
		LOG_PERROR_INFO("calloc error.");
		return NULL;
	}
	
	if (ip)
		auth->ip = calloc(1, strlen(ip) + 1);
	if (mac)
		auth->mac = calloc(1, strlen(mac) + 1);

	if (! auth->ip || ! auth->mac)
	{
		if (auth->ip) free(auth->ip);
		if (auth->mac) free(auth->mac);
		free (auth);
		return NULL;
	}

	auth_node_init(auth, mac, ip, start_time, validity_time, flag);
	
	return auth;
}

//清除单个元素
void auth_client_free(void *node)
{
	auth_client_t *auth = (auth_client_t *)node;
	if (auth)
	{
		if (auth->ip)
			free (auth->ip);
		if (auth->mac)
			free (auth->mac);
		free (auth);
	}
}

//清除整个数组
void auth_client_free_all()
{
	auth_hash_lock();
	uthash_destroy(_GET_AUTH_CLIENT_HT_HEAD(_auth_client_ht_head));
	auth_hash_unlock();
}

int get_auth_client(char *ip, auth_client_t *auth)
{
	char *key = ip;
	
	auth_hash_lock();
	auth_client_t *tmp = uthash_find(_GET_AUTH_CLIENT_HT_HEAD(_auth_client_ht_head), key);
	if (! tmp)
	{
		auth_hash_unlock();
		return -1;
	}
	
	memset(auth, 0, sizeof(auth_client_t));
	memcpy(auth, tmp, sizeof(auth_client_t));
	auth_hash_unlock();
	
	return 0;
}

int auth_client_find(char *ip, char *mac)
{
	char *key = ip;
	
	auth_hash_lock();
	
	auth_client_t *auth = uthash_find(_GET_AUTH_CLIENT_HT_HEAD(_auth_client_ht_head), key);
	if (! auth)
	{
		auth_hash_unlock();

		//不存在该IP
		return -1;
	}

	if (auth->mac && strcmp(auth->mac, mac) == 0)
	{
		bool flag = auth->flag;
		
		auth_hash_unlock();

		if (flag == TMP_ACCEPT)
			return 2;
		
		//存在
		return 1;
	}

	auth_hash_unlock();

	//存在该IP，但是MAC不一致
	return 0;
}

int auth_client_add(char *ip, char *mac, int start_time, int validity_time, bool flag)
{
	char *key = ip;
	
	auth_hash_lock();
	
	auth_client_t *auth = uthash_find(_GET_AUTH_CLIENT_HT_HEAD(_auth_client_ht_head), key);
	if (! auth)
	{
		auth = create_auth_node(mac, ip, start_time, validity_time, flag);
		if (! auth)
			goto done;

		if (uthash_put(_GET_AUTH_CLIENT_HT_HEAD(_auth_client_ht_head), key, auth, auth_client_free) < 0)
		{
			auth_client_free(auth);
			goto done;
		}
	}
	else
	{
		auth_node_init(auth, mac, ip, start_time, validity_time, flag);
	}

	LOG_HL_INFO("----------------------------------\n");
	LOG_HL_INFO("auth: ip:            %s\n", ip);
	LOG_HL_INFO("auth: mac:           %s\n", mac);
	LOG_HL_INFO("auth: start_time:    %d\n", start_time);
	LOG_HL_INFO("auth: validity_time: %d\n", validity_time);
	LOG_HL_INFO("auth: mode:          %s\n", flag ? "forma" : "temp");
	LOG_HL_INFO("----------------------------------\n");

	auth_hash_unlock();
	return 0;
	
done:
	auth_hash_unlock();
	return -1;
}

set_res_t is_auth_client_set(char *ip, char *mac)
{
	char *key = ip;
	set_res_t ret;
	
	auth_hash_lock();
	
	auth_client_t *auth = uthash_find(_GET_AUTH_CLIENT_HT_HEAD(_auth_client_ht_head), key);
	if (! auth)
	{
		auth_hash_unlock();

		//不存在该IP
		return E_NOT_FOUND;
	}

	if (auth->mac && strcmp(auth->mac, mac) == 0)
	{
		ret = E_SETED;
		
		//为假表示认证为临时
		if (! auth->flag)
			ret = E_TMP_ACCEPT;
			
		auth_hash_unlock();

		//存在
		return ret;
	}

	auth_hash_unlock();

	//存在该IP，但是MAC不一致
	return E_NOT_MATCH;
}

int auth_client_scan()
{
	int authHtCount = 0;

	hashtable_t *auth_current = NULL;

	auth_hash_lock();

	for (auth_current = _auth_client_ht_head; auth_current != NULL; )
	{
		auth_client_t *auth = (auth_client_t *)(auth_current->value);

		//删除当前结点
		hashtable_t *auth_tmp = (hashtable_t *)auth_current;
		auth_current = (hashtable_t *)(auth_tmp->hh.next);

		//判断时间
		if (auth && _IS_AUTH_CLIENT_TIMEOUT(auth))
		{
			LOG_WARN_INFO("the phone: %s, %s is timeout!\n", auth->ip, auth->mac);
			
			auth_delete(auth->ip, auth->mac);
			uthash_delete(_GET_AUTH_CLIENT_HT_HEAD(_auth_client_ht_head), _GET_AUTH_CLIENT_HT_HEAD(auth_tmp));
		}
	}

	authHtCount = uthash_count(_GET_AUTH_CLIENT_HT_HEAD(_auth_client_ht_head));
	if (authHtCount <= 0)
		_auth_client_ht_head = NULL;

	LOG_HL_INFO("*** there are %d authed client. ***\n", authHtCount);
	
	auth_hash_unlock();
	
	return 0;
}

static void *_auth_client_scan_work(void *arg)
{
	pthread_detach(pthread_self());

    pthread_cond_t cond = PTHREAD_COND_INITIALIZER;
    pthread_mutex_t cond_mutex = PTHREAD_MUTEX_INITIALIZER;
    struct timespec timeout;
	
	for ( ; ; )
	{
        /* Sleep for config.checkinterval seconds... */
        timeout.tv_sec = time(NULL) + DEFAULT_CHECKINTERVAL;
        timeout.tv_nsec = 0;

        /* Mutex must be locked for pthread_cond_timedwait... */
        pthread_mutex_lock(&cond_mutex);

        /* Thread safe "sleep" */
        pthread_cond_timedwait(&cond, &cond_mutex, &timeout);

        /* No longer needs to be locked */
        pthread_mutex_unlock(&cond_mutex);

		auth_client_scan();
	}
	
	return NULL;
}

int create_auth_client_scan_thread()
{
	pthread_t tdp;
	return pthread_create(&tdp, NULL, _auth_client_scan_work, NULL);
}

int get_cli_name(char *ip, char *mac, char *name)
{
	int fd = open("/tmp/dhcp.leases", O_RDWR);
	if (fd < 0)
	{
		LOG_PERROR_INFO("open error.");
		return -1;
	}

	char tmp[125];
	sprintf(tmp, " %s %s ", mac, ip);

	int offset = 0;
	int ret = -1;
	char buf[512] = {0};
	for ( ; ; )
	{
		ret = read(fd, buf + offset, 1);
		if (ret != 1)
			break;
		offset += ret;
		if (offset >= 512 - 1)
		{
			buf[offset] = 0;
			break;
		}
		buf[offset] = 0;
		if (buf[offset - 1] == '\n')
		{
			char *p = straddr(buf, tmp);
			if (p)
			{
				p += strlen(tmp);
				char *p1 = strchr(p, ' ');
				if (p1)
				{
					memcpy(name, p, (p1 - p >= 64) ? 64 : p1 - p);
					close(fd);
					return 0;
				}
			}
		}
	}
	
	close(fd);
	return -1;
}

