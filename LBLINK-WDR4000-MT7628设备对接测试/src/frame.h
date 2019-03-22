#ifndef __FRAME_H__
#define __FRAME_H__

//??
#define FRAME_HEAD		0x24

//?β
#define FRAME_END_0		0x0d
#define FRAME_END_1		0x0a

#define FRAME_HEAD_OFFSET	0
#define FRAME_SOURCE_OFFSET	1
#define FRAME_CMD_OFFSET	2


typedef struct framefield
{
	unsigned char head;
	unsigned char from;
	unsigned char cmd;
	unsigned char id_len;
	unsigned char *id;
	unsigned char mac_len;
	unsigned char *router_mac;
	unsigned int content_len;
	unsigned char *content;
	unsigned char checksum;
	unsigned char end_0;
	unsigned char end_1;
}s_frame;

int frame_pack(unsigned char cmd, unsigned char from, 
			unsigned char *content, int content_len, unsigned char *frame_buff);

s_frame* frame_analysis(unsigned char *frame);
int frame_free(s_frame *analysis);


#endif

