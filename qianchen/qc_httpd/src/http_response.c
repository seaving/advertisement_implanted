#if 0
#include "includes.h"

#define CONTENT_TYPE_JAVA_SCRIPT	"application/x-javascript"
#define CONTENT_TYPE_TEXT_HTML		"text/html"
#define CONTENT_TYPE_TEXT_CSS		"text/css"
#define CONTENT_TYPE_JPG			"image/jpeg"
#define CONTENT_TYPE_PNG			"image/png"

static void struct_local_url(char *url, int port, char *path)
{
	char ip[20] = {0};
	get_dev_ip(ip, LAN_DEV);
	sprintf(url, "http://%s:%d%s", ip, port, path);
}

//404
bool send_response_404(int client_sock)
{
	char *res_404 = "HTTP/1.1 404 Not Found\r\n\r\n";
	return socket_send(client_sock, res_404, strlen(res_404), 5);
}

//500
bool send_response_500(int client_sock)
{
	char *res_500 = "HTTP/1.1 500 Internal Server Error\r\n\r\n";
	return socket_send(client_sock, res_500, strlen(res_500), 5);
}

//302
bool send_response_302(int client_sock, char *to_url)
{
	char header_buff[512] = {0};
#if 1
	int Year = 0, Mon= 0, Data = 0, Hour = 0, Min = 0, Sec = 0, Weekday = 0;

	GetAllSystemTime(&Year, &Mon, &Data, &Hour, &Min, &Sec, &Weekday);
	sprintf( header_buff, 
			"HTTP/1.1 302 Found\r\n"
			"Date: %s, %d %s 20%d %d:%d:%d GMT\r\n"
			"Content-Type: text/html; charset=%s\r\n"
			"Content-Length: 0\r\n"
			"Connection: Close\r\n"
			"Location: %s\r\n"
			"Server: %s/%s\r\n"
			"\r\n",
			weekday(Weekday), Data, month(Mon), Year, Hour, Min, Sec, WEB_CHAR_SET,
			to_url, MODULE_NAME, VERSION );
#else
	sprintf(header_buff, 
			"HTTP/1.0 302 Moved Temporarily\r\n"
			"Location: http://%s\r\n"
			"Content-Type: text/html; \r\n"
			"Content-Length: 0\r\n\r\n", 
			to_url	);
#endif
  	LOG_NORMAL_INFO("%s", header_buff);
    int len = strlen(header_buff);
    if (socket_send(client_sock, header_buff, len, 10) != len)
    {
    	LOG_PERROR_INFO("send error.");
        return false;
    }
    return true;
}

bool send_response_with_cookie_302(int client_sock, char *to_url, char *cookie)
{
	char header_buff[512] = {0};
#if 1
	int Year = 0, Mon= 0, Data = 0, Hour = 0, Min = 0, Sec = 0, Weekday = 0;

	GetAllSystemTime(&Year, &Mon, &Data, &Hour, &Min, &Sec, &Weekday);
	sprintf( header_buff, 
			"HTTP/1.1 302 Found\r\n"
			"Date: %s, %d %s 20%d %d:%d:%d GMT\r\n"
			"Content-Type: text/html; charset=%s\r\n"
			"Content-Length: 0\r\n"
			"Connection: Close\r\n"
			"Location: %s\r\n"
			"Server: %s/%s\r\n"
			"Set-Cookie: %s\r\n"
			"\r\n",
			weekday(Weekday), Data, month(Mon), Year, Hour, Min, Sec, WEB_CHAR_SET,
			to_url, MODULE_NAME, VERSION, cookie );
#else
	sprintf(header_buff, 
			"HTTP/1.0 302 Moved Temporarily\r\n"
			"Location: http://%s\r\n"
			"Content-Type: text/html; \r\n"
			"Content-Length: 0\r\n\r\n", 
			to_url	);
#endif
  	LOG_NORMAL_INFO("%s", header_buff);
    int len = strlen(header_buff);
    if (socket_send(client_sock, header_buff, len, 10) != len)
    {
    	LOG_PERROR_INFO("send error.");
        return false;
    }
    return true;
}

