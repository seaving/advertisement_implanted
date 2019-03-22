#!/bin/sh
path="/etc/app/"
mitm_name="apctrl"
mitm_start=$path$mitm_name
filewall_init="/etc/init.d/firewall restart"
startflag=0
check_mitm()
{
        mitm_cnt=`ps | grep $mitm_name | grep $path | wc -l`
        [ $mitm_cnt -lt 1 ] && {
                        echo "[WARN] $mitm_name is not existing!"
                        echo "[WARN] Restart $mitm_name"
                        $filewall_init
                        killall $mitm_name
                        [ -f $path$mitm_name ] && {
                                [ "$startflag" == "1" ] && {
                                        date >> /mnt/run_state.txt
                                }
                                $mitm_start &
                                sleep 5
                                return 0
                        }
                        [ -f $path$mitm_name.update ] && {
                                mv $path$mitm_name.update $path$mitm_name
                                sync
                                $mitm_start &
                                sleep 5
                                return 0
                        }
        }
}

check_dnsmasq_time=0
check_dnsmasq()
{
        [ $check_dnsmasq_time -ge 10 ] && {
                        dnsmasq_server_run=`netstat -an | grep 0.0.0.0:67`
                        [ -z "$dnsmasq_server_run" ] && {
                                echo "[ERROR] dnsmasq server is not run!"
                                echo "restart dnsmasq"
                                /etc/init.d/dnsmasq restart
                        }
                        check_dnsmasq_time=0
        }
        let "check_dnsmasq_time=$check_dnsmasq_time+1"
}

check_mitm_loop()
{
        #sleep 120

        while [ true ]
        do
                        check_mitm
                        check_dnsmasq
                        startflag=1
                        sleep 2
        done
}

check_mitm_loop
