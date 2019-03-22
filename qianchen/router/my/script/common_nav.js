function common_nav(action) {
	
	//---------------------------------------------- 头部导航 -----------------------------------------------------
	var __header_nav__ = 
	'<div class="layui-header header header-demo">\
		<div class="layui-main">\
			<a class="logo" href="/">\
				<img src="../img/logodemo.png" alt="layui">\
			</a>\
			\
			<ul class="layui-nav">\
				<li class="layui-nav-item">\
					<a href="javascript:;" data-method="bond"  id="dev_bond">\
						<i class="layui-icon nav_icon layui-hide-xs"><img src="../img/bond.png"></i>\
						<span class="font-color-white"> 设备绑定<span>\
					</a>\
				</li>\
				\
				<li class="layui-nav-item">\
					<a href="javascript:;">\
						<i class="layui-icon nav_icon layui-hide-xs"><img src="../img/user.png"></i>\
						<span class="font-color-white"> 帐户<span>\
					</a>\
					<dl class="layui-nav-child">\
						<dd>\
							<a href="javascript:;" data-method="modify_password" id="modify_password">\
								<i class="layui-icon  layui-hide-xs"><img src="../img/passwd.png"></i> 修改密码\
							</a>\
						</dd>\
						<dd>\
							<a href="javascript:;" data-method="logout" id="logout">\
								<i class="layui-icon layui-hide-xs"><img src="../img/logout.png"></i> 退出\
							</a>\
						</dd>\
					</dl>\
				</li>\
				<!-- 占位 -->\
				<li class="layui-nav-item" style="height: 30px; text-align: center"></li>\
			</ul>\
		</div>\
	</div>';
	
	//---------------------------------------------- 左侧导航 -----------------------------------------------------
	var __left_side_nav__ = 
	'<div class="layui-side layui-bg-black">\
		<div class="layui-side-scroll">\
			<ul class="layui-nav layui-nav-tree site-demo-nav">\
				<li class="layui-nav-item layui-nav-itemed">\
					<a class="" href="../html/home.html">首页</a>\
				</li>\
				\
				<li class="layui-nav-item layui-nav-itemed">\
					<a class="" href="javascript:;">网络设置</a>\
					<dl class="layui-nav-child">\
						<dd><a href="../html/network_wan_set_dhcp.html">外网设置</a></dd>\
						<dd><a href="../html/network_lan_set.html">内网设置</a></dd>\
						<dd><a href="../html/network_wlan_set.html">无线设置</a></dd>\
						<dd><a href="../html/network_qos_set.html">QoS智能限速</a></dd>\
					</dl>\
				</li>\
				\
				<li class="layui-nav-item layui-nav-itemed">\
					<a class="" href="javascript:;">高级设置</a>\
					<dl class="layui-nav-child">\
						<dd><a href="">设备管理</a></dd>\
						<dd><a href="">免认证设置</a></dd>\
						<dd><a href="">AC设置</a></dd>\
						<dd><a href="">端口转发</a></dd>\
						<dd><a href="">第三方认证</a></dd>\
					</dl>\
				</li>\
				\
				<li class="layui-nav-item layui-nav-itemed">\
					<a class="" href="">系统设置</a>\
				</li>\
				\
				<li class="layui-nav-item layui-nav-itemed">\
					<a class="" href="">故障管理</a>\
				</li>\
				\
				<!-- 占位 -->\
				<li class="layui-nav-item" style="height: 30px; text-align: center"></li>\
			</ul>\
		</div>\
	</div>';
	
	//---------------------------------------------- footer -----------------------------------------------------
	var __footer__	= 
		'<div class="layui-footer footer footer-demo">\
			<div class="layui-main">\
				<!-- 底部固定区域 -->\
				<center>深圳市前辰科技有限公司</center>\
			</div>\
		</div>';
	//***********************************************************
	this.Function = {
		header_nav_show: function() {
			var element = document.getElementById("header_nav");
			if (element != null) {
				element.insertAdjacentHTML("beforeEnd", __header_nav__);
			}
		},
		
		left_side_nav_show: function() {
			var element = document.getElementById("left_side_nav");
			if (element != null) {
				element.insertAdjacentHTML("beforeEnd", __left_side_nav__);
			}
		},
		
		footer_show: function() {
			var element = document.getElementById("footer");
			if (element != null) {
				element.insertAdjacentHTML("beforeEnd", __footer__);
			}
		},
		
		nav_show: function() {
			this.header_nav_show();
			this.left_side_nav_show();
			this.footer_show();
		}
	};
}

var common_nav = new common_nav();
common_nav.Function.nav_show();
