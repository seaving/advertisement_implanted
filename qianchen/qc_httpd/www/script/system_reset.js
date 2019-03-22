layui.use('layer', function(){
	var layer = layui.layer;

	var Function = {
  		system_reset: function() {
			//配置一个透明的询问框
			layer.confirm(
				'恢复将会清空路由器现有配置, 恢复过程中将会重启路由器。'
				,{
					title: "警告"
					,closeBtn: false
					,btn: ['确认', '取消']
					,shade: 0.2	//背景透明度
					,btnAlign: 'c'
				}
				,function(index) {
					//确认回调
					$.ajax({
						type: 'post',
						dataType: 'json',
						url: '/systemReset',
						data: {},
						async: true,
						success: function (result) {
							if (result['res'] == 'success') {
								layer.closeAll();
								layer.msg('设备恢复完成，自动重启设备，请勿断电...', {
									time: 10000, //3s后自动关闭
									btn: ['关闭']
								});
								$.ajax({
									type: 'post',
									dataType: 'json',
									url: '/reboot',
									data: {},
									async: true,
									success: function (result) {
										if (result['res'] == 'success') {
											layer.closeAll();
											layer.msg('自动重启中，请勿断电...', {
												time: 60000, //3s后自动关闭
												btn: ['关闭']
											});
										}
										else {
											//配置一个透明的询问框
											layer.closeAll();
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
							}
							else {
								//配置一个透明的询问框
								layer.closeAll();
								layer.msg('操作失败!!! [ ' + result['msg'] + ' ]', {
									time: 10000, //3s后自动关闭
									btn: ['关闭']
								});
							}
						},
						beforeSend: function(x) {
							layer.closeAll();
							layer.msg('正在通知设备恢复出厂设置...', {
								time: 60000, //3s后自动关闭
								btn: ['关闭']
							});
						},
						error: function(e, v, m) {
						}
					});
				}
				,function(index) {
					//取消回调
					layer.closeAll();
				}
			);
		}
	};

	$('#system_reset').on('click', function() {
		Function.system_reset();
	});
});
