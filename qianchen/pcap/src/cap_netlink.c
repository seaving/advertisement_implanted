#include "includes.h"

static FILE* fp = NULL;

void nl_log(const char* fmt, ...)
{
    va_list ap; 

    va_start(ap, fmt);
    if (fp != NULL)
    {
        if (vfprintf(fp, fmt, ap) < 0)
        {
            perror("fprintf");
        }  
        fflush(fp);
    }  
    vprintf(fmt, ap);
    va_end(ap);
}

void nl_log_init()
{
	char fname[128];
	snprintf(fname, 125, "/data/local/tmp/cap_%d.log", getpid());
	//if (argc > 1 && strcmp(argv[1], "-f") == 0)
	{
		unlink(fname);
		fp = fopen(fname, "w");
		if (fp == NULL)
		{
			perror("fopen");
		}
		
		nl_log("netlink message will also write to %s. \n", fname);
	}

}

int parseBinaryNetlinkMessage(struct nlmsghdr *nh)
{	
    int len = nh->nlmsg_len - sizeof(*nh);
    struct ifinfomsg *ifi;

    if (sizeof(*ifi) > (size_t) len)
    {
        VAL_LOG("Got a short RTM_NEWLINK message\n");
        return -1;
    }
    
    ifi = (struct ifinfomsg *)NLMSG_DATA(nh);
    if ((ifi->ifi_flags & IFF_LOOPBACK) != 0)
    {
        return -1;
    }
    
    struct rtattr *rta = (struct rtattr *)
      ((char *) ifi + NLMSG_ALIGN(sizeof(*ifi)));
    len = NLMSG_PAYLOAD(nh, sizeof(*ifi));

	int ret = 0;

    while (RTA_OK(rta, len))
    {
        switch (rta->rta_type)
        {
			case IFLA_IFNAME:
			{
				char ifname[IFNAMSIZ] = {0};
				char *action;
				snprintf(ifname, sizeof(ifname), "%s",
				         (char *) RTA_DATA(rta));
				//action = (ifi->ifi_flags & IFF_LOWER_UP) ? "up" : "down";
				action = (ifi->ifi_flags & IFF_RUNNING) ? "up" : "down";
				VAL_LOG("%s link %s\n", ifname, action);
				if (strncmp(ifname, WAN_DEV, strlen(WAN_DEV)) == 0
				&& strcmp(action, "down") == 0)
				{
					kill_cap_thread(ifname);
				}
			}
        }
        
        rta = RTA_NEXT(rta, len);
    }
    
    return ret;
}

void parseNetlinkAddrMsg(struct nlmsghdr *nlh, int new)
{
    struct ifaddrmsg *ifa = (struct ifaddrmsg *) NLMSG_DATA(nlh);
    struct rtattr *rth = IFA_RTA(ifa);
    int rtl = IFA_PAYLOAD(nlh);

    while (rtl && RTA_OK(rth, rtl))
    {
        if (rth->rta_type == IFA_LOCAL)
        {
	        uint32_t ipaddr = htonl(*((uint32_t *)RTA_DATA(rth))); 
	        char name[IFNAMSIZ] = {0};
	        if_indextoname(ifa->ifa_index, name);
	        VAL_LOG("%s %s address %d.%d.%d.%d\n",
	               name, (new != 0) ? "add" : "del",
	               (ipaddr >> 24) & 0xff,
	               (ipaddr >> 16) & 0xff,
	               (ipaddr >> 8) & 0xff,
	               ipaddr & 0xff);

			if (strncmp(name, WAN_DEV, strlen(WAN_DEV)) == 0)
			{
				if (new)
				{
		        	create_cap_sub_process(name);
				}
				else
				{
					kill_cap_thread(name);
				}
			}
        }

        rth = RTA_NEXT(rth, rtl);
    }
}

int cap_netlink_start()
{
    struct sockaddr_nl addr;
    int sock, len;
    char buffer[4096];
    struct nlmsghdr *nlh;

    signal(SIGPIPE, SIG_IGN);
    if ((sock = socket(PF_NETLINK, SOCK_RAW, NETLINK_ROUTE)) == -1)
    {
        VAL_LOG("couldn't open NETLINK_ROUTE socket");
        return -1;
    }

    memset(&addr, 0, sizeof(addr));
    addr.nl_family = AF_NETLINK;
    addr.nl_groups = RTMGRP_LINK | RTMGRP_IPV4_IFADDR;

    if (bind(sock, (struct sockaddr *)&addr, sizeof(addr)) == -1)
    {
        perror("couldn't bind");
        close(sock);
        return -1;
    }

    while ((len = recv(sock, buffer, 4096, 0)) > 0)
    {
        nlh = (struct nlmsghdr *)buffer;
        while ((NLMSG_OK(nlh, len)) && (nlh->nlmsg_type != NLMSG_DONE))
        {
            if (nlh->nlmsg_type == RTM_NEWADDR)
            {
                parseNetlinkAddrMsg(nlh, 1);
            }
            else if(nlh->nlmsg_type == RTM_DELADDR)
            {
                parseNetlinkAddrMsg(nlh, 0);
            }
            else if (nlh->nlmsg_type == RTM_NEWLINK)
            {
                parseBinaryNetlinkMessage(nlh);
            }
            
            nlh = NLMSG_NEXT(nlh, len);
        }
    }
    
    close(sock);

    if (fp != NULL)
    {
        fclose(fp);
    }
    
    return 0;
}

//·â×°Íø¿¨¼à¿ØÏß³Ì
void *net_dev_monitor(void *arg)
{
	pthread_detach(pthread_self());

	cap_netlink_start();
	
	return NULL;
}

int create_net_dev_monitor_thread()
{
	pthread_t thd;
	if (pthread_create(&thd, NULL, net_dev_monitor, NULL) != 0)
	{
		perror("pthread_create error.");
		return -1;
	}

	return 0;
}

