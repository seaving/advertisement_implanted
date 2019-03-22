#include "includes.h"
extern pthread_mutex_t mutex;

struct redir_file_buffer{
    char    *front;
    int     front_len;
    char    *rear;
    int     rear_len;
};

struct res_msg_file_buffer{
    char    *front;
    int     front_len;
    char    *midle;
    int     midle_len;
    char    *rear;
    int     rear_len;
};

struct evbuffer	*evb_internet_offline_page 		= NULL;
struct evbuffer *evb_authserver_offline_page	= NULL;
struct redir_file_buffer *wifidog_redir_html 	= NULL;
struct res_msg_file_buffer *wifidog_res_msg_html 	= NULL;


#define	WIFIDOG_REDIR_HTML_CONTENT	"setTimeout(function() {location.href = \"%s\";}, 10);"

extern int g_success ;
void print_request_head_info(struct evkeyvalq *header)
{
    struct evkeyval *first_node = header->tqh_first;
    while (first_node)
    {
        LOG_NORMAL_INFO("%s: %s\r\n", first_node->key, first_node->value);
        first_node = first_node->next.tqe_next;
    }
}

#if 0
static void evhttp_gw_reply_js_redirect(struct evhttp_request *req, const char *peer_addr)
{
	char *mac = arp_get(peer_addr);
	char *req_url = evhttp_get_request_encode_url(req);
	char *redir_url = evhttpd_get_full_redir_url(mac != NULL ? mac : "ff:ff:ff:ff:ff:ff", peer_addr, req_url);
	struct evbuffer *evb = evbuffer_new();
	struct evbuffer *evb_redir_url = evbuffer_new();
	
	LOG_HL_INFO("Got a GET request for <%s> from <%s>\n", req_url, peer_addr);
	
	evbuffer_add(evb, "<script>", strlen("<script>"));
	evbuffer_add_printf(evb_redir_url, WIFIDOG_REDIR_HTML_CONTENT, redir_url);
	evbuffer_add_buffer(evb, evb_redir_url);
	evbuffer_add(evb, "</script>", strlen("</script>"));
	
	evhttp_add_header(evhttp_request_get_output_headers(req),
		    "Content-Type", "text/html");
	evhttp_send_reply(req, 200, "OK", evb);
	
	free(mac);
	free(req_url);
	free(redir_url);
	evbuffer_free(evb);
	evbuffer_free(evb_redir_url);
}
#endif
void evhttp_reply_sniff(struct evhttp_request *req,const char *url)
{
	struct evbuffer *evb = evbuffer_new();
	evhttp_add_header(evhttp_request_get_output_headers(req),
		    "Content-Type", "text/html");
	evhttp_add_header(evhttp_request_get_output_headers(req),
		    "Cache-Control", "no-cache");

	printf("send reply!\r\n");
	evhttp_send_reply(req, 200, "OK", evb);
		
}

void evhttp_gw_reply_js_redirect(struct evhttp_request *req, const char *redirect_url)
{
	const char *redir_url = redirect_url;
	struct evbuffer *evb = evbuffer_new();
	struct evbuffer *evb_redir_url = evbuffer_new();
	
	if (! evb || ! evb_redir_url)
		goto err;

	evbuffer_add(evb, wifidog_redir_html->front, wifidog_redir_html->front_len);
	evbuffer_add_printf(evb_redir_url, WIFIDOG_REDIR_HTML_CONTENT, redir_url);
	evbuffer_add_buffer(evb, evb_redir_url);
	evbuffer_add(evb, wifidog_redir_html->rear, wifidog_redir_html->rear_len);

	evhttp_add_header(evhttp_request_get_output_headers(req),
	    	"Connection", "closed");
	evhttp_add_header(evhttp_request_get_output_headers(req),
		    "Content-Type", "text/html");
	evhttp_add_header(evhttp_request_get_output_headers(req),
		    "Cache-Control", "no-cache");
	evhttp_send_reply (req, 200, "OK", evb);

err:
	if (evb)
		evbuffer_free(evb);
	if (evb_redir_url)
		evbuffer_free(evb_redir_url);
}

