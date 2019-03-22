#ifndef __BM_H__
#define __BM_H__

int bm(char *psrc, int src_len, char *pfind, int find_len);

int bm_case(char *psrc, int src_len, char *pfind, int find_len);

int SUNDAY(char *text, int text_size, char *patt, int patt_size);
int SUNDAY_CASE(char *text, int text_size, char *patt, int patt_size);

#endif

