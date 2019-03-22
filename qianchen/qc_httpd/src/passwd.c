#include "includes.h"

int get_passwd(char *passwd)
{
	get_user_config_value("password", passwd);
	return strlen(passwd);
}

int set_passwd(char *passwd)
{
	set_user_config_value("password", passwd);
	return 0;
}




