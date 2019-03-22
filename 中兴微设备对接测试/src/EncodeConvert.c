#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <assert.h>
#include "EncodeConvert.h"
#include "Devfont.h"

int UTF8_to_MultiByte(const char* szUTF8, char *strbuf)
{
	int len;
	
	len = utf8_to_unicode(NULL, szUTF8);

	unsigned short wbuf[len];
	len = utf8_to_unicode(wbuf, szUTF8);
	
	char buf[len];
	len = WideCharToMultiByte((wchar*)wbuf, (len/2)-1, (mchar*)buf, len);

	if (NULL != strbuf)
	{
		memcpy(strbuf, buf, len);
		return len;
	}

	return -1;
}

int MultiByte_to_UTF8(const char* szMultiByte, int szMultiByte_len, char *strbuf, int bufsize)
{
	int len;

	len = szMultiByte_len;
	unsigned short wbuf[len+1];
	memset(wbuf, 0, (len+1)*sizeof(unsigned short));
	// wchar 类型是unsigned short.
	MultiByteToWideChar((wchar*)wbuf, len+1, (mchar*)szMultiByte, len);

	len = unicode_to_utf8(NULL, wbuf);
	char buf[len];
	len = unicode_to_utf8(buf, wbuf);

	if ((NULL != strbuf) && (bufsize >= len))
	{
		memcpy(strbuf, buf, len);
		return len;
	}
	
	return -1;
}

unsigned int utf8_to_unicode(unsigned short* outbuf, const char* inbuf)
{
	char *out;
	const char *in;
	unsigned int count=0;

	if(inbuf)
	{
		if(outbuf)
		{
			in=inbuf, out=(char*)outbuf;

			while(*in)
			{
				if((*in & 0xE0) == 0xE0)
				{
					out[1] = ((in[0] & 0x0f)<<4) | ((in[1]>>2) & 0x0f);
					out[0] = ((in[1] & 0x03)<<6) | ((in[2]) & 0x3f);

					out += 2, in += 3, count += 2;
				}
				else if((*in & 0xC0) == 0xC0)
				{
					out[1] = (in[0]>>2) & 0x07;
					out[0] = ((in[0] & 0x03)<<6) | ((in[1]) & 0x3f);

					out += 2, in += 2, count += 2;
				}
				else 
				{
					out[1] = 0;
					out[0] = in[0];

					out += 2, in ++, count += 2;
				}
			}

			out[0] = out[1] = 0; count += 2;
		}
		else
		{
			in=inbuf;

			while(*in)
			{
				if((*in & 0xE0) == 0xE0)
				{
					in += 3, count += 2;
				}
				else if((*in & 0xC0) == 0xC0)
				{
					in += 2, count += 2;
				}
				else
				{
					in ++, count += 2;
				}
			}

			count += 2;
		}
	}

	return count;
}

unsigned int unicode_to_utf8(char* outbuf,const unsigned short* inbuf)
{
	char *out;
	const unsigned short *in;
	unsigned int count=0;
 
	if(inbuf)
	{
		if(outbuf)
		{
			in=inbuf, out=outbuf;

			while(*in != 0)
			{
				if( *in<=0x7F )
				{
					out[0] = (*in & 0x7F);
					out ++, count ++;
				}
				else if((*in>=0x80) && (*in<=0x7FF))
				{
					out[0] = ((*in>>6) & 0x1F) | (0xC0);
					out[1] = ((*in) & 0x3F);
					out += 2, count += 2;
				}
				else
				{
					out[0] = ((*in>>12) & 0x0F) | (0xE0);
					out[1] = ((*in>>6) & 0x3F) | (0x80);
					out[2] = ((*in) & 0x3F) | (0x80);
					out += 3, count += 3;
				}
				in ++;
			}

			*out = 0, count ++;
		}
		else
		{
			in=inbuf;
			
			while(*in != 0)
			{
				if( *in<=0x7F )
				{
					count ++;
				}
				else if((*in>=0x80) && (*in<=0x7FF))
				{
					count += 2;
				}
				else
				{
					count += 3;
				}
				in ++;
			}

			count ++;
		}
	}
	
	return count;
}

bool IsUtf8(const char* pszBuf)
{
	int nLen = strlen(pszBuf);
	if ( !pszBuf ) return false;
	if ( pszBuf[0] == 0 ) return false;
	unsigned char* p = (unsigned char*)pszBuf;
	
	while ( 1 )
	{
		if ( nLen == 0 ) return true;
		if ( *p < 128 )
		{
			p++;
			nLen--;
		}
		else if ( *p >> 5 == 0x6 )
		{
			// 中文的Utf8编码要占3个字节.
			return false;		
		}
		else if ( *p >> 4 == 0x0E )
		{
			if ( nLen >= 3 && (p[1] >> 6 == 2) && (p[2] >> 6 == 2) ) 
			{
				p += 3;
				nLen -= 3;
			}
			else return false;
		}
		else
		{
			return false;
		}		
	}
}
