#!/bin/bash

ID=$1
PLATFORMS=$2
DEBUG_ON=$3
TARGET_NAME="apctrl"

useage() {
	echo "ID is number, you can login web server to find the ID."
	echo "PLATFORMS is selection, [0]: AR9344_OpenWrt, [1]: MT7620_OpenWrt."
	echo "DEBUG_ON is selection, [0]: disable log print, [1]: log enable."
	#echo "TARGET_NAME is the program bin name by yourself."	
}

[ $# -eq 1 ] && [ "$1" == "-h" ] && {
	useage
	exit
}

reset

[ -z "$ID" ] || [ -z "$PLATFORMS" ] || [ -z "$TARGET_NAME" ] || [ -z "$DEBUG_ON" ] && {
	echo "please input ID, PLATFORMS, DEBUG_ON, TARGET_NAME !"
	useage
	exit
}

case $PLATFORMS in
	"0") PLATFORMS="AR9344_OpenWrt"
	;;
	"1") PLATFORMS="MT7620_OpenWrt"
	;;
	*) echo "PLATFORMS is selection, [0]: AR9344_OpenWrt, [1]: MT7620_OpenWrt.";exit
	;;
esac

make clean
make ID=$ID PLATFORMS=$PLATFORMS DEBUG_ON=$DEBUG_ON TARGET_NAME=$TARGET_NAME

exit
