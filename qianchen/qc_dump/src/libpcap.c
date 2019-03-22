#include "includes.h"

char *libpcap_lookupdev(char *dev, char *errbuf)
{
	char *cap_dev = dev;
	if (cap_dev == NULL || strlen(cap_dev) <= 0)
	{
		cap_dev = pcap_lookupdev(errbuf);
		if (cap_dev == NULL)
		{
			LOG_NORMAL_INFO("Couldn't find default device %s: %s\n", dev, errbuf);
			return NULL;
		}
	}

	return cap_dev;
}

int libpcap_lookupnet(char *dev, bpf_u_int32 *net, bpf_u_int32 *mask, char *errbuf)
{
    if (pcap_lookupnet(dev, net, mask, errbuf) == -1)
	{
		LOG_NORMAL_INFO("Couldn't get netmask for device %s: %s\n", dev, errbuf);
        *net = 0;
        *mask = 0;
        return -1;
    }

	return 0;
}

pcap_t *libpcap_open_live(char *dev, int snaplen, int promisc, int to_ms, char *errbuf)
{
    pcap_t *handle = pcap_open_live(dev, snaplen, promisc, to_ms, errbuf);
    if (handle == NULL)
	{
        LOG_NORMAL_INFO("Couldn't open device %s: %s\n", dev, errbuf);
        return NULL;
    }

    return handle;
}

int libpcap_datalink(pcap_t *handle)
{
    return pcap_datalink(handle);
}

int libpcap_filter_set(pcap_t *handle, char *filter_exp, struct bpf_program *filter, bpf_u_int32 net)
{
	if (! filter_exp || strlen(filter_exp) <= 0)
		return -1;
	
    if (pcap_compile(handle, filter, filter_exp, 0, net) == -1)
	{
        LOG_NORMAL_INFO("Couldn't parse filter %s: %s\n", filter_exp, pcap_geterr(handle));
        return -1;
    }

    if (pcap_setfilter(handle, filter) == -1)
	{
        LOG_NORMAL_INFO("Couldn't install filter %s: %s\n", filter_exp, pcap_geterr(handle));
        return -1;
    }

	return 0;
}

void libpcap_loop(pcap_t *handle, int num_packets, pcap_handler callback, void *arg)
{
	pcap_loop(handle, num_packets, callback, (u_char *)arg);
}

void libpcap_exit(pcap_t *handle, struct bpf_program *filter)
{
	if (filter)
    	pcap_freecode(filter);
    if (handle)
    	pcap_close(handle);
}

void libpcap_breakloop(pcap_t *handle)
{
    if (handle)
    	pcap_breakloop(handle);
}

