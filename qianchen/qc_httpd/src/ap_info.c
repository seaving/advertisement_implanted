#include "includes.h"
#if 0
static hashtable_t *_ap_hash_head = NULL;

static pthread_mutex_t _ap_hash_lock = PTHREAD_MUTEX_INITIALIZER;

void ap_info_init()
{
	_ap_hash_head = NULL;
}

ap_base_info_t *create_base_node(char *ip, char *mac)
{
	if (!ip || !mac)
		return NULL;
	
	ap_base_info_t *base_info = calloc(sizeof(ap_base_info_t), 1);
	if (! base_info)
	{
		LOG_PERROR_INFO("calloc error.");
		return NULL;
	}

	base_info->ip = calloc(1, strlen(ip) + 1);
	base_info->mac = calloc(1, strlen(mac) + 1);
	if (! base_info->ip || ! base_info->mac)
	{
		if (base_info->ip) free(base_info->ip);
		if (base_info->mac) free(base_info->mac);
		free(base_info);
		return NULL;
	}
	
	memcpy(base_info->ip, ip, strlen(ip));
	memcpy(base_info->mac, mac, strlen(mac));
	
	return base_info;
}

void free_base_info_node(ap_base_info_t *base_info)
{
	if (base_info)
	{
		if (base_info->ip) free(base_info->ip);
		if (base_info->mac) free(base_info->mac);
		if (base_info->dev_name) free(base_info->dev_name);
		if (base_info->address) free(base_info->address);
		free(base_info);
	}
}

ap_t *create_ap_node(char *ip, char *mac)
{
	if (! ip || ! mac)
		return NULL;
	
	ap_t *ap = calloc(sizeof(ap_t), 1);
	if (! ap)
	{
		LOG_PERROR_INFO("calloc error.");
		return NULL;
	}
	
	ap->base_info = create_base_node(ip, mac);
	if (! ap->base_info)
	{
		free(ap);
		return NULL;
	}

	return ap;
}

void free_ap_node(void *node)
{
	ap_t *ap = (ap_t *)node;
	if (ap)
	{
		free_base_info_node(ap->base_info);
		//5G or 2G
		free(ap);
	}
}

int ap_add(char *ip, char *mac)
{
	pthread_mutex_lock(&_ap_hash_lock);
	
	char *key = ip;
	ap_t *ap = uthash_find(&_ap_hash_head, key);
	if (! ap)
	{
		ap = create_ap_node(ip, mac);
		if (! ap)
			goto err;

		if (uthash_put(&_ap_hash_head, key, ap, free_ap_node) < 0)
		{
			free_ap_node(ap);
			goto err;
		}
	}
	else
	{
		
	}
	
	pthread_mutex_unlock(&_ap_hash_lock);
	return 0;

err:
	pthread_mutex_unlock(&_ap_hash_lock);
	return -1;
}

int ap_info_update(ap_t *ap_node)
{
	pthread_mutex_lock(&_ap_hash_lock);

	if (! ap_node || ! ap_node->base_info || ! ap_node->base_info->ip || ! ap_node->base_info->mac)
		goto err;
	
	char *key = ap_node->base_info->ip;
	ap_t *ap = uthash_find(&_ap_hash_head, key);
	if (! ap)
	{
		ap = create_ap_node(ap_node->base_info->ip, ap_node->base_info->mac);
		if (! ap)
			goto err;

		if (uthash_put(&_ap_hash_head, key, ap, free_ap_node) < 0)
		{
			free_ap_node(ap);
			goto err;
		}
	}
	else
	{
		memcpy(ap, &ap_node, sizeof(ap_t));
	}
	
	pthread_mutex_unlock(&_ap_hash_lock);
	return 0;

err:
	pthread_mutex_unlock(&_ap_hash_lock);
	return -1;
}
#endif

