#ifndef __AUTH_FW_H__
#define __AUTH_FW_H__

int auth_allow(char *ip, char *mac,int vaild_time, bool flag);
int auth_is_allow(char *ip, char *mac);
int auth_delete(char *ip, char *mac);

#endif

