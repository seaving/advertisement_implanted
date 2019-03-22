
/*************** Header files *********************************************/
#include "aes.h"

/*************** Assertions ***********************************************/

/*************** New Data Types *******************************************/
typedef struct {
	DWORD	k_len;
	DWORD	RK[64];
} RIJNDAEL_CIPHER_KEY;

/*************** Definitions / Macros  ************************************/
#define u1byte	BYTE
#define u4byte	DWORD
#define rotl	ROTL_DWORD
#define rotr	ROTR_DWORD
#define byte(x,n)	((u1byte)((x) >> (8 * n)))

#define LARGE_TABLES

#define ff_mult(a,b)	(a && b ? pow_tab[(log_tab[a] + log_tab[b]) % 255] : 0)

#ifdef LARGE_TABLES
	#define ls_box(x)				 \
		( fl_tab[0][byte(x, 0)] ^	 \
		  fl_tab[1][byte(x, 1)] ^	 \
		  fl_tab[2][byte(x, 2)] ^	 \
		  fl_tab[3][byte(x, 3)] )
#else
	#define ls_box(x)							 \
		((u4byte)sbx_tab[byte(x, 0)] <<  0) ^	 \
		((u4byte)sbx_tab[byte(x, 1)] <<  8) ^	 \
		((u4byte)sbx_tab[byte(x, 2)] << 16) ^	 \
		((u4byte)sbx_tab[byte(x, 3)] << 24)
#endif

/*************** Global Variables *****************************************/
static u1byte	log_tab[256];
static u1byte	pow_tab[256];
static u1byte	sbx_tab[256];
static u1byte	isb_tab[256];
static u4byte	rco_tab[ 10];
static u4byte	ft_tab[4][256];
static u4byte	it_tab[4][256];

#ifdef	LARGE_TABLES
  static u4byte  fl_tab[4][256];
  static u4byte  il_tab[4][256];
#endif

static u4byte	tab_gen = 0;


/*************** Prototypes ***********************************************/
static void gen_tabs(void)
{
	u4byte	i, t;
	u1byte	p, q;

	/* log and power tables for GF(2**8) finite field with	*/
	/* 0x11b as modular polynomial - the simplest prmitive	*/
	/* root is 0x11, used here to generate the tables		*/

	log_tab[7] = 0;
	for(i = 0,p = 1; i < 256; ++i)
	{
		pow_tab[i] = (BYTE)p;
		log_tab[p] = (BYTE)i;

		p = (BYTE)(p ^ (p << 1) ^ (p & 0x80 ? 0x01b : 0));
	}

	log_tab[1] = 0;
	p = 1;

	for(i = 0; i < 10; ++i)
	{
		rco_tab[i] = p; 

		p = (BYTE)((p << 1) ^ (p & 0x80 ? 0x1b : 0));
	}

	/* note that the affine byte transformation matrix in	*/
	/* rijndael specification is in big endian format with	*/
	/* bit 0 as the most significant bit. In the remainder	*/
	/* of the specification the bits are numbered from the	*/
	/* least significant end of a byte. 					*/

	for(i = 0; i < 256; ++i)
	{	
		p = (BYTE)(i ? pow_tab[255 - log_tab[i]] : 0);
		q = p;
		q = (BYTE)((q >> 7) | (q << 1));
		p ^= q;
		q = (BYTE)((q >> 7) | (q << 1));
		p ^= q;
		q = (BYTE)((q >> 7) | (q << 1));
		p ^= q;
		q = (BYTE)((q >> 7) | (q << 1));
		p ^= q ^ 0x63;
		sbx_tab[i] = (u1byte)p;
		isb_tab[p] = (u1byte)i;
	}

	for(i = 0; i < 256; ++i)
	{
		p = sbx_tab[i]; 

#ifdef	LARGE_TABLES
		t = p;
		fl_tab[0][i] = t;
		fl_tab[1][i] = rotl(t,  8);
		fl_tab[2][i] = rotl(t, 16);
		fl_tab[3][i] = rotl(t, 24);
#endif
		t = ((u4byte)ff_mult(2, p)) |
			((u4byte)p <<  8) |
			((u4byte)p << 16) |
			((u4byte)ff_mult(3, p) << 24);
		
		ft_tab[0][i] = t;
		ft_tab[1][i] = rotl(t,	8);
		ft_tab[2][i] = rotl(t, 16);
		ft_tab[3][i] = rotl(t, 24);

		p = isb_tab[i]; 

#ifdef	LARGE_TABLES
		t = p; il_tab[0][i] = t; 
		il_tab[1][i] = rotl(t,	8); 
		il_tab[2][i] = rotl(t, 16); 
		il_tab[3][i] = rotl(t, 24);
#endif 
		t = ((u4byte)ff_mult(14, p)) |
			((u4byte)ff_mult( 9, p) <<	8) |
			((u4byte)ff_mult(13, p) << 16) |
			((u4byte)ff_mult(11, p) << 24);
		
		it_tab[0][i] = t; 
		it_tab[1][i] = rotl(t,	8); 
		it_tab[2][i] = rotl(t, 16); 
		it_tab[3][i] = rotl(t, 24); 
	}

	tab_gen = 1;
};

