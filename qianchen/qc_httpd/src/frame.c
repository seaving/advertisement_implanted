#include "includes.h"

int frame_package(char *frame, int frame_buf_size, unsigned char src, unsigned char dst, unsigned char cmd, char *data, int data_len)
{
	if (frame_buf_size <= FRAME_MIN_LEN + data_len)
		return -1;

	int offset = 0;
	frame[offset ++] = FRAME_HEADER;
	frame[offset ++] = src;
	frame[offset ++] = dst;
	frame[offset ++] = cmd;
	frame[offset ++] = data_len;
	if (data)
	{
		memcpy(&frame[offset], data, data_len);
		offset += data_len;
	}
	frame[offset ++] = FRAME_END;

	return offset;
}


