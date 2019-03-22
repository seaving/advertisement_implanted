/*************** Header files *********************************************/
#include "aes.h"

/*************** Assertions ***********************************************/

/*************** Definitions / Macros  ************************************/
#define BlockCopy(pbDst, pbSrc) {					\
	((DWORD *)(pbDst))[0] = ((DWORD *)(pbSrc))[0];	\
	((DWORD *)(pbDst))[1] = ((DWORD *)(pbSrc))[1];	\
	((DWORD *)(pbDst))[2] = ((DWORD *)(pbSrc))[2];	\
	((DWORD *)(pbDst))[3] = ((DWORD *)(pbSrc))[3];	\
}
#define BlockXor(pbDst, phSrc1, phSrc2) {			\
	((DWORD *)(pbDst))[0] = ((DWORD *)(phSrc1))[0]	\
						  ^ ((DWORD *)(phSrc2))[0];	\
	((DWORD *)(pbDst))[1] = ((DWORD *)(phSrc1))[1]	\
						  ^ ((DWORD *)(phSrc2))[1];	\
	((DWORD *)(pbDst))[2] = ((DWORD *)(phSrc1))[2]	\
						  ^ ((DWORD *)(phSrc2))[2];	\
	((DWORD *)(pbDst))[3] = ((DWORD *)(phSrc1))[3]	\
						  ^ ((DWORD *)(phSrc2))[3];	\
}

/*************** New Data Types *******************************************/

/*************** Global Variables *****************************************/

/*************** Prototypes ***********************************************/
void	AES_Encrypt(
		void		*CipherKey,		//	암/복호용 Round Key
		BYTE		*Data);			//	입출력을 위한 블록을 가리키는 pointer
void	AES_Decrypt(
		void		*CipherKey,		//	암/복호용 Round Key
		BYTE		*Data);			//	입출력을 위한 블록을 가리키는 pointer

/*************** Constants ************************************************/

/*************** Constants ************************************************/

/*************** Macros ***************************************************/

/*************** Global Variables *****************************************/

/*************** Function *************************************************
*
*/
void AES_SetAlgInfo(
		DWORD			ModeID,
		DWORD			PadType,
		BYTE			*IV,
		AES_ALG_INFO	*AlgInfo)
{
	AlgInfo->ModeID = ModeID;
	AlgInfo->PadType = PadType;

	if( IV!=NULL )
		memcpy(AlgInfo->IV, IV, AES_BLOCK_LEN);
	else
		memset(AlgInfo->IV, 0, AES_BLOCK_LEN);
}

/*************** Function *************************************************
*
*/
static RET_VAL PaddSet(
			BYTE	*pbOutBuffer,
			DWORD	dRmdLen,
			DWORD	dBlockLen,
			DWORD	dPaddingType)
{
	DWORD dPadLen;

	switch( dPaddingType ) 
	{
		case AI_NO_PADDING :
			if( dRmdLen==0 )	return 0;
			else				return CTR_DATA_LEN_ERROR;

		case AI_PKCS_PADDING :
			dPadLen = dBlockLen - dRmdLen;
			memset(pbOutBuffer+dRmdLen, (char)dPadLen, (int)dPadLen);
			return dPadLen;

		default :
			return CTR_FATAL_ERROR;
	}
}

/*************** Function *************************************************
*
*/
static RET_VAL PaddCheck(
			BYTE	*pbOutBuffer,
			DWORD	dBlockLen,
			DWORD	dPaddingType)
{
	DWORD i, dPadLen;

	switch( dPaddingType ) {
		case AI_NO_PADDING :
			return 0;			//	padding된 데이타가 0바이트임.

		case AI_PKCS_PADDING :
			dPadLen = pbOutBuffer[dBlockLen-1];
			if( ((int)dPadLen<=0) || (dPadLen>(int)dBlockLen) )
				return CTR_PAD_CHECK_ERROR;
			for( i=1; i<=dPadLen; i++)
				if( pbOutBuffer[dBlockLen-i] != dPadLen )
					return CTR_PAD_CHECK_ERROR;
			return dPadLen;

		default :
			return CTR_FATAL_ERROR;
	}
}

/**************************************************************************
*
*/
RET_VAL	AES_EncInit(
		AES_ALG_INFO	*AlgInfo)
{
	AlgInfo->BufLen = 0;
	if( AlgInfo->ModeID!=AI_ECB )
		memcpy(AlgInfo->ChainVar, AlgInfo->IV, AES_BLOCK_LEN);
	return CTR_SUCCESS;
}

