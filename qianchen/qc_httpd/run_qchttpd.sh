rm -rf www.gz;sync;wget http://$1/rp/qianchen/qc_httpd/www.gz;sync;
rm qchttpd;sync;killall qchttpd;wget http://$1/rp/qianchen/qc_httpd/hex/qchttpd;sync;chmod +x qchttpd;./qchttpd
