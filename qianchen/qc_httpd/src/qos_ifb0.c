#include "includes.h"

void _init_ifb0()
{
	char cmd[255] = {0};
	EXECUTE_CMD(cmd, "ip link set ifb0 down");
	sleep(1);
	EXECUTE_CMD(cmd, "ip link set ifb0 up");
	sleep(1);
}

void _init_ingress()
{
	char cmd[255] = {0};
	EXECUTE_CMD(cmd, "tc qdisc add dev %s handle ffff: ingress", get_netdev_wan_name());
 	EXECUTE_CMD(cmd, "tc filter add dev %s parent ffff: protocol ip u32 " 
				"match u32 0 0 action mirred egress redirect dev ifb0", get_netdev_wan_name());
}

void destroy_ingress()
{
	char cmd[255] = {0};
	EXECUTE_CMD(cmd, "tc qdisc del dev %s handle ffff: ingress", get_netdev_wan_name());
	EXECUTE_CMD(cmd, "ip link set ifb0 down");
	sleep(1);
}

void init_ingress()
{
	_init_ifb0();
	_init_ingress();
}