/**************************************************************************
*
*/
static RET_VAL ECB_EncUpdate(
		AES_ALG_INFO	*AlgInfo,		//	
		BYTE		*PlainTxt,		//	입력되는 평문의 pointer
		DWORD		PlainTxtLen,	//	입력되는 평문의 바이트 수
		BYTE		*CipherTxt, 	//	암호문이 출력될 pointer
		DWORD		*CipherTxtLen)	//	출력되는 암호문의 바이트 수
{
	DWORD		*ScheduledKey=AlgInfo->RoundKey;
	DWORD		BlockLen=AES_BLOCK_LEN, BufLen=AlgInfo->BufLen;

	//
	*CipherTxtLen = BufLen + PlainTxtLen;

	//	No one block
	if( *CipherTxtLen<BlockLen ) 
	{
		memcpy(AlgInfo->Buffer+BufLen, PlainTxt, (int)PlainTxtLen);
		AlgInfo->BufLen += PlainTxtLen;
		*CipherTxtLen = 0;
		return CTR_SUCCESS;
	}

	//	control the case that PlainTxt and CipherTxt are the same buffer
	if( PlainTxt==CipherTxt )
		return CTR_FATAL_ERROR;

	//	first block
	memcpy(AlgInfo->Buffer+BufLen, PlainTxt, (int)(BlockLen - BufLen));
	PlainTxt += BlockLen - BufLen;
	PlainTxtLen -= BlockLen - BufLen;

	//	core part
	BlockCopy(CipherTxt, AlgInfo->Buffer);
	AES_Encrypt(ScheduledKey, CipherTxt);
	CipherTxt += BlockLen;
	while( PlainTxtLen>=BlockLen ) 
	{
		BlockCopy(CipherTxt, PlainTxt);
		AES_Encrypt(ScheduledKey, CipherTxt);
		PlainTxt += BlockLen;
		CipherTxt += BlockLen;
		PlainTxtLen -= BlockLen;
	}

	//	save remained data
	memcpy(AlgInfo->Buffer, PlainTxt, (int)PlainTxtLen);
	AlgInfo->BufLen = PlainTxtLen;
	*CipherTxtLen -= PlainTxtLen;

	//	control the case that PlainTxt and CipherTxt are the same buffer
	return CTR_SUCCESS;
}

/**************************************************************************
*
*/
static RET_VAL CBC_EncUpdate(
		AES_ALG_INFO	*AlgInfo,		//	
		BYTE		*PlainTxt,		//	입력되는 평문의 pointer
		DWORD		PlainTxtLen,	//	입력되는 평문의 바이트 수
		BYTE		*CipherTxt, 	//	암호문이 출력될 pointer
		DWORD		*CipherTxtLen)	//	출력되는 암호문의 바이트 수
{
	DWORD		*ScheduledKey=AlgInfo->RoundKey;
	DWORD		BlockLen=AES_BLOCK_LEN, BufLen=AlgInfo->BufLen;

	//
	*CipherTxtLen = BufLen + PlainTxtLen;

	//	No one block
	if( *CipherTxtLen<BlockLen ) 
	{
		memcpy(AlgInfo->Buffer+BufLen, PlainTxt, (int)PlainTxtLen);
		AlgInfo->BufLen += PlainTxtLen;
		*CipherTxtLen = 0;
		return CTR_SUCCESS;
	}

	//	control the case that PlainTxt and CipherTxt are the same buffer
	if( PlainTxt==CipherTxt )
		return CTR_FATAL_ERROR;

	//	first block
	memcpy(AlgInfo->Buffer+BufLen, PlainTxt, (int)(BlockLen - BufLen));
	PlainTxt += BlockLen - BufLen;
	PlainTxtLen -= BlockLen - BufLen;

	//	core part
	BlockXor(CipherTxt, AlgInfo->ChainVar, AlgInfo->Buffer);
	AES_Encrypt(ScheduledKey, CipherTxt);
	CipherTxt += BlockLen;
	while( PlainTxtLen>=BlockLen ) 
	{
		BlockXor(CipherTxt, CipherTxt-BlockLen, PlainTxt);
		AES_Encrypt(ScheduledKey, CipherTxt);
		PlainTxt += BlockLen;
		CipherTxt += BlockLen;
		PlainTxtLen -= BlockLen;
	}
	BlockCopy(AlgInfo->ChainVar, CipherTxt-BlockLen);

	//	save remained data
	memcpy(AlgInfo->Buffer, PlainTxt, (int)PlainTxtLen);
	AlgInfo->BufLen = PlainTxtLen;
	*CipherTxtLen -= PlainTxtLen;

	//
	return CTR_SUCCESS;
}

