layui.use(['layer', 'form', 'layedit'], function() { //独立版的layer无需执行这一句
	var $ = layui.jquery, layer = layui.layer; //独立版的layer无需执行这一句
	var form = layui.form;
	
	//自定义验证规则
	form.verify({
		login_password: function(value) {
			if (value == null || value.length <= 0) {
				return '此项不能为空';
			}
			if (value.length < 6) {
				return '密码长度必须6-12位';
			}
		}
	});
 
	//监听提交
	form.on('submit(login)', function(data){
		//ajax提交，并接收返回结果，如果返回成功，则返回
		//调用layer.close关闭当前弹层，并提示设置成功
		/*layer.alert(JSON.stringify(data.field), {
			title: '最终的提交信息'
		});
		
		var login_password = document.getElementById("login_password").value;
		if (login_password == null || login_password.length < 6) {
			return false;
		}
		
		var password = document.getElementById('login_password').value;
		var data = {};
		data.password = password;
		$.ajax({
			type: 'post',
			dataType: 'html',
			url: '/dologin',
			data: jQuery.param(data),
			async: true,
			success: function (result) {

			},
			beforeSend: function(x) {
				
			},
			error: function(e, v, m) {

			}
		});
		
		return false;*/
	});
	
	var active = {
		login: function(anim) {
			layer.open({
				type: 1
				,title: false //不显示标题栏
				,closeBtn: false
				,area: '500px'
				,shade: 0.5	//背景透明度
				,id: 'LAY_layuipro' //设定一个id，防止重复弹出
				,btn: false
				,btnAlign: 'c'
				,anim: anim//弹出动画
				,move: false
				//,moveType: 0 //拖拽模式，0或者1
				,content: '\
					<div class="layui-col-xs12 layui-col-sm12 layui-col-md12">\
						<div style="padding: 50px; line-height: 22px; background-color: #393D49; color: #fff; font-weight: 300;">\
							<form class="layui-form" action="/dologin" method="post">\
								<div class="layui-form-item">\
									<div class="layui-input-inline p60">\
										<input type="password" id="login_password" name="password" lay-verify="login_password" lay-verType="tips" placeholder="请输入密码登录" autocomplete="off" class="layui-input">\
									</div>\
									<div class="layui-input-inline p20">\
										<button class="layui-btn" lay-submit="" lay-filter="login">立即登录</button>\
									</div>\
								</div>\
								<div class="layui-form-item">\
									<p class="layui-input-inline p80 layui-word-aux">\
										<span>忘记密码, 请重置路由器, 默认密码为 admin123</span>\
									</p>\
								</div>\
							</form>\
						</div>\
					</div>'
				,success: function(layero) {

				}
				,yes: function(index, layero) {
					//do somthing
					
					layer.close(index); //如果设定了yes回调，需进行手工关闭
				}
			});
		}
	};
	
	active.login(0);
});