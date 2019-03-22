
layui.use(['layer', 'form', 'layedit'], function() { //独立版的layer无需执行这一句
	var $ = layui.jquery, layer = layui.layer; //独立版的layer无需执行这一句
	
	var form = layui.form
	
	//自定义验证规则
	form.verify({
		source_password: function(value) {
			if (value == null || value.length <= 0) {
				return '此项不能为空';
			}
			if (value.length < 6) {
				return '密码长度必须6-12位';
			}
		}
		,new_password: function(value) {
			if (value == null || value.length <= 0) {
				return '此项不能为空';
			}
			if (value.length < 6) {
				return '密码长度必须6-12位';
			}
		}
		,repeat_password: function(value) {
			if (value == null || value.length <= 0) {
				return '此项不能为空';
			}
			if (value.length < 6) {
				return '密码长度必须6-12位';
			}
			
			var new_password = document.getElementById("new_password").value;
			if (value != new_password) {
				return '两次输入的新密码不一致';
			}
		}
	});
 
	//监听提交
	form.on('submit(modify_password_submit)', function(data){
		//ajax提交，并接收返回结果，如果返回成功，则返回
		//调用layer.close关闭当前弹层，并提示设置成功
		layer.alert(JSON.stringify(data.field), {
			title: '最终的提交信息'
		});
		
		var source_password = document.getElementById("source_password").value;
		var new_password = document.getElementById("new_password").value;
		var repeat_password = document.getElementById("repeat_password").value;
		
		if (source_password == null || new_password == null || repeat_password == null) {
			return false;
		}
		if (source_password.length < 6 || new_password.length < 6 || repeat_password.length < 6) {
			return false;
		}
		if (new_password != repeat_password) {
			return false;
		}
		
		return false;
	});
  
	//触发事件
	var active = {
		modify_password: function() {
			layer.open({
				type: 1
				,title: "修改密码" //不显示标题栏
				,closeBtn: false
				,area: '300px'
				,shade: 0.2	//背景透明度
				,id: 'LAY_layuipro' //设定一个id，防止重复弹出
				,btn: ['提交', '关闭']
				,btnAlign: 'c'
				,moveType: 1 //拖拽模式，0或者1
				,content: '\
					<div style="padding: 50px; line-height: 22px; background-color: #393D49; color: #fff; font-weight: 300;">\
						<form class="layui-form" action="">\
							<div class="layui-form-item">\
								<div class="">\
									<input type="password" id="source_password" name="source_password" lay-verify="source_password" lay-verType="tips" placeholder="请输入原始密码" autocomplete="off" class="layui-input">\
								</div>\
							</div>\
							<div class="layui-form-item">\
								<div class="">\
									<input type="password" id="new_password" name="new_password" lay-verify="new_password" lay-verType="tips" placeholder="请输入新密码" autocomplete="off" class="layui-input">\
								</div>\
							</div>\
							<div class="layui-form-item">\
								<div class="">\
									<input type="password" id="repeat_password" name="repeat_password" lay-verify="repeat_password" lay-verType="tips" placeholder="再次输入新密码" autocomplete="off" class="layui-input">\
								</div>\
							</div>\
							<div class="layui-form-item" style="display:none">\
								<button class="layui-btn" id="submit_form" lay-submit="" lay-filter="modify_password_submit">立即提交</button>\
							</div>\
						</form>\
					</div>'
				,success: function(layero) {

				}
				,yes: function(index, layero) {
					//do somthing
					document.getElementById("submit_form").click();
					
					//layer.close(index); //如果设定了yes回调，需进行手工关闭
				}
			});
		}
		,logout: function() {
			//配置一个透明的询问框
			layer.confirm(
				'确认退出管理系统？'
				,{
					title: false
					,closeBtn: false
					,btn: ['确认', '取消']
					,shade: 0.2	//背景透明度
					,btnAlign: 'c'
				}
				,function(index) {
					//确认回调
					alert("确认回调");
					
					layer.close(index);
				}
				,function(index) {
					//取消回调
					
					layer.close(index);
				}
			);
		}
	};

	$('#modify_password, #logout').on('click', function() {
		var othis = $(this), method = othis.data('method');
		active[method] ? active[method].call(this, othis) : '';
	});
});