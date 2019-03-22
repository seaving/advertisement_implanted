#include "includes.h"

static char *_lan_name = "br-lan";
static char _net_mask_lan[32] = "255.255.255.0";


void init_netdev()
{
	get_dev_mask(_net_mask_lan, _lan_name);
}

char *get_netdev_lan_name()
{
	return _lan_name;
}

char *get_netdev_lan_mask()
{
	return _net_mask_lan;
}

