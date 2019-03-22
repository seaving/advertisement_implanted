#!/bin/bash

PLATFORMS=$1
DEBUG_ON=$2
VERSION=`cat src/version.h | grep -w VERSION | awk -F " " '{print $3}' | awk -F '"' '{print $2}'`
TARGET_NAME=$3
useage() {
	echo "PLATFORMS is selection, [0]: AR9344, [1]: MT7620, [2]: PC, [3]: x86."
	echo "DEBUG_ON is selection, [0]: disable log print, [1]: log enable."
	echo "TARGET_NAME is the program bin name by yourself."
}

[ $# -eq 1 ] && [ "$1" == "-h" ] && {
	useage
	exit
}

[ -z "$VERSION" ] && echo "not define version!" && exit

[ -z "$PLATFORMS" ] || [ -z "$TARGET_NAME" ] || [ -z "$DEBUG_ON" ] && {
	echo "please input PLATFORMS, DEBUG_ON, TARGET_NAME!"
	useage
	exit
}

case $PLATFORMS in
	"0") PLATFORMS="ar9344"
	;;
	"1") PLATFORMS="mt7620"
	;;
	"2") PLATFORMS="pc"
	;;
	"3") PLATFORMS="x86"
	;;
	*) echo "PLATFORMS is selection, [0]: AR9344, [1]: MT7620, [2]: PC, [3]: x86.";exit
	;;
esac

rm -rf makefile
sync
cp -rf build/makefile-$PLATFORMS makefile
sync

make clean
make PLATFORMS=$PLATFORMS DEBUG_ON=$DEBUG_ON TARGET_NAME=$TARGET_NAME VERSION=$VERSION 2>./complie.log

complie_log=`cat ./complie.log`
[ -n "$complie_log" ] && {
	echo "$complie_log"
	exit
}

#rm -rf ../package/app/$TARGET_NAME
#cp -rf hex/$TARGET_NAME ../package/app/

sync
exit
