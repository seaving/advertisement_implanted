
#ifndef __AES_API_H__
#define __AES_API_H__

#define AES_KEY		"0478110b06cdb574d2dce64c7a7f5193"

int AESEncode (const char* srcString, int srcLen, char** dstString, int* dstLen, char *encryptKey);
int AESDecode (const char* srcString, int srcLen, char** dstString, int* dstLen, char *encryptKey);



#endif

