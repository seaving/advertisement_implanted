#include "includes.h"

int connect_noblock(int clientfd, struct sockaddr_in *serveraddr)
{
	if (connect(clientfd, (SA *)serveraddr, sizeof(struct sockaddr_in)) >= 0) {
		//connect success
		return 0;
	}
#ifdef DEBUG_SOCKET	
	LOG_WARN_INFO("(s:%d)connect need to check. e:%d, %d, %d %s\r\n", 
					clientfd, errno, ENETUNREACH, EINPROGRESS, strerror(errno));
#endif

	// 不是正在连接，表示是其它错误，则应该是网络原因导致CONNECT失败，则直接返回错误
	// 网络不可到达，表示网络有问题
	// ENETUNREACH : 101
	// EINPROGRESS : 115
	if ((errno != EINPROGRESS) || (errno == ENETUNREACH))
	{
		//网络不可达，网络有问题
		return -1;
	}

	// 检测是否有数据
	fd_set socketRed;
	fd_set socketWr;
	struct timeval sTimeout;
	int i = 0, ret = -1;
    socklen_t len;
    int iError = -1;
    
	while (1)
	{
		// 实际测试中，直接关机后启动的第一次连接，一般会超时，然后再连接的时候，会很快连接上(下面打印的计数次数为7)
		i ++;
		if (i > 20)
		{
#ifdef DEBUG_SOCKET
			LOG_WARN_INFO("(s:%d) connect time out.\r\n", clientfd);
#endif
			//超时退出
			return -1;
		}
#ifdef DEBUG_SOCKET
		LOG_NORMAL_INFO("(s:%d) connect ...........................%d\r\n", clientfd, i);
#endif

		// 用select来判断是否connect成功
		// 初始化
		FD_ZERO(&socketRed);
		FD_ZERO(&socketWr);
		FD_SET (clientfd, &socketRed);
		FD_SET (clientfd, &socketWr);

		// 设定延时时间
		sTimeout.tv_sec  = 1;
		sTimeout.tv_usec = 0;

		// select
		ret = select(clientfd + 1, (fd_set*)(&socketRed), (fd_set*)(&socketWr), (fd_set*)NULL, &sTimeout);
		if (ret <= 0)
		{
			if (ret == 0)
			{
				//select超时
				continue;
			}
			if (ret < 0)
			{
				//select出错
				return -1;
			}
		}
		// 套接字可读、可写
		// 需要进一步判断
		if ((FD_ISSET(clientfd, &socketRed) > 0)
		 && (FD_ISSET(clientfd, &socketWr) > 0))
		{
#ifdef DEBUG_SOCKET		
			LOG_WARN_INFO("socket : %d read & write, need to check agin.\r\n", clientfd);
#endif
			len = sizeof(iError);
			ret = getsockopt(clientfd, SOL_SOCKET, SO_ERROR, &iError, &len);
			if (ret < 0)
			{
#ifdef DEBUG_SOCKET	
				LOG_ERROR_INFO("Get SO_ERROR error, connect failt.\r\n");
#endif
				// 获取SO_ERROR属性，也可能失败
				return -1;
			}
#ifdef DEBUG_SOCKET
			LOG_ERROR_INFO("Get ERROR parma:%d, %d\r\n", iError, ret);
#endif

			if (iError != 0)
			{
#ifdef DEBUG_SOCKET
				LOG_ERROR_INFO("ERROR != 0; connect failt.\r\n");
#endif
				// 表示链接到现在还没有建立
				// return -1;

				continue;
			}

			if (0 == iError)
			{
#ifdef DEBUG_SOCKET			
				LOG_NORMAL_INFO("ERROR = 0; connect success.\r\n");
#endif
				// 表示链接已经建立
				return 0;
			}
		}

		// socket 可写不可读，表示链接建立
		if ((FD_ISSET(clientfd, &socketRed) <= 0)
		 && (FD_ISSET(clientfd, &socketWr) > 0))
		{
#ifdef DEBUG_SOCKET
			LOG_NORMAL_INFO("socket:%d write & noread, connect success.\r\n", clientfd);
#endif
			return 0;
		}

		// socket 可读不可写，不知道怎么处理，先这么处理看看
		if ((FD_ISSET(clientfd, &socketRed) > 0)
		 && (FD_ISSET(clientfd, &socketWr) <= 0))
		{
#ifdef DEBUG_SOCKET
			LOG_ERROR_INFO("socket:%d read & nowrite, connect failt.\r\n", clientfd);
#endif
			// TCP socket 被设为非阻塞后调用 connect ，connect 函数如果没有马上成功，
			// 会立即返回 EINPROCESS（如果被中断返回EINTR） ，但 TCP 的 3 次握手继续进行。
			// 之后可以用 select 检查连接是否建立成功（但不能再次调用connect，这样会返回
			// 错误EADDRINUSE）,此处建议返回错误，走重新初始化流程(在实际测试中如果直接
			// 返回1或是continue都有可能导致程序的退出).
			return -1;
		}
	}
	
	return -1;
}


