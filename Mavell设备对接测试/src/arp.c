
#include "includes.h"


#define ARP						"busybox arp -i " LAN_DEV

#define GET_STA_HOST_NAME		ARP " -a %s | awk -F \" \" \'{print $1}\'"
#define GET_STA_HOST_MAC		ARP " -a %s | awk -F \" \" \'{print $4}\'"

int arp_extract(char *cmd, char *buf, int size)
{
	FILE *fp = NULL;

	fp = popen(cmd, "r");
	if (NULL == fp)	
	{  
		LOG_PERROR_INFO("popen error!");  
		return -1; 
	}

	while(fgets(buf, size, fp) != NULL)
	{
		buf[strlen(buf) - 1] = '\0';
		LOG_NORMAL_INFO("%s\n", buf);
		break;
	}
	pclose(fp);
	return 0;
}

int get_sta_host_name(char *ip, char *name, int size)
{
	char cmd[100];
	sprintf(cmd, GET_STA_HOST_NAME, ip);
	return arp_extract(cmd, name, size);
}

int get_sta_host_mac(char *ip, char *mac, int size)
{
	char cmd[100];
	sprintf(cmd, GET_STA_HOST_MAC, ip);
	return arp_extract(cmd, mac, size);
}

