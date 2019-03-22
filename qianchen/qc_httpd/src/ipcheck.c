#include "includes.h"


bool is_digit_str(char *str)
{
	int i;
	for (i = 0; i < strlen(str); i ++)
	{
		if (str[i] < '0' && str[i] > '9')
			return false;
	}
	return true;
}

bool is_ip_str(char *str)
{
	char ipseg0[6] = {0};
	char ipseg1[6] = {0};
	char ipseg2[6] = {0};
	char ipseg3[6] = {0};

	unsigned char i, j = 0;
	unsigned char cut[22] = {0};

	if (strlen(str) > 20 || strlen(str) < 7)
		return false;
	
	for (i = 0; i < strlen(str); i ++)
	{
		if (str[i] == '.')
			cut[j ++] = i;
	}

	if (j != 3)
		return false;

	if (cut[0] > 3 || cut[0] <= 0)
		return false;
	if (cut[1] - cut[0] - 1 > 3 || cut[1] - cut[0] - 1 <= 0)
		return false;
	if (cut[2] - cut[1] - 1 > 3 || cut[2] - cut[1] - 1 <= 0)
		return false;
	if (strlen(str) - cut[2] - 1 > 3 || strlen(str) - cut[2] - 1 <= 0)
		return false;

	memcpy(ipseg0, str, cut[0]);
	memcpy(ipseg1, str + cut[0] + 1, cut[1] - cut[0] - 1);
	memcpy(ipseg2, str + cut[1] + 1, cut[2] - cut[1] - 1);
	memcpy(ipseg3, str + cut[2] + 1, strlen(str) - cut[2] - 1);

	if (! is_digit_str(ipseg0) || ! is_digit_str(ipseg1) || ! is_digit_str(ipseg2) || ! is_digit_str(ipseg3))
		return false;

	return true;
}

bool is_mask_valid(char *mask)
{
	if (! is_ip_str(mask))
		return false;

	unsigned int mask_int = net_mask_to_int(mask);

	int i;
	unsigned int tmp = mask_int;
	for (i = 0; i < 32; i ++)
	{
		if (tmp & 0x1)
		{
			break;
		}

		tmp >>= 1;
	}

	if (i >= 32 || i <= 0)
	{
		return false;
	}

	for ( ; i < 32; i ++)
	{
		if (! (tmp & 0x1))
		{
			break;
		}

		tmp >>= 1;
	}

	if (i < 32)
	{
		return false;
	}	

	return true;
}

unsigned int net_mask_to_int(char *mask)
{
	struct in_addr net_mask;
	inet_aton(mask, &net_mask);
	unsigned int int_mask = ntohl(net_mask.s_addr);
	
	return int_mask;
}

int get_network_segment(char *ip, char *mask, char *segment)
{
	struct in_addr ipaddr;
	inet_aton(ip, &ipaddr);
	unsigned int int_ip = ntohl(ipaddr.s_addr);
	unsigned int mask_int = net_mask_to_int(mask);

	int_ip = int_ip & mask_int;
	struct in_addr addr;
	addr.s_addr = htonl(int_ip);
	sprintf(segment, "%s", inet_ntoa(addr));
	return 0;
}

int int_ip_to_str(unsigned int int_ip, char *str_ip)
{
	struct in_addr addr;
	addr.s_addr = htonl(int_ip);
	sprintf(str_ip, "%s", inet_ntoa(addr));
	return 0;
}

unsigned int str_ip_to_int(char *str_ip)
{
	struct in_addr ipaddr;
	inet_aton(str_ip, &ipaddr);
	unsigned int int_ip = ntohl(ipaddr.s_addr);

	return int_ip;
}

unsigned int get_network_segment_int(char *ip, char *mask)
{
	struct in_addr ipaddr;
	inet_aton(ip, &ipaddr);
	unsigned int int_ip = ntohl(ipaddr.s_addr);
	unsigned int mask_int = net_mask_to_int(mask);

	int_ip = int_ip & mask_int;
	return int_ip;
}

bool is_sub_ip_valid(unsigned int sub_ip, char *mask)
{
	unsigned int mask_int = net_mask_to_int(mask);
	unsigned int sub_max = 0xFFFFFFFF - mask_int;

	if (2 <= sub_ip && sub_ip < sub_max)
	{
		return true;
	}

	return false;
}

bool is_lan_ip(char *ip, char *br_dev)
{
	char br_ip[32] = {0};
	get_dev_ip(br_ip, br_dev);
	
	char mask[32] = {0};
	get_dev_mask(mask, br_dev);

	unsigned int s1 = get_network_segment_int(ip, mask);
	unsigned int s2 = get_network_segment_int(br_ip, mask);

	if (s1 == s2)
		return true;
	
	return false;
}

bool is_same_segment(char *ip1, char *ip2, char *mask)
{
	unsigned int s1 = get_network_segment_int(ip1, mask);
	unsigned int s2 = get_network_segment_int(ip2, mask);

	if (s1 == s2)
		return true;
	
	return false;
}

unsigned int get_sub_ip_min()
{
	return 2;
}

unsigned int get_sub_ip_int(char *ip, char *mask)
{
	unsigned int segment = get_network_segment_int(ip, mask);
	unsigned int ip_int = str_ip_to_int(ip);
	return ip_int - segment;
}

unsigned int get_sub_ip_max(char *mask)
{
	return 0xFFFFFFFF - net_mask_to_int(mask) - 1;
}