void evhttp_reply_response_connection_closed(struct evhttp_request *req, int res_code, char *state, char *titile, char *msg)
{
	struct evbuffer *evb = evbuffer_new();
	
	if (! evb)
		goto err;

	if (msg)
	{	
		char *utf8 = NULL;
		
		evbuffer_add(evb, wifidog_res_msg_html->front, wifidog_res_msg_html->front_len);
		if (titile)
		{
			utf8 = utf8_encode(titile);
			if (utf8) evbuffer_add(evb, utf8, strlen(utf8));
			if (utf8) utf8_encode_free(utf8);
		}
		else
		{
			utf8 = utf8_encode("温馨提示:");
			if (utf8) evbuffer_add(evb, utf8, strlen(utf8));
			if (utf8) utf8_encode_free(utf8);
		}
		evbuffer_add(evb, wifidog_res_msg_html->midle, wifidog_res_msg_html->midle_len);
		utf8 = utf8_encode(msg);
		if (utf8) evbuffer_add(evb, utf8, strlen(utf8));
		if (utf8) utf8_encode_free(utf8);
		evbuffer_add(evb, wifidog_res_msg_html->rear, wifidog_res_msg_html->rear_len);
	}
	
	evhttp_add_header(evhttp_request_get_output_headers(req),
		    "Connection", "closed");
	evhttp_send_reply (req, res_code, state, evb);

err:
	if (evb)
		evbuffer_free(evb);
}


void evhttp_gw_reply_302_redirect(struct evhttp_request *req, const char *redirect_url)
{
	struct evbuffer *evb = evbuffer_new();
	
	if (! evb)
		goto err;

	evhttp_add_header(evhttp_request_get_output_headers(req),
				"Location", redirect_url);
	evhttp_send_reply(req, 302, "Found", evb);

err:
	if (evb)
		evbuffer_free(evb);
}

void redirect_cb(struct evhttp_request *req, void *arg)
{
	/* Determine peer */
	int ret = 0;
	char *peer_addr;
	ev_uint16_t peer_port;

	char *req_encode_url = NULL;
	char *redir_url = NULL;
	char *mac = NULL;

	struct evhttp_connection *con = evhttp_request_get_connection(req);

	if (evhttp_request_get_command(req) != EVHTTP_REQ_GET)
	{
		evhttp_reply_response_connection_closed(req, 200, "OK", NULL, NULL);
		evhttp_connection_free(con);
		return ;
	}
	
	//得到请求客户端的Ip与端口号
	evhttp_connection_get_peer(con, &peer_addr, &peer_port);
	mac = arp_get(peer_addr);

	LOG_NORMAL_INFO("mac: %s, ip: %s, port: %d\r\n", mac, peer_addr, peer_port);

	int remote_port = 80;
	if (arg && *((int *)arg) == QC_DOG_HTTPS_REDIRECT_SERVER_PORT)
		remote_port = 443;
	
	req_encode_url = evhttp_get_request_encode_url(req, remote_port);
	
	
	//从hash表中找到是否存在该客户端
	ret = auth_client_find(peer_addr, mac);
	if (ret == 1)
	{
		//存在该IP和MAC，但是还是走到qcdog里面来了
		//有可能是放行没有生效，也可能是有放行延时
		//对于放行出错，这里暂时无解，现认为延时来处理
		//也可能是浏览器缓存的原因导致
		
		evhttp_reply_response_connection_closed(req, 200, "OK", NULL, "您已认证成功，无须再次认证，请直接上网吧<br>若您还是弹出此页面，请重启浏览器 ~~");
		evhttp_connection_free(con);
		LOG_NORMAL_INFO("auth_client_find: mac: %s, ip: %s, return.\r\n", mac, peer_addr);
	}
	else
	{
		if (ret == 2)
		{
			LOG_NORMAL_INFO("auth_client_find: mac: %s, ip: %s, temp accept, redirect to auth.\r\n", mac, peer_addr);
		}
		else if (ret <= 0)
		{
			LOG_NORMAL_INFO("auth_client_find: mac: %s, ip: %s, not found.\r\n", mac, peer_addr);
		}
		
		//不存在该client
		char mobile_name[65] = {0};
		get_cli_name(peer_addr, mac != NULL ? mac : "ff:ff:ff:ff:ff:ff", mobile_name);
		redir_url = evhttpd_get_full_redir_url(mac != NULL ? mac : "ff:ff:ff:ff:ff:ff", peer_addr, mobile_name, req_encode_url);
		LOG_HL_INFO("[ %s ] redir_url: %s\r\n", remote_port == 443 ? "HTTPS" : "HTTP", redir_url);
		
		//evhttp_gw_reply_302_redirect(req, redir_url);
		evhttp_gw_reply_js_redirect(req, redir_url);
		
	}
	
	if (mac)
		free(mac);
	if (req_encode_url)
		free(req_encode_url);
	if (redir_url)
		free(redir_url);
}

