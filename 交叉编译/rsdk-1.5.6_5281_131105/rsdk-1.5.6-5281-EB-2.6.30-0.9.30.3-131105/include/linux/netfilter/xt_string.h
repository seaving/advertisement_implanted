#ifndef _XT_STRING_H
#define _XT_STRING_H

#include <linux/types.h>

#define XT_STRING_MAX_PATTERN_SIZE 128
#define XT_STRING_MAX_ALGO_NAME_SIZE 16

enum {
	XT_STRING_FLAG_INVERT		= 0x01,
	XT_STRING_FLAG_IGNORECASE	= 0x02
};

struct xt_string_info
{
	__u16 from_offset;
	__u16 to_offset;
	char	  algo[XT_STRING_MAX_ALGO_NAME_SIZE];
	char 	  pattern[XT_STRING_MAX_PATTERN_SIZE];
	__u8  patlen;

#if 1
	/*linux-2.6.19*/
	u_int8_t  invert;
	u_int8_t flagStr;
#else
	union {
		struct {
			__u8  invert;
		} v0;

		struct {
			__u8  flags;
		} v1;
	} u;
#endif

	/* Used internally by the kernel */
	struct ts_config __attribute__((aligned(8))) *config;
};

/*linux-2.6.19*/
enum ipt_string_flagStr
{
	IPT_GENERAL_STRING,
	IPT_DOMAIN_STRING,
	IPT_URL_STRING,
	IPT_URL_ALW_STRING
};

#endif /*_XT_STRING_H*/
