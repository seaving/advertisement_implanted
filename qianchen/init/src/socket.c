#include "includes.h"

typedef struct sockaddr SA;

int socket_setfd_noblock(int sockfd)
{
    int flag = fcntl(sockfd, F_GETFL, 0);
    if (flag < 0)
    {
		LOG_PERROR_INFO("fcntl F_GETFL error");
		return -1;
    }
	if (fcntl(sockfd, F_SETFL, (flag | O_NONBLOCK)) < 0)
	{
		LOG_PERROR_INFO("fcntl F_SETFL error");
		return -1;
	}
	
	return 0;
}

int socket_setfd_block(int sockfd)
{
    int flag = fcntl(sockfd, F_GETFL, 0);
    if (flag < 0)
    {
		LOG_PERROR_INFO("fcntl F_GETFL error");
		return -1;
    }
	if (fcntl(sockfd, F_SETFL, (flag & ~O_NONBLOCK)) < 0)
	{
		LOG_PERROR_INFO("fcntl F_SETFL error");
		return -1;
	}
	
	return 0;
}

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

int socket_read(int sockfd, char *buf, int len)
{
	return read(sockfd, buf, len);
}

int socket_send(int sockfd, char *buf, int len)
{
	return send(sockfd, buf, len, 0);
}

int readline(int fd, void *usrbuf, int maxlen)
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
		if ((rc = read(fd, c, 1)) == 1)
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

int read_header(int fd, char * buffer, int buffer_len, int *read_end)
{  
	int total_read = 0;
	int read_len = buffer_len;
	if (read_end)
	{
		*read_end = 0;
	}
	char *line_buffer = buffer;
    while (read_len > 0)
    {
		if (read_len > MAXLINE)
		{
			read_len = MAXLINE;
		}
        int ret = readline(fd, line_buffer, read_len);
        if (ret <= 0)
        {
            break;
        }
        total_read += ret;
        read_len = buffer_len - total_read;
		
        //读到了空行，http头结束
        if (0 < ret && ret <= 2)
        {
	        if (strcmp(line_buffer, "\r\n") == 0)
	        {
				if (read_end)
				{
					*read_end = 1;
				}
	        	break;
	        }
        }
        
		line_buffer += ret;
    }
    if (total_read <= 0)
    {
		return -1;
    }
    
    buffer[total_read] = '\0';

    return total_read;
}