char *safe_strdup(const char *s)
{
    char *retval = NULL;
    if (! s)
    {
        LOG_ERROR_INFO("safe_strdup called with NULL which would have crashed strdup. Bailing out\n");    
        exit(1);
    }
    retval = strdup(s);
    if (! retval)
    {
        LOG_ERROR_INFO("Failed to duplicate a string: %s.  Bailing out\n", strerror(errno));
        exit(1);
    }
    
    return (retval);
}

char *arp_get(const char *req_ip)
{
    FILE *proc 		= NULL;
    char ip[16] 	= {0};
    char mac[18] 	= {0};
    char *reply;

    if (!(proc = fopen("/proc/net/arp", "r")))
    {
        return NULL;
    }

    /* skip first line */
    while (!feof(proc) && fgetc(proc) != '\n') ;

    /* find ip, copy mac in reply */
    reply = NULL;
    while (! feof(proc) && (fscanf(proc, " %15[0-9.] %*s %*s %17[a-fa-f0-9:] %*s %*s", ip, mac) == 2))
    {
        if (strcmp(ip, req_ip) == 0)
        {
            reply = safe_strdup(mac);
            break;
        }
    }

    fclose(proc);

    return reply;
}

static char *evb_2_string(struct evbuffer *evb, int *olen) 
{
	int rlen = evbuffer_get_length(evb);
	char *str = (char *)malloc(rlen+1);
	memset(str, 0, rlen+1);
	evbuffer_copyout(evb, str, rlen);
	if (olen)
		*olen = rlen;
	return str;
}

#define CMD_WIFI_SSID_GET	"uci get wireless.@wifi-iface[%d].ssid"
void _get_wifi_ssid_call_(char *buf, void *ssid)
{
	if (ssid && buf)
	{
		memcpy((char *)ssid, buf, strlen(buf));
	}
}

int get_wifi_ssid(int witch, char *ssid)
{
	char cmd[256];
	snprintf(cmd, 255, CMD_WIFI_SSID_GET, witch);
	if (popen_cmd(cmd, _get_wifi_ssid_call_, ssid) < 0)
	{
		return -1;
	}
	
	LOG_WARN_INFO("ssid: %s\n", ssid);

	return 0;
}

char *evhttpd_get_full_redir_url(const char *mac, const char *ip, const char *mobile_name, const char *orig_url)
{
	char parm_tmp[256] = {0};
	char md5[33] = {0};
	sprintf(parm_tmp, "%s%s", GET_DEV_NO, GET_WAN_MAC);
	CalMd5(parm_tmp, strlen(parm_tmp), md5);
	char *ssid/*[128]*/ = "QCWIFI";
	//get_wifi_ssid(0, ssid);

	char lan_ip[33] = {0};
	get_dev_ip(lan_ip, GET_LAN_NAME);
	
	struct evbuffer *evb = evbuffer_new();
	evbuffer_add_printf(evb, "%s://%s:%d%s?gw=%s&port=%d&devNo=%s&ip=%s&mac=%s&mobile_name=%s&ssid=%s&bssid=%s&url=%s&token=%s",
					"http", AUTH_SERVER_HOST, AUTH_SERVER_PORT, AUTH_LOGIN, lan_ip, QC_DOG_AUTH_SERVER_PORT, 
					GET_DEV_NO, ip, mac, mobile_name, ssid, GET_WAN_MAC, orig_url, md5);
//http://www.qcwifi.ltd?80/auther/login?gw=134.227.227.1&port=60003&devNo=0fbe290ef6d0c2287f0efdd197fef8b1&ip=134.227.227.234
//&mac=38:bc:1a:c3:5d:72&mobilename=mobile_name=Meizu-MX4-Pro&url=https%3A%2F%2Fdns.weixin.qq.com%2Fcgi-bin%2Fmicromsg-bin%2Fnewgetdns%3Fuin%3D2675218241%26clientversion%3D637863988%26scene%3D0%26net%3D1%26md5%3Daad46d8f3967e75053536c5a9bff2558%26devicetype%3Dandroid-22%26lan%3Dzh_CN%26sigver%3D2
	char *redir_url = evb_2_string(evb, NULL);
	evbuffer_free(evb);
	
	return redir_url;
}

