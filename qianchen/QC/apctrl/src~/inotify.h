#ifndef __INOTIFY_H__
#define __INOTIFY_H__

extern char monitorDir[15];
extern char monitorFile[64];

extern int restart_flag;

int create_monitor_file_thread();
int monitor_send_state(char *data, int *curTime, int *oldTime, int timeout);

void inotify_wait_cond();
void inotify_send_cond();


#endif

