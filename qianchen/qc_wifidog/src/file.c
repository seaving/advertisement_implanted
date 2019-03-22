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
	for ( ; ; )
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

int popen_cmd(char *cmd, popen_call call, void *arg)
{	
	FILE *fp = NULL;

	LOG_NORMAL_INFO("%s\n", cmd);

	fp = popen(cmd, "r");
	if (NULL == fp) 
	{  
		LOG_PERROR_INFO("popen error!");
		return -1;
	}

	char buf[255];
	while (fgets(buf, 255, fp) != NULL)
	{
		char *p = strchr(buf, '\n');
		if (p)
			*p = 0;
		p = strchr(buf, '\r');
		if (p)
			*p = 0;
		
		LOG_NORMAL_INFO("%s\n", buf);

		if (call)
			call(buf, arg);

		memset(buf, 0, sizeof(buf));
	}
	
	pclose(fp);
	
	return 0;
}


