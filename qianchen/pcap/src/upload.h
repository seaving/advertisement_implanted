#ifndef __UPLOAD_H__
#define __UPLOAD_H__

#define RE_UPLOAD_MAX_COUNT		3		//重传文件次数
#define RE_UPLOAD_TIMEOUT_SEC	30		//重上传文件时间间隔
#define UPLOAD_FILE_MAX_COUNT	10		//一次上传文件的个数

int create_upload_thread();
char *get_upload_server_ip();

time_t get_file_create_time(char *file);
int get_file_size(const char *path);
void upload_last();

char *get_sime_sn();

#endif

