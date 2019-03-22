#include "includes.h"

#define BLACK_NAME_FILE			"/etc/config/black_name"
#define CMD_DELETE_BLACK_NAME	"sed -i '/%s/d' " BLACK_NAME_FILE
#define CMD_ADD_BLACK_NAME		"echo \"%s %s\" >> " BLACK_NAME_FILE

#define CMD_IS_BLACK_NAME		"cat " BLACK_NAME_FILE " | grep \"%s\" | wc -l"

#define CMD_GET_BLACK_NAME_TOTAL	"cat " BLACK_NAME_FILE " | wc -l"
#define CMD_GET_BLACK_NAME_LIST		"cat " BLACK_NAME_FILE " | head -n %d | tail -n 1"

void delete_black_name(char *mac)
{
	char cmd[255] = {0};
	EXECUTE_CMD(cmd, CMD_DELETE_BLACK_NAME, mac);
}

void add_black_name(char *name, char *mac)
{
	char cmd[255] = {0};
	if (strlen(name) <= 0)
		name = "";
	EXECUTE_CMD(cmd, CMD_ADD_BLACK_NAME, name, mac);
}

void _get_black_name_call_(char *buf, void *val)
{
	if (val && buf)
	{
		memcpy((char *)val, buf, strlen(buf));
	}
}

int get_black_name(int offset, char *black_name)
{
	char cmd[255] = {0};
	snprintf(cmd, 255, CMD_GET_BLACK_NAME_LIST, offset);

	if (popen_cmd(cmd, _get_black_name_call_, black_name) < 0)
	{
		return -1;
	}
	
	return 0;
}

void _get_black_name_total_call_(char *buf, void *val)
{
	if (val && buf)
	{
		memcpy((char *)val, buf, strlen(buf));
	}
}

int get_black_name_total()
{
	char total[20] = {0};
	if (popen_cmd(CMD_GET_BLACK_NAME_TOTAL, _get_black_name_total_call_, total) < 0)
	{
		return -1;
	}

	if (isNumber(total))
	{
		return atoi(total);
	}
	
	return 0;
}

json_object* get_black_name_list(int page, int limit)
{
	json_object *json_array = json_object_new_array();
	if (! json_array)
	{
		return NULL;
	}

	int cnt = get_black_name_total();
	limit = cnt > limit ?  limit : cnt;
	if (limit <= 0)
	{
		return NULL;
	}

	int i = 0, k = 0;
	for (i = 0; i < limit; i ++)
	{
		json_object *my_object = json_object_new_object();
		if (! my_object) break;
		
		char black_name[128] = {0};
		get_black_name((page - 1) * limit + i + 1, black_name);
		if (strlen(black_name) <= 0)
		{
			break;
		}

		char *name = NULL;
		char *mac = NULL;
		name = black_name;
		int len = strlen(black_name);
		for (k = 0; k < len; k ++)
		{
			if (black_name[k] == ' ')
			{
				black_name[k ++] = 0;
				break;
			}
		}

		mac = k > len ? "" : &black_name[k];

		json_object_object_add(my_object, "name", json_object_new_string(name));
		json_object_object_add(my_object, "mac", json_object_new_string(mac));
		json_object_array_add(json_array, my_object);
	}

	return json_array;
}

void _is_black_name_call_(char *buf, void *val)
{
	if (val && buf)
	{
		memcpy((char *)val, buf, strlen(buf));
	}
}

bool is_black_name(char *mac)
{
	char cmd[255] = {0};
	snprintf(cmd, 255, CMD_IS_BLACK_NAME, mac);

	char count[15] = {0};

	if (popen_cmd(cmd, _is_black_name_call_, count) < 0)
	{
		return false;
	}

	if (isNumber(count) && atoi(count) > 0)
	{
		return true;
	}
	
	return false;
}

