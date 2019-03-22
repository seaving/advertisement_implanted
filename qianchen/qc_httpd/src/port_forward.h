#ifndef __PORT_FORWARD_H__
#define __PORT_FORWARD_H__



int get_port_forward_total();

int get_port_forward(int offset, char *port_forward);

int add_port_forward(char *name, char *src_port, char *dst_port, char *dst_ip);

void delete_port_forward(char *name);

#endif


