#ifndef __FW_H__
#define __FW_H__

void fw_init();
void fw_mark_set(char *ip, char *mac);
void fw_del_rule(int num);

void fw_redirect_mproxy();
void fw_reflush_mproxy();

#endif

