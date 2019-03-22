function dynamic_get_online_cnt() {
	$.ajax({
		type: 'get',
		url: '/getOnlineCnt',
		dataType: 'json',
		async: true,
		success: function (result) {
			if (result['res'] == 'success') {
				var online_client_cnt = result['online_client_cnt'];
				var auth_online_client_cnt = result['auth_online_client_cnt'];
				var no_auth_online_client_cnt = result['no_auth_online_client_cnt'];
				
				document.getElementById("online_client_cnt").innerHTML = online_client_cnt;
				document.getElementById("auth_online_client_cnt").innerHTML = auth_online_client_cnt;
				document.getElementById("no_auth_online_client_cnt").innerHTML = no_auth_online_client_cnt;
			}
			else {
				//配置一个透明的询问框
				layer.msg('获取在线终端数量失败!!! [' + result['msg'] + ']', {
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

layui.use('table', function(){
  var table = layui.table;

    //监听工具条
  table.on('tool(client_table)', function(obj) {
    var data = obj.data;
	var mac = data.mac;
	var name = data.name;
	if (mac == null || mac.length <= 0) {
		return;
	}
	
	data = {};
	data.mac = mac;
	data.name = name;
    if(obj.event === 'kick_off') {
		
		//配置一个透明的询问框
		layer.confirm(
			'踢掉终端【 mac地址为: ' + mac + ' 】 <br>踢掉后的终端重新连接AP可恢复上网'
			,{
				title: '警告'
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
					url: '/disconnect',
					dataType: 'json',
					data: jQuery.param(data),
					async: true,
					success: function (result) {
						if (result['res'] == 'success') {
							layer.msg('已中断终端【 mac地址为: ' + mac + ' 】的网络连接', {
								time: 3000, //3s后自动关闭
								btn: ['关闭']
							});
							layer.close(index);
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
	else if(obj.event === 'defriend') {
		//配置一个透明的询问框
		layer.confirm(
			'拉黑终端【 mac地址为: ' + mac + ' 】 <br>拉黑后的终端将无法连接AP上网'
			,{
				title: '警告'
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
					url: '/setBlack',
					dataType: 'json',
					data: jQuery.param(data),
					async: true,
					success: function (result) {
						if (result['res'] == 'success') {
							layer.msg('已拉黑终端【 mac地址为: ' + mac + ' 】', {
								time: 3000, //3s后自动关闭
								btn: ['关闭']
							});
							layer.close(index);
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
	elem : '#client_table',
	url : '/getOnlineClientList',
	method: 'POST',
	page : {
		limit: 10,
		limits: [10],
		theme : '#6161d1'
	},
	where: {
		br_dev: "br-guest"
	},
	skin: 'line',
	size:'lg',
	even: true, //隔行背景
	cols : [ [ 
	/*{
		field : 'brand',
		title : '设备品牌',
		width : '17%',
		align : 'center'
	}, */{
		field : 'name',
		title : '设备名',
		width : '31%',
		align : 'center'
	}, {
		field : 'mac',
		title : 'MAC地址',
		width : '19%',
		align : 'center'
	}, {
		field : 'ip',
		title : 'IP地址',
		width : '19%',
		align : 'center'
	},/* {
		field : 'traffic',
		title : '流量(MB)',
		width : '10%',
		align : 'center'	
	}, */{
		field : 'auth_state',
		title : '认证状态',
		width : '10%',
		align : 'center',
		templet: '#auth_state'
	}, /*{
		field : 'dev_type',
		title : '终端类型',
		width : '10%',
		align : 'center'
	}, */{
		field : 'signal',
		title : '信号强度',
		width : '10%',
		align : 'center',
		templet: '#signal'
	}, {
		field : 'option',
		title : '操作',
		width : '11%',
		align : 'left',
		toolbar: '#option_table'
	} ] ]
  });
});

dynamic_get_online_cnt();
