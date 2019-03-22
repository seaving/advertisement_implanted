
#ifndef _ENCODECONVERT_H
#define _ENCODECONVERT_H

#include <stdbool.h>

/*TEXT???GB2312??????UTF-8???????eg:TEXT("???????")*/

/*UTF-8?GB2312*/
int UTF8_to_MultiByte(const char* szUTF8, char *strbuf);

/*GB2312?UTF-8*/
int MultiByte_to_UTF8(const char* szMultiByte, int szMultiByte_len, char *strbuf, int bufsize);

/*UTF-8?UNICODE*/
unsigned int utf8_to_unicode(unsigned short* outbuf, const char* inbuf);

/*UNICODE?UTF-8*/
unsigned int unicode_to_utf8(char* outbuf,const unsigned short* inbuf);

/*?ж??????????UTF-8????*/
bool IsUtf8(const char* pszBuf);

#endif

