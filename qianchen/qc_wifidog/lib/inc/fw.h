#ifndef __FW_H__
#define __FW_H__


#include "common.h"

void bwlist_fw_redirect_init();
void bwlist_fw_redirect_refresh();

void bwlist_fw_host_drop_reflush();
bool bwlist_fw_set_host_drop(char *host_buf);

#endif

