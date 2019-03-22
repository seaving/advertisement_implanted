#ifndef __STATION_COUNT_H__
#define __STATION_COUNT_H__


typedef struct _station_info_
{
	char ip[32];
	char mac[22];
	char name[65];
	unsigned char auth_status;
	bool result;
	unsigned long long rx;
	unsigned long long tx;
	int signal;
	unsigned long connected_time;
	bool black;
} station_info_t;


int get_online_client_cnt(char *br_dev);
int get_wifi_client_count(char *wlan_dev);
int get_wifi_auth_client_count();
int get_wifi_not_auth_client_count();
int disconnect_wifi_client(char *mac);

station_info_t* get_client_list_info(char *client_mod, int page, int limit, int *len);

#endif


