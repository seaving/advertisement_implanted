#!/bin/sh

#init和qchttpd是放到/etc/app中

#其他程序全部放到后台，由init程序开机运行下载

/etc/init.d/firewall restart

#一定要先判断下是否有更新，有更新才进行删除操作，
#如果不进行预判，直接删除将导致程序丢失
if [ -f "/tmp/app/init" ]; then
	rm -rf /etc/app/init
	mv /tmp/app/init /etc/app/
	chmod +x /etc/app/init
fi

if [ -f "/tmp/app/qchttpd" ]; then
	rm -rf /etc/app/qchttpd
	mv /tmp/app/qchttpd /etc/app/
	chmod +x /etc/app/qchttpd
fi

if [ -f "/tmp/app/www.gz" ]; then
	rm -rf /etc/app/www.gz
	mv /tmp/app/www.gz /etc/app/
	chmod 777 /etc/app/www.gz
fi

#sync
sleep 1

killall qchttpd;sleep 1;/etc/app/qchttpd &

killall apctrl;sleep 1;chmod +x /tmp/app/apctrl;/tmp/app/apctrl &
sleep 5
killall ssh;sleep 1;chmod +x /tmp/app/ssh;/tmp/app/ssh &
sleep 5
killall qcdog;sleep 1;chmod +x /tmp/app/qcdog;/tmp/app/qcdog &
sleep 5
killall monitor;sleep 1;chmod +x /tmp/app/monitor;/tmp/app/monitor &

exit
