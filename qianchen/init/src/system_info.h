#ifndef __SYSTEM_INFO_H__
#define __SYSTEM_INFO_H__

void init_netdev();

char *get_netdev_wan_name();

char *get_netdev_lan_name();

int set_dev_id(char *devNo);

int get_model(char *model, int model_buf_size);

int get_dev_id(char *devNo);

int get_firmware_version(char *fwv);

int get_gcc_version(char *gccv);

int get_firmware_id(char *fwid);

int get_plugin_version(char *pv);


#endif


