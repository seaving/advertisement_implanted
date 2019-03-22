#include "includes.h"

static volatile int g_rule_count = 0;
static pthread_mutex_t auth_fw_lock = PTHREAD_MUTEX_INITIALIZER;

static void auth_fw_mark_set(char *ip, char *mac, int vaild_time, bool flag)
{
	fw_mark_set(ip, mac);
	g_rule_count ++;
	LOG_NORMAL_INFO("g_rule_count = %d\n", g_rule_count);

	auth_client_add(ip, mac, SYSTEM_SEC, vaild_time, flag);
}

static void auth_fw_del_rule(char *ip, char *mac)
{
	fw_del_rule(ip, mac);
	g_rule_count --;
	LOG_NORMAL_INFO("g_rule_count = %d\n", g_rule_count);
}

int auth_allow(char *ip, char *mac,int vaild_time, bool flag)
{
	if (! ip || ! mac)
		return -1;
	
	pthread_mutex_lock(&auth_fw_lock);

	//判断是否是临时放行转正式放行
	set_res_t ret = is_auth_client_set(ip, mac);
	if (ret == E_SETED)
	{
		LOG_NORMAL_INFO("the ip %s mac %s is allowed .\n", ip, mac);
		goto done;
	}
	else if (ret == E_NOT_MATCH || ret == E_TMP_ACCEPT)
	{
		if (ret == E_NOT_MATCH)
		{
			LOG_NORMAL_INFO("the ip %s mac %s is new device, delete old rule.\n", ip, mac);
		}
		else
		{
			LOG_NORMAL_INFO("the ip %s mac %s is temp device, delete temp old rule.\n", ip, mac);
		}

		auth_client_t auth;
		if (get_auth_client(ip, &auth) >= 0)
			auth_fw_del_rule(auth.ip, auth.mac);
	}
	else
	{
		LOG_NORMAL_INFO("the ip %s mac %s is new device .\n", ip, mac);
	}

	auth_fw_mark_set(ip, mac, vaild_time, flag);
	
done:
	pthread_mutex_unlock(&auth_fw_lock);

	return 0;
}

int auth_delete(char *ip, char *mac)
{
	auth_fw_del_rule(ip, mac);
	return 0;
}

void auth_accept_default_host(char *host)
{
	
}

int auth_is_allow(char *ip, char *mac)
{
	pthread_mutex_lock(&auth_fw_lock);
	int ret = auth_client_find(ip, mac);
	pthread_mutex_unlock(&auth_fw_lock);

	return ret;
}


