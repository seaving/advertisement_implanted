#include "includes.h"


int find_str(char *psrc, int src_len, char *pfind, int find_len)
{
	char *src = psrc;
	char *ptn = pfind;
	int ptn_len = find_len;
	int nextval[ptn_len];
	get_nextval(ptn, ptn_len, nextval);
	return kmp_search(src, src_len, ptn, ptn_len, nextval);
}

int find_str_case(char *psrc, int src_len, char *pfind, int find_len)
{
	char *src = psrc;
	char *ptn = pfind;
	int ptn_len = find_len;
	int nextval[ptn_len];
	get_nextval(ptn, ptn_len, nextval);
	return kmp_search_case(src, src_len, ptn, ptn_len, nextval);
}

/**替换字符串,以pstr长度为准*/
int replace(char *psrc, int src_len, char *prep, int rep_len, char *pstr, int str_len)
{
	char *p;
	int offset = find_str(psrc, src_len, prep, rep_len);
	if (offset >= 0)
	{
		p = psrc + offset;
		memcpy(p, pstr, str_len);

		return 0;
	}

	return -1;
}

int insert(char *psrc, int src_len, char *psub, int sub_len, char *pin, int pin_len)
{
	//char *p;
	//int size = 0;
	//int len = 0;
	int offset = find_str(psrc, src_len, psub, sub_len);
	if (offset >= 0)
	{
		char *pinsert_addr = psrc + offset + sub_len;
		char *pbackup_addr = pinsert_addr;
		int backup_len = psrc + src_len - pbackup_addr;
		char backup_buff[backup_len + 1];
		memcpy(backup_buff, pbackup_addr, backup_len);
		backup_buff[backup_len] = 0;
		memcpy(pinsert_addr, pin, pin_len);
		char *prebackup_addr = pinsert_addr + pin_len;
		memcpy(prebackup_addr, backup_buff, backup_len);
		*(prebackup_addr + backup_len) = 0;
		return offset;
	}
	return -1;
}

int repalce_with_pin(char *psrc, int src_len, char *psub, int sub_len, char *pin, int pin_len)
{
	int offset = find_str(psrc, src_len, psub, sub_len);
	if (offset >= 0)
	{
		char *preplace_addr = psrc + offset;
		char *pbackup_addr = preplace_addr + sub_len;
		int backup_len = psrc + src_len - pbackup_addr;
		char backup_buff[backup_len + 1];
		memcpy(backup_buff, pbackup_addr, backup_len);
		backup_buff[backup_len] = 0;

		memcpy(preplace_addr, pin, pin_len);
		char *prebackup_addr = preplace_addr + pin_len;
		memcpy(prebackup_addr, backup_buff, backup_len);
		*(prebackup_addr + backup_len) = 0;

		return 0;
	}

	return -1;
}

int isStrexit(char *src_buf, char *findstr)
{
	if (src_buf == NULL || findstr == NULL)
	{
		return  0;
	}
	int src_len = strlen(src_buf);
	int find_len = strlen(findstr);
	if (src_len <= 0 || find_len <= 0)
	{
		return 0;
	}
	int offset = find_str(src_buf, src_len, findstr, find_len);
	if (offset < 0)
	{
		return 0;
	}
	
	return 1;
}

int isStrexit_case(char *src_buf, char *findstr)
{
	if (src_buf == NULL || findstr == NULL)
	{
		return  0;
	}
	int src_len = strlen(src_buf);
	int find_len = strlen(findstr);
	if (src_len <= 0 || find_len <= 0)
	{
		return 0;
	}
	int offset = find_str_case(src_buf, src_len, findstr, find_len);
	if (offset < 0)
	{
		return 0;
	}
	
	return 1;
}

int isStrnexit(char *src_buf, int src_len, char *findstr, int find_len)
{
	if (src_buf == NULL || findstr == NULL)
	{
		return  0;
	}
	if (src_len <= 0 || find_len <= 0)
	{
		return 0;
	}
	int offset = find_str(src_buf, src_len, findstr, find_len);
	if (offset < 0)
	{
		return 0;
	}
	
	return 1;
}

