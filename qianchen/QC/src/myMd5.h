#ifndef __MD5_H__
#define __MD5_H__

#define X_AUTH_TOKEN_KEY		"gh93.87ty93gh-138hwo,ufo1"
#define X_AUTH_TOKEN_KEY_LEN	25

#define DATA_TOKEN_KEY		"ltd.qcwifi"
#define DATA_TOKEN_KEY_LEN	10

typedef struct
{
    unsigned int count[2];
    unsigned int state[4];
    unsigned char buffer[64];
}myMD5_CTX;

#define F(x, y, z) ((x & y) | (~x & z))
#define G(x, y, z) ((x & z) | (y & ~z))
#define H(x, y, z) (x^y^z)
#define I(x, y, z) (y ^ (x | ~z))
#define ROTATE_LEFT(x, n) ((x << n) | (x >> (32-n)))  
#define FF(a, b, c, d, x, s, ac) \
{ \
    a += F(b, c, d) + x + ac; \
    a = ROTATE_LEFT(a, s); \
    a += b; \
}
#define GG(a, b, c, d, x, s, ac) \
{ \
    a += G(b, c, d) + x + ac; \
    a = ROTATE_LEFT(a, s); \
    a += b; \
}
#define HH(a, b, c, d, x, s, ac) \
{ \
    a += H(b, c, d) + x + ac; \
    a = ROTATE_LEFT(a, s); \
    a += b; \
}
#define II(a, b, c, d, x, s, ac) \
{ \
    a += I(b, c, d) + x + ac; \
    a = ROTATE_LEFT(a, s); \
    a += b; \
}
void myMD5Init(myMD5_CTX *context);
void myMD5Update(myMD5_CTX *context, unsigned char *input, unsigned int inputlen);
void myMD5Final(myMD5_CTX *context, unsigned char digest[16]);
void myMD5Transform(unsigned int state[4], unsigned char block[64]);
void myMD5Encode(unsigned char *output, unsigned int *input, unsigned int len);
void myMD5Decode(unsigned int *output, unsigned char *input, unsigned int len);
int myMd5(unsigned char *src, int src_len, unsigned char *md5sum);
int CalMd5(char *src, int src_len, char *md5str);
int myMd5File(const char *path, int offset, int dataLen, const char *pstr, int pstrLen, char* md5);
int CalFileMd5(const char *path, int offset, int dataLen, char* md5);
int CalMd52(char *src, int src_len, char *serial_id, char *uri, char *key, int key_len, char *md5str);

#endif  

