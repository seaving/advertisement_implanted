#ifndef __CAP_CALL_H__
#define __CAP_CALL_H__

#include "common.h"

void print_payload(const u_char *payload, int len);
void packet_process(u_char *args, const struct pcap_pkthdr *header, const u_char *packet);


#endif