char *evhttp_get_request_encode_url(struct evhttp_request *req, int port)
{
	char url[257] = {0}; // only get 256 char from request url
	if (port == 80 || port == 8080)
	{
		snprintf(url, 256, "http://%s:%d%s",
			evhttp_request_get_host(req),
			port, evhttp_request_get_uri(req));
	}
	else
	{
		snprintf(url, 256, "https://%s:%d%s",
			evhttp_request_get_host(req),
			port, evhttp_request_get_uri(req));
	}
	
	return evhttp_encode_uri(url);
}

char *evhttp_get_request_url(struct evhttp_request *req, int port)
{
	char url[2049] = {0};
	if (port == 80 || port == 8080)
	{
		snprintf(url, 2048, "http://%s:%d%s",
			evhttp_request_get_host(req),
 			port, evhttp_request_get_uri(req));
	}
	else
	{
		snprintf(url, 2048, "https://%s:%d%s",
			evhttp_request_get_host(req),
			port, evhttp_request_get_uri(req));
	}

	char *murl = calloc(1, strlen(url) + 1);
	if (! murl)
	{
		LOG_PERROR_INFO("calloc error.");
		return NULL;
	}

	memcpy(murl, url, strlen(url));
	
	return murl;
}

static struct evbuffer *evhttp_read_file(const char *filename, struct evbuffer *evb)
{
	int fd;
	struct stat stat_info;
	
	fd = open(filename, O_RDONLY);
	if (fd == -1)
	{
		LOG_ERROR_INFO("Failed to open HTML message file %s: %s", strerror(errno), filename);
		return NULL;
	}
	
	if (fstat(fd, &stat_info) == -1)
	{
		LOG_ERROR_INFO("Failed to stat HTML message file: %s", strerror(errno));
		close(fd);
		return NULL;
	}
	
	evbuffer_add_file(evb, fd, 0, stat_info.st_size);
	close(fd);
	return evb;
}

int init_wifidog_msg_html(char *internet_offline_file, char *authserver_offline_file)
{	
	evb_internet_offline_page 	= evbuffer_new();
	if (!evb_internet_offline_page)
	{
		LOG_PERROR_INFO("evbuffer_new error.");
		goto err;
	}
	
	evb_authserver_offline_page	= evbuffer_new();
	if (!evb_authserver_offline_page)
	{
		LOG_PERROR_INFO("evbuffer_new error.");
		goto err;
	}
	
	if (!evhttp_read_file(internet_offline_file, evb_internet_offline_page) || 
		!evhttp_read_file(authserver_offline_file, evb_authserver_offline_page))
	{
		LOG_ERROR_INFO("init_wifidog_msg_html failed, exiting...");
		goto err;
	}

	return 0;
	
err:
	if (evb_internet_offline_page)
		evbuffer_free(evb_internet_offline_page);
	if (evb_authserver_offline_page)
		evbuffer_free(evb_authserver_offline_page);

	return -1;
}

