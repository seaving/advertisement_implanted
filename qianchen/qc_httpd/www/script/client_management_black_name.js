layui.use('table', function(){
  var table = layui.table;
  
  //监听工具条
  table.on('tool(black_name_table)', function(obj) {
    var data = obj.data;
	var mac = data.mac;
	var name = data.name;
	if (mac == null || mac.length <= 0) {
		return;
	}
	
	data = {};
	data.mac = mac;
	data.name = name;
    if(obj.event === 'recovery') {
		var mac = data.mac;
		if (mac == null || mac.length <= 0) {
			return;
		}
		
		//配置一个透明的询问框
		layer.confirm(
			'恢复终端【 mac地址为: ' + mac + ' 】 <br>恢复后的终端重新连接AP可恢复上网'
			,{
				title: '提示'
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
					url: '/deleteBlack',
					dataType: 'json',
					data: jQuery.param(data),
					async: true,
					success: function (result) {
						if (result['res'] == 'success') {
							layer.msg('终端【 mac地址为: ' + mac + ' 】已从黑名单中移除', {
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
	elem : '#black_name_table',
	url : '/getBlackList',
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
	/*{
		field : 'brand',
		title : '设备品牌',
		width : '28%',
		align : 'center'
	},*/ {
		field : 'name',
		title : '设备名',
		width : '41%',
		align : 'center'
	}, {
		field : 'mac',
		title : 'MAC地址',
		width : '30%',
		align : 'center'
	}, /*{
		field : 'ip',
		title : 'IP地址',
		width : '22%',
		align : 'center'
	}, {
		field : 'dev_type',
		title : '终端类型',
		width : '12%',
		align : 'center'
	},*/ {
		field : 'option',
		title : '操作',
		width : '29%',
		align : 'center',
		toolbar: '#option_table'
	} ] ]
  });
});

