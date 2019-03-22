#include "includes.h"


/**配置需要过滤的host,不进行connect链接,直接丢弃*/
filter_host_conf filterhostconfig[] = {
	{ "google.com", 0 },
	{ "connectivitycheck.gstatic.com", 0 },
	{ "goo.gl", 0 },
	{ NULL, 0 }
};
filter_host_conf * fhconfig = filterhostconfig;
/****************END***************/

/**配置需要放行的host,不进行广告注入,并且http头不被修改*/
accept_host_conf accepthostconfig[] = {
	//{ "mongodb.sz.zazaj.com", 0 },
	{ "connect.qq.com", 0 },
	//{ "qzone.qq.com", 0 },
	//{ "v.qq.com", 0 },			//qq视频
	//{ "video.qq.com", 0 },		//qq视频
	//{ "y.qq.com", 0 },			//qq音乐
	//{ "music.qq.com", 0 },		//qq音乐
	//{ "info.gamecenter.qq.com", 0 },	//qq游戏
	//{ "play.youku.com", 0 },	//优酷播放
	//{ "open.show.qq.com", 0 }, //手机QQ里面的京东购物
	{ "mmsns.qpic.cn", 0 },	//微信相片查看
	{ "qqvideo.tc.qq.com", 0 }, //微信群视频,qq视频软件
	{ "www.urlshare.cn", 0 },	//qq空间分享
	{ "cgi.kg.qq.com", 0 },	//qq空间全民K歌
	{ "c.isdspeed.qq.com", 0 },
	{ "itunes.apple.com", 0 },
	{ "product.mapi.dangdang.com", 0 }, //当当app放行
	{ "web1.app100698537.twsapp.com", 0 }, //qq游戏大厅启动
	{ NULL, 0 }
};
accept_host_conf * ahconfig = accepthostconfig;
/****************END***************/

/**配置需要放行的uri,不进行广告注入,并且http头不被修改*/
accept_uri_conf accepturiconfig[] = {
	{ "itunes.apple.com", 0 },	//苹果app下载安装
	//{ "/ftn_handler?bmd5=", 0 }, //电脑版本qq离线传输URI中的关键字
	{ NULL, 0 }
};
accept_uri_conf * auconfig = accepturiconfig;

/****************END***************/