#define star_x(x) (((x) & 0x7f7f7f7f) << 1) ^ ((((x) & 0x80808080) >> 7) * 0x1b)

#define imix_col(y,x)		\
	 u	= star_x(x);		\
	 v	= star_x(u);		\
	 w	= star_x(v);		\
	 t	= w ^ (x);			\
	(y) = u ^ v ^ w;		\
	(y) ^= rotr(u ^ t,  8) ^ \
		  rotr(v ^ t, 16) ^ \
		  rotr(t,24)

/**************************************************************************
*
*	Function Description ...
*	
*	Return values:
*		- CTR_SUCCESS						함수가 성공적으로 수행됨.
*		...
*/
static void RIJNDAEL_KeySchedule(
		BYTE		*UserKey,		//	사용자 비밀키 입력
		DWORD		k_len,			//	사용자 비밀키의 DWORD 수
		DWORD		*e_key)			//	암호용 Round Key 생성/출력
{
	u4byte	i, t;

	////
	if(!tab_gen)
		gen_tabs();

	LITTLE_B2D(&(UserKey[ 0]), e_key[0]);
	LITTLE_B2D(&(UserKey[ 4]), e_key[1]);
	LITTLE_B2D(&(UserKey[ 8]), e_key[2]);
	LITTLE_B2D(&(UserKey[12]), e_key[3]);

	switch(k_len)
	{
		case 4:
				t = e_key[3];
				for(i = 0; i < 10; ++i) {
					t = ls_box(rotr(t,  8)) ^ rco_tab[i];
					t ^= e_key[4 * i];     e_key[4 * i + 4] = t;
					t ^= e_key[4 * i + 1]; e_key[4 * i + 5] = t;
					t ^= e_key[4 * i + 2]; e_key[4 * i + 6] = t;
					t ^= e_key[4 * i + 3]; e_key[4 * i + 7] = t;
				}
				break;

		case 6:
				LITTLE_B2D(&(UserKey[16]), e_key[4]);
				LITTLE_B2D(&(UserKey[20]), e_key[5]);
				t = e_key[5];
				for(i = 0; i < 8; ++i) {
					t = ls_box(rotr(t,	8)) ^ rco_tab[i];
					t ^= e_key[6 * i];	   e_key[6 * i + 6] = t;
					t ^= e_key[6 * i + 1]; e_key[6 * i + 7] = t;
					t ^= e_key[6 * i + 2]; e_key[6 * i + 8] = t;
					t ^= e_key[6 * i + 3]; e_key[6 * i + 9] = t;
					t ^= e_key[6 * i + 4]; e_key[6 * i + 10] = t;
					t ^= e_key[6 * i + 5]; e_key[6 * i + 11] = t;
				}
//					loop6(i);
				break;

		case 8:
				LITTLE_B2D(&(UserKey[16]), e_key[4]);
				LITTLE_B2D(&(UserKey[20]), e_key[5]);
				LITTLE_B2D(&(UserKey[24]), e_key[6]);
				LITTLE_B2D(&(UserKey[28]), e_key[7]);
				t = e_key[7];
				for(i = 0; i < 7; ++i) {
					t = ls_box(rotr(t,	8)) ^ rco_tab[i];
					t ^= e_key[8 * i];	   e_key[8 * i + 8] = t;
					t ^= e_key[8 * i + 1]; e_key[8 * i + 9] = t;
					t ^= e_key[8 * i + 2]; e_key[8 * i + 10] = t;
					t ^= e_key[8 * i + 3]; e_key[8 * i + 11] = t;
					t  = e_key[8 * i + 4] ^ ls_box(t);
					e_key[8 * i + 12] = t;
					t ^= e_key[8 * i + 5]; e_key[8 * i + 13] = t;
					t ^= e_key[8 * i + 6]; e_key[8 * i + 14] = t;
					t ^= e_key[8 * i + 7]; e_key[8 * i + 15] = t;
				}
//					loop8(i);
				break;
	}
}

