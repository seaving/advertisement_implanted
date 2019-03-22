#ifndef __FILE_H__
#define __FILE_H__

#include "common.h"

#define THE_FILE_EXIST(path)	((access(path, F_OK)) != -1)

int get_file_size(char *path);

int open_file(char *filepath);
int create_file(char *filepath);

typedef void (*popen_call)(char *, void *);
int popen_cmd(char *cmd, popen_call call, void *arg);

#endif

