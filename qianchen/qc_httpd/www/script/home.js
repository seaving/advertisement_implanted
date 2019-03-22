
function setIntervalFunction() {
	window.setInterval(dynamic_get_info, 20000);
	window.setInterval(dynamic_get_online_cnt, 10000);
	window.setInterval(dynamic_get_network_speed, 5000);
}

function dynamic_get_info() {
	$.ajax({
		type: 'get',
		url: '/getInfo',
		dataType: 'json',
		async: true,
		success: function (result) {
			if (result['res'] == 'success') {
				var model = result['model'];
				var version = result['version'];
				var mac = result['mac'];
				var auth_ssid_2g = result['auth_ssid_2g'];
				var auth_ssid_5g = result['auth_ssid_5g'];
				var not_auth_ssid_2g = result['not_auth_ssid_2g'];
				var not_auth_ssid_5g = result['not_auth_ssid_5g'];
				var auth_assigned_count = result['auth_assigned_count'];
				var not_auth_assigned_count = result['not_auth_assigned_count'];
				var auth_max_count = result['auth_max_count'];
				var not_auth_max_count = result['not_auth_max_count'];
				var wan_ip = result['wan_ip'];
				var dns_first = result['dns_first'];
				var dns_second = result['dns_second'];
				var wan_protocol = result['wan_protocol'];
				if (wan_protocol == "static") {
					wan_protocol = '静态地址';
				}
				
				var reboot_time = result['reboot_time'];
				var bond_state = result['bond_state'];
				if (bond_state == null) {
					bond_state = '未查询到状态';
				}
				
				document.getElementById("model").innerHTML = model;
				document.getElementById("version").innerHTML = version;
				document.getElementById("mac").innerHTML = mac;
				document.getElementById("auth_ssid_2g").innerHTML = auth_ssid_2g;
				document.getElementById("auth_ssid_5g").innerHTML = auth_ssid_5g;
				document.getElementById("not_auth_ssid_2g").innerHTML = not_auth_ssid_2g;
				document.getElementById("not_auth_ssid_5g").innerHTML = not_auth_ssid_5g;
				document.getElementById("ip_auth_count").innerHTML = auth_assigned_count;
				document.getElementById("dhcp_auth_max_count").innerHTML = auth_max_count;
				document.getElementById("ip_not_auth_count").innerHTML = not_auth_assigned_count;
				document.getElementById("dhcp_not_auth_max_count").innerHTML = not_auth_max_count;
				document.getElementById("wan_ip").innerHTML = wan_ip;
				document.getElementById("dns").innerHTML = dns_first + '    ' + dns_second;
				document.getElementById("protocol").innerHTML = wan_protocol;
				
				document.getElementById("reboot_time").innerHTML = reboot_time;
				document.getElementById("bond_state").innerHTML = bond_state;
			}
			else {
				//配置一个透明的询问框
				layer.msg('获取设备信息失败!!! [' + result['msg'] + ']', {
					time: 3000, //3s后自动关闭
					btn: ['关闭']
				});
			}
		},
		beforeSend: function(x) {
			
		},
		error: function(e, v, m) {
		}
	});
}

function dynamic_get_online_cnt() {
	$.ajax({
		type: 'get',
		url: '/getOnlineCnt',
		dataType: 'json',
		async: true,
		success: function (result) {
			if (result['res'] == 'success') {
				var online_client_cnt = result['online_client_cnt'];
				var auth_online_client_cnt = result['auth_online_client_cnt'];
				var no_auth_online_client_cnt = result['no_auth_online_client_cnt'];
				
				document.getElementById("online_client_cnt").innerHTML = online_client_cnt;
				document.getElementById("auth_online_client_cnt").innerHTML = auth_online_client_cnt;
				document.getElementById("no_auth_online_client_cnt").innerHTML = no_auth_online_client_cnt;
			}
			else {
				//配置一个透明的询问框
				layer.msg('获取在线终端数量失败!!! [' + result['msg'] + ']', {
					time: 3000, //3s后自动关闭
					btn: ['关闭']
				});
			}
		},
		beforeSend: function(x) {
			
		},
		error: function(e, v, m) {
		}
	});
}

function dynamic_get_network_speed() {
	$.ajax({
		type: 'get',
		url: '/getNetworkSpeed',
		dataType: 'json',
		async: true,
		success: function (result) {
			if (result['res'] == 'success') {
				var upload_speed = result['upload_speed'];
				var download_speed = result['download_speed'];
				var total_speed = result['total_speed'];
				var traffic_total_count = result['traffic_total_count'];
				
				upload_speed = upload_speed / 1024;
				download_speed = download_speed / 1024;
				total_speed = total_speed / 1024;
				var unit = "MB";
				if (traffic_total_count <= 1024 * 1024 * 1024) {
					//MB
					unit = "MB";
					traffic_total_count = traffic_total_count / 1024 / 1024;
					traffic_total_count = traffic_total_count.toFixed(2);
				}
				else if (traffic_total_count <= 1024 * 1024 * 1024 * 1024) {
					//GB
					unit = "GB";
					traffic_total_count = traffic_total_count / 1024 / 1024 / 1024;
					traffic_total_count = traffic_total_count.toFixed(3);
				}
				else {
					//TB
					unit = "TB";
					traffic_total_count = traffic_total_count / 1024 / 1024 / 1024 / 1024;
					traffic_total_count = traffic_total_count.toFixed(6);
				}
				
				document.getElementById("upload_speed").innerHTML = upload_speed.toFixed(2);
				document.getElementById("download_speed").innerHTML = download_speed.toFixed(2);
				document.getElementById("total_speed").innerHTML = total_speed.toFixed(2);
				
				document.getElementById("traffic_cnt_unit").innerHTML = unit;
				document.getElementById("traffic_total_count").innerHTML = traffic_total_count;
			}
			else {
				//配置一个透明的询问框
				layer.msg('获取总带宽速率失败!!! [' + result['msg'] + ']', {
					time: 3000, //3s后自动关闭
					btn: ['关闭']
				});
			}
		},
		beforeSend: function(x) {
			
		},
		error: function(e, v, m) {
		}
	});
}

layui.use('table', function(){
  var table = layui.table;
  
  table.render({
	elem : '#traffic_table',
	url : '',
	method: 'POST',
	page : {
		limit: 10,
		limits: [30,50,100],
		theme : '#6161d1'
	},
	skin: 'nob',
	size:'lg',
	cols : [ [ 
	{
		field : 'number',
		title : '编号',
		width : 60,
		align : 'center',
		fixed : 'left'
	}, {
		field : 'ip_mac',
		title : 'IP/MAC',
		align : 'center',
		width : 150
	}, {
		field : 'upload_speed',
		title : '上行速率(KB/s)',
		width : 150,
		sort  : true,
		align : 'center'
	}, {
		field : 'download_speed',
		title : '下行速率(KB/s)',
		width : 150,
		sort  : true,
		align : 'center'
	}, {
		field : 'upload_total',
		title : '上行总流量(MB)',
		width : 140,
		sort  : true,
		align : 'center'	
	}, {
		field : 'download_total',
		title : '下行总流量(MB)',
		width : 140,
		sort  : true,
		align : 'center'
	}, {
		field : 'online_time',
		title : '在线时长',
		width : 100,
		align : 'center'
	},  {
		field : 'remarks',
		title : '备注',
		width : 140,
		align : 'center'
	} ] ]
  });
});

dynamic_get_info();
dynamic_get_online_cnt();
dynamic_get_network_speed();
setIntervalFunction();