/*************** Function *************************************************
* 
*/
RET_VAL AES_EncKeySchedule(
		BYTE		*UserKey,		//	사용자 비밀키 입력
		DWORD		UserKeyLen,		//	사용자 비밀키의 바이트 수
		AES_ALG_INFO	*AlgInfo)	//	암호용/복호용 Round Key 생성/저장
{
	RIJNDAEL_CIPHER_KEY	*RK_Struct=(RIJNDAEL_CIPHER_KEY *) AlgInfo->RoundKey;
	DWORD	*e_key=RK_Struct->RK;	//	64 DWORDs
	DWORD	k_len;

	//	UserKey의 길이가 부적절한 경우 error 처리
	if( (UserKeyLen!=16) && (UserKeyLen!=24) && (UserKeyLen!=32) )
		return CTR_INVALID_USERKEYLEN;

	////
	k_len = (UserKeyLen + 3) / 4;
	RK_Struct->k_len = k_len;

	RIJNDAEL_KeySchedule(UserKey, k_len, e_key);

	return CTR_SUCCESS;
}

/*************** Function *************************************************
* 
*/
RET_VAL AES_DecKeySchedule(
		BYTE		*UserKey,		//	사용자 비밀키 입력
		DWORD		UserKeyLen,		//	사용자 비밀키의 바이트 수
		AES_ALG_INFO	*AlgInfo)	//	암호용/복호용 Round Key 생성/저장
{
	RIJNDAEL_CIPHER_KEY	*RK_Struct=(RIJNDAEL_CIPHER_KEY *) AlgInfo->RoundKey;
	DWORD	*d_key=RK_Struct->RK;	//	64 DWORDs
	DWORD	k_len, t_key[64];
	u4byte	i, t, u, v, w;

	//	UserKey의 길이가 부적절한 경우 error 처리
	if( (UserKeyLen!=16) && (UserKeyLen!=24) && (UserKeyLen!=32) )
		return CTR_INVALID_USERKEYLEN;

	////
	k_len = (UserKeyLen + 3) / 4;
	RK_Struct->k_len = k_len;

	RIJNDAEL_KeySchedule(UserKey, k_len, t_key);

	d_key[0] = t_key[4 * k_len + 24];
	d_key[1] = t_key[4 * k_len + 25];
	d_key[2] = t_key[4 * k_len + 26];
	d_key[3] = t_key[4 * k_len + 27];

	for( i=4; i<4*(k_len+6); i+=4) {
		imix_col(d_key[i+0], t_key[4*k_len+24-i+0]);
		imix_col(d_key[i+1], t_key[4*k_len+24-i+1]);
		imix_col(d_key[i+2], t_key[4*k_len+24-i+2]);
		imix_col(d_key[i+3], t_key[4*k_len+24-i+3]);
	}
	d_key[i+0] = t_key[4*k_len+24-i+0];
	d_key[i+1] = t_key[4*k_len+24-i+1];
	d_key[i+2] = t_key[4*k_len+24-i+2];
	d_key[i+3] = t_key[4*k_len+24-i+3];

	return CTR_SUCCESS;
}

