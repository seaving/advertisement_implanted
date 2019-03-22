#include "includes.h"


//two ways to use urldecode:
// urldecode stuff
// echo stuff | urldecode 

int url_decode(const char *input, char *output)
{

	char *outputtmp = output;
	
	while (*input)
	{
		if (*input == '%')
		{
			char buffer[3] = { input[1], input[2], 0 };
			*outputtmp++ = strtol(buffer, NULL, 16);
			input += 3;
		}
		else
		{
			*outputtmp++ = *input++;
		}
	}

	*outputtmp = 0; //null terminate
	return outputtmp - output;
}

