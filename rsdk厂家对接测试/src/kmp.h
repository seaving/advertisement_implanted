#ifndef __KMP_H__
#define __KMP_H__	1

void get_nextval(char * ptn, int ptn_len, int * nextval);
int kmp_search(char * src, int src_len, char * patn, int patn_len, int const * nextval);


#endif

