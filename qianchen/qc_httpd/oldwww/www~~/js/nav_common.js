
var _nav_menu = new Array (
	'<li class="navbar-nav" id="nav_home_page" onclick="nav_menu_selected(id, null)"><a href="../html/index.html" style=""><strong style="color:white;">首页</strong></a></li>',
	'<li class="navbar-nav" id="nav_netset_page" onclick="nav_menu_selected(id, nav_network_display)"><a href="../html/network_dhcp.html" style="text-decoration:none;"><strong style="color:white;">网络设置</strong></a></li>',
	'<li class="navbar-nav" id="nav_highset_page" onclick="nav_menu_selected(id, null)"><a href="#highset"><strong style="color:white;">高级设置</strong></a></li>',
	'<li class="navbar-nav" id="nav_abort_page" onclick="nav_menu_selected(id, null)"><a href="#abort"><strong style="color:white;">故障管理</strong></a></li>',
	'<li class="navbar-nav" id="nav_sysset_page" onclick="nav_menu_selected(id, null)"><a href="#sysset"><strong style="color:white;">系统设置</strong></a></li>'
);

function nav_menu_selected(id, sub_nav_fun) {
	var element = null;
	var id_tmp = null;
	for (var i = 0; i < _nav_menu.length; i ++) {
		var offset = _nav_menu[i].indexOf('id="');
		if (offset != -1) {
			var str = _nav_menu[i].substr(offset+4);
			offset = str.indexOf('"');
			if (offset != -1) {
				id_tmp = str.substr(0, offset);
				element = document.getElementById(id_tmp);
				if (element) {
					if (id == id_tmp) {
						element.style.backgroundColor = "#3C79DC";
						if (sub_nav_fun) {
							sub_nav_fun();
						}
					}
					else {
						element.style.backgroundColor = "";
					}
				}
			}
		}
	}
}

function nav_display() {
	document.write('<nav class="navbar navbar-fixed-top" id="navbar_parent" role="navigation" style="border-left:0px;border-top:0px;border-right:0px;border-bottom:1px;width:100%;background-color:#4285F4">');
	document.write('<div class="container-fluid">');
	document.write('<div class="navbar-header">');
	document.write('<a class="navbar-brand" href="http://www.qcwifi.ltd">');
	document.write('<img alt="Brand" style="max-width:100px;margin-top:-7px;" src="../img/logo2.png">');
	document.write('</div>');
	document.write('<ul class="nav navbar-nav navbar-center">');
	for (var i =0; i < _nav_menu.length; i ++) {
		document.write(_nav_menu[i]);
	}
	document.write('</ul>');
	document.write('</div>');
	document.write('</nav>');
}

nav_display();
nav_menu_selected("nav_home_page", null);
