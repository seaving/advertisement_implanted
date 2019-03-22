#include "includes.h"


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

int socket_listen(char *bindip, int listen_port, int listen_counts)
{
    struct sockaddr_in server_addr;
    int listenfd;
    int ret, opt = 1;
    
    //初始化服务器地址
    bzero(&server_addr, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(listen_port);
    if (bindip == NULL || strcmp(bindip, localhost) == 0)
    {
    	server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    }
    else
    {
		server_addr.sin_addr.s_addr = inet_addr(bindip);
    }
    
    //创建监听套接字
    listenfd = socket(AF_INET, SOCK_STREAM, 0);
    if (listenfd < 0)
    {
        LOG_PERROR_INFO("Create socket error");
        return -1;
    }
    //设置地址可重用
    ret = setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
    if (ret < 0)
    {
        LOG_PERROR_INFO("setsockopt error");
        socket_close(listenfd);
        return -1;
    }
    //绑定地址
    int len = sizeof(struct sockaddr);
    ret = bind(listenfd, (struct sockaddr*)&server_addr, len);
    if (ret < 0)
    {
        LOG_PERROR_INFO("bind error");
        socket_close(listenfd);
        return -1;
    }
    //开始监听,设置最大连接请求
    ret = listen(listenfd, listen_counts);
    if (ret < 0)
    {
        LOG_PERROR_INFO("listen error");
        socket_close(listenfd);
        return -1;
    }
    
    return listenfd;
}

int socket_setarg(int socket)
{
    //TCP_CORK：采用Nagle算法把较小的包组装为更大的帧
    int on = 1;
    setsockopt(socket, SOL_TCP, TCP_CORK, &on, sizeof(on));
    
    //设置非阻塞
    return socket_setfd_noblock(socket);
}

int socket_close_client(int sockfd)
{
	int sock = sockfd;
	if (sock > 0)
	{
		shutdown(sock, SHUT_RDWR);
		close(sock);
	}
	return 0;
}

int socket_close(int sockfd)
{
	int sock = sockfd;
	if (sock > 0)
	{
		LOG_NORMAL_INFO("close socket fd: %d .\n", sockfd);
		shutdown(sock, SHUT_RDWR);
		close(sock);
	}
	
	return 0;	
}

#if 0
int socket_read(int sock, char *buf, int readlen)
{
	return read(sock, buf, readlen);
}
#else
int socket_read(int sock, char *buf, int readlen, unsigned char time_out)
{
    int count = 0;
    unsigned int last_time = SYSTEM_SEC;
    while (1)
    {
		if (time_out > 0 && SYSTEM_SEC - last_time > time_out)
		{
			return -1;
		}
		
		count = read(sock, buf, readlen);
		if (count < 0)
		{
			if (errno == EAGAIN || errno == EWOULDBLOCK)
			{
				//读空了
				//LOG_NORMAL_INFO("socket_read: read E_EAGIN.\n");
				usleep(1000);
				continue;
			}
			else if (errno == EINTR)
			{
				// 被信号中断
				//LOG_NORMAL_INFO("socket_read: read E_EINTR.\n");
				continue;
			}
			else
			{
				LOG_PERROR_INFO("fd: %d read error", sock);
				return -1;
			}
		}
		else if (count == 0)
		{
			// 被客户端关闭连接
			LOG_PERROR_INFO("fd: %d foreiner closed the connection. ", sock);
			return -1;
		}
		else
		{
			//printf("count = %d\n", count);
			//重新赋值开始计算时间
			last_time = SYSTEM_SEC;			
			return count;
		}
    }

    return -1;
}

#endif

#if 0
int socket_send(int sock, char *buf, int len)
{
	return send(sock, buf, len, MSG_NOSIGNAL);
}
#else
int socket_send(int sock, char *buf, int len, unsigned char time_out)
{
	ssize_t counts = 0;
	size_t total = len;
	size_t sended_len = 0;
	const char *p = buf;

	unsigned int last_time = SYSTEM_SEC;
	
	while(1)
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
			
			LOG_PERROR_INFO("[fd = %d] send error.", sock);
			return -1;
		}
		else if (counts == 0)
		{
			// 被客户端关闭连接
			LOG_PERROR_INFO("fd: %d send error, counts == 0. ", sock);
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

#endif
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
int get_local_mac(char *mac,  char *dev)
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
#endif
int get_local_mac(char *mac, char *dev)
{
	struct ifreq ifr;
	int sock = 0;	
	int i = 0;
	
	sock = socket(AF_INET, SOCK_STREAM, 0);
	if (sock < 0)
	{
        LOG_PERROR_INFO("Create socket failed");
        return -1;
	}

	strcpy(ifr.ifr_name, dev);
	if (ioctl(sock, SIOCGIFHWADDR, &ifreq) < 0)
	{
        LOG_PERROR_INFO("Get a list of interface addresses failed");
        close(sock);
        return -1;
	}

	sprintf(mac, "%02x:%02x:%02x:%02x:%02x:%02x",
		(unsigned char)ifr.ifr_hwaddr.sa_data[0],
		(unsigned char)ifr.ifr_hwaddr.sa_data[1],
		(unsigned char)ifr.ifr_hwaddr.sa_data[2],
		(unsigned char)ifr.ifr_hwaddr.sa_data[3],
		(unsigned char)ifr.ifr_hwaddr.sa_data[4],
		(unsigned char)ifr.ifr_hwaddr.sa_data[5]);
	
	LOG_NORMAL_INFO("Router Mac address is %s\n", mac);

	close(sock);
	return 0;
}

int get_peer_mac(int sockfd, char *mac)
{  
    struct arpreq s_arpreq;
    struct sockaddr_in dstadd_in;
    socklen_t len = sizeof(struct sockaddr_in);
    memset(&s_arpreq, 0, sizeof(struct arpreq));
    memset(&dstadd_in, 0, sizeof( struct sockaddr_in));
    if (getpeername(sockfd, (struct sockaddr*)&dstadd_in, &len) < 0)
    {
        LOG_PERROR_INFO("getpeername error");
        return -1;
    }
    else 
    {  
        memcpy(&s_arpreq.arp_pa, &dstadd_in, sizeof(struct sockaddr_in));  
        strcpy(s_arpreq.arp_dev, LAN_DEV);
        s_arpreq.arp_pa.sa_family = AF_INET;
        s_arpreq.arp_ha.sa_family = AF_UNSPEC;
        if (ioctl(sockfd, SIOCGARP, &s_arpreq) < 0)
        {
            LOG_PERROR_INFO("ioctl SIOCGARP error");
            return -1;
        }
        else
        {  
            unsigned char* hw = (unsigned char *)s_arpreq.arp_ha.sa_data;
            sprintf(mac, "%02x:%02x:%02x:%02x:%02x:%02x", *hw, *(hw+1), *(hw+2), *(hw+3), *(hw+4), *(hw+5));
            LOG_NORMAL_INFO("MAC: %s\n", mac);
        }  
    }  
    return 0;
}
#if 0
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

int get_dev_bcast(char *brdaddr, char *dev)
{
#define BUFSIZE 200
	int s;
	struct ifconf conf;
	struct ifreq *ifr;
	char buff[BUFSIZE];
	int num;
	int i;

	int get_ip_ok = 0;

	s = socket(AF_INET, SOCK_DGRAM, 0);
	if (s < 0)
	{
		LOG_PERROR_INFO("socket create error!! err = %d", errno);
		goto _return_;
	}
	
	conf.ifc_len = BUFSIZE;
	conf.ifc_buf = buff;
	if (ioctl(s, SIOCGIFCONF, &conf) < 0)
	{
		LOG_PERROR_INFO("ioctl error.");
		goto _return_;
	}
	
	num = conf.ifc_len / sizeof(struct ifreq);
	ifr = conf.ifc_req;

	for (i = 0; i < num; i ++)
	{
		LOG_WARN_INFO("dst dev: %s, dev: %s\n", dev, ifr->ifr_name);
		if (strcmp(ifr->ifr_name, dev) == 0)
		{
			if (! ioctl(s, SIOCGIFBRDADDR, ifr))
			{
				get_ip_ok = 1;
				sprintf(brdaddr, "%s",
					(char *)inet_ntoa(((struct sockaddr_in *)&(ifr->ifr_netmask))->sin_addr));
				LOG_NORMAL_INFO("device brd addr: %s\n", brdaddr);
			}
			else
			{
				LOG_PERROR_INFO("ioctl error.");
			}

			break;
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

int get_dev_mask(char *mask, char *dev)
{
	#define BUFSIZE 200
    int s;
    struct ifconf conf;
    struct ifreq *ifr;
    char buff[BUFSIZE];
    int num;
    int i;

    int get_ip_ok = 0;

    s = socket(AF_INET, SOCK_DGRAM, 0);
	if (s < 0)
	{
		LOG_PERROR_INFO("socket create error!! err = %d", errno);
		goto _return_;
	}
	
    conf.ifc_len = BUFSIZE;
    conf.ifc_buf = buff;
    if (ioctl(s, SIOCGIFCONF, &conf) < 0)
    {
		LOG_PERROR_INFO("ioctl error.");
		goto _return_;
    }
    
    num = conf.ifc_len / sizeof(struct ifreq);
    ifr = conf.ifc_req;

    for (i = 0; i < num; i ++)
    {
    	LOG_WARN_INFO("dst dev: %s, dev: %s\n", dev, ifr->ifr_name);
		if (strcmp(ifr->ifr_name, dev) == 0)
		{
			if (! ioctl(s, SIOCGIFNETMASK, ifr))
			{
				get_ip_ok = 1;
				sprintf(mask, "%s",
					(char *)inet_ntoa(((struct sockaddr_in *)&(ifr->ifr_netmask))->sin_addr));
				LOG_NORMAL_INFO("device subnetMask: %s\n", mask);
			}
			else
			{
				LOG_PERROR_INFO("ioctl error.");
			}

			break;
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

#if 0
int getLocalInfo(char *dev, char *IP, char *Mask, char *Mac)
{
   int fd;
   int interfaceNum = 0;
   struct ifreq buf[16];
   struct ifconf ifc;
   struct ifreq ifrcopy;
   char mac[16] = {0};
   char ip[32] = {0};
   char broadAddr[32] = {0};
   char subnetMask[32] = {0};
   if ((fd = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
   {
      LOG_PERROR_INFO("socket");
      close(fd);
      return -1;
   }
   
   ifc.ifc_len = sizeof(buf);
   ifc.ifc_buf = (caddr_t)buf;
   if (!ioctl(fd, SIOCGIFCONF, (char *)&ifc))
   {
		interfaceNum = ifc.ifc_len / sizeof(struct ifreq);
		LOG_NORMAL_INFO("interface num = %d\n", interfaceNum);
		while (interfaceNum -- > 0)
		{
			if (strcmp(buf[interfaceNum].ifr_name, dev))
				continue;
			
			LOG_NORMAL_INFO("\ndevice name: %s\n", buf[interfaceNum].ifr_name);
			//ignore the interface that not up or not runing
			ifrcopy = buf[interfaceNum];
			if (ioctl(fd, SIOCGIFFLAGS, &ifrcopy))
			{
				close(fd);
				return -1;
			}
			//get the mac of this interface
			if (!ioctl(fd, SIOCGIFHWADDR, (char *)(&buf[interfaceNum])))
			{
				memset(mac, 0, sizeof(mac));
				snprintf(mac, sizeof(mac), "%02x%02x%02x%02x%02x%02x",
				(unsigned char)buf[interfaceNum].ifr_hwaddr.sa_data[0],
				(unsigned char)buf[interfaceNum].ifr_hwaddr.sa_data[1],
				(unsigned char)buf[interfaceNum].ifr_hwaddr.sa_data[2],
				(unsigned char)buf[interfaceNum].ifr_hwaddr.sa_data[3],
				(unsigned char)buf[interfaceNum].ifr_hwaddr.sa_data[4],
				(unsigned char)buf[interfaceNum].ifr_hwaddr.sa_data[5]);
				LOG_NORMAL_INFO("device mac: %s\n", mac);
			}
			else
			{
				close(fd);
				return -1;
			}
			
			//get the IP of this interface
			if (!ioctl(fd, SIOCGIFADDR, (char *)&buf[interfaceNum]))
			{
				snprintf(ip, sizeof(ip), "%s",
					(char *)inet_ntoa(((struct sockaddr_in *)&(buf[interfaceNum].ifr_addr))->sin_addr));
				LOG_NORMAL_INFO("device ip: %s\n", ip);
			}
			else
			{
				close(fd);
				return -1;
			}
			
			//get the broad address of this interface
			if (!ioctl(fd, SIOCGIFBRDADDR, &buf[interfaceNum]))
			{
				snprintf(broadAddr, sizeof(broadAddr), "%s",
					(char *)inet_ntoa(((struct sockaddr_in *)&(buf[interfaceNum].ifr_broadaddr))->sin_addr));
				LOG_NORMAL_INFO("device broadAddr: %s\n", broadAddr);
			}
			else
			{
				close(fd);
				return -1;
			}
			
			//get the subnet mask of this interface
			if (!ioctl(fd, SIOCGIFNETMASK, &buf[interfaceNum]))
			{
				snprintf(subnetMask, sizeof(subnetMask), "%s",
					(char *)inet_ntoa(((struct sockaddr_in *)&(buf[interfaceNum].ifr_netmask))->sin_addr));
				LOG_NORMAL_INFO("device subnetMask: %s\n", subnetMask);
			}
			else
			{
				close(fd);
				return -1;
			}
		}
    }
    else
    {
        close(fd);
        return -1;
    }
    
	strcpy(IP, ip);
	//strcpy(broad,broadAddr);
	strcpy(Mask, subnetMask);
	close(fd);
	return 0;
}
#endif 

int get_peer_name(int own_socket, char *ip, int *port)
{
	struct sockaddr_in serv; 
    socklen_t serv_len = sizeof(serv);
    char serv_ip[20] = {0};
    int serv_port = 0;

	memset(serv_ip, 0, 20);
	getpeername(own_socket, (struct sockaddr *)&serv, &serv_len);
	inet_ntop(AF_INET, &serv.sin_addr, serv_ip, sizeof(serv_ip));
	serv_port = ntohs(serv.sin_port);

	if (ip)
	{
		memcpy(ip, serv_ip, strlen(serv_ip));
	}
	if (port)
	{
		*port = serv_port;
	}
	return 0;
}

int get_peer_name_n(int own_socket, unsigned int *ip, int *port)
{
	struct sockaddr_in serv; 
    socklen_t serv_len = sizeof(serv);

	getpeername(own_socket, (struct sockaddr *)&serv, &serv_len);
	*ip = serv.sin_addr.s_addr;
	if (port)
	{
		*port = ntohs(serv.sin_port);
	}
	
	return 0;
}


int get_socket_name(int own_socket, char *ip, int *port)
{
	struct sockaddr_in serv;
    socklen_t serv_len = sizeof(serv);
    char serv_ip[20] = {0};
    int serv_port = 0;

	memset(serv_ip, 0, 20);
	getsockname(own_socket, (struct sockaddr *)&serv, &serv_len);
	inet_ntop(AF_INET, &serv.sin_addr, serv_ip, sizeof(serv_ip));
	serv_port = ntohs(serv.sin_port);

	memcpy(ip, serv_ip, strlen(serv_ip));
	*port = serv_port;

	return 0;
}   


