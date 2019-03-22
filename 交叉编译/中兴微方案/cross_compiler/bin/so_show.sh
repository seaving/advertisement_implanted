#!/bin/sh

#######################################################################
# 脚本说明
# *********************************************************************
### 1. 脚本功能：在host环境获取目标机动态程序或动态库的依赖库名称
### 2. 脚本调用方式
###    ./so_show.sh TARGET=/opt/zte/x86_gcc412_glibc250/  usrapp1 usrapp2 usr-dir1 usr-dir2"
###    其中，TARGET为工具链根目录
###          usrapp*为动态程序或.so(usrapp个数不限)
###          usr-dir*为动态程序或.so所在目录（usr-dir个数不限,只处理usr-dir*下的usrapp*，不递归处理其子目录）
###          为了提高效率，同一动态程序或.so不要既在usrapp*中，又在usr-dir*（同一动态程序或.so存在多个实例，会被多次执行依赖库检查，影响效率）
#######################################################################

##############################################################
##### 参数错误信息提示
##############################################################
ERR_INFO()
{

	echo ""
	echo "############################################################################################"
	echo "so_show用于获取目标机动态程序或动态库的依赖库名称"
	echo "使用方法:"
	echo "./so_show.sh TARGET=/opt/zte/x86_gcc412_glibc250/  usrapp1 usrapp2 usr-dir1 usr-dir2"
	echo "/opt/zte/x86_gcc412_glibc250/bin/so_show.sh usrapp1 usrapp2 usr-dir1 usr-dir2"
	echo "       " 
	echo "TARGET             为工具链根目录，在工具链的bin目录下执行so_show时可以省略该参数"
	echo "usrapp*            为动态程序或.so"
	echo "usr-dir*           为动态程序或.so所在目录（只处理usr-dir*下的usrapp*，不递归处理其子目录）"
	echo ""
	echo "为了提高效率，同一动态程序或.so不要既在usrapp*中，又在usr-dir*中"
	echo "############################################################################################"
	echo ""
	if [ $ERR -eq 0 ]; then 
		exit 0
	else
		exit 1
	fi
}


#######################################################################
# 获取一个libname
#######################################################################
get_libname()
{
  start_token=$@	
	while [ "${start_token}" != "" ]
	do
		start_token=${start_token#*[}                     
		token=${start_token%%]*}     
		start_token=${start_token#*]}	
		
		((index=0))
		lib_name=${libs_name[${index}]}
		while [ "$index" -lt "$lib_count" -a "${token}" != "$lib_name" ]
		do		
			lib_name=${libs_name[${index}]}
			((index++))
		done
		if [ "$index" == "$lib_count" -a "${token}" != "$lib_name" ];then
			libs_name[${lib_count}]=${token}
			((lib_count++))
		fi
	done
}

#######################################################################
# 递归调用，获取所有依赖的库名
#######################################################################
get_need()
{	
#	lib_name=`find $TARGET -name $1`
	index=0
	while [ "$index" -lt "$so_num" ] 
	do
		if [ "${all_so_name[${index}]}" = "$1" ]; then
			lib_name=${all_so[${index}]}
			index=so_num
		fi
		((index++))
	done
	if [ "$lib_name" = "" ]; then
		echo "WALLING: 库$1未找到，请确认其是否用户私有库"
	fi
	
	lib_name=`$READELF -d ${lib_name} | grep NEEDED`
	get_libname ${lib_name}
	
	((finish_index=finish_index+1))
	if [ "$finish_index" -lt "$lib_count" ]; then		
		get_need ${libs_name[${finish_index}]}
	fi		
}

parse_deps()
{
	deps_index=0
	while [ $deps_index -lt $input_num ] 
	do		
		file=${input_file[$deps_index]}
		string="`$READELF -d ${file} | grep NEEDED`"
		if [ "${string}" != "" ]; then	
		  exe=`file ${file} | grep executable`
			target_lib_type=`$READELF -s ${file} | grep __uClibc_main`
			if [ "${target_lib_type}" = "" ]; then
				target_lib_type=glibc
			else
				target_lib_type=uClibc
			fi
			if [ "${target_lib_type}" != "${lib_type}" -a "$exe" != "" ]; then
				echo "WALLING: c库类型不匹配: ${file}=${target_lib_type}"		
				echo "                        工具链库=${lib_type}"	
			else					
				dlopen=`$READELF -s ${file} | grep dlopen`
				if [ "$dlopen" != "" ]; then
					echo "WALLING: ${file}中使用了dlopen打开so库"	
					echo "         请在源文件中确认打开的什么库，建议使用脚本进一步检查dlopen打开库的依赖库"	
				fi
				
				target_arch=`$READELF -h ${file} | grep Machine`	
				case $target_arch in 
					*ARM*)
						target_arch=ARM
						target_data=`$READELF -h ${file} | grep "big endian"` 
						if [ "$target_data" != "" ]; then
						target_arch+=EB		
						fi
						;;
					*MIPS*)
						target_arch=MIPS
#						`$READELF -h ${file} | grep ELF64` && target_arch+=64
						target_class=`$READELF -h ${file} | grep ELF64` 
						if [ "$target_class" != "" ]; then
							target_arch+=64
						fi
						target_data=`$READELF -h ${file} | grep "little endian"` 
						if [ "$target_data" != "" ]; then
						target_arch+=EL		
						fi
						;;
					*PowerPC*)
						target_arch=Powerpc;;
					*386*)
						target_arch=X86;;
					*X86-64*) 
						target_arch=X86-64;;
					*)
						target_arch=unkown;;
				esac			
				if [ "${target_arch}" != "${lib_arch}" ]; then
					echo "WALLING: arch不匹配: ${file}=${target_arch}"		
					echo "                     工具链库=${lib_arch}"				
				else
					target_obj+=" ${file##*/}"
					target_names[${target_num}]=${file}
					get_libname $string		
					((target_num++))
				fi
			fi			
		fi		
		((deps_index++))			
	done
}

