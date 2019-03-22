function get_qos_info() {
	
	$.ajax({
		type: 'get',
		url: '/getQoSInfo',
		dataType: 'json',
		async: true,
		success: function (result) {
			if (result['res'] == 'success') {
				var qos_switch = result['qos_switch'];
				var qos_tx = result['qos_tx'];
				var qos_rx = result['qos_rx'];
				var qos_type = result['qos_type'];
				var qos_disabled_p2p_switch = result['qos_disabled_p2p_switch'];
				var qos_bandwidth_allocation = result['qos_bandwidth_allocation'];
				var qos_station_max_tx = result['qos_station_max_tx'];
				var qos_station_max_rx = result['qos_station_max_rx'];
				
				if (qos_switch == "off") {
					document.getElementById("qos_switch").checked = false;
					document.getElementById("qos_switch_on").style.display = "none";
				}
				else if (qos_switch == "on") {
					document.getElementById("qos_switch").checked = true;
					document.getElementById("qos_switch_on").style.display = "block";					
				}
				
				document.getElementById("qos_tx").value = qos_tx > 0 ? qos_tx : "";
				document.getElementById("qos_rx").value = qos_rx > 0 ? qos_rx : "";
				$("input[name=qos_type][value=" + qos_type + "]").attr("checked", true);
				if (qos_type == "auto") {
					document.getElementById("qos_custom_on").style.display = "none";
					document.getElementById("qos_auto_on").style.display = "block";
				}
				else if (qos_type == "custom") {
					document.getElementById("qos_auto_on").style.display = "none";
					document.getElementById("qos_custom_on").style.display = "block";
				}
				if (qos_disabled_p2p_switch == "off") {
					document.getElementById("qos_disabled_p2p_switch").checked = false;
				}
				else if (qos_disabled_p2p_switch == "on") {
					document.getElementById("qos_disabled_p2p_switch").checked = true;					
				}
				if (qos_bandwidth_allocation == "race") {
					$("input[name=qos_bandwidth_allocation][value=race]").attr("checked", true);
					document.getElementById("bandwidth_allocation_dynamic_div").style.display = "none";
					document.getElementById("bandwidth_allocation_config_div").style.display = "none";
					document.getElementById("bandwidth_allocation_race_div").style.display = "block";
				}
				else if (qos_bandwidth_allocation == "dynamic") {
					$("input[name=qos_bandwidth_allocation][value=dynamic]").attr("checked", true);
					document.getElementById("bandwidth_allocation_race_div").style.display = "none";
					document.getElementById("bandwidth_allocation_config_div").style.display = "none";
					document.getElementById("bandwidth_allocation_dynamic_div").style.display = "block";
				}
				else if (qos_bandwidth_allocation == "config") {
					$("input[name=qos_bandwidth_allocation][value=config]").attr("checked", true);
					document.getElementById("bandwidth_allocation_race_div").style.display = "none";
					document.getElementById("bandwidth_allocation_dynamic_div").style.display = "none";
					document.getElementById("bandwidth_allocation_config_div").style.display = "block";
				}
				document.getElementById("qos_station_max_tx").value = qos_station_max_tx > 0 ? qos_station_max_tx : "";
				document.getElementById("qos_station_max_rx").value = qos_station_max_rx > 0 ? qos_station_max_rx : "";
			}
			else {
				//配置一个透明的询问框
				layer.msg('获取QOS配置信息失败!!! [' + result['msg'] + ']', {
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

function get_radio_value(name) {
	var radio = document.getElementsByName(name);  
	for (var i = 0; i < radio.length; i ++) {  
		if (radio[i].checked) {
			break;
		}
	}
	
	return radio[i].value;
}

layui.use(['layer', 'form', 'layedit'], function() { //独立版的layer无需执行这一句
	var form = layui.form
	
	//自定义验证规则
	form.verify({
		qos_tx: function(value) {
			if (get_switch_value("qos_switch") == "on") {
				if (value != null && value.length > 0) {
					if (value < 1 || value > 1000) {
						return '只能填写数字 范围1~1000';
					}
				}
			}
		}
		,qos_rx: function(value) {
			if (get_switch_value("qos_switch") == "on") {
				if (value != null && value.length > 0) {
					if (value < 4 || value > 1000) {
						return '只能填写数字 范围4~1000';
					}
				}
				else {
					return "请填写下行带宽 范围4~1000"
				}
			}
		}
		,qos_station_max_tx: function(value) {
			if (get_switch_value("qos_switch") == "on") {
				if (value != null && value.length > 0) {
					if (value < 0 || value > 1000 * 1024) {
						return '只能填写数字 范围0~1024000';
					}
				}
			}
		}
		,qos_station_max_rx: function(value) {
			if (get_switch_value("qos_switch") == "on") {
				if (value != null && value.length > 0) {
					if (value < 0 || value > 1000 * 1024) {
						return '只能填写数字 范围0~1024000';
					}
				}
				else {
					return "请填写下行速率 范围0~1024000"
				}
			}
		}
	});
	
	form.on('switch(qos_switch)', function(data){
		if (data.elem.checked == true) {
			document.getElementById("qos_switch_on").style.display = "block";
		}
		else {
			document.getElementById("qos_switch_on").style.display = "none";
		}
	});
	
	form.on('radio(qos_type)', function(data){
		if (data.value == "auto") {
			document.getElementById("qos_custom_on").style.display = "none";
			document.getElementById("qos_auto_on").style.display = "block";
		}
		else if (data.value == "custom") {
			document.getElementById("qos_auto_on").style.display = "none";
			document.getElementById("qos_custom_on").style.display = "block";
		}
	});
	
	form.on('radio(qos_bandwidth_allocation)', function(data){
		if (data.value == "race") {
			document.getElementById("bandwidth_allocation_dynamic_div").style.display = "none";
			document.getElementById("bandwidth_allocation_config_div").style.display = "none";
			document.getElementById("bandwidth_allocation_race_div").style.display = "block";
		}
		else if (data.value == "dynamic") {
			document.getElementById("bandwidth_allocation_race_div").style.display = "none";
			document.getElementById("bandwidth_allocation_config_div").style.display = "none";
			document.getElementById("bandwidth_allocation_dynamic_div").style.display = "block";
		}
		else if (data.value == "config") {
			document.getElementById("bandwidth_allocation_race_div").style.display = "none";
			document.getElementById("bandwidth_allocation_dynamic_div").style.display = "none";
			document.getElementById("bandwidth_allocation_config_div").style.display = "block";
		}
	});

	//监听提交
	form.on('submit(qos_submit)', function(data){
		//ajax提交，并接收返回结果，如果返回成功，则返回
		//调用layer.close关闭当前弹层，并提示设置成功
		//layer.alert(JSON.stringify(data.field), {
		//	title: '最终的提交信息'
		//});
		var qos_switch = get_switch_value("qos_switch");
		var qos_tx = get_input_value("qos_tx");
		var qos_rx = get_input_value("qos_rx");
		var qos_type = get_radio_value("qos_type");
		var qos_disabled_p2p_switch = get_switch_value("qos_disabled_p2p_switch");
		var qos_bandwidth_allocation = get_radio_value("qos_bandwidth_allocation");
		var qos_station_max_tx = get_input_value("qos_station_max_tx");
		var qos_station_max_rx = get_input_value("qos_station_max_rx");
		
		var data = {};
		data.qos_switch = qos_switch;
		data.qos_tx = qos_tx;
		data.qos_rx = qos_rx;
		data.qos_type = qos_type;
		data.qos_disabled_p2p_switch = qos_disabled_p2p_switch;
		data.qos_bandwidth_allocation = qos_bandwidth_allocation;
		data.qos_station_max_tx = qos_station_max_tx;
		data.qos_station_max_rx = qos_station_max_rx;

		$.ajax({
			type: 'post',
			dataType: 'json',
			url: '/setQoS',
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

document.getElementById("qos_switch").checked = false;
document.getElementById("qos_switch_on").style.display = "none";
get_qos_info();
