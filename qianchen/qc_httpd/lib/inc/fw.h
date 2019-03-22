#ifndef __FW_H__
#define __FW_H__

void bwlist_fw_redirect_init();

void bwlist_fw_redirect_refresh();

void bwlist_fw_host_drop(char *host_buf);

void bwlist_fw_host_drop_reflush();

#endif

