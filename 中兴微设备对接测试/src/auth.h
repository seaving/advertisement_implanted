
#ifndef __AUTH_H__
#define __AUTH_H__	1

#define _AUTH_ON_	0
void auth_firewall_init();
int check_validTime_thread();
void auth_struct_init();

int do_auth(int phonefd, char *host, int host_len, char *uri, int uri_len, char *header, int header_len);

#endif

