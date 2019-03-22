#include "includes.h"

static char _wan_name[32] = {0};
static char *_lan_name = LAN_DEV;

void _get_netdev_info_call_(char *buf, void *info)
{
	if (info && buf)
	{
		memcpy((char *)info, buf, strlen(buf));
	}
}

int get_netdev_info(char *cmd, char *info)
{
	if (popen_cmd(cmd, _get_netdev_info_call_, info) < 0)
	{
		return -1;
	}
	
	LOG_WARN_INFO("netdev info: %s\n", info);

	return 0;
}

void init_netdev()
{
	get_netdev_info("uci get network.wan.ifname", _wan_name);
}

char *get_netdev_wan_name()
{
	return _wan_name;
}

char *get_netdev_lan_name()
{
	return _lan_name;
}

#if 0
long get_free_ddr_Size()
{
	struct sysinfo s_info;

	int error;
	error = sysinfo(&s_info);
	if (error)
		return -1;
	
	return (long)s_info.freeram;
}

long get_total_ddr_Size()
{
	struct sysinfo s_info;

	int error;
	error = sysinfo(&s_info);
	if (error)
		return -1;
	
	return (long)s_info.totalram;
}

long get_used_ddr_Size()
{
	struct sysinfo s_info;

	int error;
	error = sysinfo(&s_info);
	if (error)
		return -1;
	
	return (long)s_info.totalram - (long)s_info.freeram;
}
#endif

void _get_cpu_info_call_(char *buf, void *cpu)
{
	char *p = strchr(buf, ':');
	if (p)
	{
		p ++;
		if (cpu)
		{
			memcpy((char *)cpu, p, strlen(p));
		}
	}
}

int get_cpu_info(char *cpu)
{
#ifdef _X86_
	if (popen_cmd("cat /proc/cpuinfo | grep \"model name\"", 
			_get_cpu_info_call_, cpu) < 0)
	{
		return -1;
	}
#else
	if (popen_cmd("cat /proc/cpuinfo | grep \"system type\"", 
			_get_cpu_info_call_, cpu) < 0)
	{
		return -1;
	}
#endif
	return 0;
}

void _get_system_version_call_(char *buf, void *version)
{
	if (version)
	{
		memcpy((char *)version, buf, strlen(buf));
	}
}

int get_system_version(char *version)
{
	if (popen_cmd("cat /proc/version", _get_system_version_call_, version) < 0)
	{
		return -1;
	}

	return 0;
}

void _get_hostname_call_(char *buf, void *hostname)
{
	if (hostname)
	{
		memcpy((char *)hostname, buf, strlen(buf));
	}
}

int get_hostname(char *hostname)
{
	if (popen_cmd("uci get system.@system[0].hostname", _get_hostname_call_, hostname) < 0)
	{
		return -1;
	}

	return 0;
}

int set_hostname(char *hostname)
{
	char cmd[255] = {0};
	EXECUTE_CMD(cmd, "uci set system.@system[0].hostname=%s", hostname);
	return 0;
}

void _get_machine_call_(char *buf, void *machine)
{
	if (buf && strlen(buf) > 0 && machine)
	{
		memcpy((char *)machine, buf, strlen(buf));
	}
	else
	{
		sprintf((char *)machine, "unkown");
	}
}

int get_machine(char *machine)
{
	if (popen_cmd("cat /proc/cpuinfo | grep \"machine\" | awk -F \":\" '{print $2}'", 
			_get_machine_call_, machine) < 0)
	{
		return -1;
	}

	return 0;
}

int ip_to_int(char *ip, unsigned int *nip)
{
	char strIp[32] = {0};
	char *pIp = strIp;
	memcpy(pIp, ip, strlen(ip));
	LOG_WARN_INFO("%s  %s\n", ip, pIp);
	
	unsigned char __ip0 = 0;
	unsigned char __ip1 = 0;
	unsigned char __ip2 = 0;
	unsigned char __ip3 = 0;
	char *p = strchr(pIp, '.');
	if (p)
	{
		*p = 0;
		__ip0 = (unsigned char)atoi(pIp);
		LOG_WARN_INFO("%d\n", __ip0);
		pIp = p + 1;
		p = strchr(pIp, '.');
		if (p)
		{
			*p = 0;
			__ip1 = (unsigned char)atoi(pIp);
			LOG_WARN_INFO("%d\n", __ip1);
			pIp = p + 1;
			p = strchr(pIp, '.');
			if (p)
			{
				*p = 0;
				__ip2 = (unsigned char)atoi(pIp);
				LOG_WARN_INFO("%d\n", __ip2);
				pIp = p + 1;
				__ip3 = (unsigned char)atoi(pIp);
				LOG_WARN_INFO("%d\n", __ip3);

				*nip = (unsigned int)((unsigned int)__ip0 << 24) 
						| (unsigned int)((unsigned int)__ip1 << 16) 
						| (unsigned int)((unsigned int)__ip2 << 8) 
						| (unsigned int)((unsigned int)__ip3 << 0);
				
				return 0;
			}
		}
	}

	return -1;
}

