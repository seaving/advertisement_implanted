
function get_lan_info() {
	$.ajax({
		type: 'get',
		url: '/getLanInfo',
		dataType: 'json',
		async: true,
		success: function (result) {
			if (result['res'] == 'success') {
				var tips_flag = result['tips_flag'];

				var ip = result['ip'];
				var mask = result['mask'];
				var dhcp_switch = result['dhcp_switch'];
				var start_ip = result['start_ip'];
				var auth_dhcp_max_count = result['auth_dhcp_max_count'];
				var auth_leasetime = result['auth_leasetime'];
				var lan_quarantine = result["lan_quarantine"];
				
				var tips = "";
				if (tips_flag == true) {
					tips = "提示: 您的内网新设置未生效，请重启路由器";
					var e = document.getElementById("tips");
					if (e != null) {
						e.innerHTML = tips;
					}
				}
				
				document.getElementById("ip").value = ip;
				document.getElementById("mask").value = mask;
				if (dhcp_switch == "on") {
					document.getElementById("dhcp_switch").checked = true;
					document.getElementById("dhcp_switch_on").style.display = "block";
				}
				else {
					document.getElementById("dhcp_switch").checked = false;
					document.getElementById("dhcp_switch_on").style.display = "none";
				}
				document.getElementById("start_ip").value = start_ip;
				document.getElementById("auth_dhcp_max_count").value = auth_dhcp_max_count;
				document.getElementById("auth_leasetime").value = auth_leasetime;
				
				if (lan_quarantine == "on") {
					document.getElementById("lan_quarantine").checked = true;
				}
				else {
					document.getElementById("lan_quarantine").checked = false;
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

function get_switch_value(id) {
	var e = document.getElementById(id);
	if (e) {
		return e.checked == true ? "on" : "off";
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
		,start_ip: function(value) {
			if (get_switch_value("dhcp_switch") == "on") {
				if (value == null || value.length <= 0) {
					return '请输入起始IP地址';
				}
				if (isValidIP(value) == false) {
					return '输入格式错误，参考格式:"192.168.1.112"';
				}
			}
		}
		,auth_dhcp_max_count: function(value) {
			if (value != null && value.length > 0) {
				var r = /^\+?[1-9][0-9]*$/;
				if (r.test(value) == false
				|| value < 1) {
					return '请输入大于0的整数';
				}
			}			
		}
		,auth_leasetime: function(value) {
			if (value != null && value.length > 0) {
				var r = /^\+?[1-9][0-9]*$/;
				if (r.test(value) == false
				|| value < 1 || value > 24) {
					return '请输入整数，范围 1~24 小时';
				}
				
			}		
		}
	});
	
	form.on('switch(dhcp_switch)', function(data){
		if (data.elem.checked == true) {
			document.getElementById("dhcp_switch_on").style.display = "block";
		}
		else {
			document.getElementById("dhcp_switch_on").style.display = "none";
		}
	});
	
	//监听提交
	form.on('submit(network_submit)', function(data){
		//ajax提交，并接收返回结果，如果返回成功，则返回
		//调用layer.close关闭当前弹层，并提示设置成功
		//layer.alert(JSON.stringify(data.field), {
		//	title: '最终的提交信息'
		//});
		var ip = get_input_value("ip");
		var mask = get_input_value("mask");
		var dhcp_switch = get_switch_value("dhcp_switch");
		var start_ip = get_input_value("start_ip");
		var auth_dhcp_max_count = get_input_value("auth_dhcp_max_count");
		var auth_leasetime = get_input_value("auth_leasetime");
		
		var lan_quarantine = get_switch_value("lan_quarantine");
		
		var data = {};
		data.ip = ip;
		data.mask = mask;
		data.dhcp_switch = dhcp_switch;
		data.start_ip = start_ip;
		data.auth_dhcp_max_count = auth_dhcp_max_count;
		data.auth_leasetime = auth_leasetime;
		data.lan_quarantine = lan_quarantine;

		$.ajax({
			type: 'post',
			dataType: 'json',
			url: '/setLan',
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
						time: 10000, //3s后自动关闭
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

get_lan_info();
