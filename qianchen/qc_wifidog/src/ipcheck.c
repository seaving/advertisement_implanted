
#include "includes.h"


bool is_digit_str(char *str)
{
	int i;
	for (i = 0; i < strlen(str); i ++)
	{
		if (str[i] < '0' && str[i] > '9')
			return false;
	}
	return true;
}

bool is_ip_str(char *str)
{
	char ipseg0[6] = {0};
	char ipseg1[6] = {0};
	char ipseg2[6] = {0};
	char ipseg3[6] = {0};

	unsigned char i, j = 0;
	unsigned char cut[22] = {0};

	if (strlen(str) > 20 || strlen(str) < 15)
		return false;
	
	for (i = 0; i < strlen(str); i ++)
	{
		if (str[i] == '.')
			cut[j ++] = i;
	}

	if (j != 3)
		return false;

	if (cut[0] > 3 || cut[0] <= 0)
		return false;
	if (cut[1] - cut[0] - 1 > 3 || cut[1] - cut[0] - 1 <= 0)
		return false;
	if (cut[2] - cut[1] - 1 > 3 || cut[2] - cut[1] - 1 <= 0)
		return false;
	if (strlen(str) - cut[2] - 1 > 3 || strlen(str) - cut[2] - 1 <= 0)
		return false;

	memcpy(ipseg0, str, cut[0]);
	memcpy(ipseg1, str + cut[0] + 1, cut[1] - cut[0] - 1);
	memcpy(ipseg2, str + cut[1] + 1, cut[2] - cut[1] - 1);
	memcpy(ipseg3, str + cut[2] + 1, strlen(str) - cut[2] - 1);

	if (! is_digit_str(ipseg0) || ! is_digit_str(ipseg1) || ! is_digit_str(ipseg2) || ! is_digit_str(ipseg3))
		return false;

	return true;
}


