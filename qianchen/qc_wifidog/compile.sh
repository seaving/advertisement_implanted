#!/bin/bash

BIN_NAME="qcdog"

PLATFORMS_NAME="$1"
ROOT_DIR="$2"
TOOL_CHAIN="$3"
TOOL_ARCH="$4"
MODEL_TARGET="$5"

VERSION=`cat src/version.h | grep -w VERSION | awk -F " " '{print $3}' | awk -F '"' '{print $2}'`

[ -z "$VERSION" ] && echo "not define version!" && exit

echo "make clean BIN_NAME=$BIN_NAME"
make clean BIN_NAME=$BIN_NAME
echo "make ROOT_DIR=$ROOT_DIR TOOL_CHAIN=$TOOL_CHAIN TOOL_ARCH=$TOOL_ARCH MODEL_TARGET=$MODEL_TARGET BIN_NAME=$BIN_NAME 2>./complie.log"
make ROOT_DIR=$ROOT_DIR TOOL_CHAIN=$TOOL_CHAIN TOOL_ARCH=$TOOL_ARCH MODEL_TARGET=$MODEL_TARGET BIN_NAME=$BIN_NAME 2>./complie.log

complie_log=`cat ./complie.log`
[ -n "$complie_log" ] && {
	err=`cat ./complie.log | grep -w "error:" | wc -l`
	warn=`cat ./complie.log | grep -w "warning:" | wc -l`
	note=`cat ./complie.log | grep -w "note:" | wc -l`
	if [ "$err" == "0" ] && [ "$warn" == "1" ]
	then
		complie_log1=`echo "$complie_log" | grep "warning: gethostbyname"`
		if [ -n "$complie_log1" ]
		then
			echo > ./complie.log
		else
			echo "[ERROR] $complie_log"
			exit
		fi
	else
		echo "[ERROR] $complie_log"
		exit
	fi
}

rm -rf ../package/app/$BIN_NAME
cp -rf hex/$BIN_NAME ../package/app/
cp -rf apfree.ca ../package/app/
cp -rf apfree.crt ../package/app/
cp -rf apfree.key ../package/app/

rm -rf ../package/app/wifidog
mkdir ../package/app/wifidog

cp -rf internet-offline.html 	../package/app/wifidog/
cp -rf authserver-offline.html 	../package/app/wifidog/
cp -rf wifidog-msg.html 		../package/app/wifidog/
cp -rf wifidog-msg.html.front 	../package/app/wifidog/
cp -rf wifidog-msg.html.midle 	../package/app/wifidog/
cp -rf wifidog-msg.html.rear 	../package/app/wifidog/
cp -rf wifidog-redir.html 		../package/app/wifidog/
cp -rf wifidog-redir.html.front ../package/app/wifidog/
cp -rf wifidog-redir.html.rear 	../package/app/wifidog/

sync
exit
