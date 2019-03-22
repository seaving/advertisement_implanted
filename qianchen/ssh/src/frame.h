#ifndef __FRAME_H__
#define __FRAME_H__

#define	FRAME_MODULE_SSH		0x01
#define	FRAME_MODULE_INIT		0x02
#define	FRAME_MODULE_QCHTTPD	0x03
#define	FRAME_MODULE_MONITOR	0x04
#define	FRAME_MODULE_QCDOG		0x05
#define	FRAME_MODULE_MITM		0x06
#define	FRAME_MODULE_FW_SET		0x07
#define FRAME_MODULE_APCTRL		0x08

#define FRAME_MIN_LEN	6

#define FRAME_HEADER	0x24

#define FRAME_HEADER_OFFSET		0
#define FRAME_SRC_OFFSET		1
#define FRAME_DST_OFFSET		2
#define FRAME_CMD_OFFSET		3
#define FRAME_DATA_LEN_OFFSET	4
#define FRAME_DATA_OFFSET		5

#define FRAME_END		0x0A


#define FRAME_CMD_SET_DEV_NO	0x00
#define FRAME_CMD_FW_VER		0x01
#define FRAME_CMD_GCC_VER		0x02
#define FRAME_CMD_LAN_NAME		0x03
#define FRAME_CMD_WAN_NAME		0x04
#define FRAME_CMD_GW_ADDR		0x05
#define FRAME_CMD_UPDATE		0x06
#define FRAME_CMD_WAN_MAC		0x07

#define FRAME_CMD_RESTART_PROGRAM	0x07

#define FRAME_BUF_SIZE(data_len)	FRAME_MIN_LEN + data_len + 1

int frame_package(char *frame, int frame_buf_size, unsigned char src, unsigned char dst, unsigned char cmd, char *data, int data_len);

#endif


