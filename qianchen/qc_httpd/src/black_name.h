#ifndef __BLACK_NAME_H__
#define __BLACK_NAME_H__

#include "common.h"

void delete_black_name(char *mac);
void add_black_name(char *name, char *mac);
int get_black_name_total();
bool is_black_name(char *mac);

json_object* get_black_name_list(int page, int limit);

#endif



