#ifndef __IP_CHECK_H__
#define __IP_CHECK_H__

#include "includes.h"

bool is_ip_str(char *str);

bool is_mask_valid(char *mask);

unsigned int net_mask_to_int(char *mask);
int get_network_segment(char *ip, char *mask, char *segment);
unsigned int get_network_segment_int(char *ip, char *mask);
bool is_sub_ip_valid(unsigned int sub_ip, char *mask);

unsigned int get_sub_ip_min();
unsigned int get_sub_ip_max(char *mask);
int int_ip_to_str(unsigned int int_ip, char *str_ip);
unsigned int get_sub_ip_int(char *ip, char *mask);
bool is_lan_ip(char *ip, char *br_dev);
bool is_same_segment(char *ip1, char *ip2, char *mask);

#endif


