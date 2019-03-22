layui.use('layer', function() { //独立版的layer无需执行这一句
	var $ = layui.jquery, layer = layui.layer; //独立版的layer无需执行这一句

	//触发事件
	var active = {
		bond: function(){
			layer.open({
				type: 1
				,title: "设备绑定" //不显示标题栏
				,closeBtn: false
				,area: '300px;'
				,shade: 0.2
				,id: 'LAY_layuipro' //设定一个id，防止重复弹出
				,btn: ['我要绑定', '残忍拒绝']
				,btnAlign: 'c'
				,moveType: 1 //拖拽模式，0或者1
				,content: '\
					<div style="padding: 50px; line-height: 22px; background-color: #393D49; color: #fff; font-weight: 300;">\
						绑定设备后，可以享受更多功能\
					</div>'
				,success: function(layero) {
					var btn = layero.find('.layui-layer-btn');
					btn.find('.layui-layer-btn0').attr({
						href: 'http://www.layui.com/'
						,target: '_blank'
					});
				}
			});
		}
	};

	$('#dev_bond').on('click', function(){
		var othis = $(this), method = othis.data('method');
		active[method] ? active[method].call(this, othis) : '';
	});
});