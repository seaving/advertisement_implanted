/**
 * Created by Michael on 2017/5/16.
 */

/**
 *  判断字符串中是否有中文
 */
function hasChinese(str){
    var reg = new RegExp("[\\u4E00-\\u9FFF]+","g");
    if(reg.test(str)) {
        return true;
    }
    return false;
}

/**
 *  判断字符串是否是数字
 */
function isNumber(str) {
    var reg = /^(-)?\d+(\.\d+)?$/;
    if (reg.exec(str) == null || str == "") {
        return false
    } else {
        return true
    }
}

/**
 *  判断字符串中是否有字母
 */
function hasEnglishWord(str) {
	var reg = /[a-z]/;
    if (reg.test(str)) {
        return true;
    }
    return false;
}

/**
 *  退出登录
 */
function logout() {
    layer.confirm('确定要退出登录吗?', {icon: 3, title:'温馨提示'}, function(index){

        $.ajax({
            type: "post",
            data: {},
            url: "../html/index.html?api=logout",
            success: function (result) {
                var tmpSS = JSON.parse(result);
                var res = tmpSS["res"];
                if (res == 'success') {
                    // 退出成功
                    window.location.href = "login.html";
                    
                } else {
                    layer.msg("退出失败", {
                        time: 3000
                    });
                }
                return;
            }
        });
    });
}

/**
 *  重启路由器
 */
function restartRoute() {
    layer.confirm('确定现在重启路由器吗?', {icon: 3, title:'温馨提示'}, function(index){

        $.ajax({
            type: "post",
            data: {},
            url: "../html/index.html?api=restart",
            success: function (result) {
                var tmpSS = JSON.parse(result);
                var res = tmpSS["res"];
                if (res == 'success') {
                    // 成功
                    window.location.href = "login.html";

                } else {
                    layer.msg("退出失败", {
                        time: 3000
                    });
                }
                return;
            }
        });

        layer.msg('路由器正在重启中,请稍后几分钟再次登录', {
            // icon: 1,
            time: 10000 //2秒关闭（如果不配置，默认是3秒）
        }, function(){
            window.location.href = "login.html";
        });
    });
}

function htmlIsOnReady(index) {
    // 头部
    configCommonNavHeader();

    var ul = $('<ul id="dashboard-menu"></ul>');
    $('#sidebar-nav').append(ul);

    var as = ['<a href="index.html"><span>系统信息</span></a>',
                '<a href="network.html"><span>网络设置</span></a>',
                '<a href="wireless.html"><span>无线设置</span></a>',
				'<a href="ap.html"><span>AP 管理</span></a>',
                '<a href="dhcp.html"><span>DHCP服务</span></a>',
                '<a href="devreg.html?api=dev_reg" target="_blank"><span>设备注册</span></a>',
                '<a href="http://www.qcwifi.ltd/rapp/login/index" target="_blank"><span>云台登录</span></a>',
                '<a href="http://www.qcwifi.ltd/rapp/login/index" target="_blank"><span>关于我们</span></a>',
                '<a href="javascript:void(0);" onclick="restartRoute();"><span>重启路由器</span></a>'
                ];

    for (var i = 0; i < as.length; i++) {
        var li;
        if (index == i) {
            li = $('<li class="active">' + as[i] + '</li>');
        } else {
            li = $('<li>' + as[i] + '</li>');
        }
        $('#dashboard-menu').append(li);
    }
}

function configCommonNavHeader() {
    var headerDiv = $('<div id="header" class="navbar-header"></div>');
    $('#nav').append(headerDiv);

    // list button
    var buttons = $('<button id="menu-toggler" class="navbar-toggle" type="button" data-toggle="collapse"></button>');
    $('#header').append(buttons);
    var span1 = $('<span class="sr-only">Toggle navigation</span>');
    var span2 = $('<span class="icon-bar"></span>');
    var span3 = $('<span class="icon-bar"></span>');
    var span4 = $('<span class="icon-bar"></span>');
    $('#menu-toggler').append(span1);
    $('#menu-toggler').append(span2);
    $('#menu-toggler').append(span3);
    $('#menu-toggler').append(span4);

    // logo
    var logo_a = $('<a id="logo" class="navbar-brand" href="index.html"></a>');
    $('#header').append(logo_a);
    var img = $('<img src="../img/logo2.png">');
    $('#logo').append(img);

    // 退出按钮
    var logoutDiv1 = $('<div id="logout1" class="navbar-right-buttons pull-right hidden-lg hidden-md hidden-sm"></div>');
    $('#header').append(logoutDiv1);
    var button1 = $('<button class="btn btn-default" onclick="logout();">退出</button>');
    $('#logout1').append(button1);

    var logoutDiv2 = $('<div id="logout2" class="navbar-right-buttons pull-right visible-lg visible-md visible-sm"></div>');
    $('#nav').append(logoutDiv2);
    var button2 = $('<button class="btn btn-default" onclick="logout();">退出</button>');
    $('#logout2').append(button2);
}