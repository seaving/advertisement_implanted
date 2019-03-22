#include "includes.h" 
  
void myMD5Init(myMD5_CTX *context)  
{  
    context->count[0] = 0;  
    context->count[1] = 0;  
    context->state[0] = 0x67452301;  
    context->state[1] = 0xEFCDAB89;  
    context->state[2] = 0x98BADCFE;  
    context->state[3] = 0x10325476;  
}

void myMD5Update(myMD5_CTX *context, unsigned char *input, unsigned int inputlen)  
{  
    unsigned int i = 0,index = 0,partlen = 0;
    index = (context->count[0] >> 3) & 0x3F;
    partlen = 64 - index;
    context->count[0] += inputlen << 3;
    if (context->count[0] < (inputlen << 3))
        context->count[1]++;
    context->count[1] += inputlen >> 29;
  
    if (inputlen >= partlen)
    {
        memcpy(&context->buffer[index], input, partlen);
        myMD5Transform(context->state, context->buffer);
        for(i = partlen; i+64 <= inputlen; i += 64)
           myMD5Transform(context->state, &input[i]);
        index = 0;
    }    
    else
    {  
        i = 0;
    }  
    memcpy(&context->buffer[index], &input[i], inputlen-i);
}

void myMD5Final(myMD5_CTX *context, unsigned char digest[16])  
{
	unsigned char PADDING[] = {
		0x80,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,  
		0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,  
		0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,  
		0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
	};

    unsigned int index = 0,padlen = 0;  
    unsigned char bits[8];  
    index = (context->count[0] >> 3) & 0x3F;  
    padlen = (index < 56) ? (56-index) : (120-index);  
    myMD5Encode(bits,context->count, 8);  
    myMD5Update(context, PADDING, padlen);  
    myMD5Update(context, bits, 8);  
    myMD5Encode(digest, context->state, 16);  
}

void myMD5Encode(unsigned char *output, unsigned int *input, unsigned int len)  
{  
    unsigned int i = 0,j = 0;  
    while (j < len)  
    {  
        output[j] = input[i] & 0xFF;    
        output[j+1] = (input[i] >> 8) & 0xFF;  
        output[j+2] = (input[i] >> 16) & 0xFF;  
        output[j+3] = (input[i] >> 24) & 0xFF;  
        i ++;  
        j += 4;  
    }  
}

void myMD5Decode(unsigned int *output, unsigned char *input, unsigned int len)  
{  
    unsigned int i = 0, j = 0;  
    while (j < len)  
    {  
        output[i] = (input[j]) |  
            (input[j+1] << 8) |  
            (input[j+2] << 16) |  
            (input[j+3] << 24);  
        i ++;  
        j += 4;   
    }  
}