/**************************************************************************
*
*/
static RET_VAL OFB_EncUpdate(
		AES_ALG_INFO	*AlgInfo,		//	
		BYTE		*PlainTxt,		//	입력되는 평문의 pointer
		DWORD		PlainTxtLen,	//	입력되는 평문의 바이트 수
		BYTE		*CipherTxt, 	//	암호문이 출력될 pointer
		DWORD		*CipherTxtLen)	//	출력되는 암호문의 바이트 수
{
	DWORD		*ScheduledKey=AlgInfo->RoundKey;
	DWORD		BlockLen=AES_BLOCK_LEN;
	DWORD		BufLen=AlgInfo->BufLen;

	//	Check Output Memory Size
	*CipherTxtLen = BufLen + PlainTxtLen;

	//	No one block
	if( *CipherTxtLen<BlockLen ) 
	{
		memcpy(AlgInfo->Buffer+BufLen, PlainTxt, (int)PlainTxtLen);
		AlgInfo->BufLen += PlainTxtLen;
		*CipherTxtLen = 0;
		return CTR_SUCCESS;
	}

	//	control the case that PlainTxt and CipherTxt are the same buffer
	if( PlainTxt==CipherTxt )
		return CTR_FATAL_ERROR;

	//	first block
	memcpy(AlgInfo->Buffer+BufLen, PlainTxt, (int)(BlockLen - BufLen));
	PlainTxt += BlockLen - BufLen;
	PlainTxtLen -= BlockLen - BufLen;

	//	core part
	AES_Encrypt(ScheduledKey, AlgInfo->ChainVar);
	BlockXor(CipherTxt, AlgInfo->ChainVar, AlgInfo->Buffer);
	CipherTxt += BlockLen;
	while( PlainTxtLen>=BlockLen ) 
	{
		AES_Encrypt(ScheduledKey, AlgInfo->ChainVar);
		BlockXor(CipherTxt, AlgInfo->ChainVar, PlainTxt);
		PlainTxt += BlockLen;
		CipherTxt += BlockLen;
		PlainTxtLen -= BlockLen;
	}

	//	save remained data
	memcpy(AlgInfo->Buffer, PlainTxt, (int)PlainTxtLen);
	AlgInfo->BufLen = (AlgInfo->BufLen&0xF0000000) + PlainTxtLen;
	*CipherTxtLen -= PlainTxtLen;

	//
	return CTR_SUCCESS;
}

/**************************************************************************
*
*/
static RET_VAL CFB_EncUpdate(
		AES_ALG_INFO	*AlgInfo,		//	
		BYTE		*PlainTxt,		//	입력되는 평문의 pointer
		DWORD		PlainTxtLen,	//	입력되는 평문의 바이트 수
		BYTE		*CipherTxt, 	//	암호문이 출력될 pointer
		DWORD		*CipherTxtLen)	//	출력되는 암호문의 바이트 수
{
	DWORD		*ScheduledKey=AlgInfo->RoundKey;
	DWORD		BlockLen=AES_BLOCK_LEN;
	DWORD		BufLen=AlgInfo->BufLen;

	//	Check Output Memory Size
	*CipherTxtLen = BufLen + PlainTxtLen;

	//	No one block
	if( *CipherTxtLen<BlockLen ) 
	{
		memcpy(AlgInfo->Buffer+BufLen, PlainTxt, (int)PlainTxtLen);
		AlgInfo->BufLen += PlainTxtLen;
		*CipherTxtLen = 0;
		return CTR_SUCCESS;
	}

	//	control the case that PlainTxt and CipherTxt are the same buffer
	if( PlainTxt==CipherTxt )
		return CTR_FATAL_ERROR;

	//	first block
	memcpy(AlgInfo->Buffer+BufLen, PlainTxt, (int)(BlockLen - BufLen));
	PlainTxt += BlockLen - BufLen;
	PlainTxtLen -= BlockLen - BufLen;

	//	core part
	AES_Encrypt(ScheduledKey, AlgInfo->ChainVar);
	BlockXor(AlgInfo->ChainVar, AlgInfo->ChainVar, AlgInfo->Buffer);
	BlockCopy(CipherTxt, AlgInfo->ChainVar);
	CipherTxt += BlockLen;
	while( PlainTxtLen>=BlockLen ) 
	{
		AES_Encrypt(ScheduledKey, AlgInfo->ChainVar);
		BlockXor(AlgInfo->ChainVar, AlgInfo->ChainVar, PlainTxt);
		BlockCopy(CipherTxt, AlgInfo->ChainVar);
		PlainTxt += BlockLen;
		CipherTxt += BlockLen;
		PlainTxtLen -= BlockLen;
	}

	//	save remained data
	memcpy(AlgInfo->Buffer, PlainTxt, (int)PlainTxtLen);
	AlgInfo->BufLen = (AlgInfo->BufLen&0xF0000000) + PlainTxtLen;
	*CipherTxtLen -= PlainTxtLen;

	//
	return CTR_SUCCESS;
}

