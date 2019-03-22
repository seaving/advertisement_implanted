#include "includes.h"

static char _wan_name[32] = {0};
static char *_lan_name = "br-lan";

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
	read_file("/tmp/app/version.txt", pv, 64);

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

