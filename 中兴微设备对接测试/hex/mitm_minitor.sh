#! /bin/sh
#  author caoxin
#  time  2012-10-10 
#  program : 判断进行是否存在，并重新启动

mitm_name="/etc/bin/mproxy_arm"
filewall_init="/etc/init.d/firewall restart"

check_mitm()
{
	mitm_pid=`ps | grep $mitm_name`
	[ -z "$mitm_pid" ] && {
		echo "[WARN] $mitm_name is not existing!"
		echo "[WARN] Restart $mitm_name"
		
		$filewall_init
		. $mitm_name
	}
}

main()
{
	while [ true ]
	do
		check_mitm
		sleep 1
	done
}

main

exit 0