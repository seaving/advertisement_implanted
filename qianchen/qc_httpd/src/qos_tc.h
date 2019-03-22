#ifndef __QOS_TC_H__
#define __QOS_TC_H__




void qos_init_tc_model(int forward_max_speed);

void qos_new_tc_speed_limit_rule(char *classid, int set_speed_limit, int mark);

unsigned long long get_traffic_upload_total_count();
unsigned long long get_traffic_download_total_count();

unsigned long long get_traffic_total_count();

int get_network_speed(int *upload_speed, int *download_speed, int *total_speed);

#endif


