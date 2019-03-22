#ifndef __LIB_PCAP_H__
#define __LIB_PCAP_H__

#include "common.h"

char *libpcap_lookupdev(char *dev, char *errbuf);
int libpcap_lookupnet(char *dev, bpf_u_int32 *net, bpf_u_int32 *mask, char *errbuf);
pcap_t *libpcap_open_live(char *dev, int snaplen, int promisc, int to_ms, char *errbuf);
int libpcap_datalink(pcap_t *handle);
int libpcap_filter_set(pcap_t *handle, char *filter_exp, struct bpf_program *filter, bpf_u_int32 net);
void libpcap_loop(pcap_t *handle, int num_packets, pcap_handler callback, void *arg);
void libpcap_exit(pcap_t *handle, struct bpf_program *filter);
void libpcap_breakloop(pcap_t *handle);


#endif

