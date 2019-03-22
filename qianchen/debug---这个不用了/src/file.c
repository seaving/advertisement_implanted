#include "includes.h"


int get_file_size(char *path)
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

int create_file(char *filepath)
{
	int mod;
	if (access(filepath, F_OK) != 0)
		mod = O_CREAT | O_RDWR | O_TRUNC;
	else
		mod = O_CREAT | O_RDWR;

	int fd = open(filepath, mod, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH);
	if(fd < 0)
	{
		LOG_PERROR_INFO("open %s error.", filepath);
		return -1;
	}
	close(fd);
	return 0;
}

int open_file(char *filepath)
{
	int fd = open(filepath, O_RDWR);
	if(fd < 0)
	{
		LOG_PERROR_INFO("open %s error.", filepath);
		return -1;
	}
	return fd;
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