#######################################################################
# 打印所有依赖库名
#######################################################################
print_libname()
{
	print_index=0
	if [ $lib_count -gt "0" ]; then
		echo 
		echo "===$target_obj 依赖so ==="
		while [ "$print_index" -le "$lib_count" ]
		do
			echo ${libs_name[${print_index}]}
			((print_index++))
		done	
	else
		echo 
		echo "=== 输入参数中没有有效动态程序或动态库 ==="
		echo
	fi
}

check_arch()
{
	index=0
	
	while [ "$index" -lt "$so_num" ] 
	do
		lib_name=${all_so_name[${index}]}
		if [ "${lib_name:0:7}" = "libc.so" ]; then
			lib_name=${all_so[${index}]}
			index=so_num
		fi
		((index++))
	done
	
	if [ "$lib_name" != "" ]; then
		lib_arch=`$READELF -h ${lib_name} | grep Machine`
		case $lib_arch in 
			*ARM*)
				lib_arch=ARM
				lib_data=`$READELF -h ${lib_name} | grep "big endian"` 
				if [ "$lib_data" != "" ]; then
					lib_arch+=EB		
				fi
				;;
			*MIPS*)
				lib_arch=MIPS
#				`$READELF -h ${lib_name} | grep ELF64` && lib_arch+=64
				lib_class=`$READELF -h ${lib_name} | grep ELF64` 
				if [ "$lib_class" != "" ]; then
					lib_arch+=64
				fi
				lib_data=`$READELF -h ${lib_name} | grep "little endian"` 
				if [ "$lib_data" != "" ]; then
					lib_arch+=EL		
				fi
				;;		
			*PowerPC*)
				lib_arch=Powerpc;;
			*386*)
				lib_arch=X86;;
			*X86-64*) 
				lib_arch=X86-64;;
			*)
				lib_arch=unkown;;
		esac
		lib_type=`$READELF -s ${lib_name} | grep __uClibc_main`
		if [ "$lib_type" = "" ]; then
			lib_type=glibc
		else
			lib_type=uClibc
		fi
	fi	
}

get_all_so()
{
	echo "正在搜索.so  ......"
	temp=`find ${TARGET} -name "*.so.*"`
	temp=`echo ${temp}`

	so_num=0
	all_so[${so_num}]=${temp%% *}
	so_name=${all_so[${so_num}]}
	all_so_name[${so_num}]=${so_name##*/}
	((so_num++))
	
	while [ "${temp}" != "${temp#* }" ]
	do
		temp=${temp#* }                     
		all_so[${so_num}]=${temp%% *}		
		so_name=${all_so[${so_num}]}
		all_so_name[${so_num}]=${so_name##*/}
		((so_num++))			
	done	
}

#######################################################################
# 主入口
#######################################################################

export LANG=zh_CN.GBK

finish_index=0
lib_count=0

target_obj=
target_num=0
path_num=0
input_num=0

#错误变量，当前只有1和0，0代表正常，1代表错误。后续错误分支多时，可以将ERR作为错误号
ERR=0

#没有任何参数时，直接输出帮助信息
if [ $# -eq 0 ];then
	ERR=0
	ERR_INFO
fi
while [ $# -gt "0" ] 
do
	param=$1
	if [ "TARGET=" = "${param:0:7}" ];then
		TARGET=${param#*TARGET=}
		if [ -d ${TARGET} ]; then
			TARGET=`cd ${TARGET}; pwd`
		else
			echo "target路径错误，请确认工具链路径"
			ERR=1
			ERR_INFO	
		fi
	else
		if [ -d ${param} ];then
			param=`cd ${param}; pwd`
			param=${param%/}
			for file in ${param}/*
			do
				if [ -f ${file} ]; then
  				ELF_FILE=`file -b ${file} | grep ELF`
  				if ! [ "_${ELF_FILE}" = "_" ]; then
    				input_file[${input_num}]=${file}
    				((input_num++))
  				fi
				fi
			done
		else
			if [ -f "${param}" ];then
				ELF_FILE=`file -b ${param} | grep ELF`
			else
				echo "请使用正确方式调用so_show"
				ERR=1
				ERR_INFO
			fi
  		if ! [ "_${ELF_FILE}" = "_" ]; then
    		input_file[${input_num}]=${param}
    		((input_num++))		
  		fi		
		fi
	fi
	shift
done

if [ "${TARGET}" = "" ]; then	
	program_name=$0
	bin_dir=${program_name%/*}
	if [ "$bin_dir" = "$program_name" ]; then
		program_patch=`pwd`
		bin_dir=${program_patch##*/}
	else
		program_patch=${program_name%/*}
		program_patch=`cd $program_patch; pwd`
		bin_dir=${program_patch##*/}
	fi
	
	exist_readelf=`ls ${program_patch} | grep readelf`
	if [ "$bin_dir" = "bin" -a "$exist_readelf" != "" ]; then
		TARGET=`cd ${program_patch}/.. ; pwd`
	else
		echo "请指定TARGET，或者确认是否在工具链的bin目录下执行so_show.sh"
		ERR=1
		ERR_INFO
	fi
fi

READELF=`find ${TARGET} -name "*-readelf*"`
if [ $READELF = "" ]; then
	READELF=readelf
fi

get_all_so

check_arch

parse_deps

if [ "${libs_name[${finish_index}]}" != "" ]; then
	get_need ${libs_name[${finish_index}]}
fi	

print_libname
