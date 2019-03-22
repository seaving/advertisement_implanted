#include "includes.h"

static volatile int _package_count = 0;

static void _print_hex_ascii_line(const u_char *payload, int len, int offset)
{
    int i;
    int gap;
    const u_char *ch;

    printf("%05d ", offset);
    
    ch = payload;
    for (i = 0; i < len; i ++)
	{
        printf("%02x ", *ch);
        ch ++;
        if (i == 7)
            printf(" ");
    }
	
    if (len < 8)
        printf(" ");
    
    if (len < 16)
	{
        gap = 16 - len;
        for (i = 0; i < gap; i ++)
		{
            printf(" ");
        }
    }
    printf(" ");
    
    ch = payload;
    for(i = 0; i < len; i ++)
	{
        if (isprint(*ch))
            printf("%c", *ch);
        else
            printf(".");
        ch ++;
    }

    printf("\n");
}

void print_payload(const u_char *payload, int len)
{
    int len_rem = len;
    int line_width = 16;
    int line_len;
    int offset = 0;
    const u_char *ch = payload;

    if (len <= 0)
        return;

    if (len <= line_width)
	{
        _print_hex_ascii_line(ch, len, offset);
        return;
    }

    for ( ; ; )
	{
        line_len = line_width % len_rem;
        _print_hex_ascii_line(ch, line_len, offset);
        len_rem = len_rem - line_len;
        ch = ch + line_len;
        offset = offset + line_width;
        if (len_rem <= line_width)
		{
            _print_hex_ascii_line(ch, len_rem, offset);
            break;
        }
    }
}

void packet_process(u_char *args, const struct pcap_pkthdr *header, const u_char *packet)
{
    //int offset = 0;
    
    const struct sniff_ip *ip;
    cap_call_fun_arg_t *cap_arg = (cap_call_fun_arg_t *)args;
    char *cap_dev = cap_arg->cap_dev;
    bool eth_type = cap_arg->eth_type;
    
	int size_ethernet = 0;
	if (eth_type)
		size_ethernet = SIZE_ETHERNET;

    _package_count ++;
	//VAL_LOG("^^^^ %d\n", _package_count);
	
	char s_mac[32] = {0};
	char d_mac[32] = {0};
	if (eth_type)
	{
    	const struct sniff_ethernet *ethernet = (const struct sniff_ethernet *)packet;
    	//VAL_LOG("TYPE: %02x%02x\n", ethernet->ether_type[0], ethernet->ether_type[1]);
    	unsigned short ethernet_proto = (unsigned short)((unsigned short)(((unsigned short)ethernet->ether_type[0]) << 8) | ethernet->ether_type[1]);
    	if (ethernet_proto != ETHERNET_IP)
    		return;

    	sprintf(s_mac, "%02x:%02x:%02x:%02x:%02x:%02x", 
    					ethernet->ether_shost[0], 
    					ethernet->ether_shost[1], 
    					ethernet->ether_shost[2], 
    					ethernet->ether_shost[3], 
    					ethernet->ether_shost[4], 
    					ethernet->ether_shost[5]);
    	
    	sprintf(d_mac, "%02x:%02x:%02x:%02x:%02x:%02x", 
    					ethernet->ether_dhost[0], 
    					ethernet->ether_dhost[1], 
    					ethernet->ether_dhost[2], 
    					ethernet->ether_dhost[3], 
    					ethernet->ether_dhost[4], 
    					ethernet->ether_dhost[5]);
	}
	else
	{
		sprintf(s_mac, "ff:ff:ff:ff:ff:ff");
		sprintf(d_mac, "ff:ff:ff:ff:ff:ff");
	}
	
    //VAL_LOG("\nPacket number %d, header->len %d, header->caplen %d\n", _package_count, header->len, header->caplen);
	//print_payload(packet, header->caplen);

    ip = (struct sniff_ip *)(packet + size_ethernet);
	
    //VAL_LOG(" From: %s : %s\n", inet_ntoa(ip->ip_src), s_mac);
    //VAL_LOG(" To: %s : %s\n", inet_ntoa(ip->ip_dst), d_mac);
    
    switch (ip->ip_p)
	{
        case IPPROTO_TCP:
		{
            //VAL_LOG(" Protocol: TCP\n");
            parse_tcp(packet, cap_dev, eth_type, s_mac, d_mac);
            break;
		}
        case IPPROTO_UDP:
		{
            //VAL_LOG(" Protocol: UDP\n");
            parse_udp(packet, cap_dev, eth_type, s_mac, d_mac);
            break;
		}
        case IPPROTO_ICMP:
		{
            //VAL_LOG(" Protocol: ICMP\n");
            parse_icmp(packet, cap_dev, eth_type, s_mac, d_mac);
            break;
		}
        case IPPROTO_IGMP:
		{
            //VAL_LOG(" Protocol: IGMP\n");
            parse_igmp(packet, cap_dev, eth_type, s_mac, d_mac);
            break;
		}
        default:
		{
            VAL_LOG(" Protocol: unknown\n");
            break;
		}
    }
}


