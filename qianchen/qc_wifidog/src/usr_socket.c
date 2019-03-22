#include "includes.h"

//#define DEBUG_SOCKET

int connect_noblock(int clientfd, struct sockaddr_in *serveraddr)
{
	if (connect(clientfd, (SA *)serveraddr, sizeof(struct sockaddr_in)) >= 0) {
		return 0;
	}
#ifdef DEBUG_SOCKET	
	LOG_NORMAL_INFO("(s:%d)connect need to check. e:%d, %d, %d %s\r\n", 
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
    
	for ( ; ; )
	{
		i ++;
		if (i > 10)
		{
#ifdef DEBUG_SOCKET
			LOG_ERROR_INFO("(s:%d) connect time out.\r\n", clientfd);
#endif
			return -1;
		}
#ifdef DEBUG_SOCKET
		LOG_NORMAL_INFO("(s:%d) connect ...........................%d\r\n", clientfd, i);
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
			LOG_NORMAL_INFO("socket : %d read & write, need to check agin.\r\n", clientfd);
#endif
			len = sizeof(iError);
			ret = getsockopt(clientfd, SOL_SOCKET, SO_ERROR, &iError, &len);
			if (ret < 0)
			{
#ifdef DEBUG_SOCKET	
				LOG_ERROR_INFO(first...)("Get SO_ERROR error, connect failt.\r\n");
#endif
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

				continue;
			}

			if (0 == iError)
			{
#ifdef DEBUG_SOCKET			
				LOG_NORMAL_INFO("ERROR = 0; connect success.\r\n");
#endif
				return 0;
			}
		}

		if ((FD_ISSET(clientfd, &socketRed) <= 0)
		 && (FD_ISSET(clientfd, &socketWr) > 0))
		{
#ifdef DEBUG_SOCKET
			LOG_NORMAL_INFO("socket:%d write & noread, connect success.\r\n", clientfd);
#endif
			return 0;
		}

		if ((FD_ISSET(clientfd, &socketRed) > 0)
		 && (FD_ISSET(clientfd, &socketWr) <= 0))
		{
#ifdef DEBUG_SOCKET
			LOG_ERROR_INFO("socket:%d read & nowrite, connect failt.\r\n", clientfd);
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
	bzero((char *) &serveraddr, sizeof(serveraddr));
	serveraddr.sin_family = AF_INET;
	if (hostname)
		serveraddr.sin_addr.s_addr = inet_addr(ip);
	else
		serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);
		
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

	bzero((char *) &serveraddr, sizeof(serveraddr));
	serveraddr.sin_family = AF_INET;
	if (hostname)
		serveraddr.sin_addr.s_addr = inet_addr(ipaddr);
	else
		serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);
		
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
			LOG_ERROR_INFO("%s:%d DNS ERROR\n", hostname, port);
		    LOG_PERROR_INFO("Open_clientfd DNS error");
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
		    LOG_PERROR_INFO("Open_clientfd Unix error");
		}
		else if (rc == -2)
		{
			LOG_ERROR_INFO("%s:%d DNS ERROR!!!\n", hostname, port);
		    LOG_PERROR_INFO("Open_clientfd DNS error");
		}
		else if (rc == -3)
		{
			LOG_ERROR_INFO("%s:%d connect error!!!\n", hostname, port);		
		    LOG_PERROR_INFO("Open_clientfd Unix error");
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

ssize_t readline(int fd, void *usrbuf, size_t maxlen)
{
    int n, rc = 0;
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

