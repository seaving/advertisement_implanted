#include "includes.h"

#define CMD_GET_ONLINE_CLIENT_CNT	"cat /proc/net/arp | grep \"%s\" | grep 0x2 | sort -u | wc -l"
#define CMD_GET_ONLINE_CLIENT_LIST	"cat /proc/net/arp | grep \"%s\" | grep 0x2 | head -n %d | tail -n %d | awk -F \" \" '{print $1 \" \" $4}'"

#define CMD_GET_STATION_INFO	"iw dev %s station get %s | grep -E \"rx bytes:|tx bytes:|signal:|connected time\" | awk -F \":\" '{print $2}' | awk -F \" \" '{printf $1 \" \"}'"
#define CMD_GET_CLIENT_NAME		"cat /tmp/dhcp.leases | grep \"%s\" | grep \"%s\" | awk -F \" \" '{printf $4}'"

#define CMD_GET_WIFI_CLIENT_CNT		"iw dev %s station dump | grep Station | wc -l"

#define CMD_DISCONNECT_CLIENT		"iw dev %s station del %s"

void _get_online_client_cnt_call_(char *buf, void *val)
{
	if (val && buf)
	{
		memcpy((char *)val, buf, strlen(buf));
	}
}

int get_online_client_cnt(char *br_dev)
{
	char cnt[15] = {0};
	char cmd[255] = {0};
	snprintf(cmd, 255, CMD_GET_ONLINE_CLIENT_CNT, br_dev);

	if (popen_cmd(cmd, _get_online_client_cnt_call_, cnt) < 0)
	{
		return -1;
	}

	if (isNumber(cnt))
	{
		return atoi(cnt);
	}

	return 0;
}

void _get_wifi_client_cnt_call_(char *buf, void *val)
{
	if (val && buf)
	{
		memcpy((char *)val, buf, strlen(buf));
	}
}

int get_wifi_client_count(char *wlan_dev)
{
	char cnt[15] = {0};
	char cmd[255] = {0};
	snprintf(cmd, 255, CMD_GET_WIFI_CLIENT_CNT, wlan_dev);

	if (popen_cmd(cmd, _get_wifi_client_cnt_call_, cnt) < 0)
	{
		return -1;
	}

	if (isNumber(cnt))
	{
		return atoi(cnt);
	}

	return 0;

}

int get_wifi_auth_client_count()
{
	int cnt1 = get_wifi_client_count("wlan0");
	int cnt2 = get_wifi_client_count("wlan1");

	return (cnt1 > 0 ? cnt1 : 0) + (cnt2 > 0 ? cnt2 : 0);
}

int get_wifi_not_auth_client_count()
{
	int cnt1 = get_wifi_client_count("wlan0-1");
	int cnt2 = get_wifi_client_count("wlan1-1");

	return (cnt1 > 0 ? cnt1 : 0) + (cnt2 > 0 ? cnt2 : 0);
}

void _get_online_client_list_call_(char *buf, void *val)
{
	if (val && buf)
	{
		memcpy((char *)val, buf, strlen(buf));
	}
}

int get_online_client_list(char *br_dev, int page, int limit, char *list)
{
	char cmd[255] = {0};
	snprintf(cmd, 255, CMD_GET_ONLINE_CLIENT_LIST, br_dev, page * limit, limit);

	if (popen_cmd(cmd, _get_online_client_list_call_, list) < 0)
	{
		return -1;
	}
	
	return 0;
}

void _get_station_info_call_(char *buf, void *val)
{
	if (val && buf)
	{
		station_info_t *station_info = (station_info_t *) val;
		if (strlen(buf) <= 0)
		{
			station_info->result = false;
		}
		else
		{
			char *rx = NULL;
			char *tx = NULL;
			char *signal = NULL;
			char *time = NULL;
			int i = 0;
			int len = strlen(buf);
			char *tmp = buf;
			for (i = 0; i < len && buf[i]; i ++)
			{
				if (buf[i] == ' ')
				{
					buf[i ++] = 0;
					break;
				}
			}
			rx = tmp;
			tmp = i < len && buf[i] ? &buf[i] : NULL;
			for ( ; i < len && buf[i]; i ++)
			{
				if (buf[i] == ' ')
				{
					buf[i ++] = 0;
					break;
				}
			}
			tx = tmp;
			tmp = i < len && buf[i] ? &buf[i] : NULL;
			for ( ; i < len && buf[i]; i ++)
			{
				if (buf[i] == ' ')
				{
					buf[i ++] = 0;
					break;
				}
			}
			signal = tmp;
			tmp = i < len && buf[i] ? &buf[i] : NULL;
			for ( ; i < len && buf[i]; i ++)
			{
				if (buf[i] == ' ')
				{
					buf[i ++] = 0;
					break;
				}
			}
			time = tmp;

			station_info->tx = atoll(tx ? tx : "0");
			station_info->rx = atoll(rx ? rx : "0");
			station_info->signal = atoi(signal ? signal : "0");
			station_info->connected_time = atol(time ? time : "0");
			station_info->result = true;
		}
	}
}

