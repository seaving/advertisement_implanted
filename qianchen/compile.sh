#!/bin/bash

CUR_DIR=`pwd`
compile_file=$CUR_DIR/compile_file.log

releaseVersion=""
program_dir=""

gccVersion=""
PLATFORMS_NUM=""
PLATFORMS_NAME=""

ROOT_DIR=""
TOOL_CHAIN=""
TOOL_ARCH=""
MODEL_TARGET=""


echo_item() {
	echo -e "\t $1"
}

menu() {
	local goon="N"
	clear
	echo
	echo
	echo_item "------- 前辰插件编译脚本 -------\n"
	echo
	echo_item "根据方案选择编译："
	echo_item "0. ar7xxx_ar9xxx"
	echo_item "1. mt7620"
	echo_item "2. x86"
	echo_item "A. ALL"
	echo_item "E. exit"

	echo_item "0. Exit program\n\n"
	echo -en "\t 请选择: "
	read -n 1 PLATFORMS_NUM
	echo
}

#gccVersion 格式 gcc-{arch}-{version}-{libc}
init_compile_env() {
	case $PLATFORMS_NUM in
		"0")
		echo "ar7xxx_ar9xxx"
		PLATFORMS_NAME="ar7xxx_ar9xxx"
		ROOT_DIR="/home/seaving/work/router_firmware/openwrt"
		TOOL_CHAIN="toolchain-mips_24kc_gcc-7.3.0_musl"
		TOOL_ARCH="mips-openwrt-linux-musl-"
		MODEL_TARGET="target-mips_24kc_musl"
		gccVersion="gcc-mips_24kc_gcc-7.3.0-musl"
		;;
		"1")
		echo "mt7620"
		PLATFORMS_NAME="mt7620"
		ROOT_DIR="/home/seaving/work/router_firmware/openwrt"
		TOOL_CHAIN="toolchain-mipsel_24kc_gcc-7.3.0_musl"
		TOOL_ARCH="mipsel-openwrt-linux-musl-"
		MODEL_TARGET="target-mipsel_24kc_musl"
		gccVersion="gcc-mipsel_24kc-7.3.0-musl"
		;;
		"2")
		echo "x86"
		PLATFORMS_NAME="x86"
		ROOT_DIR="/home/seaving/work/router_firmware/lede"
		TOOL_CHAIN="toolchain-mipsel_24kc_gcc-5.4.0_musl-1.1.16"
		TOOL_ARCH="mipsel-openwrt-linux-musl-"
		MODEL_TARGET="target-mipsel_24kc_musl-1.1.16"
		gccVersion="gcc-mipsel_24kc-7.3.0_musl"
		;;
		"A")
		echo "ALL"
		;;
		*)
		echo "PLATFORMS is selection, [0]: AR9344, [1]: MT7620, [2]: x86."
		exit
		;;
	esac
}

compile() {
	local PROGRAM_DIR=$1

	echo "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx"
	echo "x                                           x"
	echo "     compile - $PLATFORMS_NAME - $PROGRAM_DIR "
	echo "x                                           x"
	echo "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx"
	
	cd $PROGRAM_DIR
	./compile.sh $PLATFORMS_NAME $ROOT_DIR $TOOL_CHAIN $TOOL_ARCH $MODEL_TARGET
	
	complie_log=`cat ./complie.log`
	[ -n "$complie_log" ] && {
		exit
	}
	
	cd ..
}

compile_switch() {
	program_dir=$1
	if [ -n "$program_dir" ]; then
		compile "$program_dir"
		exit
	else
		compile apctrl
		compile qc_wifidog
		compile init
		compile monitor
		compile QC
		compile qc_httpd
		compile ssh
	fi

	sync

	cd package
	./pack.sh "$releaseVersion" "$gccVersion" "$PLATFORMS_NAME"
	pack_log=`cat ./pack.log`
	[ -n "$pack_log" ] && {
		exit
	}
	cat app/version.txt

	sync
	cd ..
}

compile_all() {
	PLATFORMS_NUM=$1
	init_compile_env
	compile_switch
}

if [ "$1" == "-h" ]; then
	echo "$0 -hrs"
	echo "-h help"
	echo "-r [发行版本] 编译所有程序，并且打包"
	echo "-s  [目标程序] 编译指定的目标程序，不打包"
	exit
elif [ "$1" == "-r" ]; then
	releaseVersion="$2"
	[ -z "$releaseVersion" ] && {
		echo "请输入打包发行版本!"
		exit
	}
elif [ "$1" == "-s" ]; then
	program_dir="$2"
	[ -z "$program_dir" ] && {
		echo "请输入指定的目标程序!"
		exit
	}
else
	echo "$0 -hrs"
	echo "-h help"
	echo "-r [发行版本] 编译所有程序，并且打包"
	echo "-s  [目标程序] 编译指定的目标程序，不打包"
	exit
fi

menu
init_compile_env

if [ -n "$program_dir" ]; then
	compile_switch "$program_dir"
	exit
fi

if [ "$PLATFORMS_NUM" == "A" ]; then
	compile_all "0"
	compile_all "1"
	compile_all "2"
elif [ -n "$PLATFORMS_NUM" ]; then
	compile_switch
fi

exit
