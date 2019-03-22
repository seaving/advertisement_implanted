#ifndef __AUTH_CLIENT_H__
#define __AUTH_CLIENT_H__

#define AUTH_CLIENT_SIZE	255

#define TMP_ACCEPT		false
#define FORMA_ACCEPT	true

typedef enum
{
	E_TMP_ACCEPT = 0,
	E_SETED,
	E_NOT_FOUND,
	E_NOT_MATCH
} set_res_t;

typedef struct
{
	char *mac;
	char *ip;
	int start_time;
	int validity_time;
	bool flag;
} auth_client_t;

void auth_client_init();

void auth_client_free_all();

int auth_client_add(char *ip, char *mac, int start_time, int validity_time, bool flag);

int auth_client_find(char *ip, char *mac);

int auth_client_scan();

int get_cli_name(char *ip, char *mac, char *name);

int get_auth_client(char *ip, auth_client_t *auth);

set_res_t is_auth_client_set(char *ip, char *mac);

int create_auth_client_scan_thread();

#endif


