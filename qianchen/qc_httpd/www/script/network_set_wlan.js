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

function get_2g_wlan(result)
{
	var tips_flag = result['tips_flag'];
	
	var auth_wifi_2g_switch = result['auth_wifi_2g_switch'];
	var auth_2g_ssid = result['auth_2g_ssid'];
	var auth_2g_password = result['auth_2g_password'];
	var auth_2g_channel = result['auth_2g_channel'];
	var auth_2g_bandwidth = result['auth_2g_bandwidth'];
	var auth_2g_signal_min = result['auth_2g_signal_min'];
	var auth_2g_ssid_hidden = result['auth_2g_ssid_hidden'];
	
	var tips = "";
	if (tips_flag == true) {
		tips = "提示: 您的无线2G新设置未生效，请重启路由器";
		var e = document.getElementById("2g_tips");
		if (e != null) {
			e.innerHTML = tips;
		}
	}
	
	if (auth_wifi_2g_switch == "on") {
		document.getElementById("auth_wifi_2g_switch").checked = true;
		document.getElementById("set_2g_div").style.display = "block";
	}
	else {
		document.getElementById("auth_wifi_2g_switch").checked = false;
		document.getElementById("set_2g_div").style.display = "none";
	}
	
	document.getElementById("auth_2g_ssid").value = auth_2g_ssid;
	document.getElementById("auth_2g_password").value = auth_2g_password;
	
	$("#auth_2g_channel").find("option[value='"+ auth_2g_channel +"']").attr("selected", true);
	$("#auth_2g_bandwidth").find("option[value='"+ auth_2g_bandwidth +"']").attr("selected", true);
	$("#auth_2g_signal_min").find("option[value='"+ auth_2g_signal_min +"']").attr("selected", true);
	
	if (auth_2g_ssid_hidden == "on") {
		document.getElementById("auth_2g_ssid_hidden").checked = false;
	}
	else {
		document.getElementById("auth_2g_ssid_hidden").checked = true;
	}
}

function get_5g_wlan(result)
{
	var tips_flag = result['tips_flag'];
	
	var auth_wifi_5g_switch = result['auth_wifi_5g_switch'];
	var auth_5g_ssid = result['auth_5g_ssid'];
	var auth_5g_password = result['auth_5g_password'];
	var auth_5g_channel = result['auth_5g_channel'];
	var auth_5g_bandwidth = result['auth_5g_bandwidth'];
	var auth_5g_signal_min = result['auth_5g_signal_min'];
	var auth_5g_ssid_hidden = result['auth_5g_ssid_hidden'];

	var tips = "";
	if (tips_flag == true) {
		tips = "提示: 您的无线5G新设置未生效，请重启路由器";
		var e = document.getElementById("5g_tips");
		if (e != null) {
			e.innerHTML = tips;
		}
	}

	if (auth_wifi_5g_switch == "on") {
		document.getElementById("auth_wifi_5g_switch").checked = true;
		document.getElementById("set_5g_div").style.display = "block";
	}
	else {
		document.getElementById("auth_wifi_5g_switch").checked = false;
		document.getElementById("set_5g_div").style.display = "none";
	}
	
	document.getElementById("auth_5g_ssid").value = auth_5g_ssid;
	document.getElementById("auth_5g_password").value = auth_5g_password;
	
	$("#auth_5g_channel").find("option[value='"+ auth_5g_channel +"']").attr("selected", true);
	$("#auth_5g_bandwidth").find("option[value='"+ auth_5g_bandwidth +"']").attr("selected", true);
	$("#auth_5g_signal_min").find("option[value='"+ auth_5g_signal_min +"']").attr("selected", true);
	
	if (auth_5g_ssid_hidden == "on") {
		document.getElementById("auth_5g_ssid_hidden").checked = false;
	}
	else {
		document.getElementById("auth_5g_ssid_hidden").checked = true;
	}
	
}