void get_segment(char *ipseg, char *dev)
{
	char ip[32] = {0};	
	char mask[32] = {0};
	
	get_dev_ip(ip, dev);
	get_dev_mask(mask, dev);
	
	unsigned int nip = 0;
	if (ip_to_int(ip, &nip) < 0)
	{
		LOG_ERROR_INFO("ip_to_int ERROR!\n");
		return ;
	}
	unsigned int nmask = 0;
	if (ip_to_int(mask, &nmask) < 0)
	{
		LOG_ERROR_INFO("ip_to_int ERROR!\n");
		return ;
	}

	int seg = nip & nmask;
	sprintf(ipseg, "%d.%d.%d.%d", 
		(unsigned char)(seg >> 24 & 0xff), 
		(unsigned char)(seg >> 16 & 0xff), 
		(unsigned char)(seg >> 8 & 0xff), 
		(unsigned char)(seg >> 0 & 0xff));
	
	LOG_NORMAL_INFO("segment: %s\n", ipseg);
}

int get_segment_count(char *dev)
{
	char mask[32] = {0};
	get_dev_mask(mask, dev);
	unsigned int nmask = 0;
	if (ip_to_int(mask, &nmask) < 0)
	{
		LOG_ERROR_INFO("ip_to_int ERROR!\n");
		return -1;
	}

	unsigned int sub_ip_cnt = ~nmask;
	return (sub_ip_cnt / 256);
}

void get_segment2(char *ipseg, char *dev)
{	
	char ip[32] = {0};
	char mask[32] = {0};
	
	get_dev_ip(ip, dev);
	get_dev_mask(mask, dev);
	
	unsigned int nip = 0;
	if (ip_to_int(ip, &nip) < 0)
	{
		LOG_ERROR_INFO("ip_to_int ERROR!\n");
		return ;
	}
	
	unsigned int nmask = 0;
	if (ip_to_int(mask, &nmask) < 0)
	{
		LOG_ERROR_INFO("ip_to_int ERROR!\n");
		return ;
	}

	int i;
	for (i = 0; i < 32; i ++)
	{
		if ((nmask << i) & 0x80000000)
			continue;
		break;
	}
	
	int seg = nip & nmask;
	sprintf(ipseg, "%d.%d.%d.%d/%d", 
		(unsigned char)(seg >> 24 & 0xff), 
		(unsigned char)(seg >> 16 & 0xff), 
		(unsigned char)(seg >> 8 & 0xff), 
		(unsigned char)(seg >> 0 & 0xff), i);
	LOG_NORMAL_INFO("segment2: %s\n", ipseg);
}

void get_segment3(char *ipseg, char *dev)
{
	char ip[32] = {0};	
	char mask[32] = {0};
	
	get_dev_ip(ip, dev);
	get_dev_mask(mask, dev);
	
	unsigned int nip = 0;
	if (ip_to_int(ip, &nip) < 0)
	{
		LOG_ERROR_INFO("ip_to_int ERROR!\n");
		return ;
	}
	unsigned int nmask = 0;
	if (ip_to_int(mask, &nmask) < 0)
	{
		LOG_ERROR_INFO("ip_to_int ERROR!\n");
		return ;
	}

	int seg = nip & nmask;
	sprintf(ipseg, "%d.%d.%d.", 
		(unsigned char)(seg >> 24 & 0xff), 
		(unsigned char)(seg >> 16 & 0xff), 
		(unsigned char)(seg >> 8 & 0xff));
	
	LOG_NORMAL_INFO("segment: %s\n", ipseg);
}

void _get_wifi_client_count_(char *buf, void *cnt)
{
	if (buf && cnt)
	{
		memcpy(cnt, buf, strlen(buf));
	}
}

/*int get_wifi_client_count()
{
	char count[20] = {0};
	char cmd[256] = {0};
	
	snprintf(cmd, 255, "grep -w \"%s\" /proc/net/arp | grep -w 0x2 | wc -l", LAN_DEV);
	popen_cmd(cmd, _get_wifi_client_count_, count);
	return atoi(count);
}
*/

int set_dev_id(char *devNo)
{
	unlink("/etc/config/devNo");
	create_file("/etc/config/devNo");
	return write_file("/etc/config/devNo", devNo, strlen(devNo));
}

int get_dev_id(char *devNo)
{
	read_file("/etc/config/devNo", devNo, 32);

	char *p = strchr(devNo, '\r');
	if (p) *p = '\0';
	p = strchr(devNo, '\n');
	if (p) *p = '\0';

	return strlen(devNo);
}

int get_firmware_version(char *fwv)
{
	read_file("/etc/app/firmware_version", fwv, 64);

	char *p = strchr(fwv, '\r');
	if (p) *p = '\0';
	p = strchr(fwv, '\n');
	if (p) *p = '\0';

	return strlen(fwv);
}

int get_gcc_version(char *gccv)
{
	read_file("/etc/app/gcc_version", gccv, 64);

	char *p = strchr(gccv, '\r');
	if (p) *p = '\0';
	p = strchr(gccv, '\n');
	if (p) *p = '\0';

	return strlen(gccv);
}

int get_firmware_id(char *fwid)
{
	read_file("/etc/app/firmware_id", fwid, 128);

	char *p = strchr(fwid, '\r');
	if (p) *p = '\0';
	p = strchr(fwid, '\n');
	if (p) *p = '\0';

	return strlen(fwid);
}

int get_plugin_version(char *pv)
{
	read_file("/tmp/app/version", pv, 64);

	char *p = strchr(pv, '\r');
	if (p) *p = '\0';
	p = strchr(pv, '\n');
	if (p) *p = '\0';

	return strlen(pv);
}

int get_model(char *model, int model_buf_size)
{
	if (read_file("/etc/app/model", model, model_buf_size - 1) <= 0)
	{
		return -1;
	}
	
	char *p = strchr(model, '\r');
	if (p) *p = '\0';
	p = strchr(model, '\n');
	if (p) *p = '\0';
	return strlen(model);
}


