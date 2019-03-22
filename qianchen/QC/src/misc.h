#ifndef __MISC_H__
#define __MISC_H__

void init_pid_file();
int save_pid(char *pname);

void *misc(void *arg);
void _exit_(int ret);

#endif
