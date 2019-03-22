#include "includes.h"

#define ROL(x, offset) do { \
		x = 0; \
		int ROL_iiii = 0; \
		for (ROL_iiii = 0; ROL_iiii < offset; ROL_iiii ++) \
		{ \
			x = (x << 1) | 0x01; \
		} \
	} while (0)


bool is_ethernet_protocol(const u_char *packet)
{
	const struct sniff_ethernet *ethernet = (const struct sniff_ethernet *) packet;
	//LOG_NORMAL_INFO("TYPE: %02x%02x\n", ethernet->ether_type[0], ethernet->ether_type[1]);
	
	unsigned short ethernet_proto = 
		(unsigned short)((unsigned short)((
			(unsigned short)ethernet->ether_type[0]) << 8)
			| ethernet->ether_type[1]);
	
	if (ethernet_proto != ETHERNET_IP)
		return false;

	return true;
}

bool is_lan(char *mask, unsigned int src_ip, unsigned int dst_ip)
{
	struct in_addr net_mask;
	inet_aton(mask, &net_mask);
	unsigned int int_mask = ntohl(net_mask.s_addr);

	unsigned int s1 = src_ip & int_mask;
	unsigned int s2 = dst_ip & int_mask;

	//LOG_NORMAL_INFO("s1 = %u & %u = %u\n", src_ip, int_mask, s1);
	//LOG_NORMAL_INFO("s2 = %u & %u = %u\n", dst_ip, int_mask, s2);

	if (s1 == s2)
		return true;
	
	return false;
}

int parse_ip_protocol(const u_char *packet, cap_t *cap)
{
	const struct sniff_ethernet *ethernet = (const struct sniff_ethernet *) packet;
	const struct sniff_ip *ip;
	int size_ip = 0;
	ip = (struct sniff_ip *)(packet + SIZE_ETHERNET);
    size_ip = IP_HL(ip) * 4;
    if (size_ip < 20)
	{
        LOG_ERROR_INFO(" * Invalid IP header length: %u bytes\n", size_ip);
        return -1;
    }

	if (is_lan(get_netdev_lan_mask(), ntohl(ip->ip_src.s_addr), ntohl(ip->ip_dst.s_addr)))
	{
		return -1;
	}
    
	cap->ip_parse.header_len = size_ip;
	snprintf(cap->ip_parse.src_mac, 32, "%02x:%02x:%02x:%02x:%02x:%02x", 
					ethernet->ether_shost[0], 
					ethernet->ether_shost[1], 
					ethernet->ether_shost[2], 
					ethernet->ether_shost[3], 
					ethernet->ether_shost[4], 
					ethernet->ether_shost[5]);

	snprintf(cap->ip_parse.dst_mac, 32, "%02x:%02x:%02x:%02x:%02x:%02x", 
					ethernet->ether_dhost[0], 
					ethernet->ether_dhost[1], 
					ethernet->ether_dhost[2], 
					ethernet->ether_dhost[3], 
					ethernet->ether_dhost[4], 
					ethernet->ether_dhost[5]);

	ip = (struct sniff_ip *) (packet + SIZE_ETHERNET);
	snprintf(cap->ip_parse.src_ip, 32, "%s", inet_ntoa(ip->ip_src));
	snprintf(cap->ip_parse.dst_ip, 32, "%s", inet_ntoa(ip->ip_dst));
	
	cap->ip_parse.payload_len = ntohs(ip->ip_len);

    switch (ip->ip_p)
	{
        case IPPROTO_TCP:
		{
			cap->ip_parse.ip_protocol = e_protocol_tcp;
            break;
		}
        case IPPROTO_UDP:
		{
			cap->ip_parse.ip_protocol = e_protocol_udp;
            break;
		}
        case IPPROTO_ICMP:
		{
			cap->ip_parse.ip_protocol = e_protocol_icmp;
            break;
		}
        case IPPROTO_IGMP:
		{
			cap->ip_parse.ip_protocol = e_protocol_igmp;
            break;
		}
        default:
		{
			cap->ip_parse.ip_protocol = e_protocol_unkown;
            break;
		}
    }
    
	return 0;
}

void parse_tcp(const u_char *packet, cap_t *cap)
{
	const struct sniff_tcp *tcp;
	
    int size_tcp = 0;

    tcp = (struct sniff_tcp *)(packet + SIZE_ETHERNET + cap->ip_parse.header_len);
    size_tcp = TH_OFF(tcp) * 4;
    if (size_tcp < 20)
	{
        LOG_ERROR_INFO(" * Invalid TCP header length: %u bytes\n", size_tcp);
        return ;
    }

    cap->ip_parse.proto_union.tcp_parse.header_len = size_tcp;
    cap->ip_parse.proto_union.tcp_parse.src_port = ntohs(tcp->th_sport);
    cap->ip_parse.proto_union.tcp_parse.dst_port = ntohs(tcp->th_dport);
}

void parse_udp(const u_char *packet, cap_t *cap)
{
	const struct sniff_udp *udp;
    
    udp = (struct sniff_udp *) (packet + SIZE_ETHERNET + cap->ip_parse.header_len);
    cap->ip_parse.proto_union.udp_parse.src_port = ntohs(udp->sport);
    cap->ip_parse.proto_union.udp_parse.dst_port = ntohs(udp->dport);
#if 0

	int udp_len = ntohs(udp->udp_length);

	if (cap->ip_parse.proto_union.udp_parse.src_port == DNS_PORT)
	{
		char dns_response[1024] = {0};
		if (0 < udp_len - UDP_HEADER_LEN && udp_len - UDP_HEADER_LEN < 512)
		{
			memcpy(dns_response, packet + SIZE_ETHERNET + cap->ip_parse.header_len + UDP_HEADER_LEN, udp_len - UDP_HEADER_LEN);
			parse_dns_response(cap, (unsigned char *)dns_response, udp_len - UDP_HEADER_LEN);
		}
	}
#endif
}

void parse_icmp(const u_char *packet, cap_t *cap)
{

}

void parse_igmp(const u_char *packet, cap_t *cap)
{

}