int open_clientfd(char *bind_ip, char *hostname, int port)
{
    int clientfd;
    struct sockaddr_in serveraddr;
    struct sockaddr_in local_addr;
	char ip[20] = {0};
	
    if ((clientfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
		return -1; /* check errno for cause of error */
	}
	struct linger linger;
	linger.l_onoff = 0;
	linger.l_linger = 0;
	int optlen = sizeof(linger);
	setsockopt(clientfd, SOL_SOCKET, SO_LINGER, (char *)&linger, optlen);

    /* Fill in the server's IP address and port */
	if (get_addr_info(hostname, ip) < 0)
	{
		close(clientfd);
		return -2;
	}
	
	bzero((char *) &serveraddr, sizeof(serveraddr));
	serveraddr.sin_family = AF_INET;
	serveraddr.sin_addr.s_addr = inet_addr(ip);
	serveraddr.sin_port = htons(port);
	if (bind_ip)
	{
		bzero((char *) &local_addr, sizeof(local_addr));
		local_addr.sin_family = AF_INET;
		local_addr.sin_addr.s_addr = inet_addr(bind_ip);
		local_addr.sin_port = 0;
		if (bind(clientfd, (struct sockaddr*)&local_addr, sizeof(local_addr)) != 0)
		{
			LOG_PERROR_INFO("bind error");
			close(clientfd);
			return -1;
		}
	}
    /* Establish a connection with the server */
    if (connect(clientfd, (SA *) &serveraddr, sizeof(serveraddr)) < 0)
    {
    	close(clientfd);
		return -1;
	}
    return clientfd;	
}

int open_clientfd_noblock(char *bindip, char *hostname, int port)
{
    int clientfd;
    struct sockaddr_in serveraddr;
	struct sockaddr_in local_addr;
	
    if ((clientfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
		return -1;
	}

    char ipaddr[32] = {0};
    int ret = get_addr_info(hostname, ipaddr);
    if (ret == -1)
    {
    	close(clientfd);
		return -2;
	}
	else if (ret == -2)
	{
		close(clientfd);
		return -5;
	}
	
	struct linger linger;
	linger.l_onoff = 0;
	int optlen = sizeof(linger);
	setsockopt(clientfd, SOL_SOCKET, SO_LINGER, (char *)&linger, optlen);

	socket_setfd_noblock(clientfd);

	bzero((char *) &serveraddr, sizeof(serveraddr));
	serveraddr.sin_family = AF_INET;
	serveraddr.sin_addr.s_addr = inet_addr(ipaddr);
	serveraddr.sin_port = htons(port);
	
	if (bindip)
	{
		bzero((char *) &local_addr, sizeof(local_addr));
		local_addr.sin_family = AF_INET;
		local_addr.sin_addr.s_addr = inet_addr(bindip);
		local_addr.sin_port = 0;
		if (bind(clientfd, (struct sockaddr*)&local_addr, sizeof(local_addr)) != 0)
		{
			LOG_PERROR_INFO("bind error");
			close(clientfd);
			return -1;
		}
	}

	if (connect_noblock(clientfd, &serveraddr) < 0)
	{		
		close(clientfd);
		return -3;
	}
	
	socket_setfd_block(clientfd);

    return clientfd;
}

int connect_host_block(char *hostname, int port)
{
    int rc;
	
    if ((rc = open_clientfd(NULL, hostname, port)) < 0)
    {
		if (rc == -1)
		{
		    LOG_PERROR_INFO("Open_clientfd Unix error");
		}
		else
		{
			LOG_PERROR_INFO("%s:%d DNS ERROR\n", hostname, port);
		}
    }
    return rc;
}


int connect_host_noblock(char *hostname, int port)
{
    int rc;
	
    if ((rc = open_clientfd_noblock(NULL, hostname, port)) < 0)
    {
		if (rc == -1)
		{			
			LOG_ERROR_INFO("%s:%d socket create error!!!\n", hostname, port);
		}
		else if (rc == -2)
		{
			LOG_ERROR_INFO("%s:%d DNS ERROR!!!\n", hostname, port);
		}
		else if (rc == -3)
		{
			LOG_ERROR_INFO("%s:%d connect error!!!\n", hostname, port);		
		}
		else if (rc == -5)
		{
			LOG_ERROR_INFO("filter host : %s.\n", hostname);
		}
    }
    return rc;
}

int connecthost(char *hostname, int port, e_connectmode mode)
{
	int ret = -1;
	switch (mode)
	{
		case E_CONNECT_BLOCK:
			ret = connect_host_block(hostname, port);
			break;
		case E_CONNECT_NOBLOCK:
			ret = connect_host_noblock(hostname, port);
			break;
		default:
			break;
	}
	return ret;
}

ssize_t readline(int fd, void *usrbuf, size_t maxlen, socketstate_t *state)
{
    int n, rc = 0;
    char c[2] = {0}, *bufp = (char *)usrbuf;

    n = 0;
    if ((maxlen - 1) <= 0)
    {
    	*state = E_ERROR;
    	return -1;
	}
	
    while (n < maxlen - 1)
    {
		if ((rc = socket_read(fd, c, 1, state)) == 1)
		{
			n ++;
			*bufp ++ = c[0];
			if (c[0] == '\n')
			{
				break;
			}
		}
		else
		{
			break;
		}
    }
    *bufp = 0;

    return n;
}

