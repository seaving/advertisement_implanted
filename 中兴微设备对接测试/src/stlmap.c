#include "includes.h"



static node* hashtab[HASHSIZE];

void stlmap_hash_init()
{
	int i;
	for (i = 0; i < HASHSIZE; i ++)
	{
		hashtab[i] = NULL;
	}
}

unsigned int stlmap_hash(char *s)
{
	unsigned int h = 0;
	for ( ; *s; s ++)
	{
		h = *s + h * 31;
	}
	return h % HASHSIZE;
}

node* stlmap_hash_lookup(char *n)
{
	unsigned int hi = stlmap_hash(n);
	node* np = hashtab[hi];
	for ( ; np != NULL; np = np->next)
	{
		if (!strcmp(np->name, n))
		{
			return np;
		}
	}
    
    return NULL;
}

char* stlmap_m_strdup(char *o)
{
	int l = strlen(o) + 1;
	char *ns = (char *)malloc(l * sizeof(char));
	if (ns == NULL)
	{
		return NULL;
	}
	
	strcpy(ns, o);
	
	return ns;
}

char* stlmap_hash_get(char* name)
{
	node* n = stlmap_hash_lookup(name);
	if (n == NULL)
	{
		return NULL;
	}
	return n->desc;
}

int stlmap_hash_insert(char* name, char* desc)
{
	unsigned int hi;
	node* np;
	if ((np = stlmap_hash_lookup(name)) == NULL)
	{
		hi = stlmap_hash(name);
		np = (node *)malloc(sizeof(node));
		if (np == NULL)
		{
			return 0;
		}
		np->name = stlmap_m_strdup(name);
		if (np->name == NULL)
		{
			return 0;
		}
		np->next = hashtab[hi];
		hashtab[hi] = np;
	}
	else
	{
		free(np->desc);
	}
	
	np->desc = stlmap_m_strdup(desc);
	if (np->desc == NULL)
	{
		return 0;
	}
	
	return 1;
}

int stlmap_hash_delect(char* name)
{
	unsigned int hi = stlmap_hash(name);
	node* np = hashtab[hi];
	node* ntmp = np;
	for ( ; np != NULL; np = np->next)
	{
		if (!strcmp(np->name, name))
		{
			break;
		}
		ntmp = np;
	}

	if (np == NULL)
	{
		return 0;
	}
	
	ntmp->next = np->next;

	free(np->name);
	free(np->desc);
	free(np);
	
	return 1;
}

node** gethashtab()
{
	return hashtab;
}

/* A pretty useless but good debugging function,
which simply displays the hashtable in (key.value) pairs
*/
void stlmap_displaytable()
{
   	int i;
	node *t;
	for (i = 0; i < HASHSIZE; i ++)
	{
		if (hashtab[i] == NULL)
		{
			printf("()");
		}
		else
		{
			t = hashtab[i];
			for ( ; t != NULL; t = t->next)
			{
				printf("(%s.%s) ", t->name, t->desc);
			}
		}
	}
}

void stlmap_hash_cleanup()
{
	int i;
	node *np, *t;
	for (i = 0; i < HASHSIZE; i ++)
	{
		if (hashtab[i] != NULL)
		{
			np = hashtab[i];
			while (np != NULL)
			{
				t = np->next;
				free(np->name);
				free(np->desc);
				free(np);
				np = t;
			}
		}
	}
}


