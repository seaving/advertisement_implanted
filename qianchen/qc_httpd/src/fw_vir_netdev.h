#ifndef __FW_VIR_NETDEV_H__
#define __FW_VIR_NETDEV_H__

void fw_tc_del_redirect_to_vir_netdev_rule();
void fw_vir_netdev_destroy();

void fw_vir_netdev_create();
void fw_tc_redirect_to_vir_netdev();

#endif


