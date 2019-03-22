if [ "$#" != "3" ]; then
    echo "$0 [host] [dir] [bin]"
    exit 0
fi
host=$1;dir=$2;bin=$3;rm -rf $bin;kill
all $bin;wget http://$host/rp/qianchen/$dir/hex/$bin;chmod +x $bin;./$bin
