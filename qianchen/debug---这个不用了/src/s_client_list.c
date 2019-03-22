#include "includes.h"

#define SCL_MAX_COUNT	255

static hashtable_t *_scl_hashHead = NULL;

sclist_t *sclist_create_node(char *ip, char *mac, 
			int sockfd, struct sockaddr_in *client)
{
	sclist_t *scl = calloc(sizeof(sclist_t), 1);
	if (! scl)
	{
		LOG_PERROR_INFO("calloc error.");
		goto err;
	}

	if (ip)
	{
		snprintf(scl->ip, sizeof(scl->ip), "%s", ip);
	}

	if (mac)
	{
		snprintf(scl->mac, sizeof(scl->mac), "%s", mac);
	}

	scl->sockfd = sockfd;
	memcpy(&scl->client, client, sizeof(struct sockaddr_in));

	return scl;

err:
	if (scl)
	{
		if (scl->ip) free(scl->ip);
		if (scl->mac) free(scl->mac);
		free(scl);
	}
	
	return NULL;
}

void free_scl_node(void *node)
{
	sclist_t *scl = (sclist_t *)node;
	if (scl)
	{
		if (scl->ip) free(scl->ip);
		if (scl->mac) free(scl->mac);
		free(scl);
	}
}

int scl_count()
{
	return uthash_count(&_scl_hashHead);
}

int scl_add(char *ip, char *mac, int sockfd, struct sockaddr_in *client)
{
	char *key = mac;
	sclist_t *scl = uthash_find(&_scl_hashHead, key);
	if (scl)
	{
		//已经存在该key对应的value,这里进行更新一下
		snprintf(scl->ip, sizeof(scl->ip), "%s", ip);
		scl->sockfd = sockfd;
		memcpy(&scl->client, client, sizeof(struct sockaddr_in));
	}
	else
	{
		if (scl_count() >= SCL_MAX_COUNT)
		{
			LOG_WARN_INFO("scl count is full!\n");
			return 1;
		}
	
		//不存在该MAC
		scl = sclist_create_node(ip, mac, sockfd, client);
		if (! scl)
		{
			return -1;
		}

		if (uthash_put(&_scl_hashHead, key, scl, free_scl_node) < 0)
		{
			LOG_ERROR_INFO("uthash_put failt!\n");
			free_scl_node(scl);
			return -1;
		}
	}

	LOG_NORMAL_INFO("********************************\n");
	LOG_NORMAL_INFO("* sclist count: %d *\n", scl_count());
	LOG_NORMAL_INFO("********************************\n");

	return 0;
}

sclist_t *scl_find(char *mac)
{
	return uthash_find(&_scl_hashHead, mac);
}

