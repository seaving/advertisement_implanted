layui.use('layer', function() { //独立版的layer无需执行这一句
	var $ = layui.jquery, layer = layui.layer; //独立版的layer无需执行这一句

	//触发事件
	var active = {
		bond: function() {
			$.ajax({
				type: 'get',
				url: '/bond',
				dataType: 'json',
				async: true,
				success: function (result) {
					layer.closeAll();
					
					if (result['res'] == 'success') {
						//配置一个透明的询问框
						var bond_code = result['code'];
						layer.confirm(
							'设备绑定码：&nbsp;&nbsp;&nbsp;&nbsp;' + '<strong>' + bond_code + '</strong>'
							,{
								title: false
								,closeBtn: false
								,btn: ['重新获取', '取消']
								,shade: 0.2	//背景透明度
								,btnAlign: 'c'
							}
							,function(index) {
								layer.close(index);
								$('#dev_bond').click();
							}
							,function(index) {
								//取消回调
								
								layer.close(index);
							}
						);
					}
					else {
						//配置一个透明的询问框
						layer.msg('获取绑定码失败!!! [' + result['msg'] + ']', {
							time: 3000, //3s后自动关闭
							btn: ['关闭']
						});
					}
				},
				beforeSend: function(x) {
					layer.closeAll();
					
					//配置一个透明的询问框
					layer.msg('正在请求绑定码，请稍候. . .', {
						time: 60000, //3s后自动关闭
						btn: ['关闭']
					});
				},
				error: function(e, v, m) {
					layer.closeAll();
				}
			});
		}
	};

	$('#dev_bond').on('click', function(){
		var othis = $(this), method = othis.data('method');
		active[method] ? active[method].call(this, othis) : '';
	});
});
