#include "includes.h"


//two ways to use urlencode:
// urlencode stuff
// echo stuff | urlencode 

//note unicode support should happen upstream of this.
bool is_non_symbol(unsigned char c)
{
    int c_int = (int)c;
    return (c_int >= 48 && c_int <= 57) || (c_int >= 65 && c_int <= 90) || (c_int >= 97 && c_int <= 122);
}

int url_encode(const unsigned char *input, int in_len, unsigned char *output, int out_size)
{
	if (out_size < in_len)
	{
		return -1;
	}
	unsigned char *outputtmp = output;
	int i = 0;
    for (i = 0; i < in_len; i ++)
    {
        const unsigned char c = *input;
        if (c < 0)
        {
            input++;
        }
        else if (is_non_symbol(c))
        {
            *outputtmp++ = *input++;
        }
        else
        {
            char encoded[4] = {0};
            snprintf(encoded, 4, "%%%02x", c);

            *outputtmp++ = encoded[0];
            *outputtmp++ = encoded[1];
            *outputtmp++ = encoded[2];
            input++;
        }
    }

    *outputtmp = 0; //null term
    return outputtmp - output;
}


