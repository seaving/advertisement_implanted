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

layui.use(['table', 'layer', 'form'], function(){
	var table = layui.table;
	var layer = layui.layer;
	var form = layui.form;
	
	//自定义验证规则
	form.verify({
		rule_name: function(value) {
			if (value == null || value.length <= 0) {
				return '此项不能为空';
			}
			if (value.length > 32) {
				return '长度限1~32个字符';
			}
		}
		,src_port: function(value) {
			if (value == null || value.length <= 0) {
				return '此项不能为空且必须填写数字';
			}
			if (value > 65535) {
				return '端口范围在 1~65535';
			}
		}
		,dst_port: function(value) {
			if (value == null || value.length <= 0) {
				return '此项不能为空且必须填写数字';
			}
			if (value > 65535) {
				return '端口范围在 1~65535';
			}
		}
		,dst_ip: function(value) {
			if (value == null || value.length <= 0) {
				return '此项不能为空';
			}
			
			if (isValidIP(value) == false) {
				return 'IP格式错误，格式例子: 192.168.1.1';
			}
		}
	});
	
	var Function = {
  		add_rule: function() {
			layer_index = layer.open({
				type: 1
				,title: "添加规则" //不显示标题栏
				,area: '500px'
				,shade: 0.2	//背景透明度
				,id: 'LAY_layuipro' //设定一个id，防止重复弹出
				,btn: false
				,btnAlign: 'c'
				,moveType: 1 //拖拽模式，0或者1
				,content: '<div style="padding: 50px; line-height: 22px; color: #fff; font-weight: 300;">\
						<form class="layui-form" action="">\
							<div class="layui-form-item">\
								<label class="layui-form-label"><strong style="color:black;">规则名</strong> <strong class="colorR">*</strong></label>\
								<div class="layui-input-block">\
									<input type="text" required id="rule_name" name="rule_name" lay-verify="rule_name" lay-verType="tips" placeholder="* 必填 *" autocomplete="off" class="layui-input">\
								</div>\
							</div>\
							<div class="layui-form-item">\
								<label class="layui-form-label"><strong style="color:black;">外部端口</strong> <strong class="colorR">*</strong></label>\
								<div class="layui-input-block">\
									<input type="number" required id="src_port" name="src_port" lay-verify="src_port" lay-verType="tips" placeholder="* 必填 只能填写数字 *" autocomplete="off" class="layui-input">\
								</div>\
							</div>\
							<div class="layui-form-item">\
								<label class="layui-form-label"><strong style="color:black;">内部端口</strong> <strong class="colorR">*</strong></label>\
								<div class="layui-input-block">\
									<input type="number" required id="dst_port" name="dst_port" lay-verify="dst_port" lay-verType="tips" placeholder="* 必填 只能填写数字 *" autocomplete="off" class="layui-input">\
								</div>\
							</div>\
							<div class="layui-form-item">\
								<label class="layui-form-label"><strong style="color:black;">内部IP</strong> <strong class="colorR">*</strong></label>\
								<div class="layui-input-block">\
									<input type="text" required id="dst_ip" name="dst_ip" lay-verify="dst_ip" lay-verType="tips" placeholder="* 必填 *" autocomplete="off" class="layui-input">\
								</div>\
							</div>\
							<div class="layui-form-item">\
								<div class="layui-input-block">\
									<button class="layui-btn layui-btn-normal" id="submit_form" lay-submit="" lay-filter="submit_form">保存</button>\
									<button type="button" class="layui-btn layui-btn-primary" id="cancel_form" onclick="close_layer();">取消</button>\
								</div>\
							</div>\
						</form>\
					</div>'
			});
		}
	};
  
  	//监听提交
	form.on('submit(submit_form)', function(data) {
		//ajax提交，并接收返回结果，如果返回成功，则返回
		//调用layer.close关闭当前弹层，并提示设置成功
		/*layer.alert(JSON.stringify(data.field), {
			title: '最终的提交信息'
		});*/
		
		var rule_name = get_input_value("rule_name");
		var src_port = get_input_value("src_port");
		var dst_port = get_input_value("dst_port");
		var dst_ip = get_input_value("dst_ip");
		
		var data = {};
		data.name = rule_name;
		data.src_port = src_port;
		data.dst_port = dst_port;
		data.dst_ip = dst_ip;
		
		$.ajax({
			type: 'post',
			dataType: 'json',
			url: '/setPortForward',
			data: jQuery.param(data),
			async: true,
			success: function (result) {
				if (result['res'] == 'success') {
					layer.closeAll();
					
					//配置一个透明的询问框
					layer.msg('添加成功', {
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
					layer.msg('添加失败，参数异常!!! [ ' + result['msg'] + ' ]', {
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
	
   //监听工具条
  table.on('tool(port_forwarding_table)', function(obj) {
    var data = obj.data;
	var name = data.name;
	data = {};
	data.name = name;
	
	if (obj.event === 'delete') {
		//配置一个透明的询问框
		layer.confirm(
			'删除规则【 ' + name + ' 】？'
			,{
				title: '确认提示'
				,closeBtn: false
				,btn: ['确认', '取消']
				,shade: 0.2	//背景透明度
				,btnAlign: 'c'
			}
			,function(index) {
				//确认回调
				//alert("确认回调");
				$.ajax({
					type: 'post',
					url: '/deletePortForward',
					dataType: 'json',
					data: jQuery.param(data),
					async: true,
					success: function (result) {
						if (result['res'] == 'success') {
							layer.msg('删除规则【 ' + name + ' 】成功', {
								time: 3000, //3s后自动关闭
								btn: ['关闭']
							});
							layer.close(index);
							obj.del();
						}
						else {
							//配置一个透明的询问框
							layer.msg('操作失败!!! [' + result['msg'] + ']', {
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
			,function(index) {
				//取消回调
				
				layer.close(index);
			}
		);
    }
  });
  
  table.render({
	elem : '#port_forwarding_table',
	url : '/getPortForwardList',
	method: 'POST',
	page : {
		limit: 10,
		limits: [10],
		theme : '#6161d1'
	},
	skin: 'line',
	size:'lg',
	even: true, //隔行背景
	cols : [ [ 
	{
		field : 'name',
		title : '规则名',
		width : '26%',
		align : 'center'
	}, {
		field : 'src_port',
		title : '外部端口',
		width : '25%',
		align : 'center'
	}, {
		field : 'dst_to',
		title : '转发到',
		width : '25%',
		align : 'center'
	}, {
		field : 'option',
		title : '操作',
		width : '25%',
		align : 'center',
		toolbar: '#option_table'
	} ] ]
  });
  
  
	$('#add_rule').on('click', function() {
		Function.add_rule();
	});
});