int isStrnexit_case(char *src_buf, int src_len, char *findstr, int find_len)
{
	if (src_buf == NULL || findstr == NULL)
	{
		return  0;
	}
	if (src_len <= 0 || find_len <= 0)
	{
		return 0;
	}
	int offset = find_str_case(src_buf, src_len, findstr, find_len);
	if (offset < 0)
	{
		return 0;
	}
	
	return 1;
}

char *straddr(char *src_buf, char *findstr)
{
	if (src_buf == NULL || findstr == NULL)
	{
		return NULL;
	}
	int src_len = strlen(src_buf);
	int find_len = strlen(findstr);
	if (src_len <= 0 || find_len <= 0)
	{
		return NULL;
	}
	int offset = find_str(src_buf, src_len, findstr, find_len);
	if (offset < 0)
	{
		return NULL;
	}
	
	return src_buf + offset;
}

char *straddr_case(char *src_buf, char *findstr)
{
	if (src_buf == NULL || findstr == NULL)
	{
		return NULL;
	}
	int src_len = strlen(src_buf);
	int find_len = strlen(findstr);
	if (src_len <= 0 || find_len <= 0)
	{
		return NULL;
	}
	int offset = find_str_case(src_buf, src_len, findstr, find_len);
	if (offset < 0)
	{
		return NULL;
	}
	
	return src_buf + offset;
}

char *strnaddr(char *src_buf, int src_len, char *findstr, int find_len)
{
	if (src_buf == NULL || findstr == NULL)
	{
		return NULL;
	}

	int offset = find_str(src_buf, src_len, findstr, find_len);
	if (offset < 0)
	{
		return NULL;
	}
	
	return src_buf + offset;
}

char *strnaddr_case(char *src_buf, int src_len, char *findstr, int find_len)
{
	if (src_buf == NULL || findstr == NULL)
	{
		return NULL;
	}

	int offset = find_str_case(src_buf, src_len, findstr, find_len);
	if (offset < 0)
	{
		return NULL;
	}
	
	return src_buf + offset;
}

int strline(char *psrc, char *pdest, int size)
{
	int i = 0, j = 0;
	for (i = 0; psrc[i] != '\0'; i ++)
	{
		pdest[j ++] = psrc[i];
		if (j == size - 1 || psrc[i] == '\n')
		{
			break;
		}
	}
	pdest[j] = '\0';
	if (j == 0)
	{
		return -1;
	}
	return j;
}

int substr(char *text, int text_len, char *start, char *end, char *buf, int bufsize)
{
	char *title = start;
	int title_len = strlen(start);
	char *title_end = end;
	int title_end_len = strlen(end);
	int offset = find_str(text, text_len, title, title_len);
	if (offset >= 0)
	{
		char *title_start_addr = text + offset;
		char *find_addr = text + offset + title_len;
		if (title_start_addr)
		{
			offset = find_str(find_addr, text + text_len - find_addr, title_end, title_end_len);
			if (offset >= 0)
			{
				char *title_end_addr = find_addr + offset + title_end_len;
				if (title_end_addr)
				{
					int len = title_end_addr - title_start_addr;
					if (bufsize > len)
					{
						memcpy(buf, title_start_addr, len);
						return len;
					}
				}
			}
		}
	}

	return -1;
}

int cutstr(char *text, int text_len, char *start, char *end, char *buf, int bufsize)
{
	char *title = start;
	int title_len = strlen(start);
	char *title_end = end;
	int title_end_len = strlen(end);
	int offset = find_str(text, text_len, title, title_len);
	if (offset >= 0)
	{
		char *title_start_addr = text + offset + title_len;
		char *find_addr = text + offset + title_len;
		if (title_start_addr)
		{
			offset = find_str(find_addr, text + text_len - find_addr, title_end, title_end_len);
			if (offset >= 0)
			{
				char *title_end_addr = find_addr + offset;
				if (title_end_addr)
				{
					int len = title_end_addr - title_start_addr;
					if (bufsize > len)
					{
						memcpy(buf, title_start_addr, len);
						return len;
					}
				}
			}
		}
	}

	return -1;
}

char *strchr_from_right(char *str, int strlen, char ch)
{
	int i, j = -1;
	
	for (i = 0; i < strlen; i ++)
	{
		if (ch == str[i])
			j = i;
	}

	if (j < 0)
		return NULL;

	return str + j;
}

void free_malloc(char *m)
{
	if (m)
	{
		free(m);
		m = NULL;
	}
}

