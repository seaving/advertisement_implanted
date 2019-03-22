#ifndef __FILE_H__
#define __FILE_H__ 1

int get_file_size(const char *path);
int readLineFromFile(char *file, int offset, char *buf, int size);

int safe_read_file(char *file, char *buf, int len);

#endif


