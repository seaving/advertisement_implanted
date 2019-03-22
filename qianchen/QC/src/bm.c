#include "includes.h"

#define _T(c)	(('A' <= c && c <= 'Z') ? (c + 32) : (('a' <= c && c <= 'z') ? (c - 32) : c))
#define _CMP(s, c) (((('A' <= s && s <= 'Z') || ('a' <= s && s <= 'z')) && (('A' <= c && c <= 'Z') || ('a' <= c && c <= 'z'))) ? ((s == c) ? 1 : ((s == _T(c)) ? 1 : 0)) : ((s == c) ? 1 : 0))

static inline int _bm_cmp(char s, char c)
{
	return _CMP(s, c);
}

#if 1
static int MakeSkip(char *ptrn, int pLen, int *skip);
static int MakeShift(char* ptrn,int pLen, int *shift);
static int BMSearch(char *buf, int blen, char *ptrn, int plen, int *skip, int *shift);
static int BMSearch_case(char *buf, int blen, char *ptrn, int plen, int *skip, int *shift);


static int MakeSkip(char *ptrn, int pLen, int *skip)
{	
	int i;

	for (i = 0; i < 256; i ++)
	{LOG_NORMAL_INFO("MakeSkip1.\n");
		*(skip + i) = pLen;
	}

	while (pLen != 0)
	{LOG_NORMAL_INFO("MakeSkip2. pLen=%d\n", pLen);
		*(skip + (unsigned char)*ptrn ++) = pLen --;
		//unsigned char cc = *(ptrn - 1);
		//int value = pLen + 1;
		//printf("charactor is %c , position is %d , value is %d \n", cc, cc, value);
	}

	return 0;
}

static int MakeShift(char* ptrn,int pLen, int *shift)
{
	int *sptr = shift + pLen - 1;
	char *pptr = ptrn + pLen - 1;
	char c;

	c = *(ptrn + pLen - 1);

	*sptr = 1;

	pptr --;
	while (sptr -- != shift)
	{
		char *p1 = ptrn + pLen - 2, *p2, *p3;
		LOG_NORMAL_INFO("1.\n");
		do {
			while (p1 >= ptrn && *p1 -- != c)LOG_NORMAL_INFO("2.\n");
			LOG_NORMAL_INFO("3.\n");
			p2 = ptrn + pLen - 2;
			p3 = p1;
			
			while (p3 >= ptrn && *p3 -- == *p2 -- && p2 >= pptr)LOG_NORMAL_INFO("4.\n");

		} while (p3 >= ptrn && p2 >= pptr);

		*sptr = shift + pLen - sptr + p2 - p3;

		pptr --;
	}

	return 0;
}

static int BMSearch(char *buf, int blen, char *ptrn, int plen, int *skip, int *shift)
{
	int b_idx = plen;  

	if (plen <= 0)
		return -1;

	while (b_idx <= blen)
	{
		int p_idx = plen;
		int skip_stride = 0;
		int shift_stride = 0;
		while (buf[-- b_idx] == ptrn[-- p_idx])
		{			
 			if (b_idx < 0)
				return -1;
			
			if (p_idx == 0)
			{
				return b_idx;
			}
		}
		
		skip_stride = skip[(unsigned char)buf[b_idx]];
		shift_stride = shift[p_idx];
		b_idx += (skip_stride > shift_stride) ? skip_stride : shift_stride;
	}
	
	return -1;
}

int bm(char *psrc, int src_len, char *pfind, int find_len)
{
	if (find_len <= 0)
		return -1;
	if (find_len == 1)
	{
		char *p = strchr(psrc, *pfind);
		if (p)
			return p - psrc;
		return -1;
	}

	int skip[256];
	int shift[find_len];
	
	MakeSkip(pfind, find_len, skip);
	MakeShift(pfind, find_len, shift);

	int offset = BMSearch(psrc, src_len, pfind, find_len, skip, shift);
	if (offset < 0)
	{
		return -1;
	}
	
	return offset;
}

static int BMSearch_case(char *buf, int blen, char *ptrn, int plen, int *skip, int *shift)
{
	int b_idx = plen;

	LOG_NORMAL_INFO("%d - %s\n", plen, ptrn);

	if (plen <= 0)
		return -1;

	while (b_idx <= blen)
	{
		int p_idx = plen;
		int skip_stride = 0;
		int shift_stride = 0;
		LOG_NORMAL_INFO("%c %c\n", buf[b_idx - 1], ptrn[p_idx - 1]);
		while (_bm_cmp(buf[-- b_idx], ptrn[-- p_idx]))
		{
 			if (b_idx < 0)
				return -1;
			
			if (p_idx == 0)
			{
				return b_idx;
			}
		}
		
		skip_stride = skip[(unsigned char)buf[b_idx]];
		shift_stride = shift[p_idx];
		b_idx += (skip_stride > shift_stride) ? skip_stride : shift_stride;
	}
	
	return -1;
}

int bm_case(char *psrc, int src_len, char *pfind, int find_len)
{
	if (find_len <= 0)
		return -1;
	if (find_len == 1)
	{
		char *p = strchr(psrc, *pfind);
		if (p)
			return p - psrc;
		return -1;
	}
	
	int skip[256];
	int shift[find_len];
	
	MakeSkip(pfind, find_len, skip);
	MakeShift(pfind, find_len, shift);

	int offset = BMSearch_case(psrc, src_len, pfind, find_len, skip, shift);
	if (offset < 0)
	{
		return -1;
	}

	return offset;
}
#endif

int SUNDAY(char *text, int text_size, char *patt, int patt_size)
{
	int temp[256];
	int *shift = temp;
	int i;
	
	for (i = 0; i < 256; i ++)
	{
		*(shift + i) = patt_size + 1;
	}
	
	for (i = 0; i < patt_size; i ++)
	{
		*(shift + (unsigned char)(*(patt + i))) = patt_size - i;
	}
	//shift['s']=6 步,shitf['e']=5 以此类推
	int limit = text_size - patt_size + 1;
	for (i = 0; i < limit; i += shift[(unsigned char)text[i + patt_size]])
	{		
		if (text[i] == *patt)
		{
			char *match_text = text + i + 1;
			int match_size = 1;
			do
			{				
				// 输出所有匹配的位置, 但是我们只查询第一次出现的位置
				if (match_size == patt_size)
				{
					return i;
				}
				
			} while ((*match_text ++) == patt[match_size ++]);
		}
	}

	return -1;
}

int SUNDAY_CASE(char *text, int text_size, char *patt, int patt_size)
{
	int temp[256];
	int *shift = temp;
	int i;
	
	for (i = 0; i < 256; i ++)
	{
		*(shift + i) = patt_size + 1;
	}
	
	for (i = 0; i < patt_size; i ++)
	{
		*(shift + (unsigned char)(*(patt + i))) = patt_size - i;
		char ch = _T((*(patt + i)));
		if (ch != (*(patt + i)))
			*(shift + (unsigned char)(ch)) = patt_size - i;
	}
	
	//shift['s']=6 步,shitf['e']=5 以此类推
	int limit = text_size - patt_size + 1;
	for (i = 0; i < limit; i += shift[(unsigned char)text[i + patt_size]])
	{
		if (_bm_cmp(text[i], *patt))
		{
			char *match_text = text + i + 1;
			int match_size = 1;
			do
			{
				// 输出所有匹配的位置, 但是我们只查询第一次出现的位置
				if (match_size == patt_size)
				{
					return i;
				}
				
			} while (_bm_cmp((*match_text ++), patt[match_size ++]));
		}
	}

	return -1;
}

