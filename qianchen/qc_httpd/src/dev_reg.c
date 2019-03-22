
#include "includes.h"

//保存到/etc/config下，保证下次刷机不会被擦除
#define DEV_NO_FILE		"/etc/config/devno.conf"

int save_devno(char *devNo)
{
	unlink(DEV_NO_FILE);

	create_file(DEV_NO_FILE);

	int fd = open_file(DEV_NO_FILE);
	if (fd < 0)
		return -1;

	if (write(fd, devNo, strlen(devNo)) != strlen(devNo))
	{
		close(fd);
		LOG_PERROR_INFO("write error.");
		return -1;
	}

	close(fd);

	return 0;
}

int read_devno(char *devNo)
{
	int fd = open_file(DEV_NO_FILE);
	if (fd < 0)
		return -1;

	int ret = read(fd, devNo, 32);
	if (ret <= 0)
	{
		close(fd);
		LOG_PERROR_INFO("read error.");
		return -1;
	}

	close(fd);

	return ret;
}

