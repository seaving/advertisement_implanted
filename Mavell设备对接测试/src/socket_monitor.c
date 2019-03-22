#include "includes.h"

#if __SOCKET_MONITOR__

#define ALARM_TIME			1
#define SOCKET_VALID_TIME	60	//1分钟无操作，释放资源

static SocketMonitor_t *SocketMonitorHeader = NULL;
static SocketMonitor_t *SocketMonitorInsert = NULL;

static SocketMonitor_t *SocketMonitorHeaderTmp = NULL;
static SocketMonitor_t *SocketMonitorInsertTmp = NULL;

static pthread_mutex_t node_lock = PTHREAD_MUTEX_INITIALIZER;

static int nodeCnt = 0;
static int nodeCntTmp = 0;

int activity_socket_monitor(phoneinfo_t *phone)
{
	if (phone->monitorflag)
	{
		pthread_mutex_lock(&phone->SM->lock);
		LOG_NORMAL_INFO("%d, had add socket to monitor.\n", phone->fd);
		phone->SM->start = GetSystemTime_Sec();
		pthread_mutex_unlock(&phone->SM->lock);
		return 0;
	}
	return -1;
}

int add_socket_monitor(phoneinfo_t *phone)
{
	SocketMonitor_t *SM = calloc(1, sizeof(SocketMonitor_t));
	if (! SM)
	{
		LOG_PERROR_INFO("calloc error.");
		return -1;
	}
	phone->SM = SM;
	SM->phone = phone;
	SM->start = GetSystemTime_Sec();
	SM->validTime = SOCKET_VALID_TIME;
	pthread_mutex_init(&SM->lock, NULL);
	SM->next = NULL;
	SM->pre = NULL;

	pthread_mutex_lock(&node_lock);
	if (! SocketMonitorHeaderTmp)
	{
		SocketMonitorInsertTmp = SM;
		SocketMonitorHeaderTmp = SocketMonitorInsertTmp;
	}
	else
	{
		SocketMonitorInsertTmp->next = SM;
		SM->pre = SocketMonitorInsertTmp;
		SocketMonitorInsertTmp = SM;
	}
	nodeCntTmp ++;
	phone->monitorflag = 1;	
	LOG_WARN_INFO("----- fd:%d cnttmp:%d -----\n", phone->fd, nodeCntTmp);
	pthread_mutex_unlock(&node_lock);

	return 0;
}

int socket_monitor()
{
	pthread_mutex_lock(&node_lock);
	if (SocketMonitorHeaderTmp
	&& SocketMonitorInsertTmp)
	{
		if (! SocketMonitorHeader)
		{
			//无结点
			SocketMonitorHeader = SocketMonitorHeaderTmp;
			SocketMonitorInsert = SocketMonitorInsertTmp;
		}
		else
		{
			SocketMonitorInsert->next = SocketMonitorHeaderTmp;
			SocketMonitorHeaderTmp->pre = SocketMonitorInsert;
			SocketMonitorInsert = SocketMonitorInsertTmp;
		}
		SocketMonitorHeaderTmp = NULL;
		SocketMonitorInsertTmp = NULL;

		nodeCnt += nodeCntTmp;
		nodeCntTmp = 0;
	}
	else if ((SocketMonitorHeaderTmp && !SocketMonitorInsertTmp)
	|| (!SocketMonitorHeaderTmp && SocketMonitorInsertTmp))
	{
		LOG_ERROR_INFO("So important error !\n");
		_exit_(-1);
	}
	pthread_mutex_unlock(&node_lock);

	SocketMonitor_t *SM = SocketMonitorHeader;
	for ( ; SM; )
	{		
		pthread_mutex_lock(&SM->lock);
		
		SocketMonitor_t *SMtmp = SM->next;
		LOG_NORMAL_INFO("\n************************************\n");
		LOG_NORMAL_INFO("&&&&   %d\n", (int)SM);
		LOG_NORMAL_INFO("processflag: %d\n", SM->phone->processflag);
		//if (! SM->phone->processflag)
		{
			LOG_NORMAL_INFO("phonfd: %d\n", SM->phone->fd);
			if (SM->phone->webserver)
				LOG_NORMAL_INFO("webfd: %d\n", SM->phone->webserver->serverfd);
			LOG_NORMAL_INFO("validTime: %d\n", SM->validTime);
			LOG_NORMAL_INFO("start:    %d\n", SM->start);
			LOG_NORMAL_INFO("free validTime: %d\n", SM->validTime - (GetSystemTime_Sec() - SM->start));
		}
		
		if (SM->phone->processflag
		|| GetSystemTime_Sec() - SM->start >= SM->validTime)
		{
			if (SM->phone->processflag)
			{
				LOG_NORMAL_INFO(">>>>> release .\n");
			}
			else
			{
				LOG_NORMAL_INFO(">>>>> doesn't working !\n");
			}
			if (SM->pre == NULL)
			{
				//头部
				if (SM->next)
				{
					SM->next->pre = NULL;
					SocketMonitorHeader = SM->next;
				}
				else
				{
					//只有一个头结点
					SocketMonitorHeader = NULL;
					SocketMonitorInsert = NULL;
				}
			}
			else if (SM->next == NULL)
			{
				//尾部
				if (SM->pre)
				{
					SM->pre->next = NULL;
					SocketMonitorInsert = SM->pre;
				}
				else
				{
					//只有一个结点
					SocketMonitorHeader = NULL;
					SocketMonitorInsert = NULL;
				}
			}
			else if (SM->pre && SM->next)
			{
				//中间
				SM->next->pre = SM->pre;
				SM->pre->next = SM->next;
			}
			if (! SM->phone->processflag)
			{
				socket_close(SM->phone->serverfd);
				socket_close_client(SM->phone->fd);
				epoll_del_sockfd(SM->phone->epoll_fd, SM->phone->fd);
				epoll_del_sockfd(SM->phone->epoll_serverfd, SM->phone->serverfd);
				free_webserver(SM->phone->webserver);
				free_phoneinfo(SM->phone);
			}
			pthread_mutex_unlock(&SM->lock);
			pthread_mutex_destroy(&SM->lock);
			free (SM->phone);
			SM->phone = NULL;
			free (SM);
			SM = NULL;
			nodeCnt --;
		}
		if (SM)
		{
			pthread_mutex_unlock(&SM->lock);
		}
		LOG_NORMAL_INFO("nodeCnt: %d\n", nodeCnt);
		LOG_NORMAL_INFO("************************************\n");
		SM = SMtmp;
	}

	return 0;
}

void timer(int sig)
{  
    if(SIGALRM == sig)
    {
		socket_monitor();
		alarm(ALARM_TIME);
    }

    return ;  
}

int set_timer()
{
	signal(SIGALRM, timer);
	alarm(ALARM_TIME);
	return 0;
}

void socket_monitor_init()
{
	set_timer();
}
#endif
