#ifndef __USR_UTHASH_H__
#define __USR_UTHASH_H__

#include "uthash.h"

typedef void (*_free_call_)(void *);

typedef struct HASH_TABLE
{
	char *key;
	void *value;
	UT_hash_handle hh;
	_free_call_ f;
} hashtable_t;


int uthash_put(hashtable_t **hashHead, char *ht_key, void *ht_value, _free_call_ f);
void *uthash_find(hashtable_t **hashHead, char *ht_key);
int uthash_count(hashtable_t **hashHead);
void uthash_destroy(hashtable_t **hashHead);
void uthash_delete(hashtable_t **hashHead, hashtable_t **current);


#endif


