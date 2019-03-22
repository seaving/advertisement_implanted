#ifndef __DATA_INFO_H__
#define __DATA_INFO_H__	1

typedef struct
{	
	char *buf;
	int bufsize;
	int datalen;
} data_t;

data_t * init_data();
int calloc_buf(data_t *data, int bufsize);
int free_data(data_t *data);


#endif

