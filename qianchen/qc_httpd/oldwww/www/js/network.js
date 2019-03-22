/**
 * Created by xuym on 2016/12/15.
 */

$(function () {
	htmlIsOnReady(1);
});

function tips(msg) {
	layer.closeAll();

	layer.msg(msg, {
		time: 120000
	});
}

function redirect() {
	$.ajax({
		type: "get",
		data: {},
		url: "../html/network.html?api=get_wan_info",
		success: function (result) {
			var tmpSS = JSON.parse(result);
			show_wan_info(tmpSS);
		}
	});
}

function selectProto(proto) {
	if (proto == 'dhcp') {
		$("#network-input").css("display","none");
	}
	else if (proto == 'pppoe') {
		$("#network-input").css("display","block");
	}
	else if (proto == 'static') {

	}
	else {
		
	}
}

function show_wan_info(tmpSS) {
	var proto = tmpSS["proto"];

	if (proto == "dhcp") {
		$("#network-input").css("display","none");

		// $("#proto").val('dhcp');
		// $("#proto").find("option[value='pppoe']").removeAttr("selected");
		// $("#proto").find("option[value='dhcp']").attr("selected",true);
		configSelectOption("proto", 'dhcp');
		$("#used_proto").val("动态IP");

		$("#username").val("");
		$("#password").val("");
	}
	else if (proto == 'pppoe') {
		$("#network-input").css("display","block");

		// $("#proto").val('pppoe');
		// $("#proto").find("option[value='dhcp']").removeAttr("selected");
		// $("#proto").find("option[value='pppoe']").attr("selected",true);
		configSelectOption("proto", 'pppoe');
		$("#used_proto").val("PPPoE(宽带拨号)");

		$("#username").val(tmpSS["username"]);
		$("#password").val(tmpSS["password"]);
	}

	$("#ipaddr").html(tmpSS["ip"]);
	$("#mask").html(tmpSS["mask"]);
	$("#gateway").html(tmpSS["gateway"]);
	$("#dns_first").html(tmpSS["dns_first"]);
	$("#dns_second").html(tmpSS["dns_second"]);
}

function submitNetworkSetEvent() {
	var proto = $("#proto").val();
	if (proto == 'pppoe') {
		submitNetworkPPPoERequest();
	}
	else if (proto == 'dhcp') {
		submitNetworkDHCPRequest();
	}
	else {
		layer.msg("请选择连接类型");
	}
}

function submitNetworkDHCPRequest() {
	if (!checkDynamicIPInput()) {
		return;
	}

	var proto = $('#proto').val();

    var data = {};
    data.proto = proto;
	
    $.ajax({
        type: "post",
        data: jQuery.param(data),
        url: "../html/network.html?api=set_network_dhcp",
        success: function (result) {
            var tmpSS = JSON.parse(result);
            var res = tmpSS["res"];
			if (res == 'success') {
				// 成功
				layer.closeAll();

				layer.msg("提交成功，等待路由器设置完成...", {
					time: 3000
				}, function() {
					// 刷新当前页面
					window.location.reload();
				});
			} else {
				layer.msg("网络设置失败", {
					time: 3000
				});
			}
			return ;
        }
    })
	tips("设置过程路由器会重启网络，如若页面刷新不成功，请等待1分钟手动刷新浏览器...");
}

function submitNetworkPPPoERequest() {
	if (!checkPPPoEInput()) {
		return;
	}

	var proto = $('#proto').val();
	var username= $('#username').val();
	var passwd = $('#password').val();

    var data = {};
    data.proto = proto;
	data.username = username;
	data.password = passwd;
	
    $.ajax({
        type: "post",
        data: jQuery.param(data),
        url: "../html/network.html?api=set_network_pppoe",
        success: function (result) {
            var tmpSS = JSON.parse(result);
            var res = tmpSS["res"];
			if (res == 'success') {
				// 成功
				layer.closeAll();

				layer.msg("提交成功，等待路由器设置完成...", {
					time: 3000
				}, function() {
					// 刷新当前页面
					window.location.reload();
				});
			} else {
				layer.msg("网络设置失败", {
					time: 3000
				});
			}
			return ;
        }
    })
	tips("设置过程路由器会重启网络，如若页面刷新不成功，请等待1分钟手动刷新浏览器...");
}

/**
 *	PPPoE拨号连接
 */
function checkPPPoEInput() {
	var proto = $('#proto').val();
	if (proto == null || proto.length == 0) {
		layer.msg("请选择连接类型");
		return false;
	}
	var username = $('#username').val();
	if (username = null || username.length == 0) {
		layer.msg("请输入上网账号");
		return false;
	}
	var passwd = $('#password').val();
	if (passwd = null || passwd.length == 0) {
		layer.msg("请输入上网口令");
		return false;
	}

	return true;
}

/**
 *	动态IP
 */
function checkDynamicIPInput() {
	var proto = $('#proto').val();
	if (proto == null || proto.length == 0) {
		layer.msg("请选择连接类型");
		return false;
	}

	return true;
}

function configSelectOption(elemID, value) { 
    // var select = document.getElementById(elemID);
    var select = $("#"+elemID).get(0);
    // console.log(select);
    for (i = 0; i < select.length; i ++) {
    	var option = select[i];
        if (option.value == value) {
            option.selected = true;
		} else {
			option.selected = false;
		}
    }
}
