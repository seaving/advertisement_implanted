#ifndef __LINUX_NETLINK_H
#define __LINUX_NETLINK_H

#include <linux/socket.h> /* for sa_family_t */
#include <linux/types.h>

#define NETLINK_ROUTE		0	/* Routing/device hook				*/
#define NETLINK_UNUSED		1	/* Unused number				*/
#define NETLINK_USERSOCK	2	/* Reserved for user mode socket protocols 	*/
#define NETLINK_FIREWALL	3	/* Firewalling hook				*/
#define NETLINK_INET_DIAG	4	/* INET socket monitoring			*/
#define NETLINK_NFLOG		5	/* netfilter/iptables ULOG */
#define NETLINK_XFRM		6	/* ipsec */
#define NETLINK_SELINUX		7	/* SELinux event notifications */
#define NETLINK_ISCSI		8	/* Open-iSCSI */
#define NETLINK_AUDIT		9	/* auditing */
#define NETLINK_FIB_LOOKUP	10	
#define NETLINK_CONNECTOR	11
#define NETLINK_NETFILTER	12	/* netfilter subsystem */
#define NETLINK_IP6_FW		13
#define NETLINK_DNRTMSG		14	/* DECnet routing messages */
#define NETLINK_KOBJECT_UEVENT	15	/* Kernel messages to userspace */
#define NETLINK_GENERIC		16
/* leave room for NETLINK_DM (DM Events) */
#define NETLINK_SCSITRANSPORT	18	/* SCSI Transports */
#define NETLINK_ECRYPTFS	19

#define MAX_LINKS 32		

//#define CONFIG_RTL_NLMSG_PROTOCOL
#ifdef CONFIG_RTL_NLMSG_PROTOCOL
#define NETLINK_RTL_NLMSG		26 
#endif

struct net;

struct sockaddr_nl
{
	sa_family_t	nl_family;	/* AF_NETLINK	*/
	unsigned short	nl_pad;		/* zero		*/
	__u32		nl_pid;		/* port ID	*/
       	__u32		nl_groups;	/* multicast groups mask */
};

struct nlmsghdr
{
	__u32		nlmsg_len;	/* Length of message including header */
	__u16		nlmsg_type;	/* Message content */
	__u16		nlmsg_flags;	/* Additional flags */
	__u32		nlmsg_seq;	/* Sequence number */
	__u32		nlmsg_pid;	/* Sending process port ID */
};

/* Flags values */

#define NLM_F_REQUEST		1	/* It is request message. 	*/
#define NLM_F_MULTI		2	/* Multipart message, terminated by NLMSG_DONE */
#define NLM_F_ACK		4	/* Reply with ack, with zero or error code */
#define NLM_F_ECHO		8	/* Echo this request 		*/

/* Modifiers to GET request */
#define NLM_F_ROOT	0x100	/* specify tree	root	*/
#define NLM_F_MATCH	0x200	/* return all matching	*/
#define NLM_F_ATOMIC	0x400	/* atomic GET		*/
#define NLM_F_DUMP	(NLM_F_ROOT|NLM_F_MATCH)

/* Modifiers to NEW request */
#define NLM_F_REPLACE	0x100	/* Override existing		*/
#define NLM_F_EXCL	0x200	/* Do not touch, if it exists	*/
#define NLM_F_CREATE	0x400	/* Create, if it does not exist	*/
#define NLM_F_APPEND	0x800	/* Add to end of list		*/

/*
   4.4BSD ADD		NLM_F_CREATE|NLM_F_EXCL
   4.4BSD CHANGE	NLM_F_REPLACE

   True CHANGE		NLM_F_CREATE|NLM_F_REPLACE
   Append		NLM_F_CREATE
   Check		NLM_F_EXCL
 */

#define NLMSG_ALIGNTO	4
#define NLMSG_ALIGN(len) ( ((len)+NLMSG_ALIGNTO-1) & ~(NLMSG_ALIGNTO-1) )
#define NLMSG_HDRLEN	 ((int) NLMSG_ALIGN(sizeof(struct nlmsghdr)))
#define NLMSG_LENGTH(len) ((len)+NLMSG_ALIGN(NLMSG_HDRLEN))
#define NLMSG_SPACE(len) NLMSG_ALIGN(NLMSG_LENGTH(len))
#define NLMSG_DATA(nlh)  ((void*)(((char*)nlh) + NLMSG_LENGTH(0)))
#define NLMSG_NEXT(nlh,len)	 ((len) -= NLMSG_ALIGN((nlh)->nlmsg_len), \
				  (struct nlmsghdr*)(((char*)(nlh)) + NLMSG_ALIGN((nlh)->nlmsg_len)))
#define NLMSG_OK(nlh,len) ((len) >= (int)sizeof(struct nlmsghdr) && \
			   (nlh)->nlmsg_len >= sizeof(struct nlmsghdr) && \
			   (nlh)->nlmsg_len <= (len))
#define NLMSG_PAYLOAD(nlh,len) ((nlh)->nlmsg_len - NLMSG_SPACE((len)))

#define NLMSG_NOOP		0x1	/* Nothing.		*/
#define NLMSG_ERROR		0x2	/* Error		*/
#define NLMSG_DONE		0x3	/* End of a dump	*/
#define NLMSG_OVERRUN		0x4	/* Data lost		*/

#define NLMSG_MIN_TYPE		0x10	/* < 0x10: reserved control messages */

struct nlmsgerr
{
	int		error;
	struct nlmsghdr msg;
};

