#include "includes.h"

void parse_tcp(const u_char *packet, char *cap_dev, bool eth_type, char *s_mac, char *d_mac)
{
	const struct sniff_tcp *tcp;
	const struct sniff_ip *ip;
	
    int size_ip = 0;
    int size_tcp = 0;
    int size_payload = 0;

	int size_ethernet = 0;
	if (eth_type)
		size_ethernet = SIZE_ETHERNET;

	ip = (struct sniff_ip *)(packet + size_ethernet);
    size_ip = IP_HL(ip) * 4;
    if (size_ip < 20)
	{
        VAL_LOG(" * Invalid IP header length: %u bytes\n", size_ip);
        return ;
    }

    tcp = (struct sniff_tcp *)(packet + size_ethernet + size_ip);
    size_tcp = TH_OFF(tcp) * 4;
    if (size_tcp < 20)
	{
        VAL_LOG(" * Invalid TCP header length: %u bytes\n", size_tcp);
        return;
    }
    
    //VAL_LOG(" Src port: %d\n", ntohs(tcp->th_sport));
    //VAL_LOG(" Dst port: %d\n", ntohs(tcp->th_dport));
    
    //size_payload = ntohs(ip->ip_len) - (size_ip + size_tcp);
    size_payload = ntohs(ip->ip_len);
    if (size_payload > 0)
	{
		/*if (strcmp(inet_ntoa(ip->ip_dst), get_upload_server_ip()) == 0
		//|| strcmp(inet_ntoa(ip->ip_src), get_upload_server_ip()) == 0
		|| strcmp(inet_ntoa(ip->ip_src), "192.168.43.1") == 0
		|| strcmp(inet_ntoa(ip->ip_dst), "192.168.43.1") == 0
		|| (strncmp(inet_ntoa(ip->ip_src), "192.168.43", strlen("192.168.43")) == 0
			&& strncmp(inet_ntoa(ip->ip_dst), "192.168.43", strlen("192.168.43")) == 0))
			//return ;*/
		
        //VAL_LOG("Payload (%d bytes):\n", size_payload);
        if (! eth_type)
	    {
	    	char cap_dev_ip[32] = {0};
	    	get_dev_ip(cap_dev_ip, cap_dev);

			if (strncmp(inet_ntoa(ip->ip_dst), cap_dev_ip, strlen(cap_dev_ip)))
			{
				//目标地址不是局域网内的，此时统计的是上行的外网流量
				traffic_add(cap_dev, s_mac, inet_ntoa(ip->ip_dst), ntohs(tcp->th_dport), PROTO_TCP, size_payload, UPLOAD);
			}
			else
			{
				//目标地址是局域网内的，此时统计的是下行的外网流量
				traffic_add(cap_dev, d_mac, inet_ntoa(ip->ip_src), ntohs(tcp->th_sport), PROTO_TCP, size_payload, DOWNLOAD);
			}
		}
		else
		{
			if (strncmp(inet_ntoa(ip->ip_dst), "192.168.43.", strlen("192.168.43.")))
			{
				//目标地址不是局域网内的，此时统计的是上行的外网流量
				traffic_add(cap_dev, s_mac, inet_ntoa(ip->ip_dst), ntohs(tcp->th_dport), PROTO_TCP, size_payload, UPLOAD);
			}
			else
			{
				//目标地址是局域网内的，此时统计的是下行的外网流量
				traffic_add(cap_dev, d_mac, inet_ntoa(ip->ip_src), ntohs(tcp->th_sport), PROTO_TCP, size_payload, DOWNLOAD);
			}
		}
	}
}

