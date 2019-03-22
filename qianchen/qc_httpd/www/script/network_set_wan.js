function check_password(id) {
	var value = get_input_value(id);
	if (value == null || value == "") {
		return;
	}

	var oEvent = window.event;
	if (oEvent == 8) {
		return;
	}
	
　　var reg = new RegExp("[\\u4E00-\\u9FFF]+","g");
　　if(reg.test(value)) {
		//配置一个透明的询问框
		layer.msg('不能输入中文以及中文符号!!!', {
			time: 3000, //3s后自动关闭
			btn: ['关闭']
		});
		
		document.getElementById(id).value = "";
		document.getElementById(id).focus();
　　}
}

function get_wan_info(page) {
	$.ajax({
		type: 'get',
		url: '/getWanInfo',
		dataType: 'json',
		async: true,
		success: function (result) {
			if (result['res'] == 'success') {
				var wan_protocol = result['wan_protocol'];
				var mtu = result['mtu'];
				var dns_first = result['dns_first'];
				var dns_second = result['dns_second'];
				var wan_ip = result['wan_ip'];
				var wan_mask = result['wan_mask'];
				var wan_gw = result['wan_gw'];
				var username = result['username'];
				var password = result['password'];
				var tips_flag = result['tips_flag'];
				
				var tips = "";
				if (tips_flag == true) {
					tips = "提示: 您的外网新设置未生效，请重启路由器";
					var e = document.getElementById("tips");
					if (e != null) {
						e.innerHTML = tips;
					}
				}
				
				if (wan_protocol == "dhcp" && page == "dhcp") {
					document.getElementById("dns_first").value = dns_first;
					document.getElementById("dns_second").value = dns_second;
					document.getElementById("mtu").value = mtu;
				}
				else if (wan_protocol == "static" && page == "static") {		
					document.getElementById("ip").value = wan_ip;
					document.getElementById("mask").value = wan_mask;
					document.getElementById("gateway").value = wan_gw;					
					document.getElementById("dns_first").value = dns_first;
					document.getElementById("dns_second").value = dns_second;
					document.getElementById("mtu").value = mtu;
				}
				else if (wan_protocol == "pppoe" && page == "pppoe") {
					document.getElementById("username").value = username;
					document.getElementById("password").value = password;			
					document.getElementById("dns_first").value = dns_first;
					document.getElementById("dns_second").value = dns_second;
					document.getElementById("mtu").value = mtu;
				}
			}
			else {
				//配置一个透明的询问框
				layer.msg('获取外网信息失败!!! [' + result['msg'] + ']', {
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

var my_page = null;
function select_page(page) {
	my_page = page;
	layui.use('form', function() { //独立版的layer无需执行这一句
		var form = layui.form
		
		form.on('radio(radio_protocol)', function(data){
			if (data.value == page) {
				return;
			}
			
			if (data.value == "dhcp") {
				window.location.href = "./network_wan_set_dhcp.html";
			}
			else if (data.value == "static") {
				window.location.href = "./network_wan_set_static.html";
			}
			else if (data.value == "pppoe") {
				window.location.href = "./network_wan_set_pppoe.html";
			}
		});
	});
}

function get_wan_protocol()
{
	$.ajax({
		type: 'get',
		url: '/getWanProtocol',
		dataType: 'json',
		async: true,
		success: function (result) {
			if (result['res'] == 'success') {
				var wan_protocol = result['wan_protocol'];
				if (wan_protocol == "dhcp") {
					window.location.href = "./network_wan_set_dhcp.html";
				}
				else if (wan_protocol == "static") {
					window.location.href = "./network_wan_set_static.html";
				}
				else if (wan_protocol == "pppoe") {
					window.location.href = "./network_wan_set_pppoe.html";
				}
			}
			else {
				//配置一个透明的询问框
				layer.msg('获取外网协议失败!!! [' + result['msg'] + ']', {
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


function isValidIP(ip) {
    var reg = /^(\d{1,2}|1\d\d|2[0-4]\d|25[0-5])\.(\d{1,2}|1\d\d|2[0-4]\d|25[0-5])\.(\d{1,2}|1\d\d|2[0-4]\d|25[0-5])\.(\d{1,2}|1\d\d|2[0-4]\d|25[0-5])$/
    return reg.test(ip);
} 

function get_input_value(id) {
	var e = document.getElementById(id);
	if (e) {
		return e.value;
	}
	
	return null;
}

layui.use(['layer', 'form', 'layedit'], function() {
	var $ = layui.jquery, layer = layui.layer; //独立版的layer无需执行这一句
	
	var form = layui.form;
	
	//自定义验证规则
	form.verify({
		ip: function(value) {
			if (value == null || value.length <= 0) {
				return '请输入IP地址';
			}
			if (isValidIP(value) == false) {
				return '输入格式错误，参考格式:"192.168.1.112"';
			}
		}
		,mask: function(value) {
			if (value == null || value.length <= 0) {
				return '请输入子网掩码';
			}
			if (isValidIP(value) == false) {
				return '输入格式错误，参考格式:"255.255.255.0"';
			}
		}
		,gateway: function(value) {
			if (value == null || value.length <= 0) {
				return '请输入网关地址';
			}
			if (isValidIP(value) == false) {
				return '输入格式错误，参考格式:"192.168.1.1"';
			}
		}
		,dns_first: function(value) {
			if (my_page == "static") {
				if (value == null || value.length <= 0) {
					return '请输入网关地址';
				}
				if (isValidIP(value) == false) {
					return '输入格式错误，参考格式:"192.168.1.1"';
				}
			}
			else {
				if (value != null && value.length > 0) {
					if (isValidIP(value) == false) {
						return '输入格式错误，参考格式:"142.168.1.23"';
					}
				}
			}
		}
		,dns_second: function(value) {
			if (value != null && value.length > 0) {
				if (isValidIP(value) == false) {
					return '输入格式错误，参考格式:"142.168.1.23"';
				}
			}
		}
		,username: function(value) {
			if (value == null || value.length <= 0) {
				return '请输入用户名';
			}
		}
		,password: function(value) {
			if (value == null || value.length <= 0) {
				return '请输入密码';
			}
		}
		,mtu: function(value) {
			if (value != null && value.length > 0) {
				var r = /^\+?[1-9][0-9]*$/;
				if (r.test(value) == false
				|| value < 1 || value > 1500) {
					return '请输入数字，范围为1~1500';
				}
			}
		}
	});

		//监听提交
	form.on('submit(network_submit)', function(data){
		//ajax提交，并接收返回结果，如果返回成功，则返回
		//调用layer.close关闭当前弹层，并提示设置成功
		//layer.alert(JSON.stringify(data.field), {
		//	title: '最终的提交信息'
		//});
		var radio = document.getElementsByName("protocol");  
		for (var i = 0; i < radio.length; i ++) {  
			if (radio[i].checked) {
				break;
			}  
		}
		var protocol = radio[i].value;
		var ip = get_input_value("ip");
		var mask = get_input_value("mask");
		var gateway = get_input_value("gateway");
		var dns_first = get_input_value("dns_first");
		var dns_second = get_input_value("dns_second");
		var mtu = get_input_value("mtu");
		
		var username = get_input_value("username");
		var password = get_input_value("password");

		var data = {};
		data.protocol = protocol;
		data.ip = ip;
		data.mask = mask;
		data.gateway = gateway;
		data.dns_first = dns_first;
		data.dns_second = dns_second;
		data.mtu = mtu;
		data.username = username;
		data.password = password;
		$.ajax({
			type: 'post',
			dataType: 'json',
			url: '/setWan',
			data: jQuery.param(data),
			async: true,
			success: function (result) {
				if (result['res'] == 'success') {
					layer.closeAll();
					
					//配置一个透明的询问框
					layer.msg('设置成功，重启路由器后生效', {
						time: 5000, //3s后自动关闭
						btn: ['关闭'],
						yes: function(index) {
							layer.close(index);
							location.reload();
						},
						end: function(index) {
							layer.close(index);
							location.reload();
						}
					});
				}
				else {
					//配置一个透明的询问框
					layer.msg('设置失败，参数异常!!! [ ' + result['msg'] + ' ]', {
						time: 5000, //3s后自动关闭
						btn: ['关闭']
					});
				}
			},
			beforeSend: function(x) {
				
			},
			error: function(e, v, m) {
			}
		});
		
		return false;
	});
});