/*
	DWORD	A, B, C, D, T0, T1, *K=AlgInfo->RoundKey;

	////
	if( UserKeyLen!=SEED_USER_KEY_LEN )
		return CTR_INVALID_USERKEYLEN;

	////
	BIG_B2D( &(UserKey[0]), A);
	BIG_B2D( &(UserKey[4]), B);
	BIG_B2D( &(UserKey[8]), C);
	BIG_B2D( &(UserKey[12]), D);

	T0 = A + C - KC0;
	T1 = B - D + KC0;
	K[0] = SEED_SL[0][(T0    )&0xFF] ^ SEED_SL[1][(T0>> 8)&0xFF]
		 ^ SEED_SL[2][(T0>>16)&0xFF] ^ SEED_SL[3][(T0>>24)&0xFF];
	K[1] = SEED_SL[0][(T1    )&0xFF] ^ SEED_SL[1][(T1>> 8)&0xFF]
		 ^ SEED_SL[2][(T1>>16)&0xFF] ^ SEED_SL[3][(T1>>24)&0xFF];;

	EncRoundKeyUpdate0(K+ 2, A, B, C, D, KC1 );
	EncRoundKeyUpdate1(K+ 4, A, B, C, D, KC2 );
	EncRoundKeyUpdate0(K+ 6, A, B, C, D, KC3 );
	EncRoundKeyUpdate1(K+ 8, A, B, C, D, KC4 );
	EncRoundKeyUpdate0(K+10, A, B, C, D, KC5 );
	EncRoundKeyUpdate1(K+12, A, B, C, D, KC6 );
	EncRoundKeyUpdate0(K+14, A, B, C, D, KC7 );
	EncRoundKeyUpdate1(K+16, A, B, C, D, KC8 );
	EncRoundKeyUpdate0(K+18, A, B, C, D, KC9 );
	EncRoundKeyUpdate1(K+20, A, B, C, D, KC10);
	EncRoundKeyUpdate0(K+22, A, B, C, D, KC11);
	EncRoundKeyUpdate1(K+24, A, B, C, D, KC12);
	EncRoundKeyUpdate0(K+26, A, B, C, D, KC13);
	EncRoundKeyUpdate1(K+28, A, B, C, D, KC14);
	EncRoundKeyUpdate0(K+30, A, B, C, D, KC15);

	//	Remove sensitive data
	A = B = C = D = T0 = T1 = 0;
	K = NULL;

	//
	return CTR_SUCCESS;
*/

/*************** Macros ***************************************************/
#define f_nround(bo, bi, k) {				\
	bo[0] = ft_tab[0][byte(bi[0],0)]		\
		  ^ ft_tab[1][byte(bi[1],1)]		\
		  ^ ft_tab[2][byte(bi[2],2)]		\
		  ^ ft_tab[3][byte(bi[3],3)] ^ k[0];\
	bo[1] = ft_tab[0][byte(bi[1],0)]		\
		  ^ ft_tab[1][byte(bi[2],1)]		\
		  ^ ft_tab[2][byte(bi[3],2)]		\
		  ^ ft_tab[3][byte(bi[0],3)] ^ k[1];\
	bo[2] = ft_tab[0][byte(bi[2],0)]		\
		  ^ ft_tab[1][byte(bi[3],1)]		\
		  ^ ft_tab[2][byte(bi[0],2)]		\
		  ^ ft_tab[3][byte(bi[1],3)] ^ k[2];\
	bo[3] = ft_tab[0][byte(bi[3],0)]		\
		  ^ ft_tab[1][byte(bi[0],1)]		\
		  ^ ft_tab[2][byte(bi[1],2)]		\
		  ^ ft_tab[3][byte(bi[2],3)] ^ k[3];\
	k += 4;									\
}

#define i_nround(bo, bi, k) {				\
	bo[0] = it_tab[0][byte(bi[0],0)]		\
		  ^ it_tab[1][byte(bi[3],1)]		\
		  ^ it_tab[2][byte(bi[2],2)]		\
		  ^ it_tab[3][byte(bi[1],3)] ^ k[0];\
	bo[1] = it_tab[0][byte(bi[1],0)]		\
		  ^ it_tab[1][byte(bi[0],1)]		\
		  ^ it_tab[2][byte(bi[3],2)]		\
		  ^ it_tab[3][byte(bi[2],3)] ^ k[1];\
	bo[2] = it_tab[0][byte(bi[2],0)]		\
		  ^ it_tab[1][byte(bi[1],1)]		\
		  ^ it_tab[2][byte(bi[0],2)]		\
		  ^ it_tab[3][byte(bi[3],3)] ^ k[2];\
	bo[3] = it_tab[0][byte(bi[3],0)]		\
		  ^ it_tab[1][byte(bi[2],1)]		\
		  ^ it_tab[2][byte(bi[1],2)]		\
		  ^ it_tab[3][byte(bi[0],3)] ^ k[3];\
	k += 4;					\
}

