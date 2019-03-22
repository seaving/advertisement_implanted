#include "includes.h"

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
        strcpy(s_arpreq.arp_dev, GET_LAN_NAME);
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
            	// ¿½±´
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
            LOG_NORMAL_INFO("num : %d %s (%s). find dev: %s\r\n", num, ifr->ifr_name, inet_ntoa(sin->sin_addr), dev);
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

int get_ip(char *ip, char *dev)
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
            	// ¿½±´
            	strcpy(ip, inet_ntoa(sin->sin_addr));
	  			if ((ret >= 0) && (ip[0] > 0) && (strlen(ip) > 7))
	  			{	
					get_ip_ok = 1;
				}
				else
				{			
					LOG_ERROR_INFO("[%s] Get Ip error %d %d\r\n", dev, ip[0], ip[1]);
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

