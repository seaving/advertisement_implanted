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

function not_auth_process(result)
{
	var tips_flag = result['tips_flag'];
	
	var not_auth_switch = result['not_auth_switch'];
	var not_auth_2g_ssid = result['not_auth_2g_ssid'];
	var not_auth_2g_password = result['not_auth_2g_password'];
	var not_auth_5g_ssid = result['not_auth_5g_ssid'];
	var not_auth_5g_password = result['not_auth_5g_password'];
	var not_auth_ssid_hidden = result['not_auth_ssid_hidden'];
	var not_auth_ip = result['not_auth_ip'];
	var not_auth_mask = result['not_auth_mask'];
	var not_auth_dhcp_switch = result['not_auth_dhcp_switch'];
	var start_ip = result['start_ip'];
	var not_auth_dhcp_max_count = result['not_auth_dhcp_max_count'];
	var not_auth_leasetime = result['not_auth_leasetime'];
	var tips = "";
	if (tips_flag == true) {
		tips = "提示: 您的免认证新设置未生效，请重启路由器";
		var e = document.getElementById("tips");
		if (e != null) {
			e.innerHTML = tips;
		}
	}
	
	if (not_auth_switch == "on") {
		document.getElementById("not_auth_switch").checked = true;
		document.getElementById("not_auth_set_div").style.display = "block";
	}
	else {
		document.getElementById("not_auth_switch").checked = false;
		document.getElementById("not_auth_set_div").style.display = "none";
	}
	
	document.getElementById("not_auth_2g_ssid").value = not_auth_2g_ssid;
	document.getElementById("not_auth_2g_password").value = not_auth_2g_password;
	document.getElementById("not_auth_5g_ssid").value = not_auth_5g_ssid;
	document.getElementById("not_auth_5g_password").value = not_auth_5g_password;
	
	if (not_auth_ssid_hidden == "on") {
		document.getElementById("not_auth_ssid_hidden").checked = false;
	}
	else {
		document.getElementById("not_auth_ssid_hidden").checked = true;
	}
	
	document.getElementById("not_auth_ip").value = not_auth_ip;
	document.getElementById("not_auth_mask").value = not_auth_mask;
	if (not_auth_dhcp_switch == "on") {
		document.getElementById("not_auth_dhcp_switch").checked = true;
		document.getElementById("dhcp_switch_on").style.display = "block";
	}
	else {
		document.getElementById("not_auth_dhcp_switch").checked = false;
		document.getElementById("dhcp_switch_on").style.display = "none";
	}
	document.getElementById("start_ip").value = start_ip;
	document.getElementById("not_auth_dhcp_max_count").value = not_auth_dhcp_max_count;
	document.getElementById("not_auth_leasetime").value = not_auth_leasetime;
}