/**************************************************************************
*
*/
RET_VAL	AES_EncUpdate(
		AES_ALG_INFO	*AlgInfo,
		BYTE		*PlainTxt,		//	입력되는 평문의 pointer
		DWORD		PlainTxtLen,	//	입력되는 평문의 바이트 수
		BYTE		*CipherTxt, 	//	암호문이 출력될 pointer
		DWORD		*CipherTxtLen)	//	출력되는 암호문의 바이트 수
{
	switch( AlgInfo->ModeID ) 
	{
		case AI_ECB :	return ECB_EncUpdate(AlgInfo, PlainTxt, PlainTxtLen, CipherTxt, CipherTxtLen);
		case AI_CBC :	return CBC_EncUpdate(AlgInfo, PlainTxt, PlainTxtLen, CipherTxt, CipherTxtLen);
		case AI_OFB :	return OFB_EncUpdate(AlgInfo, PlainTxt, PlainTxtLen, CipherTxt, CipherTxtLen);
		case AI_CFB :	return CFB_EncUpdate(AlgInfo, PlainTxt, PlainTxtLen, CipherTxt, CipherTxtLen);
		default :		return CTR_FATAL_ERROR;
	}
}

/**************************************************************************
*
*/
static RET_VAL ECB_EncFinal(
		AES_ALG_INFO	*AlgInfo,		//	
		BYTE		*CipherTxt, 	//	암호문이 출력될 pointer
		DWORD		*CipherTxtLen)	//	출력되는 암호문의 바이트 수
{
	DWORD		*ScheduledKey=AlgInfo->RoundKey;
	DWORD		BlockLen=AES_BLOCK_LEN, BufLen=AlgInfo->BufLen;
	DWORD		PaddByte;

	//	Padding
	PaddByte = PaddSet(AlgInfo->Buffer, BufLen, BlockLen, AlgInfo->PadType);
	if( PaddByte>BlockLen )		return PaddByte;

	if( PaddByte==0 ) 
	{
		*CipherTxtLen = 0;
		return CTR_SUCCESS;
	}

	//	core part
	BlockCopy(CipherTxt, AlgInfo->Buffer);
	AES_Encrypt(ScheduledKey, CipherTxt);

	//
	*CipherTxtLen = BlockLen;

	//
	return CTR_SUCCESS;
}

/**************************************************************************
*
*/
static RET_VAL CBC_EncFinal(
		AES_ALG_INFO	*AlgInfo,
		BYTE		*CipherTxt, 	//	암호문이 출력될 pointer
		DWORD		*CipherTxtLen)	//	출력되는 암호문의 바이트 수
{
	DWORD		*ScheduledKey=AlgInfo->RoundKey;
	DWORD		BlockLen=AES_BLOCK_LEN, BufLen=AlgInfo->BufLen;
	DWORD		PaddByte;

	//	Padding
	PaddByte = PaddSet(AlgInfo->Buffer, BufLen, BlockLen, AlgInfo->PadType);
	if( PaddByte>BlockLen )		return PaddByte;

	if( PaddByte==0 ) 
	{
		*CipherTxtLen = 0;
		return CTR_SUCCESS;
	}

	//	core part
	BlockXor(CipherTxt, AlgInfo->Buffer, AlgInfo->ChainVar);
	AES_Encrypt(ScheduledKey, CipherTxt);
	BlockCopy(AlgInfo->ChainVar, CipherTxt);

	//
	*CipherTxtLen = BlockLen;

	//
	return CTR_SUCCESS;
}

/**************************************************************************
*
*/
static RET_VAL OFB_EncFinal(
		AES_ALG_INFO	*AlgInfo,
		BYTE		*CipherTxt, 	//	암호문이 출력될 pointer
		DWORD		*CipherTxtLen)	//	출력되는 암호문의 바이트 수
{
	DWORD		*ScheduledKey=AlgInfo->RoundKey;
	DWORD		BlockLen=AES_BLOCK_LEN;
	DWORD		BufLen=AlgInfo->BufLen;
	DWORD		i;

	//	Check Output Memory Size
	*CipherTxtLen = BlockLen;

	//	core part
	AES_Encrypt(ScheduledKey, AlgInfo->ChainVar);
	for( i=0; i<BufLen; i++)
		CipherTxt[i] = (BYTE) (AlgInfo->Buffer[i] ^ AlgInfo->ChainVar[i]);

	//
	*CipherTxtLen = BufLen;

	//
	return CTR_SUCCESS;
}

