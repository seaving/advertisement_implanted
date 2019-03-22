/* atmarp.h - ATM ARP protocol and kernel-demon interface definitions */
 
/* Written 1995-1999 by Werner Almesberger, EPFL LRC/ICA */
 

#ifndef _LINUX_ATMARP_H
#define _LINUX_ATMARP_H

#include <linux/types.h>
#include <linux/atmapi.h>
#include <linux/atmioc.h>


#define ATMARP_RETRY_DELAY 30		/* request next resolution or forget
					   NAK after 30 sec - should go into
					   atmclip.h */
#define ATMARP_MAX_UNRES_PACKETS 5	/* queue that many packets while
					   waiting for the resolver */


#define ATMARPD_CTRL	_IO('a',ATMIOC_CLIP+1)	/* become atmarpd ctrl sock */
#define ATMARP_MKIP	_IO('a',ATMIOC_CLIP+2)	/* attach socket to IP */
#define ATMARP_SETENTRY	_IO('a',ATMIOC_CLIP+3)	/* fill or hide ARP entry */
#define ATMARP_ENCAP	_IO('a',ATMIOC_CLIP+5)	/* change encapsulation */
// Jenny
#define ATMARP_IP	_IO('a',ATMIOC_CLIP+6)	/* change ip */
#define ATMARP_SRV	_IO('a',ATMIOC_CLIP+7)	/* arp server ip */
#define ATMARP_INARPREP	_IO('a',ATMIOC_CLIP+8)	/* send InATMArpReply */


enum atmarp_ctrl_type {
	act_invalid,		/* catch uninitialized structures */
	act_need,		/* need address resolution */
	act_up,			/* interface is coming up */
	act_down,		/* interface is going down */
	act_change		/* interface configuration has changed */
};

struct atmarp_ctrl {
	enum atmarp_ctrl_type	type;	/* message type */
	int			itf_num;/* interface number (if present) */
	__be32			ip;	/* IP address (act_need only) */
};

// Jenny
/* RFC 1577 ATM ARP header */

struct atmarphdr {
	uint16_t ar_hrd;	/* Hardware type */
	uint16_t ar_pro;	/* Protocol type */
	uint8_t ar_shtl;	/* Type & length of source ATM number (q) */
	uint8_t ar_sstl;	/* Type & length of source ATM subaddress (r) */
	uint16_t ar_op;	/* Operation code (request, reply, or NAK) */
	uint8_t ar_spln;	/* Length of source protocol address (s) */
	uint8_t ar_thtl;	/* Type & length of target ATM number (x) */
	uint8_t ar_tstl;	/* Type & length of target ATM subaddress (y) */
	uint8_t ar_tpln;	/* Length of target protocol address (z) */
	/* ar_sha, at_ssa, ar_spa, ar_tha, ar_tsa, ar_tpa */
	unsigned char data[1];
};

#define TL_LEN 0x3f	/* ATMARP Type/Length field structure */

#endif
