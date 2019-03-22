layui.use('layer', function() { //独立版的layer无需执行这一句
	var $ = layui.jquery, layer = layui.layer; //独立版的layer无需执行这一句

	//触发事件
	var active = {
		reboot_dev: function(){
			layer.open({
				type: 1
				,title: "设备重启" //不显示标题栏
				,closeBtn: false
				,area: '400px;'
				,shade: 0.2
				,id: 'LAY_layuipro' //设定一个id，防止重复弹出
				,btn: ['确定', '取消']
				,btnAlign: 'c'
				,moveType: 1 //拖拽模式，0或者1
				,content: '\
					<div style="padding: 50px; line-height: 22px; font-weight: 300;">\
						如您提交过设置，设备重启后您的设置将会全部生效，是否立即重启？\
					</div>'
				,success: function(layero) {

				}
				,yes: function(index, layero) {
					//do somthing
					
					$.ajax({
						type: 'get',
						dataType: 'json',
						url: '/reboot',
						data: '',
						async: true,
						success: function (result) {
							if (result['res'] == 'success') {
								layer.closeAll();
								
								//配置一个透明的询问框
								layer.msg('正在重启中，请等候大约2分钟时间 ...', {
									time: 120000, //3s后自动关闭
									btn: ['关闭']
								});
							}
							else {
								//配置一个透明的询问框
								layer.msg('重启失败，请自行断开设备电源 ...', {
									time: 5000, //3s后自动关闭
									btn: ['关闭']
								});
							}
						},
						beforeSend: function(x) {
							
						},
						error: function(e, v, m) {
							//配置一个透明的询问框
							layer.msg('异常提示:<br>无法与路由器建立连接，请保证与路由器连接正常，您可自行断开设备电源 ...', {
								time: 10000, //3s后自动关闭
								btn: ['关闭']
							});
						}
					});
					
					layer.close(index); //如果设定了yes回调，需进行手工关闭
				}
			});
		}
	};

	$('#reboot_dev').on('click', function(){
		var othis = $(this), method = othis.data('method');
		active[method] ? active[method].call(this, othis) : '';
	});
});