#ifdef LARGE_TABLES
	#define f_lround(bo, bi, k) {				\
		bo[0] = fl_tab[0][byte(bi[0],0)]		\
			  ^ fl_tab[1][byte(bi[1],1)]		\
			  ^ fl_tab[2][byte(bi[2],2)]		\
			  ^ fl_tab[3][byte(bi[3],3)] ^ k[0];\
		bo[1] = fl_tab[0][byte(bi[1],0)]		\
			  ^ fl_tab[1][byte(bi[2],1)]		\
			  ^ fl_tab[2][byte(bi[3],2)]		\
			  ^ fl_tab[3][byte(bi[0],3)] ^ k[1];\
		bo[2] = fl_tab[0][byte(bi[2],0)]		\
			  ^ fl_tab[1][byte(bi[3],1)]		\
			  ^ fl_tab[2][byte(bi[0],2)]		\
			  ^ fl_tab[3][byte(bi[1],3)] ^ k[2];\
		bo[3] = fl_tab[0][byte(bi[3],0)]		\
			  ^ fl_tab[1][byte(bi[0],1)]		\
			  ^ fl_tab[2][byte(bi[1],2)]		\
			  ^ fl_tab[3][byte(bi[2],3)] ^ k[3];\
	}

	#define i_lround(bo, bi, k) {				\
		bo[0] = il_tab[0][byte(bi[0],0)]		\
			  ^ il_tab[1][byte(bi[3],1)]		\
			  ^ il_tab[2][byte(bi[2],2)]		\
			  ^ il_tab[3][byte(bi[1],3)] ^ k[0];\
		bo[1] = il_tab[0][byte(bi[1],0)]		\
			  ^ il_tab[1][byte(bi[0],1)]		\
			  ^ il_tab[2][byte(bi[3],2)]		\
			  ^ il_tab[3][byte(bi[2],3)] ^ k[1];\
		bo[2] = il_tab[0][byte(bi[2],0)]		\
			  ^ il_tab[1][byte(bi[1],1)]		\
			  ^ il_tab[2][byte(bi[0],2)]		\
			  ^ il_tab[3][byte(bi[3],3)] ^ k[2];\
		bo[3] = il_tab[0][byte(bi[3],0)]		\
			  ^ il_tab[1][byte(bi[2],1)]		\
			  ^ il_tab[2][byte(bi[1],2)]		\
			  ^ il_tab[3][byte(bi[0],3)] ^ k[3];\
		}
#else
	#define f_rl(bo, bi, n, k)										\
		bo[n] = (u4byte)sbx_tab[byte(bi[n],0)] ^					\
			rotl(((u4byte)sbx_tab[byte(bi[(n + 1) & 3],1)]),  8) ^	\
			rotl(((u4byte)sbx_tab[byte(bi[(n + 2) & 3],2)]), 16) ^	\
			rotl(((u4byte)sbx_tab[byte(bi[(n + 3) & 3],3)]), 24) ^ *(k + n)

	#define i_rl(bo, bi, n, k)										\
		bo[n] = (u4byte)isb_tab[byte(bi[n],0)] ^					\
			rotl(((u4byte)isb_tab[byte(bi[(n + 3) & 3],1)]),  8) ^	\
			rotl(((u4byte)isb_tab[byte(bi[(n + 2) & 3],2)]), 16) ^	\
			rotl(((u4byte)isb_tab[byte(bi[(n + 1) & 3],3)]), 24) ^ *(k + n)

	#define f_lround(bo, bi, k) \
		f_rl(bo, bi, 0, k); 	\
		f_rl(bo, bi, 1, k); 	\
		f_rl(bo, bi, 2, k); 	\
		f_rl(bo, bi, 3, k)

	#define i_lround(bo, bi, k) \
		i_rl(bo, bi, 0, k); 	\
		i_rl(bo, bi, 1, k); 	\
		i_rl(bo, bi, 2, k); 	\
		i_rl(bo, bi, 3, k)
