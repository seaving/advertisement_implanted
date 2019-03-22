#include "includes.h"

#define KEY_SHM_DEV_SHR    0x60

static int _dev_share_id = -1;

dev_share_t *dev_share = NULL;

int dev_share_init()
{
	dev_share = (dev_share_t *)InitSham(&_dev_share_id, sizeof(dev_share_t), KEY_SHM_DEV_SHR);
	if (dev_share == (dev_share_t *)(-1))
	{
		return -1;
	}
	
	return 0;
}



