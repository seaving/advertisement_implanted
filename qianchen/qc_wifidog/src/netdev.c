#include "includes.h"
#if 0
typedef struct S_NET_DEV
{
	char segment[20];
	char ip[32];
	char mac[20];
	char devname[15];
}sNetdevice;
//extern sNetdevice wan;
//extern sNetdevice lan;

sNetdevice wan;
sNetdevice lan;

int read_line_from_network_config(int fd, char *buf)
{
	char *tmp = buf;
	for ( ; ; )
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
	for ( ; ; )
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
	if (get_dev_ip(wan.ip, wan.devname) < 0)
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
	if (get_dev_ip(lan.ip, lan.devname) < 0)
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
	sprintf(lan.segment, "134.227.0");
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
#endif


