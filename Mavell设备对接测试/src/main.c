#include "includes.h"

void dns_test(char *host)
{
	char ip[32] = {0};
	struct timeval tBegin, tEnd;
	gettimeofday(&tBegin, NULL);
	int ret = get_addr_info(host, ip);
	gettimeofday(&tEnd, NULL);
	long deltaTime = 1000000L * (tEnd.tv_sec - tBegin.tv_sec ) + (tEnd.tv_usec - tBegin.tv_usec);
	if (ret >= 0)
	{
		printf("%s -- > %s timstrip = %ld ms\n", host, ip, deltaTime / 1000);
	}
}

void connecttest(char *host, int port)
{
	struct timeval tBegin, tEnd;
	gettimeofday(&tBegin, NULL);
	int fd = connecthost(host, port, E_CONNECT_NOBLOCK);
	gettimeofday(&tEnd, NULL);
	long deltaTime = 1000000L * (tEnd.tv_sec - tBegin.tv_sec ) + (tEnd.tv_usec - tBegin.tv_usec);
	if (fd > 0)
	{
		printf("connect success. fd = %d, timstrip = %ld ms\n", fd, deltaTime / 1000);
		close(fd);
	}
}

void *test_thread(void *arg)
{
	char *host = "img4.bitautoimg.com";
	dns_test(host);
	connecttest(host, 80);
	return NULL;
}

int main()
{
	mitm_start();

#if 0
	signal_task_exit();
	signal_sigpipe();
	tpool_create(300);
	while (1)
	{
		tpool_add_work(test_thread, NULL);
		usleep(10000);
	}
#endif
	return 0;
}

