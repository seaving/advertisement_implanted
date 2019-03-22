#include "includes.h"

bool qc_pipe_create(int *rfd, int *wfd)
{
	int pipefd[2];
	if (pipe(pipefd) != 0)
	{
		LOG_PERROR_INFO("pipe error!");
		return false;
	}
	*rfd = pipefd[0];
	*wfd = pipefd[1];
	return true;
}

void qc_pipe_destroy(int rfd, int wfd)
{
	if (rfd > 0)
	{
		close(rfd);
	}
	if (wfd > 0)
	{
		close(wfd);
	}
}

int qc_pipe_write(int wfd, char *data, int data_len)
{
	int len = write(wfd, data, data_len);
	if (len != data_len)
	{
		LOG_PERROR_INFO("write error.");
		return -1;
	}
	
	return len;
}

int qc_pipe_read(int rfd, char *buf, int read_len)
{
	int len = read(rfd, buf, read_len);
	if (len <= 0)
	{
		LOG_PERROR_INFO("read error.");
		return -1;
	}
	
	return len;	
}

