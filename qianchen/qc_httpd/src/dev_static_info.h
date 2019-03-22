#ifndef __DEV_STATIC_INFO_H__
#define __DEV_STATIC_INFO_H__

#include "includes.h"

typedef struct _wan_info_
{
	char protocol[10];
	char wan_mac[20];
	char wan_ip[32];
	char wan_mask[32];
	int mtu;
	char username[33];
	char password[33];
} wan_info_t;

typedef struct _wireless_info_
{
	char auth_2g_ssid[SSID_LEN];
	char auth_5g_ssid[SSID_LEN];
	char no_auth_2g_ssid[SSID_LEN];
	char no_auth_5g_ssid[SSID_LEN];
} wireless_info_t;

typedef struct _dhcp_info_
{
	int auth_dhcp_max;
	int no_auth_dhcp_max;
} dhcp_info_t;

typedef struct _info_
{
	wan_info_t wan_info;
	wireless_info_t wireless_info;
	dhcp_info_t dhcp_info;
} info_t;

extern info_t info;

void dev_static_info_init();


int get_2g_auth_ssid(char *ssid);
int get_5g_auth_ssid(char *ssid);
int get_2g_not_auth_ssid(char *ssid);
int get_5g_not_auth_ssid(char *ssid);
int get_auth_dhcp_assigned_count();
int get_not_auth_dhcp_assigned_count();
int get_auth_dhcp_max_count();
int get_not_auth_dhcp_max_count();
int get_2g_not_auth_password(char *password);
int get_5g_not_auth_password(char *password);
bool get_not_auth_disable();
bool get_not_auth_hidden();

int set_not_auth_hidden(bool isHidden);

int set_not_auth_disable();
int set_not_auth_enable();

#endif


