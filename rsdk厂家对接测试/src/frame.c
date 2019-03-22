#include "includes.h"

//0x24 FROM CMD 渠道号长度 渠道号 MAC长度 MAC CONTENT_LEN CONTENT CHECKSUM 0x0d 0x0a

unsigned char checksum(unsigned char *data, int len)
{
	int i;
	unsigned char sum = data[0];
	for (i = 1; i < len; i ++)
	{
		sum ^= data[i];
	}
	return sum & 0xff;
}

int frame_pack(unsigned char cmd, unsigned char from, 
			unsigned char *content, int content_len, unsigned char *frame_buff)
{

	unsigned char *ptmp = frame_buff;
	char *local_mac = get_netdev_wan_mac();
	*ptmp = FRAME_HEAD;
	ptmp += 1;
	*ptmp = 0x00;
	ptmp += 1;
	*ptmp = cmd;
	ptmp += 1;	
	*ptmp = strlen(ID);
	ptmp += 1;
	memcpy(ptmp, ID, *(ptmp - 1));
	ptmp += *(ptmp - 1);
	*ptmp = strlen(local_mac);
	ptmp += 1;
	memcpy(ptmp, local_mac, *(ptmp - 1));
	ptmp += *(ptmp - 1);
	if (content_len > 0 && content)
	{
		*ptmp = content_len;
		ptmp += 1;
		memcpy(ptmp, content, content_len);
		ptmp += content_len;
	}
	else
	{
		*ptmp = 0x00;
		ptmp += 1;
	}
	*ptmp = checksum(frame_buff, ptmp - frame_buff);
	ptmp += 1;
	*ptmp = FRAME_END_0;
	ptmp += 1;
	*ptmp = FRAME_END_1;

	return ptmp - frame_buff;
}

s_frame* frame_analysis(unsigned char *frame)
{
	int analysis_size = sizeof(s_frame);
	s_frame *analysis = malloc(analysis_size);
	if (analysis == NULL)
	{
		return NULL;
	}
	memset(analysis, 0, analysis_size);
	
	int offset = 0;
	analysis->head = frame[offset ++];
	analysis->from = frame[offset ++];
	analysis->cmd = frame[offset ++];
	analysis->id_len = frame[offset ++];
	if (analysis->id_len > 0)
	{
		analysis->id = malloc(analysis->id_len);
		if (analysis->id == NULL)
		{
			free(analysis);
			return NULL;
		}
		memcpy(analysis->id, &frame[offset], analysis->id_len);
	}
	offset += analysis->id_len;
	analysis->mac_len = frame[offset ++];
	if (analysis->mac_len > 0)
	{
		analysis->router_mac = malloc(analysis->mac_len);
		if (analysis->router_mac == NULL)
		{
			free(analysis->id);			
			free(analysis);
			return NULL;
		}
		memcpy(analysis->router_mac, &frame[offset], analysis->mac_len);
	}
	offset += analysis->mac_len;
	analysis->content_len = frame[offset ++];
	if (analysis->content_len > 0)
	{
		analysis->content = malloc(analysis->content_len);
		if (analysis->content == NULL)
		{
			free(analysis->id);
			free(analysis->router_mac);
			free(analysis);
			return NULL;
		}
		memcpy(analysis->content, &frame[offset], analysis->content_len);
	}
	offset += analysis->content_len;
	analysis->checksum = frame[offset ++];
	analysis->end_0 = frame[offset ++];
	analysis->end_1 = frame[offset ++];
	
	return 0;
}

int frame_free(s_frame *analysis)
{
	free(analysis->id);
	free(analysis->router_mac);
	free(analysis->content);
	free(analysis);
	return 0;
}