int init_wifidog_redir_html(char *htmlredirfile)
{
	struct evbuffer *evb_front = NULL;
	struct evbuffer *evb_rear = NULL;
	char	front_file[128] = {0};
	char	rear_file[128] = {0};
	
	
	wifidog_redir_html = (struct redir_file_buffer *)malloc(sizeof(struct redir_file_buffer));
	if (wifidog_redir_html == NULL)
	{
		LOG_PERROR_INFO("malloc error.");
		goto err;
	}
	
	evb_front 	= evbuffer_new();
	evb_rear	= evbuffer_new();
	if (evb_front == NULL || evb_rear == NULL)
	{
		LOG_PERROR_INFO("evbuffer_new error.");
		goto err;
	}
	
	snprintf(front_file, 128, "%s.front", htmlredirfile);
	snprintf(rear_file, 128, "%s.rear", htmlredirfile);
	if (!evhttp_read_file(front_file, evb_front) || 
		!evhttp_read_file(rear_file, evb_rear))
	{
		goto err;
	}
	
	int len = 0;
	wifidog_redir_html->front 		= evb_2_string(evb_front, &len);
	wifidog_redir_html->front_len	= len;
	wifidog_redir_html->rear		= evb_2_string(evb_rear, &len);
	wifidog_redir_html->rear_len	= len;
	
	if (evb_front) evbuffer_free(evb_front);	
	if (evb_rear) evbuffer_free(evb_rear);
	return 0;

err:
	if (evb_front) evbuffer_free(evb_front);	
	if (evb_rear) evbuffer_free(evb_rear);
	if (wifidog_redir_html)
	{
		if (wifidog_redir_html->front) free(wifidog_redir_html->front);
		if (wifidog_redir_html->rear) free(wifidog_redir_html->rear);
		free(wifidog_redir_html);
	}
	wifidog_redir_html = NULL;
	return -1;
}

int init_wifidog_result_msg_html(char *htmlresmsgfile)
{

	struct evbuffer *evb_front = NULL;
	struct evbuffer *evb_rear = NULL;
	struct evbuffer *evb_midle = NULL;

	char	front_file[128] = {0};
	char	midle_file[128] = {0};
	char	rear_file[128] = {0};

	wifidog_res_msg_html = (struct res_msg_file_buffer *)malloc(sizeof(struct res_msg_file_buffer));
	if (wifidog_res_msg_html == NULL)
	{
		LOG_PERROR_INFO("malloc error.");
		goto err;
	}

	evb_front 	= evbuffer_new();
	evb_midle	= evbuffer_new();
	evb_rear	= evbuffer_new();
	if (evb_front == NULL || evb_rear == NULL)
	{
		LOG_PERROR_INFO("evbuffer_new error.");
		goto err;
	}
	
	snprintf(front_file, 128, "%s.front", htmlresmsgfile);
	snprintf(midle_file, 128, "%s.midle", htmlresmsgfile);
	snprintf(rear_file, 128, "%s.rear", htmlresmsgfile);
	if (!evhttp_read_file(front_file, evb_front) || 
		!evhttp_read_file(midle_file, evb_midle) ||
		!evhttp_read_file(rear_file, evb_rear))
	{
		goto err;
	}
	
	int len = 0;
	wifidog_res_msg_html->front 	= evb_2_string(evb_front, &len);
	wifidog_res_msg_html->front_len	= len;
	wifidog_res_msg_html->midle 	= evb_2_string(evb_midle, &len);
	wifidog_res_msg_html->midle_len	= len;
	wifidog_res_msg_html->rear		= evb_2_string(evb_rear, &len);
	wifidog_res_msg_html->rear_len	= len;
	
	if (evb_front) evbuffer_free(evb_front);
	if (evb_midle) evbuffer_free(evb_midle);
	if (evb_rear) evbuffer_free(evb_rear);

	return 0;

err:
	if (evb_front) evbuffer_free(evb_front);
	if (evb_midle) evbuffer_free(evb_midle);
	if (evb_rear) evbuffer_free(evb_rear);
	if (wifidog_res_msg_html)
	{
		if (wifidog_res_msg_html->front) free(wifidog_res_msg_html->front);
		if (wifidog_res_msg_html->midle) free(wifidog_res_msg_html->midle);
		if (wifidog_res_msg_html->rear) free(wifidog_res_msg_html->rear);
		free(wifidog_res_msg_html);
	}
	wifidog_res_msg_html = NULL;
	return -1;
}

void *safe_malloc(size_t size)
{
    void *retval = NULL;
    retval = malloc(size);
    if (!retval)
    {
        LOG_ERROR_INFO("Failed to malloc %d bytes of memory: %s.  Bailing out", size, strerror(errno));
        return NULL;
    }
    memset(retval, 0, size);
    return (retval);
}

void page_release()
{
	if (wifidog_redir_html) free(wifidog_redir_html);
	if (evb_internet_offline_page) evbuffer_free(evb_internet_offline_page);
	if (evb_authserver_offline_page) evbuffer_free(evb_authserver_offline_page);
}