int get_station_info(char *br_dev, char *mac, station_info_t *station_info)
{
	char cmd[255] = {0};
	snprintf(cmd, 255, CMD_GET_STATION_INFO, br_dev, mac);

	if (popen_cmd(cmd, _get_station_info_call_, station_info) < 0)
	{
		return -1;
	}

	if (! station_info->result)
	{
		return -1;
	}
	
	return 0;
}

void _get_client_name_call_(char *buf, void *val)
{
	if (val && buf)
	{
		memcpy((char *)val, buf, strlen(buf) > 64 ? 64 : strlen(buf));
	}
}

int get_client_name(char *ip, char *mac, char *name)
{	
	char cmd[255] = {0};
	snprintf(cmd, 255, CMD_GET_CLIENT_NAME, mac, ip);
	
	if (popen_cmd(cmd, _get_client_name_call_, name) < 0)
	{
		return -1;
	}
	
	return 0;
}

station_info_t* get_client_list_info(char *br_dev, int page, int limit, int *len)
{
	int cnt = get_online_client_cnt(br_dev);
	limit = cnt > limit ?  limit : cnt;
	if (limit <= 0)
	{
		*len = 0;
		return NULL;
	}

	station_info_t *station_info_list = calloc(sizeof(station_info_t), limit);
	if (! station_info_list)
	{
		*len = 0;
		return NULL;
	}
	
	int i = 0, k = 0;
	for (i = 0; i < limit; i ++)
	{
		char list[64] = {0};
		get_online_client_list(br_dev, (page - 1) * limit + i + 1, 1, list);
		if (strlen(list) <= 0)
		{
			break;
		}

		char *ip = NULL;
		char *mac = NULL;

		int list_len = strlen(list);
		int j = 0;
		for (j = 0; j < list_len;j ++)
		{
			if (list[j] == ' ')
			{
				list[j ++] = 0;
				break;
			}
		}

		ip = list;
		mac = j < list_len && list[j] ? &list[j] : NULL;

		if (ip && mac)
		{
			station_info_t *station_tmp = station_info_list + i;
			snprintf(station_tmp->ip, 32, "%s", ip);
			snprintf(station_tmp->mac, 32, "%s", mac);
			if (strcmp(br_dev, "br-guest") == 0)
			{
				station_tmp->auth_status = 2;
			}
			get_client_name(ip, mac, station_tmp->name);

			station_tmp->black = false;
			if (is_black_name(mac))
			{
				station_tmp->black = true;
			}

			char *wlan_dev = "wlan0";
			get_station_info(wlan_dev, mac, station_tmp);
			if (! station_tmp->result)
			{
				wlan_dev = "wlan0-1";
				get_station_info(wlan_dev, mac, station_tmp);
				if (! station_tmp->result)
				{
					wlan_dev = "wlan1";
					get_station_info(wlan_dev, mac, station_tmp);
					if (! station_tmp->result)
					{
						wlan_dev = "wlan1-1";
						get_station_info(wlan_dev, mac, station_tmp);
						if (! station_tmp->result)
						{
						}
					}
				}
			}
			
			LOG_WARN_INFO(
				"ip: %s\n"
				"mac: %s\n"
				"name: %s\n"
				"result: %d\n"
				"rx: %llu\n"
				"tx: %llu\n"
				"signal: %d\n"
				"black: %d\n",
				station_tmp->ip, 
				station_tmp->mac, 
				station_tmp->name, 
				station_tmp->result, 
				station_tmp->rx, 
				station_tmp->tx, 
				station_tmp->signal,
				station_tmp->black);

			k ++;
		}
	}

	if (k <= 0)
	{
		*len = 0;
		free_malloc(station_info_list);
		return NULL;
	}
	
	*len = k;
	return station_info_list;
}

int _disconnect_wifi_client_(char *wlan_dev, char *mac)
{
	char cmd[255] = {0};
	EXECUTE_CMD(cmd, CMD_DISCONNECT_CLIENT, wlan_dev, mac);
	return 0;
}

int disconnect_wifi_client(char *mac)
{
	_disconnect_wifi_client_("wlan0", mac);
	_disconnect_wifi_client_("wlan0-1", mac);
	_disconnect_wifi_client_("wlan1", mac);
	_disconnect_wifi_client_("wlan1-1", mac);
	return 0;
}

