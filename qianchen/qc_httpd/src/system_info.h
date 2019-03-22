#ifndef __SYSTEM_INFO_H__
#define __SYSTEM_INFO_H__




//long get_used_ddr_Size();

//long get_total_ddr_Size();

//long get_free_ddr_Size();

int get_cpu_info(char *cpu);

int get_system_version(char *version);

int get_hostname(char *hostname);

int set_hostname(char *hostname);

int get_machine(char *machine);

char *get_netdev_wan_name();
char *get_netdev_lan_name();

void get_segment(char *ipseg, char *dev);
void get_segment2(char *ipseg, char *dev);
void get_segment3(char *ipseg, char *dev);

//int get_wifi_client_count();
int get_segment_count(char *dev);

void init_netdev();

int set_dev_id(char *devNo);

int get_model(char *model, int model_buf_size);

int get_dev_id(char *devNo);

int get_firmware_version(char *fwv);

int get_gcc_version(char *gccv);

int get_firmware_id(char *fwid);

int get_plugin_version(char *pv);

#endif