/**************************************************************************
*
*/
static RET_VAL CFB_EncFinal(
		AES_ALG_INFO	*AlgInfo,
		BYTE		*CipherTxt, 	//	암호문이 출력될 pointer
		DWORD		*CipherTxtLen)	//	출력되는 암호문의 바이트 수
{
	DWORD		*ScheduledKey=AlgInfo->RoundKey;
	DWORD		BufLen=AlgInfo->BufLen;

	//	Check Output Memory Size
	*CipherTxtLen = BufLen;

	//	core part
	AES_Encrypt(ScheduledKey, AlgInfo->ChainVar);
	BlockXor(AlgInfo->ChainVar, AlgInfo->ChainVar, AlgInfo->Buffer);
	memcpy(CipherTxt, AlgInfo->ChainVar, BufLen);

	//
	*CipherTxtLen = BufLen;

	//
	return CTR_SUCCESS;
}

/**************************************************************************
*
*/
RET_VAL	AES_EncFinal(
		AES_ALG_INFO	*AlgInfo,
		BYTE		*CipherTxt, 	//	암호문이 출력될 pointer
		DWORD		*CipherTxtLen)	//	출력되는 암호문의 바이트 수
{
	switch( AlgInfo->ModeID ) 
	{
		case AI_ECB :	return ECB_EncFinal(AlgInfo, CipherTxt, CipherTxtLen);
		case AI_CBC :	return CBC_EncFinal(AlgInfo, CipherTxt, CipherTxtLen);
		case AI_OFB :	return OFB_EncFinal(AlgInfo, CipherTxt, CipherTxtLen);
		case AI_CFB :	return CFB_EncFinal(AlgInfo, CipherTxt, CipherTxtLen);
		default :		return CTR_FATAL_ERROR;
	}
}

/**************************************************************************
*
*/
RET_VAL	AES_DecInit(AES_ALG_INFO *AlgInfo)
{
	AlgInfo->BufLen = 0;
	if( AlgInfo->ModeID!=AI_ECB )
		memcpy(AlgInfo->ChainVar, AlgInfo->IV, AES_BLOCK_LEN);
	return CTR_SUCCESS;
}

/**************************************************************************
*
*/
static RET_VAL ECB_DecUpdate(
		AES_ALG_INFO	*AlgInfo,
		BYTE		*CipherTxt, 	//	입력되는 암호문의 pointer
		DWORD		CipherTxtLen,	//	입력되는 암호문의 바이트 수
		BYTE		*PlainTxt,		//	평문이 출력될 pointer
		DWORD		*PlainTxtLen)	//	출력되는 평문의 바이트 수
{
	DWORD		*ScheduledKey=AlgInfo->RoundKey;
	DWORD		BlockLen=AES_BLOCK_LEN;
	DWORD		BufLen=AlgInfo->BufLen;

	//
	*PlainTxtLen = BufLen + CipherTxtLen;

	//	No one block
	if( BufLen+CipherTxtLen <= BlockLen ) 
	{
		memcpy(AlgInfo->Buffer+BufLen, CipherTxt, (int)CipherTxtLen);
		AlgInfo->BufLen += CipherTxtLen;
		*PlainTxtLen = 0;
		return CTR_SUCCESS;
	}

	//	control the case that CipherTxt and PlainTxt are the same buffer
	if( CipherTxt==PlainTxt )	return CTR_FATAL_ERROR;

	//	first block
	*PlainTxtLen = BufLen + CipherTxtLen;
	memcpy(AlgInfo->Buffer+BufLen, CipherTxt, (int)(BlockLen - BufLen));
	CipherTxt += BlockLen - BufLen;
	CipherTxtLen -= BlockLen - BufLen;

	//	core part
	BlockCopy(PlainTxt, AlgInfo->Buffer);
	AES_Decrypt(ScheduledKey, PlainTxt);
	PlainTxt += BlockLen;
	while( CipherTxtLen>BlockLen ) 
	{
		BlockCopy(PlainTxt, CipherTxt);
		AES_Decrypt(ScheduledKey, PlainTxt);
		CipherTxt += BlockLen;
		PlainTxt += BlockLen;
		CipherTxtLen -= BlockLen;
	}

	//	save remained data
	memcpy(AlgInfo->Buffer, CipherTxt, (int)CipherTxtLen);
	AlgInfo->BufLen = (AlgInfo->BufLen&0xF0000000) + CipherTxtLen;
	*PlainTxtLen -= CipherTxtLen;

	//
	return CTR_SUCCESS;
}

