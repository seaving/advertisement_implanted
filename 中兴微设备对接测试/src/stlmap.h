#ifndef __STLMAP_H__
#define __STLMAP_H__	1

#define HASH_TABLE_MAX_SIZE 0

#define HASHSIZE 1

typedef struct _node
{
	char *name;
	char *desc;
	struct _node *next;
}node;

void stlmap_hash_init();
unsigned int stlmap_hash(char *s);
node* stlmap_hash_lookup(char *n);
char* stlmap_m_strdup(char *o);
char* stlmap_hash_get(char* name);
int stlmap_hash_insert(char* name, char* desc);
void stlmap_displaytable();
void stlmap_hash_cleanup();
int stlmap_hash_delect(char* name);
node** gethashtab();


#endif