function get_not_auth_info() {
	
	$.ajax({
		type: 'get',
		url: '/getNotAuthInfo',
		dataType: 'json',
		async: true,
		success: function (result) {
			if (result['res'] == 'success') {
				not_auth_process(result);
			}
			else {
				//配置一个透明的询问框
				layer.msg('获取免认证配置信息失败!!! [' + result['msg'] + ']', {
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

function get_select_value(id) {
	var e = document.getElementById(id);
	if (e) {
		var index = e.selectedIndex;
		return e.options[index].value;
	}
	
	return null;
}

layui.use(['layer', 'form', 'layedit'], function() {
	var $ = layui.jquery, layer = layui.layer; //独立版的layer无需执行这一句
	
	var form = layui.form;
	
	//自定义验证规则
	form.verify({
		not_auth_2g_ssid: function(value) {
			if (get_switch_value("not_auth_switch") == "on") {
				if (value == null || value.length <= 0) {
					return '请输入SSID';
				}
				if (value.length > 32) {
					return '长度不能超过32个字符，中文及中文符号算2个字符';
				}
			}
		}
		,not_auth_5g_ssid: function(value) {
			if (get_switch_value("not_auth_switch") == "on") {
				if (value != null && value.length > 32) {
					return '长度不能超过32个字符，中文及中文符号算2个字符';
				}
			}
		}
		,password: function(value) {
			if (get_switch_value("not_auth_switch") == "on") {
				if (value != null && value.length > 16) {
					return '密码长度不能超过16位';
				}
			}
		}
		,ip: function(value) {
			if (get_switch_value("not_auth_switch") == "on") {
				if (value == null || value.length <= 0) {
					return '请输入IP地址';
				}
				if (isValidIP(value) == false) {
					return '输入格式错误，参考格式:"192.168.1.112"';
				}
			}
		}
		,mask: function(value) {
			if (get_switch_value("not_auth_switch") == "on") {
				if (value == null || value.length <= 0) {
					return '请输入子网掩码';
				}
				if (isValidIP(value) == false) {
					return '输入格式错误，参考格式:"255.255.255.0"';
				}
			}
		}
		,start_ip: function(value) {
			if (get_switch_value("not_auth_switch") == "on") {
				if (get_switch_value("not_auth_dhcp_switch") == "on") {
					if (value == null || value.length <= 0) {
						return '请输入起始IP地址';
					}
					if (isValidIP(value) == false) {
						return '输入格式错误，参考格式:"192.168.1.112"';
					}
				}
			}
		}
		,auth_dhcp_max_count: function(value) {
			if (get_switch_value("not_auth_switch") == "on") {
				if (value != null && value.length > 0) {
					var r = /^\+?[1-9][0-9]*$/;
					if (r.test(value) == false
					|| value < 1) {
						return '请输入大于0的整数';
					}
				}
			}
		}
		,auth_leasetime: function(value) {
			if (get_switch_value("not_auth_switch") == "on") {
				if (value != null && value.length > 0) {
					var r = /^\+?[1-9][0-9]*$/;
					if (r.test(value) == false
					|| value < 1 || value > 24) {
						return '请输入整数，范围 1~24 小时';
					}
					
				}
			}
		}
	});
	
	form.on('switch(not_auth_dhcp_switch)', function(data){
		if (data.elem.checked == true) {
			document.getElementById("dhcp_switch_on").style.display = "block";
		}
		else {
			document.getElementById("dhcp_switch_on").style.display = "none";
		}
	});

	form.on('switch(not_auth_switch)', function(data){
		//alert("2g: " + data.elem.checked);
		if (data.elem.checked == true) {
			document.getElementById("not_auth_set_div").style.display = "block";
		}
		else {
			document.getElementById("not_auth_set_div").style.display = "none";
		}
	});
	
	//监听提交
	form.on('submit(not_auth_submit)', function(data){
		//ajax提交，并接收返回结果，如果返回成功，则返回
		//调用layer.close关闭当前弹层，并提示设置成功
		//layer.alert(JSON.stringify(data.field), {
		//	title: '最终的提交信息'
		//});
		var not_auth_switch = get_switch_value("not_auth_switch");
		var not_auth_2g_ssid = get_input_value("not_auth_2g_ssid");
		var not_auth_2g_password = get_input_value("not_auth_2g_password");
		var not_auth_5g_ssid = get_input_value("not_auth_5g_ssid");
		var not_auth_5g_password = get_input_value("not_auth_5g_password");
		var not_auth_ssid_hidden = get_switch_value("not_auth_ssid_hidden");
		var not_auth_ip = get_input_value("not_auth_ip");
		var not_auth_mask = get_input_value("not_auth_mask");
		var not_auth_dhcp_switch = get_switch_value("not_auth_dhcp_switch");
		var start_ip = get_input_value("start_ip");
		var not_auth_dhcp_max_count = get_input_value("not_auth_dhcp_max_count");
		var not_auth_leasetime = get_input_value("not_auth_leasetime");
		
		var data = {};
		data.not_auth_switch = not_auth_switch;
		data.not_auth_2g_ssid = not_auth_2g_ssid;
		data.not_auth_2g_password = not_auth_2g_password;
		data.not_auth_5g_ssid = not_auth_5g_ssid;
		data.not_auth_5g_password = not_auth_5g_password;
		data.not_auth_ssid_hidden = not_auth_ssid_hidden == "on" ? "off" : "on";
		data.not_auth_ip = not_auth_ip;
		data.not_auth_mask = not_auth_mask;
		data.not_auth_dhcp_switch = not_auth_dhcp_switch;
		data.start_ip = start_ip;
		data.not_auth_dhcp_max_count = not_auth_dhcp_max_count;
		data.not_auth_leasetime = not_auth_leasetime;

		$.ajax({
			type: 'post',
			dataType: 'json',
			url: '/setNotAuth',
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

document.getElementById("not_auth_switch").checked = false;
document.getElementById("not_auth_set_div").style.display = "none";
get_not_auth_info();
