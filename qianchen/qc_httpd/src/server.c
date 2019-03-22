#if 0

#include "includes.h"

#define _close_all_fd_ { \
	int close_all_fd_loop_i = 0; \
	for (close_all_fd_loop_i = 3; close_all_fd_loop_i < sysconf(_SC_OPEN_MAX); close_all_fd_loop_i ++) \
	{ \
		close(close_all_fd_loop_i); \
	} \
}

#define MAXBUF	512

static volatile int g_client_count = 0;

void *webtalk_thread(void *arg);
int webtalk(int cfd);

int server_create()
{
	int listen_fd = socket_listen(NULL, WEB_SERVER_PORT, 300);
	if (listen_fd <= 0)
	{
		return -1;
	}

	//初始化cookie
	s_cookie_init();

	struct sockaddr_in clientaddr;
	int clientlen = sizeof(clientaddr);
	
	int maxfd = listen_fd;
	fd_set fd_read;
	struct timeval timeout;
	
#if DEBUG_ON
	unsigned char time_cnt = 0;
#endif

	while (1)
	{
		timeout.tv_sec = 5;
		timeout.tv_usec = 0;

		FD_ZERO(&fd_read);
		FD_SET(listen_fd, &fd_read);
		int ret = select(maxfd + 1, (fd_set *)&fd_read, (fd_set *)NULL, (fd_set *)NULL, &timeout);
		if (ret == 0)
		{
			//timeout
		}
		else if (ret < 0)
		{
			//select error
			goto _return_error_;
		}
		else
		{
			if (FD_ISSET(listen_fd, &fd_read))
			{
				int phonefd = accept(listen_fd, (SA *)&clientaddr, (socklen_t *)&clientlen);
				if (phonefd < 0)
				{
					//error
					goto _return_error_;
				}
				else
				{
#if 1				
					int *arg = calloc(1, sizeof(int));
					if (! arg)
					{
						LOG_PERROR_INFO("calloc error.");
						goto _return_error_;
					}
					
					*arg = phonefd;
				
					pthread_t thip;
					if (pthread_create(&thip, NULL, webtalk_thread, arg) != 0)
					{
						LOG_PERROR_INFO("pthread_create error.");
						free (arg);
						socket_close_client(phonefd);
						goto _return_error_;
					}
#endif
				}
			}
		}

#if DEBUG_ON
		time_cnt ++;
		if (time_cnt >= 6)
		{
			time_cnt = 0;
			
			LOG_NORMAL_INFO("There are [ %d ] client connection .\n", g_client_count);
		}
#endif
	}

_return_error_:

	socket_close(listen_fd);
	_close_all_fd_;

	exit(-1);
	
	return -1;
}

void *webtalk_thread(void *arg)
{
	pthread_detach(pthread_self());
	if (! arg)
		return NULL;

	int fd = *((int *)arg);
	free (arg);
	arg = NULL;

	LOG_NORMAL_INFO("new phone connected. [ fd: %d ]\n", fd);

	//设置为非阻塞
	socket_setfd_noblock(fd);

	g_client_count ++;
	webtalk(fd);
	socket_close_client(fd);
	g_client_count --;

	return NULL;
}

int webtalk(int cfd)
{	
	char method[20];
	int method_len;
	method_len = read_method(cfd, method);
	if (method_len <= 0)
	{
		goto _return_;
	}
	method[method_len] = '\0';
	
	switch (httpmethod_cmp(method))
	{
		case E_GET:
		{
			analysis_get(cfd);
			break;
		}
		case E_POST:
		{
			analysis_post(cfd);
			break;
		}
		default :
		{
			//LOG_NORMAL_INFO("unkown http method!\n");
			break;
		}
	}

_return_:
	return 0;
}

void *server_work(void *arg)
{
	pthread_detach(pthread_self());
	
	server_create();
	return NULL;
}

int create_server_thread()
{
	pthread_t tdp;
	return pthread_create(&tdp, NULL, server_work, NULL);
}
#endif

