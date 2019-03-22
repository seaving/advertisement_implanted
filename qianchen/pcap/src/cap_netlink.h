#ifndef __FW_NETLINK_H__
#define __FW_NETLINK_H__

void nl_log_init();
void nl_log(const char* fmt, ...);

int create_net_dev_monitor_thread();
int cap_netlink_start();

#endif


