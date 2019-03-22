var layer_index = null;
	
function isValidIP(ip) {
    var reg = /^(\d{1,2}|1\d\d|2[0-4]\d|25[0-5])\.(\d{1,2}|1\d\d|2[0-4]\d|25[0-5])\.(\d{1,2}|1\d\d|2[0-4]\d|25[0-5])\.(\d{1,2}|1\d\d|2[0-4]\d|25[0-5])$/
    return reg.test(ip);
}

function close_layer() {
	if (layer_index != null) {
		layer.close(layer_index);
	}
}

function get_input_value(id) {
	var e = document.getElementById(id);
	if (e) {
		return e.value;
	}
	
	return null;
}

function stop_ping() {
	$.ajax({
		type: 'post',
		dataType: 'json',
		url: '/stopPing',
		data: {},
		async: true,
		success: function (result) {
			
		},
		beforeSend: function(x) {
			
		},
		error: function(e, v, m) {
		}
	});
}

function _get_ping_result() {
	$.ajax({
		type: 'post',
		dataType: 'json',
		url: '/getResult',
		data: {},
		async: true,
		success: function (result) {
			if (result['res'] == 'success') {
				layer.closeAll();
				if (result['ping_result'] != null) {
					var e = document.getElementById('result');
					if (e != null) {
						e.value = result['ping_result'];
					}
				}
			}
			else {
				//配置一个透明的询问框
				layer.closeAll();
				layer.msg('操作失败!!! [ ' + result['msg'] + ' ]', {
					time: 10000, //3s后自动关闭
					btn: ['关闭']
				});
			}
			
			if (result['ping_finished'] == true) {
				clearInterval(interval);
			}
		},
		beforeSend: function(x) {
			
		},
		error: function(e, v, m) {
		}
	});
}

var interval = null;
function get_ping_result() {
	if (interval != null) {
		clearInterval(interval);
	}
	
	interval = setInterval(function() {
		_get_ping_result();
	}, 2000);
}

layui.use(['table', 'layer', 'form'], function(){
	var table = layui.table;
	var layer = layui.layer;
	var form = layui.form;
	
	//自定义验证规则
	form.verify({
		domain: function(value) {
			if (value == null || value.length <= 0) {
				return '此项不能为空';
			}
			if (value.length > 255) {
				return '长度限255个字符';
			}
		}
		,count: function(value) {
			if (value == null || value.length <= 0) {
				return '此项不能为空且必须填写数字';
			}
			if (value > 20 || value < 1) {
				return '端口范围在 1~20';
			}
		}
		,size: function(value) {
			if (value == null || value.length <= 0) {
				return '此项不能为空且必须填写数字';
			}
			if (value > 1472 || value < 4) {
				return '端口范围在 4~1472';
			}
		}
	});
	
  	//监听提交
	form.on('submit(submit)', function(data) {
		//ajax提交，并接收返回结果，如果返回成功，则返回
		//调用layer.close关闭当前弹层，并提示设置成功
		/*layer.alert(JSON.stringify(data.field), {
			title: '最终的提交信息'
		});*/
		
		var domain = get_input_value("domain");
		var count = get_input_value("count");
		var size = get_input_value("size");
		
		var data = {};
		data.domain = domain;
		data.count = count;
		data.size = size;

		layer.closeAll();
		
		//配置一个透明的询问框
		layer.msg('开始诊断，请稍后...', {
			time: 0, //3s后自动关闭
			btn: ['关闭'],
			yes: function(index) {
				layer.close(index);
			},
			end: function(index) {
				layer.close(index);
			}
		});
				
		$.ajax({
			type: 'post',
			dataType: 'json',
			url: '/ping',
			data: jQuery.param(data),
			async: true,
			success: function (result) {
				if (result['res'] == 'success') {
					get_ping_result();
				}
				else {
					//配置一个透明的询问框
					layer.msg('操作失败!!! [ ' + result['msg'] + ' ]', {
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

