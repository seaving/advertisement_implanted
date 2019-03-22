#!/bin/bash

cur_dir=`pwd`
package_dir="$cur_dir/app/"

appv=$1
gccv=$2
platform_name=$3

app_name="app.tar"

date_time=$(date +%Y%m%d-%H%M%S)

release() {
	local app=$1
	local output_dir=$2
	
	local md5=`md5sum $app | awk -F " " '{printf $1}'`
	
	local output="$output_dir/$platform_name/$gccv/$appv/$date_time"
	
	if [ ! -d "$output" ]; then
		mkdir -p "$output"
	fi
	
	local header="###!!!;;;$appv;$gccv;$md5;;;"
	
	local release_app="$output/app-$platform_name-$appv.tar"
	
	echo "release $app to $release_app"
	
	echo "$header" > "$release_app"
	cat "$app" >> "$release_app"
}

if [ "$appv" == "" ]; then
	echo "请输入打包发行的插件版本!"
	exit 0
fi

if [ "$gccv" == "" ]; then
	echo "请输入打包发行的 gcc 版本!"
	exit 0
fi

if [ "$platform_name" == "" ]; then
	echo "请输入打包发行的平台方案!"
	exit 0
fi

echo
echo "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n"
echo "请确认输入的信息: "
echo "插件版本: $appv"
echo "gcc版本: $gccv"
echo "平台方案: $platform_name"
echo "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n"
echo -en "\t 请选择y/N: "
read -n 1 goon
if [ "$goon" != "y" ]; then
	exit 0
fi
echo

cd $package_dir
echo $appv > version.txt
rm -rf $app_name

cp -rf ../start.sh ./
sync

if [ ! -f "./init" ]; then
	echo "严重：不存在 init 程序，严禁打包!!!"
	exit -1
fi

tar -cf $app_name ./* 2>../pack.log
pack_log=`cat ../pack.log`
[ -n "$pack_log" ] && {
	cat $pack_log
	exit
}

release $app_name "/mnt/hgfs/share/route_pri/project/rp/qianchen/package/release/application"

sync

echo "----------------- done ----------------------"
exit 0




