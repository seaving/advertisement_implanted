#include "includes.h"

int AESEncode (const char* srcString, int srcLen, char** dstString, int* dstLen, char *encryptKey)
{
	//16 * (trunc(string_length / 16) + 1)¡£
	char *pOut=0;
	unsigned int len = 16 * (srcLen/16 + 1);

	BYTE	UserKey[AES_USER_KEY_LEN]={0};
	BYTE	IV[AES_BLOCK_LEN]={0};

	DWORD	UKLen = 0, /*IVLen = 0,*/ SrcLen = 0, DstLen = 0;
	RET_VAL	ret = 0;
	AES_ALG_INFO	AlgInfo;
	int eelen = 0;

	UKLen = 16;
	//IVLen = 16;
#ifdef _DEBUG
	int t = 0x10;
	for (int i=0; i<16; i++)
	{
		UserKey[i] = t+i;
	}
#else
	snprintf ((char*)UserKey, sizeof(UserKey)-1, "%s", encryptKey);
#endif

	pOut = (char*)calloc (1, len+4);
	if (pOut == NULL)	
		return -1;
	DstLen = len;

	//
	AES_SetAlgInfo(AES_ModeType, AES_PadType, IV, &AlgInfo);

	//	Encryption
	ret = AES_EncKeySchedule(UserKey, UKLen, &AlgInfo);
	if( ret!=CTR_SUCCESS )	
	{
		free_malloc (pOut);
		return -1;
	}
	ret = AES_EncInit(&AlgInfo);
	if( ret!=CTR_SUCCESS )	
	{
		free_malloc (pOut);
		return -1;
	}

	ret = AES_EncUpdate(&AlgInfo, (unsigned char*)srcString, SrcLen, (unsigned char*)pOut, &DstLen);
	if( ret!=CTR_SUCCESS )	
	{
		free_malloc (pOut);
		return -1;
	}

	eelen = DstLen;

	ret = AES_EncFinal(&AlgInfo, (unsigned char*)pOut+eelen, &DstLen);
	if( ret!=CTR_SUCCESS )	
	{
		free_malloc (pOut);
		return -1;
	}

	eelen += DstLen;
	*dstLen = eelen;
	*dstString = pOut;

	return 0;

}

int AESDecode (const char* srcString, int srcLen, char** dstString, int* dstLen, char *encryptKey)
{
	//FILE	*pIn, *pOut;
	char* pOut = 0;
	unsigned char UserKey[AES_USER_KEY_LEN]={0};
	unsigned char IV[AES_BLOCK_LEN]={0};
	//unsigned char SrcData[1024+32], DstData[1024+32];
	unsigned int  UKLen = 0/*, IVLen*/;
	unsigned int SrcLen = 0, DstLen = 0;
	RET_VAL	ret = 0;
	AES_ALG_INFO	AlgInfo;
	int ddlen = 0;

	SrcLen = srcLen;
	
	pOut = (char*)calloc(1, SrcLen+2);
	if (pOut == NULL) return -1;

	DstLen = SrcLen;

	UKLen = 16;
	//IVLen = 16;
#ifdef _DEBUG
	int t = 0x10;
	for (int i=0; i<16; i++)
	{
		UserKey[i] = t+i;
	}
#else
	snprintf ((char*)UserKey, sizeof(UserKey)-1, "%s", encryptKey);
#endif

	AES_SetAlgInfo(AES_ModeType, AES_PadType, IV, &AlgInfo);

	//Decryption
	//if( ModeType==AI_ECB || ModeType==AI_CBC )
	ret = AES_DecKeySchedule(UserKey, UKLen, &AlgInfo);
	//else if( ModeType==AI_OFB || ModeType==AI_CFB )
	//	ret = AES_EncKeySchedule(UserKey, UKLen, &AlgInfo);

	if( ret!=CTR_SUCCESS )	
	{
		free_malloc (pOut);
		return -1;
	}

	ret = AES_DecInit(&AlgInfo);
	if( ret!=CTR_SUCCESS )	
	{
		free_malloc (pOut);
		return -1;
	}

	ret = AES_DecUpdate(&AlgInfo, (unsigned char*)srcString, SrcLen, (unsigned char*)pOut, &DstLen);
	if( ret!=CTR_SUCCESS )	
	{
		free_malloc (pOut);
		return -1;
	}
	ddlen = DstLen;

	ret = AES_DecFinal(&AlgInfo, (unsigned char*)pOut+ddlen, &DstLen);
	if( ret!=CTR_SUCCESS )	
	{
		free_malloc (pOut);
		return -1;
	}
	ddlen += DstLen;
	*dstLen = ddlen;
	*dstString = pOut;
	return 0;

}