int get_addr_info(char *hostname, char *ipaddr)
{  
	struct addrinfo *answer = NULL, hint/*, *curr*/;
	char *ipstr = ipaddr;
	struct sockaddr_in *addr;
	bzero(&hint, sizeof(hint));
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
			LOG_NORMAL_INFO("[ %s ] getaddrinfo timeout !\n", hostname);
			return -1;
		}
		ret = getaddrinfo(hostname, NULL, &hint, &answer);
		if (ret != 0)
		{
			LOG_NORMAL_INFO("[ %s ] getaddrinfo: ret = %d, %s\n", hostname, ret, gai_strerror(ret));
			if (ret == EAI_AGAIN)
				continue;
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
#if 0
int get_local_mac(char *mac, char *dev)
{
    int sock_fd;
    struct ifreq buf[20];
    struct ifconf ifc;
    int interface_num;
	if (!mac)
	{
		return -1;
	}
	sprintf(mac, "00:00:00:00:00:00");
	
    if ((sock_fd = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
    {
        LOG_PERROR_INFO("Create socket failed");
        return -1;
    }

    ifc.ifc_len = sizeof(buf);
    ifc.ifc_req = buf;
    if (ioctl(sock_fd, SIOCGIFCONF, (char *)&ifc) < 0)
    {
        LOG_PERROR_INFO("Get a list of interface addresses failed");
        close(sock_fd);
        return -1;
    }
    
    interface_num = ifc.ifc_len / sizeof(struct ifreq);
    LOG_NORMAL_INFO("The number of interfaces is %d\n", interface_num);

	int success_flag = 0;
    while (interface_num --)
    {
		if (strcmp(buf[interface_num].ifr_name, dev))
		{
			continue;
		}
        LOG_NORMAL_INFO("Net device: %s\n", buf[interface_num].ifr_name);
        if (ioctl(sock_fd, SIOCGIFFLAGS, (char *)&buf[interface_num]) < 0)
        {
            LOG_NORMAL_INFO("Get the active flag word of the device failed !\n");
            close(sock_fd);
            return -1;
		}
        if (buf[interface_num].ifr_flags & IFF_PROMISC)
        {
            LOG_NORMAL_INFO("Interface is in promiscuous mode\n");
		}
        if (buf[interface_num].ifr_flags & IFF_UP)
        {
            LOG_NORMAL_INFO("Interface is running\n");
        }
        else {
            LOG_NORMAL_INFO("Interface is not running\n");
		}
        if (ioctl(sock_fd, SIOCGIFHWADDR, (char *)&buf[interface_num]) < 0)
        {
            LOG_PERROR_INFO("Get the hardware address of a device failed");
            close(sock_fd);
            return -1;      
        }

        sprintf(mac, "%02x:%02x:%02x:%02x:%02x:%02x",
            (unsigned char)buf[interface_num].ifr_hwaddr.sa_data[0],
            (unsigned char)buf[interface_num].ifr_hwaddr.sa_data[1],
            (unsigned char)buf[interface_num].ifr_hwaddr.sa_data[2],
            (unsigned char)buf[interface_num].ifr_hwaddr.sa_data[3],
            (unsigned char)buf[interface_num].ifr_hwaddr.sa_data[4],
            (unsigned char)buf[interface_num].ifr_hwaddr.sa_data[5]);

        LOG_NORMAL_INFO("Router Mac address is %s\n", mac);
        success_flag = 1;
    }
	
	close(sock_fd);
	if (!success_flag)
	{
		return -1;
	}
    return 0;
}

int get_dev_ip(char *ip, char *dev)
{
    #define BUFSIZE 200
    int s;
    struct ifconf conf;
    struct ifreq *ifr;
    char buff[BUFSIZE];
    int num;
    int i;

    int get_ip_ok = 0;

    int ret = -1;

    s = socket(AF_INET, SOCK_DGRAM, 0);
	if (s < 0)
	{
		LOG_PERROR_INFO("socket create error!! err = %d", errno);
		goto _return_;
	}
	
    conf.ifc_len = BUFSIZE;
    conf.ifc_buf = buff;
    ioctl(s, SIOCGIFCONF, &conf);
    num = conf.ifc_len / sizeof(struct ifreq);
    ifr = conf.ifc_req;

    for (i = 0; i < num; i ++)
    {
        struct sockaddr_in *sin = (struct sockaddr_in *)(&ifr->ifr_addr);

        ret = ioctl(s, SIOCGIFFLAGS, ifr);
        if (((ifr->ifr_flags & IFF_LOOPBACK) == 0) && (ifr->ifr_flags & IFF_UP))
        {
        	if (0 == strcmp(ifr->ifr_name, dev))
        	{
            	// 拷贝
            	strcpy(ip, inet_ntoa(sin->sin_addr));
	  			if ((ret >= 0) && (ip[0] > 0) && (strlen(ip) > 7))
	  			{	
					get_ip_ok = 1;
				}
				else
				{			
					LOG_NORMAL_INFO("[%s] Get Ip error %d %d\r\n", dev, ip[0], ip[1]);
				}          	
			}
            LOG_NORMAL_INFO("num : %d %s (%s).\r\n", num, ifr->ifr_name, inet_ntoa(sin->sin_addr));
        }
        ifr++;
    }

_return_:
    close(s);

	if (get_ip_ok)
	{
		return 0;
	}

    return -1;
}
#endif

int get_local_mac(char *mac, char *dev)
{
	struct ifreq ifreq;
	int sock = 0;	
	
	sock = socket(AF_INET, SOCK_STREAM, 0);
	if (sock < 0)
	{
        LOG_PERROR_INFO("Create socket failed");
        return -1;
	}

	strcpy(ifreq.ifr_name, dev);
	if (ioctl(sock, SIOCGIFHWADDR, &ifreq) < 0)
	{
        LOG_PERROR_INFO("Get a list of interface addresses failed");
        close(sock);
        return -1;
	}

	sprintf(mac, "%02x:%02x:%02x:%02x:%02x:%02x",
		(unsigned char)ifreq.ifr_hwaddr.sa_data[0],
		(unsigned char)ifreq.ifr_hwaddr.sa_data[1],
		(unsigned char)ifreq.ifr_hwaddr.sa_data[2],
		(unsigned char)ifreq.ifr_hwaddr.sa_data[3],
		(unsigned char)ifreq.ifr_hwaddr.sa_data[4],
		(unsigned char)ifreq.ifr_hwaddr.sa_data[5]);
	
	LOG_NORMAL_INFO("Router Mac address is %s\n", mac);

	close(sock);
	return 0;
}

int get_dev_ip(char *ip, char *dev)
{
    int s;
    struct ifreq ifr;

    s = socket(AF_INET, SOCK_DGRAM, 0);
	if (s < 0)
	{
		LOG_PERROR_INFO("socket create error!! err = %d", errno);
		return -1;
	}
	
	strcpy(ifr.ifr_name, dev);
	
    struct sockaddr_in *sin = (struct sockaddr_in *)(&ifr.ifr_addr);

    if (ioctl(s, SIOCGIFADDR, &ifr) >= 0)
    {
		// 拷贝
		strcpy(ip, inet_ntoa(sin->sin_addr));
        LOG_NORMAL_INFO("dev : %s (%s).\r\n", ifr.ifr_name, ip);
        
		close(s);
		return 0;
    }

	close(s);
	return -1;
}



