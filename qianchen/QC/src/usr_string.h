#ifndef __USR_STRING_H__
#define __USR_STRING_H__

//#define _KMP_

#ifndef _KMP_
#define find_str(psrc, src_len, pfind, find_len) \
					SUNDAY(psrc, src_len, pfind, find_len)
#define find_str_case(psrc, src_len, pfind, find_len) \
					SUNDAY_CASE(psrc, src_len, pfind, find_len)
#else
int find_str(char *psrc, int src_len, char *pfind, int find_len);
int find_str_case(char *psrc, int src_len, char *pfind, int find_len);
#endif

int replace(char *psrc, int src_len, char *prep, int rep_len, char *pstr, int str_len);
int repalce_with_pin(char *psrc, int src_len, char *psub, int sub_len, char *pin, int pin_len);

int insert(char *psrc, int src_len, char *psub, int sub_len, char *pin, int pin_len);
int isStrexit(char *src_buf, char *findstr);
int isStrnexit(char *src_buf, int src_len, char *findstr, int find_len);
int insert_case(char *psrc, int src_len, char *psub, int sub_len, char *pin, int pin_len);
int isStrexit_case(char *src_buf, char *findstr);
int isStrnexit_case(char *src_buf, int src_len, char *findstr, int find_len);

char *straddr(char *src_buf, char *findstr);
char *strnaddr(char *src_buf, int src_len, char *findstr, int find_len);
char *straddr_case(char *src_buf, char *findstr);
char *strnaddr_case(char *src_buf, int src_len, char *findstr, int find_len);

int strline(char *psrc, char *pdest, int size);


int substr(char *text, int text_len, char *start, char *end, char *buf, int bufsize);
int cutstr(char *text, int text_len, char *start, char *end, char *buf, int bufsize);

void free_malloc(char *m);

void del_tab_newline(char *src);



#endif