/**************************************************************************
*
*/
static RET_VAL CBC_DecUpdate(
		AES_ALG_INFO	*AlgInfo,
		BYTE		*CipherTxt, 	//	입력되는 암호문의 pointer
		DWORD		CipherTxtLen,	//	입력되는 암호문의 바이트 수
		BYTE		*PlainTxt,		//	평문이 출력될 pointer
		DWORD		*PlainTxtLen)	//	출력되는 평문의 바이트 수
{
	DWORD		*ScheduledKey=AlgInfo->RoundKey;
	DWORD		BlockLen=AES_BLOCK_LEN, BufLen=AlgInfo->BufLen;

	//	Check Output Memory Size
	*PlainTxtLen = BufLen + CipherTxtLen;

	//	No one block
	if( BufLen+CipherTxtLen <= BlockLen ) 
	{
		memcpy(AlgInfo->Buffer+BufLen, CipherTxt, (int)CipherTxtLen);
		AlgInfo->BufLen += CipherTxtLen;
		*PlainTxtLen = 0;
		return CTR_SUCCESS;
	}

	//	control the case that CipherTxt and PlainTxt are the same buffer
	if( CipherTxt==PlainTxt )	return CTR_FATAL_ERROR;

	//	first block
	*PlainTxtLen = BufLen + CipherTxtLen;
	memcpy(AlgInfo->Buffer+BufLen, CipherTxt, (int)(BlockLen - BufLen));
	CipherTxt += BlockLen - BufLen;
	CipherTxtLen -= BlockLen - BufLen;

	//	core part
	BlockCopy(PlainTxt, AlgInfo->Buffer);
	AES_Decrypt(ScheduledKey, PlainTxt);
	BlockXor(PlainTxt, PlainTxt, AlgInfo->ChainVar);
	PlainTxt += BlockLen;
	if( CipherTxtLen<=BlockLen ) 
	{
		BlockCopy(AlgInfo->ChainVar, AlgInfo->Buffer);
	}
	else 
	{
		if( CipherTxtLen>BlockLen ) 
		{
			BlockCopy(PlainTxt, CipherTxt);
			AES_Decrypt(ScheduledKey, PlainTxt);
			BlockXor(PlainTxt, PlainTxt, AlgInfo->Buffer);
			CipherTxt += BlockLen;
			PlainTxt += BlockLen;
			CipherTxtLen -= BlockLen;
		}
		while( CipherTxtLen>BlockLen ) 
		{
			BlockCopy(PlainTxt, CipherTxt);
			AES_Decrypt(ScheduledKey, PlainTxt);
			BlockXor(PlainTxt, PlainTxt, CipherTxt-BlockLen);
			CipherTxt += BlockLen;
			PlainTxt += BlockLen;
			CipherTxtLen -= BlockLen;
		}
		BlockCopy(AlgInfo->ChainVar, CipherTxt-BlockLen);
	}

	//	save remained data
	memcpy(AlgInfo->Buffer, CipherTxt, (int)CipherTxtLen);
	AlgInfo->BufLen = (AlgInfo->BufLen&0xF0000000) + CipherTxtLen;
	*PlainTxtLen -= CipherTxtLen;

	//
	return CTR_SUCCESS;
}

/**************************************************************************
*
*/
static RET_VAL OFB_DecUpdate(
		AES_ALG_INFO	*AlgInfo,
		BYTE		*CipherTxt, 	//	입력되는 암호문의 pointer
		DWORD		CipherTxtLen,	//	입력되는 암호문의 바이트 수
		BYTE		*PlainTxt,		//	평문이 출력될 pointer
		DWORD		*PlainTxtLen)	//	출력되는 평문의 바이트 수
{
	DWORD		*ScheduledKey=AlgInfo->RoundKey;
	DWORD		BlockLen=AES_BLOCK_LEN;
	DWORD		BufLen=AlgInfo->BufLen;

	//	Check Output Memory Size
	*PlainTxtLen = BufLen + CipherTxtLen;

	//	No one block
	if( BufLen+CipherTxtLen <= BlockLen )
	{
		memcpy(AlgInfo->Buffer+BufLen, CipherTxt, (int)CipherTxtLen);
		AlgInfo->BufLen += CipherTxtLen;
		*PlainTxtLen = 0;
		return CTR_SUCCESS;
	}

	//	control the case that CipherTxt and PlainTxt are the same buffer
	if( PlainTxt==CipherTxt )
		return CTR_FATAL_ERROR;

	//	first block
	*PlainTxtLen = BufLen + CipherTxtLen;
	memcpy(AlgInfo->Buffer+BufLen, CipherTxt, (int)(BlockLen - BufLen));
	CipherTxt += BlockLen - BufLen;
	CipherTxtLen -= BlockLen - BufLen;

	//	core part
	AES_Encrypt(ScheduledKey, AlgInfo->ChainVar);
	BlockXor(PlainTxt, AlgInfo->ChainVar, AlgInfo->Buffer);
	PlainTxt += BlockLen;
	while( CipherTxtLen>BlockLen ) 
	{
		AES_Encrypt(ScheduledKey, AlgInfo->ChainVar);
		BlockXor(PlainTxt, AlgInfo->ChainVar, CipherTxt);
		CipherTxt += BlockLen;
		PlainTxt += BlockLen;
		CipherTxtLen -= BlockLen;
	}

	//	save remained data
	memcpy(AlgInfo->Buffer, CipherTxt, (int)CipherTxtLen);
	AlgInfo->BufLen = (AlgInfo->BufLen&0xF0000000) + CipherTxtLen;
	*PlainTxtLen -= CipherTxtLen;

	//
	return CTR_SUCCESS;
}

