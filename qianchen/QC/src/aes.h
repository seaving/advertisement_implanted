#ifndef __AES_H__
#define __AES_H__
  
/*************** Header files *********************************************/  
#include <stdlib.h>
#include <string.h>
#include <memory.h>

#define AES_ModeType  AI_ECB  
#define AES_PadType   AI_PKCS_PADDING  

/*************** Assertions ***********************************************/  
////////    Define the Endianness   ////////  
#undef BIG_ENDIAN
#undef LITTLE_ENDIAN

#define USER_LITTLE_ENDIAN

#if defined(USER_BIG_ENDIAN)
    #define BIG_ENDIAN
#elif defined(USER_LITTLE_ENDIAN) 
    #define LITTLE_ENDIAN
#else
    #if 0
        #define BIG_ENDIAN      //  Big-Endian machine with pointer casting
    #elif defined(_MSC_VER)
        #define LITTLE_ENDIAN   //  Little-Endian machine with pointer casting
    #else
        #error
    #endif
#endif

/*************** Macros ***************************************************/
////////    rotate by using shift operations    ////////
#if defined(_MSC_VER)
    #define ROTL_DWORD(x, n) _lrotl((x), (n))
    #define ROTR_DWORD(x, n) _lrotr((x), (n)) 
#else
    #define ROTL_DWORD(x, n) ((DWORD)((x) << (n)) | (DWORD)((x) >> (32-(n))))
    #define ROTR_DWORD(x, n) ((DWORD)((x) >> (n)) | (DWORD)((x) << (32-(n))))
#endif

////////    reverse the byte order of DWORD(DWORD:4-bytes integer) and WORD.
#define ENDIAN_REVERSE_DWORD(dwS)   ( (ROTL_DWORD((dwS),  8) & 0x00ff00ff) | (ROTL_DWORD((dwS), 24) & 0xff00ff00) ) 

////////    move DWORD type to BYTE type and BYTE type to DWORD type
#if defined(BIG_ENDIAN)     ////    Big-Endian machine
    #define BIG_B2D(B, D)       D = *(DWORD *)(B)
    #define BIG_D2B(D, B)       *(DWORD *)(B) = (DWORD)(D)
    #define LITTLE_B2D(B, D)    D = ENDIAN_REVERSE_DWORD(*(DWORD *)(B))
    #define LITTLE_D2B(D, B)    *(DWORD *)(B) = ENDIAN_REVERSE_DWORD(D)
#elif defined(LITTLE_ENDIAN)    ////    Little-Endian machine
    #define BIG_B2D(B, D)       D = ENDIAN_REVERSE_DWORD(*(DWORD *)(B))
    #define BIG_D2B(D, B)       *(DWORD *)(B) = ENDIAN_REVERSE_DWORD(D)
    #define LITTLE_B2D(B, D)    D = *(DWORD *)(B)
    #define LITTLE_D2B(D, B)    *(DWORD *)(B) = (DWORD)(D)
#else
    #error ERROR : Invalid DataChangeType
#endif

/*************** Definitions / Macros *************************************/    
#define AI_ECB                  1  
#define AI_CBC                  2  
#define AI_OFB                  3  
#define AI_CFB                  4

#define AI_NO_PADDING           1
#define AI_PKCS_PADDING         2

#define AES_BLOCK_LEN           16      //  in BYTEs
#define AES_USER_KEY_LEN        32      //  (16,24,32) in BYTEs
#define AES_NO_ROUNDS           10
#define AES_NO_ROUNDKEY         68      //  in DWORDs

/*************** New Data Types *******************************************/
////////    Determine data types depand on the processor and compiler.
#define BOOL    int                 //  1-bit data type
#define BYTE    unsigned char       //  unsigned 1-byte data type
#define WORD    unsigned short int  //  unsigned 2-bytes data type
#define DWORD   unsigned int        //  unsigned 4-bytes data type
#define RET_VAL     DWORD           //  return values

typedef struct{
    DWORD       ModeID;
    DWORD       PadType;
    BYTE        IV[AES_BLOCK_LEN];
    BYTE        ChainVar[AES_BLOCK_LEN];
    BYTE        Buffer[AES_BLOCK_LEN];
    DWORD       BufLen;
    DWORD       RoundKey[AES_NO_ROUNDKEY];
} AES_ALG_INFO;

/*************** Constant (Error Code) ************************************/
#define CTR_SUCCESS                 0
#define CTR_FATAL_ERROR             0x1001
#define CTR_INVALID_USERKEYLEN      0x1002
#define CTR_PAD_CHECK_ERROR         0x1003
#define CTR_DATA_LEN_ERROR          0x1004
#define CTR_CIPHER_LEN_ERROR        0x1005

#ifdef __cplusplus
extern "C" {
#endif

/*************** Prototypes ***********************************************/
void    AES_SetAlgInfo(
        DWORD           ModeID,
        DWORD           PadType,
        BYTE            *IV,
        AES_ALG_INFO    *AlgInfo);

RET_VAL AES_EncKeySchedule(
        BYTE            *UserKey,
        DWORD           UserKeyLen,
        AES_ALG_INFO    *AlgInfo);
RET_VAL AES_DecKeySchedule(
        BYTE            *UserKey,
        DWORD           UserKeyLen,
        AES_ALG_INFO    *AlgInfo);

RET_VAL AES_EncInit(
        AES_ALG_INFO    *AlgInfo);
RET_VAL AES_EncUpdate(
        AES_ALG_INFO    *AlgInfo,
        BYTE            *PlainTxt,
        DWORD           PlainTxtLen,
        BYTE            *CipherTxt,
        DWORD           *CipherTxtLen);
RET_VAL AES_EncFinal(
        AES_ALG_INFO    *AlgInfo,
        BYTE            *CipherTxt,
        DWORD           *CipherTxtLen);

RET_VAL AES_DecInit(
        AES_ALG_INFO    *AlgInfo);
RET_VAL AES_DecUpdate(
        AES_ALG_INFO    *AlgInfo,
        BYTE            *CipherTxt,
        DWORD           CipherTxtLen,
        BYTE            *PlainTxt,
        DWORD           *PlainTxtLen);
RET_VAL AES_DecFinal(
        AES_ALG_INFO    *AlgInfo,
        BYTE            *PlainTxt,
        DWORD           *PlainTxtLen);

/*************** END OF FILE **********************************************/

#ifdef __cplusplus
}
#endif

#endif  //  _AES_H

