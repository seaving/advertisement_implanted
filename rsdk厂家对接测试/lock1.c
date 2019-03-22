#include <sys/file.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

int main(void)
{

	int fd = open("lock1file", O_CREAT | O_RDWR);
	if (fd < 0)
	{
		perror("open lock1 error.");
		return -1;
	}
	
	int ret = flock(fd, LOCK_EX);
	if (ret < 0)
	{
		perror("flock lock1 error.");
	}

	sleep(5);
	printf("begin...\n");
	int fd2 = open("lock2file", O_RDWR);
	if (fd < 0)
	{
		perror("open lock2file error.");
		return -1;
	}
	
	ret = flock(fd2, LOCK_EX);
	if (ret < 0)
	{
		perror("flock lock2 error.");
	}
	
	while(1)
	{
		printf("lock2 is exit!\n");
		sleep(3);
	}
	return 0;
}
