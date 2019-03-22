#include "includes.h"


void qos_init(int forward_max_speed)
{
	qos_init_tc_model(forward_max_speed);
}