#define NETLINK_ADD_MEMBERSHIP	1
#define NETLINK_DROP_MEMBERSHIP	2
#define NETLINK_PKTINFO		3
#define NETLINK_BROADCAST_ERROR	4
#define NETLINK_NO_ENOBUFS	5

struct nl_pktinfo
{
	__u32	group;
};

#define NET_MAJOR 36		/* Major 36 is reserved for networking 						*/

enum {
	NETLINK_UNCONNECTED = 0,
	NETLINK_CONNECTED,
};

/*
 *  <------- NLA_HDRLEN ------> <-- NLA_ALIGN(payload)-->
 * +---------------------+- - -+- - - - - - - - - -+- - -+
 * |        Header       | Pad |     Payload       | Pad |
 * |   (struct nlattr)   | ing |                   | ing |
 * +---------------------+- - -+- - - - - - - - - -+- - -+
 *  <-------------- nlattr->nla_len -------------->
 */

struct nlattr
{
	__u16           nla_len;
	__u16           nla_type;
};

/*
 * nla_type (16 bits)
 * +---+---+-------------------------------+
 * | N | O | Attribute Type                |
 * +---+---+-------------------------------+
 * N := Carries nested attributes
 * O := Payload stored in network byte order
 *
 * Note: The N and O flag are mutually exclusive.
 */
#define NLA_F_NESTED		(1 << 15)
#define NLA_F_NET_BYTEORDER	(1 << 14)
#define NLA_TYPE_MASK		~(NLA_F_NESTED | NLA_F_NET_BYTEORDER)

#define NLA_ALIGNTO		4
#define NLA_ALIGN(len)		(((len) + NLA_ALIGNTO - 1) & ~(NLA_ALIGNTO - 1))
#define NLA_HDRLEN		((int) NLA_ALIGN(sizeof(struct nlattr)))


#ifdef CONFIG_RTL_NLMSG_PROTOCOL
typedef enum _tag_RTL_NLMSG_GROUPS
{
	RTL_BASIC_GRP 	= 1,
	RTL_2ND_GRP	= 2,
	RTL_3RD_GRP	= 4,
}RTL_NL_GRPS;

//august: the USB wrapper of netlink is not perfect, I decide to make a new flexable protocol
/*common structure of rtl nl msg*/
/*
The reason why I create a field called rtl_magic is that
	1) the MSB is the source of the msg.
			1'b == MSB, it is from user space;
			0'b == MSB, it is from kernel space;
	2) avoid being mixed up with other netlink message. 
			u see, as the "NETLINK_@#$%" is just a MACRO everybody can define. if someone else use 
			a same value, this may result in confusion. Use a magic can help us make an accept or ignore
	3) as a Reserved field.
*/
typedef struct _tag_RTL_STANDARD_NETLINKMESSAGE
{
	__u8	 rtl_magic;	//Most 1st Sig Bit is source, Low 7st SB were magic num 
	__u8	 type;			//type identify 
    __u16 length;		//the length of extended point to.
    __u8  extended[0];
} RTL_ST_NLMSG;

#define RTL_NLMSG_HEAD_SZ		0x4 	//just header, not include extended

/*rtl_magic field*/
#define RTL_NL_MAGIC_NUM			0x5A 	//0x5A is quite magic
#define CHECK_RTL_NL_MSG_MAGIC(magic)		(RTL_NL_MAGIC_NUM == (magic & ~0x80))	
#define CHECK_RTL_NL_MSG_SRC(magic)			(magic & (0x1 << 8))
#define RTL_NL_SRC_KENERL			0x00
#define RTL_NL_SRC_USER			0x80
/*--------------------------*/


/*type field*/
#define RTL_NL_TYPE_LINKCHANGE				0x1
#define RTL_NL_TYPE_SOMETHINGE				0x2
//add new rtl netlink type id here
/*--------------------------*/



/* #define RTL_NL_TYPE_LINKCHANGE				0x1 */
typedef struct _tag_EXT_RTL_NLMSG_LINKCHG
{
	__u16 old_ptmks;
	__u16 new_ptmks;
} _EXT_RTL_NLMSG_LINKCHG;
typedef struct _tag_RTL_NLMSG_LINKCHG
{
    __u8	rtl_magic;
    __u8	type;
    __u16 	length;		/* size of RTL_NL_LINK */                
    _EXT_RTL_NLMSG_LINKCHG extended;
} RTL_NLMSG_LINKCHG;
#define EXTSIZE_RTL_NLMSG_LINKCHG			sizeof(_EXT_RTL_NLMSG_LINKCHG)
/*--------------------------------------*/


//add new rtl netlink message struct like this
/*#define RTL_NL_TYPE_SOMETHING				0x2*/
typedef struct _tag_EXT_RTL_NLMSG_SOMETHING
{
	__u32 something;
	//new something
} _EXT_RTL_NLMSG_SOMETHING;
typedef struct _tag_RTL_NLMSG_SOMETHING
{
    __u8	 	rtl_magic;
    __u8	 	type;
    __u16 	length;		/* size of RTL_NL_SOMETHING */
    _EXT_RTL_NLMSG_SOMETHING extended;
} RTL_NLMSG_SOMETHING;
#define EXTSIZE_RTL_NLMSG_SOMETHING		sizeof(_EXT_RTL_NLMSG_SOMETHING)
/*--------------------------------------*/


#endif /* CONFIG_RTL_NL_PROTOCOL */

#endif	/* __LINUX_NETLINK_H */