bool send_response_200(int client_sock, char *path)
{
	int size = 0;
	if (path)
	{
		size = get_file_size(path);
	}

	char content_type[64] = {0};
	if (isStrexit(path, ".htm"))
	{
		sprintf(content_type, "%s", CONTENT_TYPE_TEXT_HTML);
	}
	else if (isStrexit(path, ".css"))
	{
		sprintf(content_type, "%s", CONTENT_TYPE_TEXT_CSS);
	}
	else if (isStrexit(path, ".js"))
	{
		sprintf(content_type, "%s", CONTENT_TYPE_JAVA_SCRIPT);
	}
	else if (isStrexit(path, ".jpg"))
	{
		sprintf(content_type, "%s", CONTENT_TYPE_JPG);
	}
	else if (isStrexit(path, ".png"))
	{
		sprintf(content_type, "%s", CONTENT_TYPE_PNG);
	}
	else
	{
		sprintf(content_type, " ");
	}
	
	char response_header[255];
	sprintf(response_header, 
				"HTTP/1.1 200 OK\r\n"
				"Content-Type: %s; charset=%s\r\n"
				"Content-Length: %d\r\n"
				"Connection: Close\r\n"
				"Server: qchttpd/%s\r\n"
				"\r\n",
				content_type, WEB_CHAR_SET, size, VERSION);
	
	if (path)
	{
		//打开文件
		int file_fd = open_file(path);
		if (file_fd < 0)
		{
			return send_response_500(client_sock);
		}
		
		//发送头部
		if (! socket_send(client_sock, response_header, strlen(response_header), 5))
		{
			close(file_fd);
			return false;
		}
		
		//边读边发送
		int readlen = 0;
		while (size > 0)
		{
			if (size > 255)
				readlen = 255;
			else
				readlen = size;
			
			char *buf = response_header;
			int ret = read(file_fd, buf, readlen);
			if (ret <= 0)
			{
				break;
			}
			
			if (! socket_send(client_sock, buf, ret, 5))
			{
				break;
			}

			size -= ret;
		}

		close(file_fd);
		
		if (size > 0)
		{
			return false;
		}
	}
	else
	{
		return socket_send(client_sock, response_header, strlen(response_header), 5);
	}
	
	return true;
}

bool send_response_with_data_200(int client_sock, char *data)
{
	int size = 0;
	if (data)
	{
		size = strlen(data);
	}

	char response_header[size + 160];
	if (size > 0)
	{
		sprintf(response_header, 
					"HTTP/1.1 200 OK\r\n"
					"Content-Type: text/html; charset=%s\r\n"
					"Content-Length: %d\r\n"
					"Connection: Close\r\n"
					"Server: qchttpd/%s\r\n"
					"\r\n"
					"%s",
					WEB_CHAR_SET, size, VERSION, data);
	}
	else
	{
		sprintf(response_header, 
					"HTTP/1.1 200 OK\r\n"
					"Content-Type: text/html; charset=%s\r\n"
					"Content-Length: %d\r\n"
					"Connection: Close\r\n"
					"Server: qchttpd/%s\r\n"
					"\r\n",
					WEB_CHAR_SET, size, VERSION);

	}
	
	if (socket_send(client_sock, response_header, strlen(response_header), 5) < 0)
		return false;

	return true;
}

bool html_page_process(int client_sock, char *path)
{
	char *p = strchr(path, '?');
	if (p)
		*p = 0;

	if (strlen(path) > HEADER_PATH_LEN)
		return send_response_404(client_sock);

	char *path_tmp = path;
	char file_path[HEADER_PATH_LEN + 10] = {0};
	if (path[0] == '/')
		path_tmp = &path[1];
	
	sprintf(file_path, WEB_DOC_BASE"/%s", path_tmp);
	LOG_WARN_INFO("file_path: %s\n", file_path);
	
	if (! THE_FILE_EXIST(file_path))
	{
		return send_response_404(client_sock);
	}

	return send_response_200(client_sock, file_path);
}

bool http_response_process(int client_sock, char *path, char *cookie, char *post_data)
{
	//送到过滤器
	switch (http_filter(path, cookie, post_data))
	{
		case E_HTTP_FILTER_REDICRECT_LOGIN:
		{
			struct_local_url(path, WEB_SERVER_PORT, WEB_LOGIN_PATH);
			return send_response_302(client_sock, path);
		}
		case E_HTTP_FILTER_REDICRECT_HOME:
		{
			memset(cookie, 0, HEADER_COOKIE_LEN);
			create_cookie(cookie);
			struct_local_url(path, WEB_SERVER_PORT, WEB_HOME_PATH);
			return send_response_with_cookie_302(client_sock, path, cookie);
		}
		case E_HTTP_FILTER_REDICRECT_SET_PASSWD:
		{
			struct_local_url(path, WEB_SERVER_PORT, WEB_SET_PASSWD);
			return send_response_302(client_sock, path);
		}
		case E_HTTP_FILTER_NORMAL:
		{
			break;
		}
		default:
		{
			return false;
		}
	}

	char *data_buf = NULL;
	int ret = cgi_service(path, post_data, (void *)&data_buf);
	if (ret != -9999)
	{
		if (data_buf && strlen(data_buf) > 0)
		{
			LOG_WARN_INFO("----> %s\n", data_buf);

			switch (ret)
			{
				case 302:
				{
					ret = send_response_302(client_sock, data_buf);
					break;
				}
				case 404:
				{
					ret = send_response_404(client_sock);
					break;
				}
				default:
				{
					ret = send_response_with_data_200(client_sock, data_buf);
					break;
				}
			}
			
			free (data_buf);
			data_buf = NULL;
			return ret;
		}

		if (data_buf)
		{
			free (data_buf);
			data_buf = NULL;
		}
		
		if (ret < 0)
		{
			return send_response_with_data_200(client_sock, "{\"res\":\"error\"}");
		}
		return send_response_with_data_200(client_sock, "{\"res\":\"success\"}");
	}
	
	if (data_buf)
	{
		free (data_buf);
		data_buf = NULL;
	}
	
	return html_page_process(client_sock, path);
}
#endif