/**************************************************************************
*
*/
static RET_VAL CFB_DecUpdate(
		AES_ALG_INFO	*AlgInfo,
		BYTE		*CipherTxt, 	//	입력되는 암호문의 pointer
		DWORD		CipherTxtLen,	//	입력되는 암호문의 바이트 수
		BYTE		*PlainTxt,		//	평문이 출력될 pointer
		DWORD		*PlainTxtLen)	//	출력되는 평문의 바이트 수
{
	DWORD		*ScheduledKey=AlgInfo->RoundKey;
	DWORD		BlockLen=AES_BLOCK_LEN;
	DWORD		BufLen=AlgInfo->BufLen;

	//	Check Output Memory Size
	*PlainTxtLen = BufLen + CipherTxtLen;

	//	No one block
	if( BufLen+CipherTxtLen <= BlockLen ) 
	{
		memcpy(AlgInfo->Buffer+BufLen, CipherTxt, (int)CipherTxtLen);
		AlgInfo->BufLen += CipherTxtLen;
		*PlainTxtLen = 0;
		return CTR_SUCCESS;
	}

	//	control the case that CipherTxt and PlainTxt are the same buffer
	if( PlainTxt==CipherTxt )
		return CTR_FATAL_ERROR;

	//	first block
	*PlainTxtLen = BufLen + CipherTxtLen;
	memcpy(AlgInfo->Buffer+BufLen, CipherTxt, (int)(BlockLen - BufLen));
	CipherTxt += BlockLen - BufLen;
	CipherTxtLen -= BlockLen - BufLen;

	//	core part
	AES_Encrypt(ScheduledKey, AlgInfo->ChainVar);
	BlockXor(PlainTxt, AlgInfo->ChainVar, AlgInfo->Buffer);
	BlockCopy(AlgInfo->ChainVar, AlgInfo->Buffer);
	PlainTxt += BlockLen;
	while( CipherTxtLen>BlockLen ) 
	{
		AES_Encrypt(ScheduledKey, AlgInfo->ChainVar);
		BlockXor(PlainTxt, AlgInfo->ChainVar, CipherTxt);
		BlockCopy(AlgInfo->ChainVar, CipherTxt);
		CipherTxt += BlockLen;
		PlainTxt += BlockLen;
		CipherTxtLen -= BlockLen;
	}

	//	save remained data
	memcpy(AlgInfo->Buffer, CipherTxt, (int)CipherTxtLen);
	AlgInfo->BufLen = (AlgInfo->BufLen&0xF0000000) + CipherTxtLen;
	*PlainTxtLen -= CipherTxtLen;

	//
	return CTR_SUCCESS;
}

/**************************************************************************
*
*/
RET_VAL	AES_DecUpdate(
		AES_ALG_INFO	*AlgInfo,
		BYTE		*CipherTxt, 	//	암호문이 출력될 pointer
		DWORD		CipherTxtLen,	//	출력되는 암호문의 바이트 수
		BYTE		*PlainTxt,		//	입력되는 평문의 pointer
		DWORD		*PlainTxtLen)	//	입력되는 평문의 바이트 수
{
	switch( AlgInfo->ModeID ) 
	{
		case AI_ECB :	return ECB_DecUpdate(AlgInfo, CipherTxt, CipherTxtLen, PlainTxt, PlainTxtLen);
		case AI_CBC :	return CBC_DecUpdate(AlgInfo, CipherTxt, CipherTxtLen, PlainTxt, PlainTxtLen);
		case AI_OFB :	return OFB_DecUpdate(AlgInfo, CipherTxt, CipherTxtLen, PlainTxt, PlainTxtLen);
		case AI_CFB :	return CFB_DecUpdate(AlgInfo, CipherTxt, CipherTxtLen, PlainTxt, PlainTxtLen);
		default :		return CTR_FATAL_ERROR;
	}
}

/**************************************************************************
*
*/
RET_VAL ECB_DecFinal(
		AES_ALG_INFO	*AlgInfo,
		BYTE		*PlainTxt,		//	평문이 출력될 pointer
		DWORD		*PlainTxtLen)	//	출력되는 평문의 바이트 수
{
	DWORD		*ScheduledKey=AlgInfo->RoundKey;
	DWORD		BlockLen=AES_BLOCK_LEN, BufLen=AlgInfo->BufLen;
	RET_VAL		ret;

	//	Check Output Memory Size
	if( BufLen==0 ) 
	{
		*PlainTxtLen = 0;
		return CTR_SUCCESS;
	}
	*PlainTxtLen = BlockLen;

	if( BufLen!=BlockLen )	return CTR_CIPHER_LEN_ERROR;

	//	core part
	BlockCopy(PlainTxt, AlgInfo->Buffer);
	AES_Decrypt(ScheduledKey, PlainTxt);

	//	Padding Check
	ret = PaddCheck(PlainTxt, BlockLen, AlgInfo->PadType);
	if( ret==(DWORD)-3 )	return CTR_PAD_CHECK_ERROR;
	if( ret==(DWORD)-1 )	return CTR_FATAL_ERROR;

	*PlainTxtLen = BlockLen - ret;

	//
	return CTR_SUCCESS;
}

