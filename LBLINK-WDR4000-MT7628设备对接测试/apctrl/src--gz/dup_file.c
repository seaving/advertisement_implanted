#include "includes.h"

static int g_dup_fd = -1;

#define APCTRL_LOG	"/mnt/apctrl_log"

int dup_file()
{
    //打开一个文件
    mkdir(APCTRL_LOG, 0777);
    
    int year, mon, data, hour, min, sec;
    
    GetAllSystemTime(&year, &mon, &data, &hour, &min, &sec, NULL);
    char pathname[256];
    sprintf(pathname, APCTRL_LOG"/%d%d-%d%d-%d%d-%d%d-%d%d-%d%d.txt", year/10, year%10, 
    				mon/10, mon%10, data/10, data%10, hour/10, hour%10, min/10, min%10, sec/10, sec%10);
    int fd = open(pathname, O_CREAT | O_RDWR | O_APPEND, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH);
    if(fd < 0)
    {
        LOG_PERROR_INFO("open %s error.", pathname);
        return -1;
    }
#if 0
	g_dup_fd = fd;
	return 0;
#else
	//进行重定向  
	g_dup_fd = dup2(fd, 1);
	if(g_dup_fd < 0)
	{
		LOG_PERROR_INFO("dup2 error.");
		goto _return_error_;
	}
	close(fd);
	return 0;

_return_error_:

	if (fd > 0)
		close(fd);
	if (g_dup_fd > 0)
    	close(g_dup_fd);
    g_dup_fd = -1;
    return -1;
#endif
    
}

int dup_close()
{
	if (g_dup_fd > 0)
		close(g_dup_fd);
	g_dup_fd = -1;
	return 0;
}

int dup_remove()
{
	remove(APCTRL_LOG);
	return 0;
}