void myMD5Transform(unsigned int state[4], unsigned char block[64])  
{  
    unsigned int a = state[0];  
    unsigned int b = state[1];  
    unsigned int c = state[2];  
    unsigned int d = state[3];  
    unsigned int x[64];  
    myMD5Decode(x, block, 64);  
    FF(a, b, c, d, x[ 0], 7, 0xd76aa478);   
    FF(d, a, b, c, x[ 1], 12, 0xe8c7b756);   
    FF(c, d, a, b, x[ 2], 17, 0x242070db);   
    FF(b, c, d, a, x[ 3], 22, 0xc1bdceee);   
    FF(a, b, c, d, x[ 4], 7, 0xf57c0faf);   
    FF(d, a, b, c, x[ 5], 12, 0x4787c62a);   
    FF(c, d, a, b, x[ 6], 17, 0xa8304613);   
    FF(b, c, d, a, x[ 7], 22, 0xfd469501);   
    FF(a, b, c, d, x[ 8], 7, 0x698098d8);   
    FF(d, a, b, c, x[ 9], 12, 0x8b44f7af);   
    FF(c, d, a, b, x[10], 17, 0xffff5bb1);   
    FF(b, c, d, a, x[11], 22, 0x895cd7be);   
    FF(a, b, c, d, x[12], 7, 0x6b901122);   
    FF(d, a, b, c, x[13], 12, 0xfd987193);   
    FF(c, d, a, b, x[14], 17, 0xa679438e);   
    FF(b, c, d, a, x[15], 22, 0x49b40821);   
  
      
    GG(a, b, c, d, x[ 1], 5, 0xf61e2562);   
    GG(d, a, b, c, x[ 6], 9, 0xc040b340);   
    GG(c, d, a, b, x[11], 14, 0x265e5a51);   
    GG(b, c, d, a, x[ 0], 20, 0xe9b6c7aa);   
    GG(a, b, c, d, x[ 5], 5, 0xd62f105d);   
    GG(d, a, b, c, x[10], 9,  0x2441453);   
    GG(c, d, a, b, x[15], 14, 0xd8a1e681);   
    GG(b, c, d, a, x[ 4], 20, 0xe7d3fbc8);   
    GG(a, b, c, d, x[ 9], 5, 0x21e1cde6);   
    GG(d, a, b, c, x[14], 9, 0xc33707d6);   
    GG(c, d, a, b, x[ 3], 14, 0xf4d50d87);   
    GG(b, c, d, a, x[ 8], 20, 0x455a14ed);   
    GG(a, b, c, d, x[13], 5, 0xa9e3e905);   
    GG(d, a, b, c, x[ 2], 9, 0xfcefa3f8);   
    GG(c, d, a, b, x[ 7], 14, 0x676f02d9);   
    GG(b, c, d, a, x[12], 20, 0x8d2a4c8a);   
  
      
    HH(a, b, c, d, x[ 5], 4, 0xfffa3942);   
    HH(d, a, b, c, x[ 8], 11, 0x8771f681);   
    HH(c, d, a, b, x[11], 16, 0x6d9d6122);   
    HH(b, c, d, a, x[14], 23, 0xfde5380c);   
    HH(a, b, c, d, x[ 1], 4, 0xa4beea44);   
    HH(d, a, b, c, x[ 4], 11, 0x4bdecfa9);   
    HH(c, d, a, b, x[ 7], 16, 0xf6bb4b60);   
    HH(b, c, d, a, x[10], 23, 0xbebfbc70);   
    HH(a, b, c, d, x[13], 4, 0x289b7ec6);   
    HH(d, a, b, c, x[ 0], 11, 0xeaa127fa);   
    HH(c, d, a, b, x[ 3], 16, 0xd4ef3085);   
    HH(b, c, d, a, x[ 6], 23,  0x4881d05);   
    HH(a, b, c, d, x[ 9], 4, 0xd9d4d039);   
    HH(d, a, b, c, x[12], 11, 0xe6db99e5);   
    HH(c, d, a, b, x[15], 16, 0x1fa27cf8);   
    HH(b, c, d, a, x[ 2], 23, 0xc4ac5665);   
  
      
    II(a, b, c, d, x[ 0], 6, 0xf4292244);   
    II(d, a, b, c, x[ 7], 10, 0x432aff97);   
    II(c, d, a, b, x[14], 15, 0xab9423a7);   
    II(b, c, d, a, x[ 5], 21, 0xfc93a039);   
    II(a, b, c, d, x[12], 6, 0x655b59c3);   
    II(d, a, b, c, x[ 3], 10, 0x8f0ccc92);   
    II(c, d, a, b, x[10], 15, 0xffeff47d);   
    II(b, c, d, a, x[ 1], 21, 0x85845dd1);   
    II(a, b, c, d, x[ 8], 6, 0x6fa87e4f);   
    II(d, a, b, c, x[15], 10, 0xfe2ce6e0);   
    II(c, d, a, b, x[ 6], 15, 0xa3014314);   
    II(b, c, d, a, x[13], 21, 0x4e0811a1);   
    II(a, b, c, d, x[ 4], 6, 0xf7537e82);   
    II(d, a, b, c, x[11], 10, 0xbd3af235);   
    II(c, d, a, b, x[ 2], 15, 0x2ad7d2bb);   
    II(b, c, d, a, x[ 9], 21, 0xeb86d391);   
    state[0] += a;  
    state[1] += b;  
    state[2] += c;  
    state[3] += d;  
}  

int myMd5(unsigned char *src, int src_len, unsigned char *md5sum)
{  
    myMD5_CTX md5;
    myMD5Init(&md5);
    myMD5Update(&md5, src, src_len);
    myMD5Final(&md5, md5sum);
    return 0;  
}

/**************************************
切记: src 必须预留出TOKEN_KEY_LEN+1的长度来
保存TOKEN_KEY，从而计算md5
直接在src上操作，少了内存拷贝
***************************************/
int CalMd5(char *src, int src_len, char *key, int key_len, char *md5str)
{
 	unsigned char md5sum[16] = {0};
 	char *psrc = src + src_len;
 	memcpy(psrc, key, key_len);
	myMd5((unsigned char *)src, psrc - src + key_len, (unsigned char *)md5sum);
	sprintf(md5str, "%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x", 
			md5sum[0], md5sum[1], md5sum[2], md5sum[3], md5sum[4], md5sum[5], md5sum[6],
			md5sum[7], md5sum[8], md5sum[9], md5sum[10], md5sum[11], md5sum[12], md5sum[13],
			md5sum[14], md5sum[15]);
	*psrc = 0;
	return 0;
}