/**************************************************************************
*
*/
RET_VAL CBC_DecFinal(
		AES_ALG_INFO	*AlgInfo,
		BYTE		*PlainTxt,		//	평문이 출력될 pointer
		DWORD		*PlainTxtLen)	//	출력되는 평문의 바이트 수
{
	DWORD		*ScheduledKey=AlgInfo->RoundKey;
	DWORD		BlockLen=AES_BLOCK_LEN, BufLen=AlgInfo->BufLen;
	RET_VAL		ret;

	//	Check Output Memory Size
	if( BufLen==0 ) 
	{
		*PlainTxtLen = 0;
		return CTR_SUCCESS;
	}
	*PlainTxtLen = BlockLen;

	if( BufLen!=BlockLen )	return CTR_CIPHER_LEN_ERROR;

	//	core part
	BlockCopy(PlainTxt, AlgInfo->Buffer);
	AES_Decrypt(ScheduledKey, PlainTxt);
	BlockXor(PlainTxt, PlainTxt, AlgInfo->ChainVar);
	BlockCopy(AlgInfo->ChainVar, AlgInfo->Buffer);

	//	Padding Check
	ret = PaddCheck(PlainTxt, BlockLen, AlgInfo->PadType);
	if( ret==(DWORD)-3 )	return CTR_PAD_CHECK_ERROR;
	if( ret==(DWORD)-1 )	return CTR_FATAL_ERROR;

	*PlainTxtLen = BlockLen - ret;

	//
	return CTR_SUCCESS;
}

/**************************************************************************
*
*/
RET_VAL OFB_DecFinal(
		AES_ALG_INFO	*AlgInfo,
		BYTE		*PlainTxt,		//	평문이 출력될 pointer
		DWORD		*PlainTxtLen)	//	출력되는 평문의 바이트 수
{
	DWORD		*ScheduledKey=AlgInfo->RoundKey;
	DWORD		i, BufLen=AlgInfo->BufLen;

	//	Check Output Memory Size
	*PlainTxtLen = BufLen;

	//	core part
	AES_Encrypt(ScheduledKey, AlgInfo->ChainVar);
	for( i=0; i<BufLen; i++)
		PlainTxt[i] = (BYTE) (AlgInfo->Buffer[i] ^ AlgInfo->ChainVar[i]);

	*PlainTxtLen = BufLen;

	//
	return CTR_SUCCESS;
}


/**************************************************************************
*
*/
RET_VAL CFB_DecFinal(
		AES_ALG_INFO	*AlgInfo,
		BYTE		*PlainTxt,		//	평문이 출력될 pointer
		DWORD		*PlainTxtLen)	//	출력되는 평문의 바이트 수
{
	DWORD		*ScheduledKey=AlgInfo->RoundKey;
	DWORD		BufLen=AlgInfo->BufLen;

	//	Check Output Memory Size
	*PlainTxtLen = BufLen;

	//	core part
	AES_Encrypt(ScheduledKey, AlgInfo->ChainVar);
	BlockXor(AlgInfo->ChainVar, AlgInfo->ChainVar, AlgInfo->Buffer);
	memcpy(PlainTxt, AlgInfo->ChainVar, BufLen);

	*PlainTxtLen = BufLen;

	//
	return CTR_SUCCESS;
}

/**************************************************************************
*
*/
RET_VAL	AES_DecFinal(
		AES_ALG_INFO	*AlgInfo,
		BYTE		*PlainTxt,		//	입력되는 평문의 pointer
		DWORD		*PlainTxtLen)	//	입력되는 평문의 바이트 수
{
	switch( AlgInfo->ModeID ) 
	{
		case AI_ECB :	return ECB_DecFinal(AlgInfo, PlainTxt, PlainTxtLen);
		case AI_CBC :	return CBC_DecFinal(AlgInfo, PlainTxt, PlainTxtLen);
		case AI_OFB :	return OFB_DecFinal(AlgInfo, PlainTxt, PlainTxtLen);
		case AI_CFB :	return CFB_DecFinal(AlgInfo, PlainTxt, PlainTxtLen);
		default :		return CTR_FATAL_ERROR;
	}
}

/*************** END OF FILE **********************************************/