void parse_udp(const u_char *packet, char *cap_dev, bool eth_type, char *s_mac, char *d_mac)
{
	const struct sniff_udp *udp;
	const struct sniff_ip *ip;

    int size_ip = 0;
    int size_payload = 0;

	int size_ethernet = 0;
	if (eth_type)
		size_ethernet = SIZE_ETHERNET;

	ip = (struct sniff_ip *)(packet + size_ethernet);
    size_ip = IP_HL(ip) * 4;
    if (size_ip < 20)
	{
        VAL_LOG(" * Invalid IP header length: %u bytes\n", size_ip);
        return;
    }

    udp = (struct sniff_udp *) (packet + size_ethernet + size_ip);
    //VAL_LOG("Src port: %d\n", ntohs(udp->sport));
    //VAL_LOG("Dst port: %d\n", ntohs(udp->dport));
    //VAL_LOG("udp length:%d\n", ntohs(udp->udp_length));
    //VAL_LOG("udp sum:%d\n", ntohs(udp->udp_sum));

	if (ntohs(udp->sport) == DNS_PORT)
	{
		char dns_response[1024] = {0};
		if (0 < ntohs(udp->udp_length) - UDP_HEADER_LEN && ntohs(udp->udp_length) - UDP_HEADER_LEN < 512)
		{
			memcpy(dns_response, packet + size_ethernet + size_ip + UDP_HEADER_LEN, ntohs(udp->udp_length) - UDP_HEADER_LEN);
			parse_dns_response(cap_dev, d_mac, (unsigned char *)dns_response, ntohs(udp->udp_length) - UDP_HEADER_LEN);
		}
	}

    //size_payload = ntohs(ip->ip_len) - (size_ip + UDP_HEADER_LEN);
    size_payload = ntohs(ip->ip_len);
	if (size_payload > 0)
	{
		/*if (strcmp(inet_ntoa(ip->ip_dst), get_upload_server_ip()) == 0
		|| strcmp(inet_ntoa(ip->ip_src), get_upload_server_ip()) == 0
		|| strcmp(inet_ntoa(ip->ip_src), "192.168.43.1") == 0
		|| strcmp(inet_ntoa(ip->ip_dst), "192.168.43.1") == 0
		|| (strncmp(inet_ntoa(ip->ip_src), "192.168.43", strlen("192.168.43")) == 0
			&& strncmp(inet_ntoa(ip->ip_dst), "192.168.43", strlen("192.168.43")) == 0))
			return ;*/
		
		//VAL_LOG("Payload (%d bytes):\n", size_payload);
		if (! eth_type)
		{
			char cap_dev_ip[32] = {0};
			get_dev_ip(cap_dev_ip, cap_dev);
			
			if (strncmp(inet_ntoa(ip->ip_dst), cap_dev_ip, strlen(cap_dev_ip)))
			{
				//目标地址不是局域网内的，此时统计的是上行的外网流量
				traffic_add(cap_dev, s_mac, inet_ntoa(ip->ip_dst), ntohs(udp->dport), PROTO_UDP, size_payload, UPLOAD);
			}
			else
			{
				//目标地址是局域网内的，此时统计的是下行的外网流量
				traffic_add(cap_dev, d_mac, inet_ntoa(ip->ip_src), ntohs(udp->sport), PROTO_UDP, size_payload, DOWNLOAD);
			}
		}
		else
		{
			if (strncmp(inet_ntoa(ip->ip_dst), "192.168.43.", strlen("192.168.43.")))
			{
				//目标地址不是局域网内的，此时统计的是上行的外网流量
				traffic_add(cap_dev, s_mac, inet_ntoa(ip->ip_dst), ntohs(udp->dport), PROTO_UDP, size_payload, UPLOAD);
			}
			else
			{
				//目标地址是局域网内的，此时统计的是下行的外网流量
				traffic_add(cap_dev, d_mac, inet_ntoa(ip->ip_src), ntohs(udp->sport), PROTO_UDP, size_payload, DOWNLOAD);
			}
		}
	}
}

