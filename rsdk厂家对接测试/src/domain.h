#ifndef __DOMAIN_H__
#define __DOMAIN_H__	1


#define	REQUEST_LINK_URL	"http://adplugin.sz.zazaj.com/api.php?action=links"
#define REQUEST_LINK_PORT	80

int insert_domain_conver(char * s_url, char * c_url);
int init_domain_map();
char * get_conver_url(char * s_url);
int del_domain_conver(char * s_url);



#endif

