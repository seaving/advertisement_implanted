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
				<li class="layui-nav-item">\
					<a href="javascript:;" data-method="reboot_dev"  id="reboot_dev">\
						<i class="layui-icon nav_icon power layui-hide-xs"><img src="../img/power.png"></i>\
						<span class="font-color-white"> 立即重启<span>\
					</a>\
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
			<ul class="layui-nav layui-nav-tree site-demo-nav" id="left_nav_tree">\
				<li class="layui-nav-item" id="home">\
					<a class="" href="../html/home.html">首页</a>\
				</li>\
				\
				<li class="layui-nav-item" id="network_set">\
					<a class="" href="javascript:;">网络设置</a>\
					<dl class="layui-nav-child">\
						<dd><a href="../html/network_wan_set.html">外网设置</a></dd>\
						<dd><a href="../html/network_lan_set.html">内网设置</a></dd>\
						<dd><a href="../html/network_wlan_set.html">无线设置</a></dd>\
						<dd><a href="../html/network_qos_set.html">QoS智能限速</a></dd>\
					</dl>\
				</li>\
				\
				<li class="layui-nav-item" id="advanced_set">\
					<a class="" href="javascript:;">高级设置</a>\
					<dl class="layui-nav-child">\
						<dd><a href="../html/client_management_auth_online.html">终端管理</a></dd>\
						<dd><a href="../html/non_authentication.html">免认证设置</a></dd>\
						<dd><a href="../html/port_forwarding.html">端口转发</a></dd>\
						<!--<dd><a href="../html/third_certification.html">第三方认证</a></dd>-->\
					</dl>\
				</li>\
				\
				<li class="layui-nav-item" id="system_tool">\
					<a class="" href="javascript:;">系统工具</a>\
					<dl class="layui-nav-child">\
						<dd><a href="../html/system_network_diagnosis.html">网络诊断</a></dd>\
						<dd><a href="../html/system_update.html">软件升级</a></dd>\
						<dd><a href="../html/system_reset.html">恢复出厂设置</a></dd>\
						<dd><a href="../html/system_reboot.html">重启设置</a></dd>\
					</dl>\
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
				<p>@ 2018 <a href="http://www.qcwifi.ltd">深圳前辰科技有限公司</a></p>\
				<p>\
					<a href="http://www.qcwifi.ltd/rapp" target="_blank">前辰云平台</a>\
					<a href="mailto:31427227@qq.com">联系我们</a>\
					<a href="http://fly.layui.com/jie/2461/" target="_blank">微信公众号</a>\
				</p>\
			</div>\
		</div>';
	//***********************************************************
	this.Function = {
		activity_left_nav: function(id) {
			var ul = document.getElementById("left_nav_tree");
			if (ul == null) {
				return;
			}
			
			var li_list = ul.getElementsByTagName("li");
			for (var i = 0; i < li_list.length; i ++) {
				if (li_list[i].id != null) {
					if (id != "all") {
						if (li_list[i].id == id) {
							li_list[i].classList.add("layui-nav-itemed");
						}
						else {
							li_list[i].classList.remove("layui-nav-itemed");
						}
					}
					else {
						li_list[i].classList.add("layui-nav-itemed");
					}
				}
			}
		},
		
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
		
		nav_show: function(id) {
			this.header_nav_show();
			this.left_side_nav_show();
			this.activity_left_nav(id);
			this.footer_show();
		}
	};
}

var common_nav = new common_nav();
function nav_show(id) {
	common_nav.Function.nav_show(id);
}

