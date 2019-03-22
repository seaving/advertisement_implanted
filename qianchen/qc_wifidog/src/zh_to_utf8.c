#include "includes.h"

char *utf8_encode(char *s)
{
	int i;
	int s_len = strlen(s);
	if (s_len <= 0)
		return NULL;

	int wc_len = s_len + 1;
    wchar *wc = calloc(sizeof(wchar), wc_len);
    if (! wc)
    	return NULL;
    
    // ½«ANSI±àÂëµÄ¶à×Ö½Ú×Ö·û´®×ª»»³É¿í×Ö·û×Ö·û´®
    int n = MultiByteToWideChar(wc, wc_len, (mchar*)s, s_len);
    if (n > 0)
    {
        wc[n] = 0;
        char *utf8_str = calloc(8, n + 1);
        if (! utf8_str)
        {
			free(wc);
			return NULL;
        }

        int offset = 0;
        for (i = 0; i < n; i ++)
        {
        	sprintf(utf8_str + offset, "&#x%04x;", wc[i]);
  			offset += 8;
		}
  		printf("%s\n", utf8_str);

  		free(wc);
  		return utf8_str;
    }

	free(wc);
    return NULL;
}

void utf8_encode_free(void *utf)
{
	if (utf)
		free(utf);
}

