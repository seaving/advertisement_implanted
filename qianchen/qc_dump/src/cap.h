#ifndef __CAP_H__
#define __CAP_H__

#include "common.h"

typedef enum
{
	e_protocol_tcp = 0,
	e_protocol_udp,
	e_protocol_icmp,
	e_protocol_igmp,
	e_protocol_unkown
} protocol_t;

typedef struct
{
	int src_port;
	int dst_port;
	int header_len;
} tcp_parse_t;

typedef struct
{
	int src_port;
	int dst_port;
} udp_parse_t;

typedef struct
{
	int src_port;
	int dst_port;
} icmp_parse_t;

typedef struct
{
	int src_port;
	int dst_port;
} igmp_parse_t;

typedef struct
{
	char src_ip[32];
	char dst_ip[32];
	char src_mac[32];
	char dst_mac[32];
	int header_len;
	int payload_len;
	int ip_protocol;
	union _proto_union_ {
		tcp_parse_t tcp_parse;
		udp_parse_t udp_parse;
		icmp_parse_t icmp_parse;
		igmp_parse_t igmp_parse;
	} proto_union;
} ip_parse_t;


typedef struct 
{
	char dev_name[10];
	pcap_t *handle;
	ip_parse_t ip_parse;
} cap_t;

int cap_init(cap_t *cap);


#endif




