#include "includes.h"

#define SNAP_LEN 			1518

static void init_ip_parse(cap_t *cap)
{
	memset(&cap->ip_parse, 0, sizeof(ip_parse_t));
}

static void protocol_parse(const u_char *packet, cap_t *cap)
{
    switch (cap->ip_parse.ip_protocol)
	{
        case e_protocol_tcp:
		{
            //LOG_NORMAL_INFO(" Protocol: TCP\n");
            parse_tcp(packet, cap);
            break;
		}
        case e_protocol_udp:
		{
            //LOG_NORMAL_INFO(" Protocol: UDP\n");
            parse_udp(packet, cap);
            break;
		}
        case e_protocol_icmp:
		{
            //LOG_NORMAL_INFO(" Protocol: ICMP\n");
            parse_icmp(packet, cap);
            break;
		}
        case e_protocol_igmp:
		{
            //LOG_NORMAL_INFO(" Protocol: IGMP\n");
            parse_igmp(packet, cap);
            break;
		}
		case e_protocol_unkown:
        default:
		{
            LOG_ERROR_INFO(" Protocol: unknown\n");
            break;
		}
    }
}

void protocol_parse_debug(cap_t *cap)
{
	LOG_NORMAL_INFO("-----------------------------------------------\n");
    switch (cap->ip_parse.ip_protocol)
	{
        case e_protocol_tcp:
		{
            LOG_NORMAL_INFO(" Protocol: TCP\n");
            LOG_NORMAL_INFO(" package size: %d\n", cap->ip_parse.payload_len);
            LOG_NORMAL_INFO(" src_IP: %s\n", cap->ip_parse.src_ip);
            LOG_NORMAL_INFO(" dst_IP: %s\n", cap->ip_parse.dst_ip);
            LOG_NORMAL_INFO(" src_MAC: %s\n", cap->ip_parse.src_mac);
            LOG_NORMAL_INFO(" dst_MAC: %s\n", cap->ip_parse.dst_mac);
            LOG_NORMAL_INFO(" src_PORT: %d\n", cap->ip_parse.proto_union.tcp_parse.src_port);
            LOG_NORMAL_INFO(" dst_PORT: %d\n", cap->ip_parse.proto_union.tcp_parse.dst_port);
            break;
		}
        case e_protocol_udp:
		{
            LOG_NORMAL_INFO(" Protocol: UDP\n");
            LOG_NORMAL_INFO(" package size: %d\n", cap->ip_parse.payload_len);
            LOG_NORMAL_INFO(" src_IP: %s\n", cap->ip_parse.src_ip);
            LOG_NORMAL_INFO(" dst_IP: %s\n", cap->ip_parse.dst_ip);
            LOG_NORMAL_INFO(" src_MAC: %s\n", cap->ip_parse.src_mac);
            LOG_NORMAL_INFO(" dst_MAC: %s\n", cap->ip_parse.dst_mac);
            LOG_NORMAL_INFO(" src_PORT: %d\n", cap->ip_parse.proto_union.udp_parse.src_port);
            LOG_NORMAL_INFO(" dst_PORT: %d\n", cap->ip_parse.proto_union.udp_parse.dst_port);

            break;
		}
        case e_protocol_icmp:
		{
            LOG_NORMAL_INFO(" Protocol: ICMP\n");
            LOG_NORMAL_INFO(" package size: %d\n", cap->ip_parse.payload_len);
            LOG_NORMAL_INFO(" src_IP: %s\n", cap->ip_parse.src_ip);
            LOG_NORMAL_INFO(" dst_IP: %s\n", cap->ip_parse.dst_ip);
            LOG_NORMAL_INFO(" src_MAC: %s\n", cap->ip_parse.src_mac);
            LOG_NORMAL_INFO(" dst_MAC: %s\n", cap->ip_parse.dst_mac);

            break;
		}
        case e_protocol_igmp:
		{
            LOG_NORMAL_INFO(" Protocol: IGMP\n");
            LOG_NORMAL_INFO(" package size: %d\n", cap->ip_parse.payload_len);
            LOG_NORMAL_INFO(" src_IP: %s\n", cap->ip_parse.src_ip);
            LOG_NORMAL_INFO(" dst_IP: %s\n", cap->ip_parse.dst_ip);
            LOG_NORMAL_INFO(" src_MAC: %s\n", cap->ip_parse.src_mac);
            LOG_NORMAL_INFO(" dst_MAC: %s\n", cap->ip_parse.dst_mac);

            break;
		}
		case e_protocol_unkown:
        default:
		{
            LOG_ERROR_INFO(" Protocol: unknown\n");
            break;
		}
    }
	LOG_NORMAL_INFO("-----------------------------------------------\n\n");
}

static void packet_process(u_char *args, const struct pcap_pkthdr *header, const u_char *packet)
{
	cap_t *cap = (cap_t *) args;

	init_ip_parse(cap);

	if (! is_ethernet_protocol(packet))
	{
		//LOG_ERROR_INFO("This packet is not ethernet protocol !\n");
		return;
	}

	if (parse_ip_protocol(packet, cap) < 0)
	{
		return;
	}
	
    protocol_parse(packet, cap);

    protocol_parse_debug(cap);
}

int cap_init(cap_t *cap)
{
	char errbuf[PCAP_ERRBUF_SIZE];
		
	bpf_u_int32 mask;
	bpf_u_int32 net;

	int num_packets = -1;
	
	char *dev = libpcap_lookupdev(cap->dev_name, errbuf);
	if (! dev)
		return -1;

	if (strcmp(cap->dev_name, dev))
		snprintf(cap->dev_name, 10, "%s", dev);

	LOG_HL_INFO("package dump for net device: %s .\n", dev);

	libpcap_lookupnet(dev, &net, &mask, errbuf);

	cap->handle = libpcap_open_live(dev, SNAP_LEN, 1, 0, errbuf);
	if (! cap->handle)
		return -1;

	if (libpcap_datalink(cap->handle) != DLT_EN10MB)
		return -1;

	libpcap_loop(cap->handle, num_packets, packet_process, (void *) cap);
	libpcap_exit(cap->handle, NULL);
	cap->handle = NULL;

	return 0;
}



