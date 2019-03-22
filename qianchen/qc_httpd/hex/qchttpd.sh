#!/bin/sh

. /usr/share/libubox/jshn.sh

ubus_call() {
	json_init
	local _data="$(ubus -S call "$1" "$2")"
	[ -z "$_data" ] && return
	json_load "$_data"
}

test()
{
	ubus_call "network.interface.wan" status || return
	json_get_var up up
	echo "up=$up"

	json_select route
	json_get_keys route

	echo "keys=$route"
	json_select 1
	json_get_vars target target
	echo "target=$target"
}

get_wan_info() {

	ubus_call "network.interface.wan" status || return

	json_get_vars proto

	json_select ipv4-address
	json_select 1
	json_get_vars address
	json_get_vars mask
	json_select ..
	json_select ..

	json_select route
	json_get_keys route_len
	route_len=`echo $route_len | grep -o " " | wc -l`
	[ -n "$route_len" ] && {
		route_len=$(($route_len + 1))
		route_index=0
		while true; do
			if [ $route_index -lt $route_len ]; then
				break
			fi
			json_select $route_index
			json_get_vars nexthop
			json_select ..
			if [ "$nexthop" != "" ] && [ "$nexthop" != "0.0.0.0" ]; then
				break
			fi
			route_index=$(($route_index + 1))
		done
	}
	json_select ..
	
	json_select dns-server
	json_get_values dns_list
	json_select ..
	
	echo "{\"proto\":\"$proto\", \"address\":\"$address\", \"mask\":$mask, \"dns_list\":\"$dns_list\"}"
}

get_wan_info

json_cleanup