#endif

/*************** Function *************************************************
*	
*/
void	AES_Encrypt(
		void		*CipherKey,		//	암/복호용 Round Key
		BYTE		*Data)			//	입출력을 위한 블록을 가리키는 pointer
{
	RIJNDAEL_CIPHER_KEY	*RK_Struct=CipherKey;
	DWORD	*e_key=RK_Struct->RK;	//	64 DWORDs
	DWORD	k_len=RK_Struct->k_len;
	u4byte	b0[4], b1[4], *kp;

	LITTLE_B2D(&(Data[ 0]), b0[0]);
	LITTLE_B2D(&(Data[ 4]), b0[1]);
	LITTLE_B2D(&(Data[ 8]), b0[2]);
	LITTLE_B2D(&(Data[12]), b0[3]);

	//
	b0[0] ^= e_key[0];
	b0[1] ^= e_key[1];
	b0[2] ^= e_key[2];
	b0[3] ^= e_key[3];

	kp = e_key + 4;

	switch( k_len ) {
		case 8 :
			f_nround(b1, b0, kp); f_nround(b0, b1, kp);
		case 6 :
			f_nround(b1, b0, kp); f_nround(b0, b1, kp);
		case 4 :
			f_nround(b1, b0, kp); f_nround(b0, b1, kp);
			f_nround(b1, b0, kp); f_nround(b0, b1, kp);
			f_nround(b1, b0, kp); f_nround(b0, b1, kp);
			f_nround(b1, b0, kp); f_nround(b0, b1, kp);
			f_nround(b1, b0, kp); f_lround(b0, b1, kp);
	}

	//
	LITTLE_D2B(b0[0], &(Data[ 0]));
	LITTLE_D2B(b0[1], &(Data[ 4]));
	LITTLE_D2B(b0[2], &(Data[ 8]));
	LITTLE_D2B(b0[3], &(Data[12]));
}

/*************** Function *************************************************
*	
*/
void	AES_Decrypt(
		void		*CipherKey,		//	암/복호용 Round Key
		BYTE		*Data)			//	입출력을 위한 블록을 가리키는 pointer
{
	RIJNDAEL_CIPHER_KEY	*RK_Struct=CipherKey;
	DWORD	*d_key=RK_Struct->RK;	//	64 DWORDs
	DWORD	k_len=RK_Struct->k_len;
	u4byte	b0[4], b1[4], *kp;

	LITTLE_B2D(&(Data[ 0]), b0[0]);
	LITTLE_B2D(&(Data[ 4]), b0[1]);
	LITTLE_B2D(&(Data[ 8]), b0[2]);
	LITTLE_B2D(&(Data[12]), b0[3]);

	//
	b0[0] ^= d_key[0];
	b0[1] ^= d_key[1];
	b0[2] ^= d_key[2];
	b0[3] ^= d_key[3];

	kp = d_key + 4;

	switch( k_len ) {
		case 8 :
			i_nround(b1, b0, kp); i_nround(b0, b1, kp);
		case 6 :
			i_nround(b1, b0, kp); i_nround(b0, b1, kp);
		case 4 :
			i_nround(b1, b0, kp); i_nround(b0, b1, kp);
			i_nround(b1, b0, kp); i_nround(b0, b1, kp);
			i_nround(b1, b0, kp); i_nround(b0, b1, kp);
			i_nround(b1, b0, kp); i_nround(b0, b1, kp);
			i_nround(b1, b0, kp); i_lround(b0, b1, kp);
	}

	//
	LITTLE_D2B(b0[0], &(Data[ 0]));
	LITTLE_D2B(b0[1], &(Data[ 4]));
	LITTLE_D2B(b0[2], &(Data[ 8]));
	LITTLE_D2B(b0[3], &(Data[12]));
}

/*************** END OF FILE **********************************************/



