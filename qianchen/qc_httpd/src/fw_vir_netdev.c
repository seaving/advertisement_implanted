#include "includes.h"

//创建虚拟网卡
void fw_vir_netdev_create()
{
	char cmd[255];
	EXECUTE_CMD(cmd, "ip link set %s up", VIR_NET_DEV);
}

//销毁虚拟网卡
void fw_vir_netdev_destroy()
{
	char cmd[255];
	EXECUTE_CMD(cmd, "ip link set %s down", VIR_NET_DEV);
}

//重定向下行流量到虚拟网卡
void fw_tc_redirect_to_vir_netdev()
{
	char cmd[255];

	//在WAN设备上建立一个下行跟结点
	EXECUTE_CMD(cmd, "tc qdisc add dev %s handle ffff: ingress", LAN_DEV);
	
	//把下行的流量重定向到虚拟网卡
	EXECUTE_CMD(cmd, "tc filter add dev %s parent ffff: protocol ip u32 match u32 0 0 action mirred egress redirect dev %s", LAN_DEV, VIR_NET_DEV);
}

//删除重定向队列规则
void fw_tc_del_redirect_to_vir_netdev_rule()
{
	char cmd[255];
	EXECUTE_CMD(cmd, "tc qdisc del dev %s handle ffff: ingress", LAN_DEV);
}