int CalMd5ByCopy(char *src, int src_len, char *key, int key_len, char *md5str)
{
 	unsigned char md5sum[16] = {0};
 	char *psrc = calloc(1, src_len + key_len + 1);
 	if (psrc == NULL)
 		return -1;
 	
 	char *ptmp = psrc;
 	memcpy(ptmp, src, src_len); ptmp += src_len;
 	memcpy(ptmp, key, key_len);
 	LOG_WARN_INFO("====== %s\n", psrc);
	myMd5((unsigned char *)psrc, src_len + key_len, (unsigned char *)md5sum);
	sprintf(md5str, "%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x", 
			md5sum[0], md5sum[1], md5sum[2], md5sum[3], md5sum[4], md5sum[5], md5sum[6],
			md5sum[7], md5sum[8], md5sum[9], md5sum[10], md5sum[11], md5sum[12], md5sum[13],
			md5sum[14], md5sum[15]);
	free_malloc(psrc);
	return 0;
}

int CalMd52(char *src, int src_len, char *serial_id, char *uri, char *key, int key_len, char *md5str)
{
 	unsigned char md5sum[16] = {0};

	char mac[33] = {0};
	get_local_mac(mac, get_netdev_wan_name());

	int size = CHANNEL_LEN + strlen(mac) 
 				+ (serial_id ? strlen(serial_id) : 0) 
 				+ key_len
 				+ (uri ? strlen(uri) : 0)
 				+ strlen(src)
 				+ 1;
 	
 	char *psrc = calloc(1, size);
 	if (! psrc)
		return -1;
	char *ptmp = psrc;
	memcpy(ptmp, CHANNEL, CHANNEL_LEN); ptmp += CHANNEL_LEN;
	memcpy(ptmp, mac, strlen(mac)); ptmp += strlen(mac);
	memcpy(ptmp, serial_id ? serial_id : "", serial_id ? strlen(serial_id) : 0); ptmp += serial_id ? strlen(serial_id) : 0;
	memcpy(ptmp, key, key_len); ptmp += key_len;
	memcpy(ptmp, uri ? uri : "", uri ? strlen(uri) : 0); ptmp += uri ? strlen(uri) : 0;
 	memcpy(ptmp, src, src_len);
 	LOG_WARN_INFO("====== %s\n", psrc);
	myMd5((unsigned char *)psrc, size - 1, (unsigned char *)md5sum);
	sprintf(md5str, "%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x", 
			md5sum[0], md5sum[1], md5sum[2], md5sum[3], md5sum[4], md5sum[5], md5sum[6],
			md5sum[7], md5sum[8], md5sum[9], md5sum[10], md5sum[11], md5sum[12], md5sum[13],
			md5sum[14], md5sum[15]);
	free_malloc(psrc);
	return 0;
}

int myMd5File(const char *path, int offset, int dataLen, const char *pstr, int pstrLen, char* md5)
{
    int fd;
    myMD5_CTX md5_ctx;
    unsigned char md5sum[33] = {0};
	int filesize = 0;

	if (dataLen > 0)
		filesize = dataLen;
	else
		filesize = get_file_size((char *)path);
		
	if (filesize <= 0)
	{
		LOG_ERROR_INFO("get_file_size error! size=%d\n", filesize);
		return -1;
	}
    fd = open(path, O_RDONLY);
    if (fd < 0) {
		LOG_PERROR_INFO("could not open %s\n", path);
        return -1;
    }

    lseek(fd, offset, SEEK_SET);
    
    /* Note that bionic's MD5_* functions return void. */
    myMD5Init(&md5_ctx);

	ssize_t readed_len = 0;
    while (1) {
        char buf[4096];
        ssize_t rlen;
        rlen = read(fd, buf, sizeof(buf));
        if (rlen <= 0)
            break;
		readed_len += rlen;
        myMD5Update(&md5_ctx, (unsigned char*)buf, rlen);
    }
    close(fd);
	if (readed_len != filesize)
	{
		LOG_ERROR_INFO("file read error. readed_len=%d, filesize=%d !\n", readed_len, filesize);
		return -1;
	}

	if (pstr && pstrLen > 0)
		myMD5Update(&md5_ctx, (unsigned char*)pstr, pstrLen);

    myMD5Final(&md5_ctx, md5sum);
	sprintf(md5, "%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x", 
			md5sum[0], md5sum[1], md5sum[2], md5sum[3], md5sum[4], md5sum[5], md5sum[6],
			md5sum[7], md5sum[8], md5sum[9], md5sum[10], md5sum[11], md5sum[12], md5sum[13],
			md5sum[14], md5sum[15]);
	
    return 0;
}

int CalFileMd5(const char *path, int offset, int dataLen, char *key, int key_len, char* md5)
{
	return myMd5File(path, offset, dataLen, key, key_len, md5);
}

