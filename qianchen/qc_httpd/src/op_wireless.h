
#ifndef __WIRELESS_OP_H__
#define __WIRELESS_OP_H__

#define SSID_LEN	33
#define WIFI_PASSWD_LEN	16

int get_wifi_ssid(char *wifi_iface, char *ssid);
int set_wifi_ssid(char *wifi_iface, char *ssid);

int get_wifi_txpower(char *wifi_iface);
int set_wifi_txpower(char *wifi_iface, char *txpower);

int get_wifi_channel(char *wifi_iface, char *channel);
int set_wifi_channel(char *wifi_iface, char *channel);

int get_wifi_mode(char *wifi_iface, char *mode);
int set_wifi_mode(char *wifi_iface, char *mode);

int get_wifi_encryption(char *wifi_iface, char *encryption);
int set_wifi_encryption(char *wifi_iface, char *encryption);

int get_wifi_key(char *wifi_iface, char *key);
int set_wifi_key(char *wifi_iface, char *key);

int get_wifi_disabled(char *wifi_iface);
int set_wifi_disabled(char *wifi_iface, char *disabled);

int get_wifi_htmode(char *wifi_iface, char *htmode);
int set_wifi_htmode(char *wifi_iface, char *htmode);

int get_wifi_hwmode(char *wifi_iface, char *hwmode);
int set_wifi_hwmode(char *wifi_iface, char *hwmode);

int get_wifi_hidden(char *wifi_iface);
int set_wifi_hidden(char *wifi_iface, char *hidden);


int get_wifi_device(char *wifi_iface, char *device);
int set_wifi_device(char *wifi_iface, char *device);

int get_2g_wifi_iface(char *wifi_iface);
int get_5g_wifi_iface(char *wifi_iface);

int get_2g_wifi_device(char *wifi_device);
int get_5g_wifi_device(char *wifi_device);

int get_guest_2g_wifi_iface(char *wifi_iface);
int get_guest_5g_wifi_iface(char *wifi_iface);

int get_guest_2g_wifi_device(char *wifi_device);
int get_guest_5g_wifi_device(char *wifi_device);


typedef enum {
	e_auth_2g_signal_min = 0,
	e_auth_5g_signal_min,
	e_not_auth_2g_signal_min,
	e_not_auth_5g_signal_min
} node_name_t;

int get_signal_min(node_name_t node, char *signal);
int set_signal_min(node_name_t node, char *signal_min);
int del_wifi_section(char *wifi_dev, char *section);

#endif



