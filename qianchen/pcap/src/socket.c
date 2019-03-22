#include "includes.h"

int connect_noblock(int clientfd, struct sockaddr_in *serveraddr)
{
	if (connect(clientfd, (SA *)serveraddr, sizeof(struct sockaddr_in)) >= 0) {
		return 0;
	}
#ifdef DEBUG_SOCKET	
	printf("(s:%d)connect need to check. e:%d, %d, %d %s\r\n", 
					clientfd, errno, ENETUNREACH, EINPROGRESS, strerror(errno));
#endif

	if ((errno != EINPROGRESS) || (errno == ENETUNREACH))
	{
		return -1;
	}

	fd_set socketRed;
	fd_set socketWr;
	struct timeval sTimeout;
	int i = 0, ret = -1;
    socklen_t len;
    int iError = -1;
    
	while (1)
	{
		i ++;
		if (i > 10)
		{
#ifdef DEBUG_SOCKET
			printf("(s:%d) connect time out.\r\n", clientfd);
#endif
			return -1;
		}
#ifdef DEBUG_SOCKET
		printf("(s:%d) connect ...........................%d\r\n", clientfd, i);
#endif

		FD_ZERO(&socketRed);
		FD_ZERO(&socketWr);
		FD_SET (clientfd, &socketRed);
		FD_SET (clientfd, &socketWr);

		sTimeout.tv_sec  = 1;
		sTimeout.tv_usec = 0;

		ret = select(clientfd + 1, (fd_set*)(&socketRed), (fd_set*)(&socketWr), (fd_set*)NULL, &sTimeout);
		if (ret <= 0)
		{
			if (ret == 0)
			{
				continue;
			}
			if (ret < 0)
			{
				return -1;
			}
		}
		if ((FD_ISSET(clientfd, &socketRed) > 0)
		 && (FD_ISSET(clientfd, &socketWr) > 0))
		{
#ifdef DEBUG_SOCKET		
			printf("socket : %d read & write, need to check agin.\r\n", clientfd);
#endif
			len = sizeof(iError);
			ret = getsockopt(clientfd, SOL_SOCKET, SO_ERROR, &iError, &len);
			if (ret < 0)
			{
#ifdef DEBUG_SOCKET	
				printf(first...)("Get SO_ERROR error, connect failt.\r\n");
#endif
				return -1;
			}
#ifdef DEBUG_SOCKET
			printf("Get ERROR parma:%d, %d\r\n", iError, ret);
#endif

			if (iError != 0)
			{
#ifdef DEBUG_SOCKET
				printf("ERROR != 0; connect failt.\r\n");
#endif

				continue;
			}

			if (0 == iError)
			{
#ifdef DEBUG_SOCKET			
				printf("ERROR = 0; connect success.\r\n");
#endif
				return 0;
			}
		}

		if ((FD_ISSET(clientfd, &socketRed) <= 0)
		 && (FD_ISSET(clientfd, &socketWr) > 0))
		{
#ifdef DEBUG_SOCKET
			printf("socket:%d write & noread, connect success.\r\n", clientfd);
#endif
			return 0;
		}

		if ((FD_ISSET(clientfd, &socketRed) > 0)
		 && (FD_ISSET(clientfd, &socketWr) <= 0))
		{
#ifdef DEBUG_SOCKET
			printf("socket:%d read & nowrite, connect failt.\r\n", clientfd);
#endif
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
	char ip[32] = {0};
	
    if ((clientfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
		return -1;
	}
	struct linger linger;
	linger.l_onoff = 0;
	linger.l_linger = 0;
	int optlen = sizeof(linger);
	setsockopt(clientfd, SOL_SOCKET, SO_LINGER, (char *)&linger, optlen);

	if (hostname)
	{
		if (get_addr_info(hostname, ip) < 0)
		{
			close(clientfd);
			return -2;
		}
	}
	memset((char *) &serveraddr, 0, sizeof(serveraddr));
	serveraddr.sin_family = AF_INET;
	if (hostname)
		serveraddr.sin_addr.s_addr = inet_addr(ip);
	else
		serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);
		
	serveraddr.sin_port = htons(port);
	if (bind_ip)
	{
		memset((char *) &local_addr, 0, sizeof(local_addr));
		local_addr.sin_family = AF_INET;
		local_addr.sin_addr.s_addr = inet_addr(bind_ip);
		local_addr.sin_port = 0;
		if (bind(clientfd, (struct sockaddr*)&local_addr, sizeof(local_addr)) != 0)
		{
			perror("bind error");
			close(clientfd);
			return -1;
		}
	}
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
	if (hostname)
	{
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
	}
	struct linger linger;
	linger.l_onoff = 0;
	int optlen = sizeof(linger);
	setsockopt(clientfd, SOL_SOCKET, SO_LINGER, (char *)&linger, optlen);

	socket_setfd_noblock(clientfd);

	memset((char *) &serveraddr, 0, sizeof(serveraddr));
	serveraddr.sin_family = AF_INET;
	if (hostname)
		serveraddr.sin_addr.s_addr = inet_addr(ipaddr);
	else
		serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);
		
	serveraddr.sin_port = htons(port);
	
	if (bindip)
	{
		memset((char *) &local_addr, 0, sizeof(local_addr));
		local_addr.sin_family = AF_INET;
		local_addr.sin_addr.s_addr = inet_addr(bindip);
		local_addr.sin_port = 0;
		if (bind(clientfd, (struct sockaddr*)&local_addr, sizeof(local_addr)) != 0)
		{
			perror("bind error");
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
		    VAL_PERROR("Open_clientfd Unix error");
		}
		else
		{
			VAL_LOG("%s:%d DNS ERROR\n", hostname, port);
		    VAL_PERROR("Open_clientfd DNS error");
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
			VAL_LOG("%s:%d socket create error!!!\n", hostname, port);
		    VAL_PERROR("Open_clientfd Unix error");
		}
		else if (rc == -2)
		{
			VAL_LOG("%s:%d DNS ERROR!!!\n", hostname, port);
		    VAL_PERROR("Open_clientfd DNS error");
		}
		else if (rc == -3)
		{
			VAL_LOG("%s:%d connect error!!!\n", hostname, port);		
		    VAL_PERROR("Open_clientfd Unix error");
		}
		else if (rc == -5)
		{
			VAL_LOG("filter host : %s.\n", hostname);
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

ssize_t readline(int fd, void *usrbuf, size_t maxlen)
{
    size_t n;
    int rc = 0;
    char c[2] = {0}, *bufp = (char *)usrbuf;

    n = 0;
    if ((maxlen - 1) <= 0)
    {
    	return -1;
	}
	
    while (n < maxlen - 1)
    {
		if ((rc = socket_read(fd, c, 1, 5)) == 1)
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

int socket_setfd_noblock(int sockfd)
{
    int flag = fcntl(sockfd, F_GETFL, 0);
    if (flag < 0)
    {
		perror("fcntl F_GETFL error");
		return -1;
    }
	if (fcntl(sockfd, F_SETFL, (flag | O_NONBLOCK)) < 0)
	{
		perror("fcntl F_SETFL error");
		return -1;
	}
	
	return 0;
}

int socket_setfd_block(int sockfd)
{
    int flag = fcntl(sockfd, F_GETFL, 0);
    if (flag < 0)
    {
		perror("fcntl F_GETFL error");
		return -1;
    }
	if (fcntl(sockfd, F_SETFL, (flag & ~O_NONBLOCK)) < 0)
	{
		perror("fcntl F_SETFL error");
		return -1;
	}
	
	return 0;
}

#if 0
int socket_send(int sock, char *buf, int len, unsigned char time_out)
{
	ssize_t counts = 0;
	size_t total = len;
	size_t sended_len = 0;
	const char *p = buf;

	unsigned int last_time = SYSTEM_SEC;
	
	while (1)
	{
		if (time_out > 0 && SYSTEM_SEC - last_time > time_out)
		{
			return -1;
		}
		counts = send(sock, p, total, MSG_NOSIGNAL);
		if (counts < 0)
		{
			if (errno == EAGAIN || errno == EWOULDBLOCK)
			{
				// EAGAIN 表示缓冲区为满，不能写入,稍加延时,在写
				usleep(1000);
				continue;
			}
			else if (errno == EINTR)
			{
				// 被信号中断,这里不进行写，直接退出
				return -1;
			}
			
			perror("send error.");
			return -1;
		}
		else if (counts == 0)
		{
			// 被客户端关闭连接
			perror("send error.");
			return -1;
		}
		else
		{
			//重新赋值开始计算时间
			last_time = SYSTEM_SEC;
			
			total -= counts;
			sended_len += counts;
			p += counts;
			if (total <= 0)
			{
				return sended_len;
			}
		}
	}
	
	return 0;
}
#else
int socket_send(int sock, char *buf, int len, unsigned char time_out)
{
	ssize_t counts = 0;
	size_t total = len;
	size_t sended_len = 0;
	const char *p = buf;

	socket_setfd_block(sock);
	
	while (1)
	{
		counts = send(sock, p, total, MSG_NOSIGNAL);
		if (counts < 0)
		{	
			perror("send error.");
			socket_setfd_noblock(sock);
			return -1;
		}
		else if (counts == 0)
		{
			// 被客户端关闭连接
			perror("send error.");
			socket_setfd_noblock(sock);
			return -1;
		}
		else
		{	
			total -= counts;
			sended_len += counts;
			p += counts;
			if (total <= 0)
			{
				socket_setfd_noblock(sock);
				return sended_len;
			}
		}
	}
	
	socket_setfd_noblock(sock);

	return 0;
}
#endif

#if 0
int socket_read(int sock, char *buf, int readlen, unsigned char time_out)
{
    int count = 0;
    unsigned int last_time = SYSTEM_SEC;
    while (1)
    {
		if (time_out > 0 && SYSTEM_SEC - last_time > time_out)
		{
			VAL_LOG("read time out!\n");
			return -1;
		}
		
		count = read(sock, buf, readlen);
		if (count < 0)
		{
			if (errno == EAGAIN || errno == EWOULDBLOCK)
			{
				//读空了
				usleep(1000);
				continue;
			}
			else if (errno == EINTR)
			{
				// 被信号中断
				usleep(100);
				continue;
			}
			else
			{
				VAL_PERROR("read error.");
				return -1;
			}
		}
		else if (count == 0)
		{
			// 被客户端关闭连接
			VAL_PERROR("read error.");
			return -1;
		}
		else
		{
			//VAL_LOG("count = %d\n", count);
			//重新赋值开始计算时间
			last_time = SYSTEM_SEC;			
			return count;
		}
    }

    return -1;
}
#else
int socket_read(int sock, char *buf, int readlen, unsigned char time_out)
{
    int count = 0;
    socket_setfd_block(sock);
	count = read(sock, buf, readlen);
	socket_setfd_noblock(sock);
    return count;
}
#endif

int socket_close(int sockfd)
{
	int sock = sockfd;
	if (sock > 0)
	{
		VAL_LOG("close socket fd: %d .\n", sockfd);
		shutdown(sock, SHUT_RDWR);
		close(sock);
	}
	
	return 0;	
}

int get_addr_info(char *hostname, char *ipaddr)
{  
	struct addrinfo *answer = NULL, hint/*, *curr*/;
	char *ipstr = ipaddr;
	struct sockaddr_in *addr;
	memset(&hint, 0, sizeof(hint));
	hint.ai_family = AF_INET;//AF_UNSPEC;
	hint.ai_socktype = SOCK_STREAM;

	int ret = -1;

	long last_time = 0L;
	long cur_time = GetTimestrip_us() / 1000000;
	last_time = cur_time;
	
	while (1)
	{
		cur_time = GetTimestrip_us() / 1000000;
		if (cur_time - last_time > 10)
		{
			VAL_LOG("[ %s ] getaddrinfo timeout !\n", hostname);
			if (answer)
				freeaddrinfo(answer);
			return -1;
		}
		ret = getaddrinfo(hostname, NULL, &hint, &answer);
		if (ret != 0)
		{
			VAL_LOG("[ %s ] getaddrinfo: ret = %d, %s\n", hostname, ret, gai_strerror(ret));
			if (ret == EAI_AGAIN)
				continue;
			if (answer)
				freeaddrinfo(answer);
			return -1;
		}
		break;
	}

	//for (curr = answer; curr != NULL; curr = curr->ai_next)
	{
		if (! answer)
			return -1;
		
		addr = (struct sockaddr_in *)answer->ai_addr;
		inet_ntop(AF_INET, &addr->sin_addr, ipstr, 16);
		//break;
	}
	
	freeaddrinfo(answer);

	return 0;
}

int get_dev_ip(char *ip, char *dev)
{
    int s;
    struct ifreq ifr;

    s = socket(AF_INET, SOCK_STREAM, 0);
	if (s < 0)
	{
		LOG_PERROR_INFO("socket create error!! err = %d", errno);
	}
	
	strcpy(ifr.ifr_name, dev);
	
    struct sockaddr_in *sin = (struct sockaddr_in *)(&ifr->ifr_addr);

    ioctl(s, SIOCGIFFLAGS, ifr);
    if (((ifr->ifr_flags & IFF_LOOPBACK) == 0) && (ifr->ifr_flags & IFF_UP))
    {
		// 拷贝
		strcpy(ip, inet_ntoa(sin->sin_addr));
        LOG_NORMAL_INFO("dev : %s (%s).\r\n", ifr->ifr_name, inet_ntoa(sin->sin_addr));
        
		close(s);
		return 0;
    }
    
	close(s);
	return -1;
}

