
var _nav_network_menu = new Array (
	'<li id="nav_network_wan_page" onclick="nav_network_menu_selected(id)"><a href="network_dhcp.html"><strong>外网设置</strong></a></li>',
	'<li id="nav_network_lan_page" onclick="nav_network_menu_selected(id)"><a href="network_lan.html"><strong>内网设置</strong></a></li>',
	'<li id="nav_network_wireless_page" onclick="nav_network_menu_selected(id)"><a href="network_wireless.html"><strong>无线设置</strong></a></li>',
	'<li id="nav_network_QoS_page" onclick="nav_network_menu_selected(id)"><a href="network_qos.html"><strong>QoS智能限速</strong></a></li>'
);

function nav_network_menu_selected(id) {
	var element = null;
	var id_tmp = null;
	for (var i = 0; i < _nav_network_menu.length; i ++) {
		var offset = _nav_network_menu[i].indexOf('id="');
		if (offset != -1) {
			var str = _nav_network_menu[i].substr(offset+4);
			offset = str.indexOf('"');
			if (offset != -1) {
				id_tmp = str.substr(0, offset);
				element = document.getElementById(id_tmp);
				if (element) {
					if (id == id_tmp) {
						element.style.backgroundColor = "#EEEEEE";
					}
					else {
						element.style.backgroundColor = "";
					}
				}
			}
		}
	}
}

function nav_network_display() {
	var insertData = '<div class="container" style="background-color:white;width:100%">' 
		+ '<ul class="nav nav-tabs col-md-offset-3" style="border-left:0px;border-top:0px;border-right:0px;border-bottom:1px">'
		+ '<div class="navbar-header">'
		+ '<ul class="nav navbar-nav navbar-center">';
	for (var i =0; i < _nav_network_menu.length; i ++) {
		insertData += _nav_network_menu[i];
	}
	insertData += '</ul>'
			+ '</ul>'
			+ '</div>';
	document.getElementById("navbar_parent").insertAdjacentHTML("beforeEnd", insertData);
}

function nav_network_start(nav_network_menu_selected_id) {
	nav_menu_selected("nav_netset_page", nav_network_display);
	nav_network_menu_selected(nav_network_menu_selected_id);
}


