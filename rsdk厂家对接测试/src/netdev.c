#include "includes.h"

typedef struct S_NET_DEV
{
	char segment[20];
	char ip[32];
	char mac[32];
	char devname[15];
}sNetdevice;
//extern sNetdevice wan;
//extern sNetdevice lan;

sNetdevice wan;
sNetdevice lan;

int read_line_from_network_config(int fd, char *buf)
{
	char *tmp = buf;
	while (1)
	{
		char ch;
		if (read(fd, &ch, 1) != 1)
		{
			LOG_PERROR_INFO("read error.");
			return -1;
		}
		if (ch == '\n')
			break;
		*tmp = ch;
		tmp ++;
	}
	*tmp = 0;
	return tmp - buf;
}

int get_wandev_name(char *name)
{
	int fd = open("/etc/config/network", O_RDWR);
	if (fd <= 0)
	{
		LOG_PERROR_INFO("open error.");
		return -1;
	}

	int flag = 0;
	char buf[MAXBUF];
	while (1)
	{
		int ret = read_line_from_network_config(fd, buf);
		if (ret < 0)
			break;
		if (ret == 0) //¿ÕÐÐ
			continue;
		if (flag == 0)
		{
			if (strnaddr(buf, ret, "config", strlen("config")))
			{
				if (strnaddr(buf, ret, "interface", strlen("interface")))
				{
					if (strnaddr(buf, ret, "wan", strlen("wan")))
					{
						flag = 1;
					}				
				}
			}
		}
		else
		{
			if (strnaddr(buf, ret, "config", strlen("config")))
				break;
				
			if (strnaddr(buf, ret, "option", strlen("option")))
			{
				char *p = strnaddr(buf, ret, "ifname", strlen("ifname"));
				if (p)
				{
					p += strlen("ifname");
					int i, j = 0;
					for (i = 0; i < strlen(p); i ++)
					{
						if (p[i] == ' '
						|| p[i] == '\''
						|| p[i] == '\"'
						|| p[i] == '\r'
						|| p[i] == '\n')
							continue;

						name[j ++] = p[i];
						name[j] = 0;
					}
					break;
				}
			}			
		}
	}
	close(fd);
	return 0;
}

int init_netdev()
{
	char wan_name[32] = {0};
	get_wandev_name(wan_name);
	if (strlen(wan_name) == 0)
	{
		LOG_ERROR_INFO("get wan dev name failt !\n");
		return -1;
	}

	memset(wan.ip, 0, sizeof(wan.ip));
	memset(wan.mac, 0, sizeof(wan.mac));
	memset(wan.devname, 0, sizeof(wan.devname));
	memcpy(wan.devname, wan_name, strlen(wan_name));
	if (get_ip(wan.ip, wan.devname) < 0)
	{
		LOG_ERROR_INFO("[%s] get ip error!\n", wan.devname);
		return -1;
	}
	if (get_local_mac(wan.mac, wan.devname))
	{
		LOG_ERROR_INFO("[%s] get mac error!\n", wan.devname);
		return -1;
	}
	//----------------------------------------------
	memset(lan.ip, 0, sizeof(lan.ip));
	memset(lan.mac, 0, sizeof(lan.mac));
	memset(lan.devname, 0, sizeof(lan.devname));
	memcpy(lan.devname, LAN_DEV, strlen(LAN_DEV));
	if (get_ip(lan.ip, lan.devname) < 0)
	{
		LOG_ERROR_INFO("[%s] get ip error!\n", lan.devname);
		return -1;
	}
	if (get_local_mac(lan.mac, lan.devname))
	{
		LOG_ERROR_INFO("[%s] get mac error!\n", lan.devname);
		return -1;
	}

	int i, j = 0;
	for (i = 0; i < strlen(lan.ip); i ++)
	{
		if (lan.ip[i] == '.')
		{
			j = i;
		}
	}
	memcpy(lan.segment, lan.ip, j);
	memcpy(wan.segment, lan.ip, j);
	lan.segment[j] = '\0';
	wan.segment[j] = '\0';
	
	return 0;
}

char * get_netdev_wan_segment()
{
	return wan.segment;
}

char * get_netdev_wan_ip()
{
	return wan.ip;
}

char * get_netdev_wan_mac()
{
	return wan.mac;
}

char * get_netdev_wan_name()
{
	return wan.devname;
}

char * get_netdev_lan_segment()
{
	return lan.segment;
}

char * get_netdev_lan_ip()
{
	return lan.ip;
}

char * get_netdev_lan_mac()
{
	return lan.mac;
}

char * get_netdev_lan_name()
{
	return lan.devname;
}

char * get_router_gw()
{
	return lan.ip;
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
        strcpy(s_arpreq.arp_dev, get_netdev_lan_name());
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
            LOG_HL_INFO("MAC: %s\n", mac);
        }  
    }  
    return 0;
}   

int getpeermac_by_ip(char *ipaddr, char *mac)
{  
 	int s;
 	struct arpreq s_arpreq;
 	struct sockaddr_in *addr = (struct sockaddr_in *)&s_arpreq.arp_pa;
 	unsigned char *hw;
 	int err = -1;
 	s = socket(AF_INET, SOCK_DGRAM, 0);
 	if (s < 0)
 	{
		LOG_PERROR_INFO("socket error");
		return err;
 	}
 	addr->sin_family = AF_INET;
 	addr->sin_addr.s_addr = inet_addr(ipaddr);
 	if (addr->sin_addr.s_addr == INADDR_NONE)
 	{
		LOG_ERROR_INFO("IP addr famart error!\n");
		close(s);
		return err;
 	}
 	strcpy(s_arpreq.arp_dev, get_netdev_lan_name());
 	err = ioctl(s, SIOCGARP, &s_arpreq);
 	if (err < 0)
 	{
		LOG_ERROR_INFO("ioctl ERROR!\n");
		close(s);
		return err;
 	}
 	else
 	{
		hw = (unsigned char *)&s_arpreq.arp_ha.sa_data;
		sprintf(mac, "%02x:%02x:%02x:%02x:%02x:%02x", *hw, *(hw+1), *(hw+2), *(hw+3), *(hw+4), *(hw+5));
		LOG_HL_INFO("MAC: %s ==> %s\n", ipaddr, mac);
 	}
 	close(s);
    return 0;  
}

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

        LOG_HL_INFO("Router Mac address is %s\n", mac);
        success_flag = 1;
    }
	
	close(sock_fd);
	if (!success_flag)
	{
		return -1;
	}
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
			LOG_ERROR_INFO("[ %s ] getaddrinfo timeout !\n", hostname);
			return -1;
		}
		ret = getaddrinfo(hostname, NULL, &hint, &answer);
		if (ret != 0)
		{
			LOG_ERROR_INFO("[ %s ] getaddrinfo: ret = %d, %s\n", hostname, ret, gai_strerror(ret));
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



