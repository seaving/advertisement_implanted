#include "includes.h"

int get_file_size(const char *path)
{  
    int filesize = -1;  
    FILE *fp;
    fp = fopen(path, "r");
    if(fp == NULL)
        return filesize;
    fseek(fp, 0L, SEEK_END);
    filesize = ftell(fp);
    fclose(fp);
    return filesize;
}

int readLineFromFile(char *file, int offset, char *buf, int size)
{
	int fd = open(file, O_RDWR);
	if (fd <= 0)
	{
		LOG_PERROR_INFO("open %s error!", file);
		goto _return_error_;
	}

	lseek(fd, offset, SEEK_SET);
	offset = 0;
	int ret = -1;
	while (1)
	{
		ret = read(fd, buf + offset, 1);
		if (ret != 1)
			goto _return_error_;
		offset += ret;
		if (offset >= size - 1)
		{
			buf[offset] = 0;
			break;
		}
		buf[offset] = 0;
		if (buf[offset - 1] == '\n')
			break;
	}

	close(fd);
	return offset;
_return_error_:
	close(fd);
	return -1;
}

int safe_read_file(char *file, char *buf, int len)
{
	int fd = open(file, O_RDWR);
	if (fd < 0)
	{
		LOG_PERROR_INFO("open %s error.", file);
		return -1;
	}
	
	flock(fd, LOCK_EX);
	int ret = read(fd, buf, len);
	if (ret <= 0)
	{
		LOG_PERROR_INFO("read error.");
	}
	flock(fd, LOCK_UN);

	close(fd);

	return ret;
}