function get_wlan_info() {
	
	$.ajax({
		type: 'get',
		url: '/getWlanInfo',
		dataType: 'json',
		async: true,
		success: function (result) {
			//{"res": "", "msg":"", "data":[{"fre":"2g", "ssid":"", ...}, {"fre":"5g", "ssid":"", ...}]}
			if (result['res'] == 'success') {
				data_array = result['data'];
				if (data_array == null || data_array == "") {
					return;
				}
				
				o = data_array[0];
				if (o == null || o == "") {
					return;
				}
				
				if (o['fre'] == "2g") {
					get_2g_wlan(o);
				}
				else if (o['fre'] == "5g") {
					get_5g_wlan(o);
				}

				o = data_array[1];
				if (o == null || o == "") {
					return;
				}
				
				if (o['fre'] == "2g") {
					get_2g_wlan(o);
				}
				else if (o['fre'] == "5g") {
					get_5g_wlan(o);
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
		ssid: function(value) {
			if (value == null || value.length <= 0) {
				return '请输入SSID';
			}
			if (value.length > 32) {
				return '长度不能超过32个字符，中文及中文符号算2个字符';
			}
		}
		,password: function(value) {
			if (value != null && value.length > 16) {
				return '密码长度不能超过16位';
			}
		}
	});
	
	form.on('switch(auth_wifi_2g_switch)', function(data){
		//alert("2g: " + data.elem.checked);
		if (data.elem.checked == true) {
			document.getElementById("set_2g_div").style.display = "block";
		}
		else {
			document.getElementById("set_2g_div").style.display = "none";
		}
	});

	form.on('switch(auth_wifi_5g_switch)', function(data){
		//alert("5g: " + data.elem.checked);
		if (data.elem.checked == true) {
			document.getElementById("set_5g_div").style.display = "block";
		}
		else {
			document.getElementById("set_5g_div").style.display = "none";
		}
	});
	
	//监听提交
	form.on('submit(auth_2g_wlan_submit)', function(data){
		//ajax提交，并接收返回结果，如果返回成功，则返回
		//调用layer.close关闭当前弹层，并提示设置成功
		//layer.alert(JSON.stringify(data.field), {
		//	title: '最终的提交信息'
		//});
		var auth_wifi_2g_switch = get_switch_value("auth_wifi_2g_switch");
		var auth_2g_ssid = get_input_value("auth_2g_ssid");
		var auth_2g_password = get_input_value("auth_2g_password");
		var auth_2g_channel = get_select_value("auth_2g_channel");
		var auth_2g_bandwidth = get_select_value("auth_2g_bandwidth");
		var auth_2g_signal_min = get_select_value("auth_2g_signal_min");
		var auth_2g_ssid_hidden = get_switch_value("auth_2g_ssid_hidden");
		
		var data = {};
		data.fre = "2g";
		data.auth_wifi_2g_switch = auth_wifi_2g_switch;
		data.auth_2g_ssid = auth_2g_ssid;
		data.auth_2g_password = auth_2g_password;
		data.auth_2g_channel = auth_2g_channel;
		data.auth_2g_bandwidth = auth_2g_bandwidth;
		data.auth_2g_signal_min = auth_2g_signal_min;
		data.auth_2g_ssid_hidden = (auth_2g_ssid_hidden == "on" ? "off" : "on");

		$.ajax({
			type: 'post',
			dataType: 'json',
			url: '/setWlan',
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
	
	//监听提交
	form.on('submit(auth_5g_wlan_submit)', function(data){
		//ajax提交，并接收返回结果，如果返回成功，则返回
		//调用layer.close关闭当前弹层，并提示设置成功
		//layer.alert(JSON.stringify(data.field), {
		//	title: '最终的提交信息'
		//});
		var auth_wifi_5g_switch = get_switch_value("auth_wifi_5g_switch");
		var auth_5g_ssid = get_input_value("auth_5g_ssid");
		var auth_5g_password = get_input_value("auth_5g_password");
		var auth_5g_channel = get_select_value("auth_5g_channel");
		var auth_5g_bandwidth = get_select_value("auth_5g_bandwidth");
		var auth_5g_signal_min = get_select_value("auth_5g_signal_min");
		var auth_5g_ssid_hidden = get_switch_value("auth_5g_ssid_hidden");
		
		var data = {};
		data.fre = "5g";
		data.auth_wifi_5g_switch = auth_wifi_5g_switch;
		data.auth_5g_ssid = auth_5g_ssid;
		data.auth_5g_password = auth_5g_password;
		data.auth_5g_channel = auth_5g_channel;
		data.auth_5g_bandwidth = auth_5g_bandwidth;
		data.auth_5g_signal_min = auth_5g_signal_min;
		data.auth_5g_ssid_hidden = (auth_5g_ssid_hidden == "on" ? "off" : "on");

		$.ajax({
			type: 'post',
			dataType: 'json',
			url: '/setWlan',
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

document.getElementById("auth_wifi_2g_switch").checked = false;
document.getElementById("set_2g_div").style.display = "none";
document.getElementById("auth_wifi_5g_switch").checked = false;
document.getElementById("set_5g_div").style.display = "none";
get_wlan_info();
