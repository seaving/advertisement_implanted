#include <sys/file.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

int main(void)
{

	int fd = open("lock2file", O_CREAT | O_RDWR);
	if (fd < 0)
	{
		perror("open lock2 error.");
		return -1;
	}
	
	int ret = flock(fd, LOCK_EX);
	if (ret < 0)
	{
		perror("flock lock2 error.");
	}

	sleep(5);
	printf("begin...\n");
	int fd2 = open("lock1file", O_RDWR);
	if (fd < 0)
	{
		perror("open lock1file error.");
		return -1;
	}
	
	ret = flock(fd2, LOCK_EX);
	if (ret < 0)
	{
		perror("flock lock1 error.");
	}
	
	while(1)
	{
		printf("lock1 is exit!\n");
		sleep(3);
	}
	return 0;
}