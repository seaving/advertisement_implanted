#ifndef __PARSE_PROTO_H__
#define __PARSE_PROTO_H__

/* default snap length (maximum bytes per packet to capture) */
#define SNAP_LEN 			1518

/* ethernet headers are always exactly 14 bytes [1] */
#define SIZE_ETHERNET 		14

/* Ethernet addresses are 6 bytes */
#define ETHER_ADDRESS_LEN   6
#define ETHER_HEADER_LEN   	8
#define ETHER_TYPE_LEN    	2

/* Ethernet header */
#define ETHERNET_IP				0x0800
#define ETHERNEDNS_T_ARP		0x0806
#define ETHERNET_RARP			0x0835
#define ETHERNET_Novell_IPX		0x8137
#define ETHERNEDNS_T_Apple_Talk	0x809b

struct sniff_ethernet
{
	u_char ether_dhost[ETHER_ADDRESS_LEN]; 	/* destination host address */
	u_char ether_shost[ETHER_ADDRESS_LEN]; 	/* source host address */
	u_char ether_type[ETHER_TYPE_LEN]; 		/* IP? ARP? RARP? etc */
};

/* IP header */
struct sniff_ip
{
	u_char ip_vhl; 							/* version << 4 | header length >> 2 */
	u_char ip_tos; 							/* type of service */
	u_short ip_len; 						/* total length */
	u_short ip_id; 							/* identification */
	u_short ip_off; 						/* fragment offset field */
	#define IP_RF 0x8000 					/* reserved fragment flag */
	#define IP_DF 0x4000 					/* dont fragment flag */
	#define IP_MF 0x2000 					/* more fragments flag */
	#define IP_OFFMASK 0x1fff 				/* mask for fragmenting bits */
	u_char ip_ttl; 							/* time to live */
	u_char ip_p; 							/* protocol */
	u_short ip_sum; 						/* checksum */
	struct in_addr ip_src,ip_dst; 			/* source and dest address */
};

/* TCP header */
typedef u_int tcp_seq;

struct sniff_tcp {
	u_short th_sport; 						/* source port */
	u_short th_dport; 						/* destination port */
	tcp_seq th_seq; 						/* sequence number */
	tcp_seq th_ack; 						/* acknowledgement number */
	u_char th_offx2; 						/* data offset, rsvd */
	#define TH_OFF(th) (((th)->th_offx2 & 0xf0) >> 4)
	u_char th_flags;
	#define TH_FIN 0x01
	#define TH_SYN 0x02
	#define TH_RST 0x04
	#define TH_PUSH 0x08
	#define TH_ACK 0x10
	#define TH_URG 0x20
	#define TH_ECE 0x40
	#define TH_CWR 0x80
	#define TH_FLAGS (TH_FIN|TH_SYN|TH_RST|TH_ACK|TH_URG|TH_ECE|TH_CWR)
	u_short th_win; 						/* window */
	u_short th_sum; 						/* checksum */
	u_short th_urp; 						/* urgent pointer */
};

/* UDP header */  
struct sniff_udp
{
  uint16_t sport;       					/* source port */
  uint16_t dport;       					/* destination port */
  uint16_t udp_length;
  uint16_t udp_sum;     					/* checksum */
};

#define DNS_PORT		53
#define UDP_HEADER_LEN	8

#define IP_HL(ip) (((ip)->ip_vhl) & 0x0f)
#define IP_V(ip) (((ip)->ip_vhl) >> 4)


void parse_tcp(const u_char *packet, char *cap_dev, bool eth_type, char *s_mac, char *d_mac);
void parse_udp(const u_char *packet, char *cap_dev, bool eth_type, char *s_mac, char *d_mac);
void parse_icmp(const u_char *packet, char *cap_dev, bool eth_type, char *s_mac, char *d_mac);
void parse_igmp(const u_char *packet, char *cap_dev, bool eth_type, char *s_mac, char *d_mac);


#endif