void parse_icmp(const u_char *packet, char *cap_dev, bool eth_type, char *s_mac, char *d_mac)
{
	const struct sniff_ip *ip;

    int size_ip = 0;
    int size_payload = 0;

	int size_ethernet = 0;
	if (eth_type)
		size_ethernet = SIZE_ETHERNET;

	ip = (struct sniff_ip *)(packet + size_ethernet);
    size_ip = IP_HL(ip) * 4;
    if (size_ip < 20)
	{
        VAL_LOG(" * Invalid IP header length: %u bytes\n", size_ip);
        return;
    }

	//ICMP内容

    size_payload = ntohs(ip->ip_len);
	if (size_payload > 0)
	{
		/*if (strcmp(inet_ntoa(ip->ip_dst), get_upload_server_ip()) == 0
		|| strcmp(inet_ntoa(ip->ip_src), get_upload_server_ip()) == 0
		|| strcmp(inet_ntoa(ip->ip_src), "192.168.43.1") == 0
		|| strcmp(inet_ntoa(ip->ip_dst), "192.168.43.1") == 0
		|| (strncmp(inet_ntoa(ip->ip_src), "192.168.43", strlen("192.168.43")) == 0
			&& strncmp(inet_ntoa(ip->ip_dst), "192.168.43", strlen("192.168.43")) == 0))
			return ;*/
		
		//VAL_LOG("Payload (%d bytes):\n", size_payload);
		if (! eth_type)
		{
			char cap_dev_ip[32] = {0};
			get_dev_ip(cap_dev_ip, cap_dev);
			
			if (strncmp(inet_ntoa(ip->ip_dst), cap_dev_ip, strlen(cap_dev_ip)))
			{
				//目标地址不是局域网内的，此时统计的是上行的外网流量
				traffic_add(cap_dev, s_mac, inet_ntoa(ip->ip_dst), ICMP_PORT, PROTO_ICMP, size_payload, UPLOAD);
			}
			else
			{
				//目标地址是局域网内的，此时统计的是下行的外网流量
				traffic_add(cap_dev, d_mac, inet_ntoa(ip->ip_src), ICMP_PORT, PROTO_ICMP, size_payload, DOWNLOAD);
			}
		}
		else
		{
			if (strncmp(inet_ntoa(ip->ip_dst), "192.168.43.", strlen("192.168.43.")))
			{
				//目标地址不是局域网内的，此时统计的是上行的外网流量
				traffic_add(cap_dev, s_mac, inet_ntoa(ip->ip_dst), ICMP_PORT, PROTO_ICMP, size_payload, UPLOAD);
			}
			else
			{
				//目标地址是局域网内的，此时统计的是下行的外网流量
				traffic_add(cap_dev, d_mac, inet_ntoa(ip->ip_src), ICMP_PORT, PROTO_ICMP, size_payload, DOWNLOAD);
			}
		}
	}
}

void parse_igmp(const u_char *packet, char *cap_dev, bool eth_type, char *s_mac, char *d_mac)
{
	const struct sniff_ip *ip;

    int size_ip = 0;
    int size_payload = 0;

	int size_ethernet = 0;
	if (eth_type)
		size_ethernet = SIZE_ETHERNET;

	ip = (struct sniff_ip *)(packet + size_ethernet);
    size_ip = IP_HL(ip) * 4;
    if (size_ip < 20)
	{
        VAL_LOG(" * Invalid IP header length: %u bytes\n", size_ip);
        return;
    }

	//IGMP内容

    size_payload = ntohs(ip->ip_len);
	if (size_payload > 0)
	{
		/*if (strcmp(inet_ntoa(ip->ip_dst), get_upload_server_ip()) == 0
		|| strcmp(inet_ntoa(ip->ip_src), get_upload_server_ip()) == 0
		|| strcmp(inet_ntoa(ip->ip_src), "192.168.43.1") == 0
		|| strcmp(inet_ntoa(ip->ip_dst), "192.168.43.1") == 0
		|| (strncmp(inet_ntoa(ip->ip_src), "192.168.43", strlen("192.168.43")) == 0
			&& strncmp(inet_ntoa(ip->ip_dst), "192.168.43", strlen("192.168.43")) == 0))
			return ;*/
		
		//VAL_LOG("Payload (%d bytes):\n", size_payload);
		if (! eth_type)
		{
			char cap_dev_ip[32] = {0};
			get_dev_ip(cap_dev_ip, cap_dev);
			
			if (strncmp(inet_ntoa(ip->ip_dst), cap_dev_ip, strlen(cap_dev_ip)))
			{
				//目标地址不是局域网内的，此时统计的是上行的外网流量
				traffic_add(cap_dev, s_mac, inet_ntoa(ip->ip_dst), IGMP_PORT, PROTO_IGMP, size_payload, UPLOAD);
			}
			else
			{
				//目标地址是局域网内的，此时统计的是下行的外网流量
				traffic_add(cap_dev, d_mac, inet_ntoa(ip->ip_src), IGMP_PORT, PROTO_IGMP, size_payload, DOWNLOAD);
			}
		}
		else
		{
			if (strncmp(inet_ntoa(ip->ip_dst), "192.168.43.", strlen("192.168.43.")))
			{
				//目标地址不是局域网内的，此时统计的是上行的外网流量
				traffic_add(cap_dev, s_mac, inet_ntoa(ip->ip_dst), IGMP_PORT, PROTO_IGMP, size_payload, UPLOAD);
			}
			else
			{
				//目标地址是局域网内的，此时统计的是下行的外网流量
				traffic_add(cap_dev, d_mac, inet_ntoa(ip->ip_src), IGMP_PORT, PROTO_IGMP, size_payload, DOWNLOAD);
			}
		}
	}
}

