#ifndef __PING_H__
#define __PING_H__


void ping(int c, int s, char *domain);
void stop_ping();

int ping_result_size();
char* get_result();
bool is_ping_finished(char *result);

#endif


