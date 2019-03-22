#include "includes.h"

int uthash_put(hashtable_t **hashHead, char *ht_key, void *ht_value, _free_call_ f)
{
	hashtable_t *ht = (hashtable_t *)calloc(sizeof(hashtable_t), 1);
	if (! ht)
	{
		LOG_ERROR_INFO("calloc error.");
		return -1;
	}
	
	ht->key = calloc(1, strlen(ht_key) + 1);
	if (! ht->key)
	{
		LOG_ERROR_INFO("calloc error.");
		free(ht);
		return -1;
	}
	
	memcpy(ht->key, ht_key, strlen(ht_key));
	
	ht->value = ht_value;
	ht->f = f;

	HASH_ADD_STR((*hashHead), key, ht);

	return 0;
}

void *uthash_find(hashtable_t **hashHead, char *ht_key)
{
	hashtable_t *out = NULL;

	HASH_FIND_STR((*hashHead), ht_key, out);
	if (! out)
	{
		//LOG_NORMAL_INFO("the key(%s) is not exist in hash!\n", ht_key);
		return NULL;
	}

	return out->value;
}

int uthash_count(hashtable_t **hashHead)
{
	return HASH_COUNT((*hashHead));
}

void uthash_delete(hashtable_t **hashHead, hashtable_t **current)
{
	if (! (*current))
		return;
	
	if ((*current) && (*current)->key)
	{
		free((*current)->key);
		(*current)->key = NULL;
	}
	
	if ((*current) && (*current)->f)
	{
		(*current)->f((*current)->value);
		(*current)->value = NULL;
	}

	HASH_DEL((*hashHead), (*current));
	
	if ((*current))
		free((*current));

	(*current) = NULL;
}

void uthash_destroy(hashtable_t **hashHead)
{
	hashtable_t *current = NULL;
	hashtable_t *tmp = NULL;
	
	HASH_ITER(hh, (*hashHead), current, tmp)
	{
		//LOG_NORMAL_INFO("[safe traversal]key:%s, value:%s\n", current->key, current->value);
		uthash_delete(hashHead, &current);
		current = NULL;
	}
}

#if 0
int test_uthash()
{
	hashtable_t *hashHead = NULL; //hash表头，必须初始化为NULL
	hashtable_t *out = NULL;
	hashtable_t *tmp = NULL;
	hashtable_t *current = NULL;

	int i;
	for (i = 0; i < 10; i ++)
	{
		hashtable_t *new = (hashtable_t *)malloc(sizeof(hashtable_t));
		new->key = calloc(1, 10);
		new->value = calloc(1, 10);
		sprintf(new->key, "%d%d%d", i,i,i);
		sprintf(new->value, "%d%d%d", i,i,i);
		HASH_ADD_STR(hashHead, key, new);
	}

	LOG_NORMAL_INFO("hash count = %d\n", HASH_COUNT(hashHead));

	for (i = 0; i < 11; i ++)
	{
		char key[10];
		sprintf(key, "%d%d%d", i,i,i);
		HASH_FIND_STR(hashHead, key, out);
		if (! out)
		{
			LOG_NORMAL_INFO("the key(%s) is not exist in hash!\n", key);
			continue;
		}

		//LOG_NORMAL_INFO("find key:%s, value:%s\n", out->key, out->value);
	}

	//不安全遍历
	for (current = hashHead; current != NULL; current = (hashtable_t *)(current->hh.next))
	{
		//LOG_NORMAL_INFO("[unsafe traversal]key:%s, value:%s\n", current->key, current->value);
	}

	//安全遍历
	HASH_ITER(hh, hashHead, current, tmp)
	{
		//LOG_NORMAL_INFO("[safe traversal]key:%s, value:%s\n", current->key, current->value);

		HASH_DEL(hashHead, current);
		free(current);
		current = NULL;
	}

	// 打印节点个数
	LOG_NORMAL_INFO("hash count=%d\n", HASH_COUNT(hashHead));

	return 0;
}

int test_uthash1()
{
	hashtable_t *hashHead = NULL, *current = NULL, *tmp = NULL; //hash表头，必须初始化为NULL
	char ht_key[20];
	char *ht_value;

	int i;
	for (i = 0; i < 10; i ++)
	{
		ht_value = calloc(1, 10);
		sprintf(ht_key, "%d%d%d", i,i,i);
		sprintf(ht_value, "%d%d%d", i,i,i);
		uthash_put(&hashHead, ht_key, ht_value, NULL);
	}

	LOG_NORMAL_INFO("hash count = %d\n", uthash_count(&hashHead));

	for (i = 0; i < 11; i ++)
	{
		sprintf(ht_key, "%d%d%d", i,i,i);
		ht_value = uthash_find(&hashHead, ht_key);
		LOG_NORMAL_INFO("find key:%s, value:%s\n", ht_key, ht_value);
	}

	//不安全遍历
	for (current = hashHead; current != NULL; current = (hashtable_t *)(current->hh.next))
	{
		LOG_NORMAL_INFO("[unsafe traversal]key:%s, value:%s\n", current->key, current->value);
	}

	//安全遍历
	HASH_ITER(hh, hashHead, current, tmp)
	{
		LOG_NORMAL_INFO("[safe traversal]key:%s, value:%s\n", current->key, current->value);

		uthash_delete(&hashHead, &current);
		current = NULL;
	}

	// 打印节点个数
	LOG_NORMAL_INFO("hash count=%d\n", uthash_count(&hashHead));

	return 0;
}
#endif

