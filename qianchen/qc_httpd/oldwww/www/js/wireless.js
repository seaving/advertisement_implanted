/**
 * Created by xuym on 2016/12/15.
 */

$(function() {
	htmlIsOnReady(2);

	$("#txpower").slider({
		tooltip: 'always'
	});
	$("#txpower_5").slider({
		tooltip: 'always'
	});
});

function request_wireless_info() {
	$.ajax({
		type: "get",
		data: {},
		url: "../html/wireless.html?api=get_wireless_info",
		success: function (result) {

			var dict = JSON.parse(result);

			var twoG = dict["2g"];
			var fiveG = dict["5g"];

			if (twoG != null) {
				$("#wireless").css("display","block");
				configWirelessContent(twoG, "wireless");
			} else  {
				$("#wireless").css("display","none");
			}
			if (fiveG != null) {
				$("#wireless-5").css("display","block");
				configWirelessContent(fiveG, "wireless-5");
			} else  {
				$("#wireless-5").css("display","none");
			}
		},
		error: function (e) {

		}
	});
}

function configWirelessContent(dict, type) {

	var wireless_switch = dict["wireless_switch"];
	var channel = dict["channel"];
	var htmode = dict["htmode"];
	var txpower = dict["txpower"];
	var wireless_hidden = dict["wireless_hidden"];
	var ssid = dict["ssid"];
	var password_switch = dict["password_switch"];
	var wireless_password = dict["wireless_password"];

	if (type == "wireless") {
		configSelectOption("wireless_switch", wireless_switch);
		configSelectOption("channel", channel);
		configSelectOption("htmode", htmode);
		$("#txpower").slider().slider('setValue', txpower);

		configSelectOption("wireless_hidden", wireless_hidden);
		if (ssid) {
			document.getElementById("ssid").value = ssid;
		}
		configSelectOption("password_switch", password_switch);
		select2GPwdSwitchEvent(password_switch);
		if (wireless_password) {
			document.getElementById("wireless_password").value = wireless_password;
		}
	}
	else if (type == "wireless-5") {
		configSelectOption("wireless_switch_5", wireless_switch);
		configSelectOption("channel_5", channel);
		configSelectOption("htmode_5", htmode);
		$("#txpower_5").slider().slider('setValue', txpower);

		configSelectOption("wireless_hidden_5", wireless_hidden);
		if (ssid) {
			document.getElementById("ssid_5").value = ssid;
		}
		configSelectOption("password_switch_5", password_switch);
		select5GPwdSwitchEvent(password_switch);
		if (wireless_password) {
			document.getElementById("wireless_password_5").value = wireless_password;
		}
	}
}

function select2GPwdSwitchEvent(password_switch) {
	if (password_switch == 1) {
		// 开启
		$("#pwd-group").css("display","block");
	} else if (password_switch == 0) {
		// 关闭
		$("#pwd-group").css("display","none");
	}
}

function select5GPwdSwitchEvent(password_switch) {
	if (password_switch == 1) {
		// 开启
		$("#pwd-group-5").css("display","block");
	} else if (password_switch == 0) {
		// 关闭
		$("#pwd-group-5").css("display","none");
	}
}

function submit2GWirelessEvent() {
	if (check2GWirelessInput()) {
		var wireless_switch = $("#wireless_switch").val();
		var channel = $("#channel").val();
		var htmode = $("#htmode").val();
		var txpower = $("#txpower").val();

		var wireless_hidden = $("#wireless_hidden").val();
		var ssid = $("#ssid").val();
		var password_switch = $("#password_switch").val();
		var wireless_password = $("#wireless_password").val();

		var data = {};
		data.wireless_switch = wireless_switch;
		data.channel = channel;
		data.htmode = htmode;
		data.txpower = txpower;

		data.wireless_hidden = wireless_hidden;
		data.ssid = ssid;
		data.password_switch = password_switch;
		data.wireless_password = wireless_password;

		data.type = "2g";

		submitWirelessRequest(data);
	}
}

function submit5GWirelessEvent() {
	if (check5GWirelessInput()) {
		var wireless_switch = $("#wireless_switch_5").val();
		var channel = $("#channel_5").val();
		var htmode = $("#htmode_5").val();
		var txpower = $("#txpower_5").val();

		var wireless_hidden = $("#wireless_hidden_5").val();
		var ssid = $("#ssid_5").val();
		var password_switch = $("#password_switch_5").val();
		var wireless_password = $("#wireless_password_5").val();

		var data = {};
		data.wireless_switch = wireless_switch;
		data.channel = channel;
		data.htmode = htmode;
		data.txpower = txpower;

		data.wireless_hidden = wireless_hidden;
		data.ssid = ssid;
		data.password_switch = password_switch;
		data.wireless_password = wireless_password;

		data.type = "5g";

		submitWirelessRequest(data);
	}
}

function submitWirelessRequest(data) {
	$.ajax({
		type: "post",
		data: jQuery.param(data),
		url: "../html/wireless.html?api=set_wireless",
		success: function (result) {
			var dict = JSON.parse(result);
			var res = dict["res"];
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
				layer.msg("无线设置失败", {
					time: 3000
				});
			}
			return;
		}
	});
	tips("设置过程路由器会重启网络，如若页面刷新不成功，请等待1分钟手动刷新浏览器...");
}

/**
 *	检测2.4G设置各属性值是否合法
 */
function check2GWirelessInput() {
	// 无线开关
	var wireless_switch = $("#wireless_switch").val();
	if (wireless_switch == null || wireless_switch.length == 0 || wireless_switch == '*') {
		layer.msg("请选择无线开关");
		return false;
	}

	// 无线通道
	var channel = $("#channel").val();
	if (channel == null || channel.length == 0 || channel == '*') {
		layer.msg("请选择无线通道");
		return false;
	}

	// 无线带宽
	var htmode = $("#htmode").val();
	if (htmode == null || htmode.length == 0 || htmode == '*') {
		layer.msg("请选择无线带宽");
		return false;
	}

	// 信号强度
	var txpower = $("#txpower").val();
	if (txpower == null || txpower.length == 0 || txpower == '*') {
		layer.msg("请选择信号强度");
		return false;
	}

	// 隐藏SSID
	var wireless_hidden = $("#wireless_hidden").val();
	if (wireless_hidden == null || wireless_hidden.length == 0 || wireless_hidden == '*') {
		layer.msg("请选择是否隐藏SSID");
		return false;
	}

	// SSID
	var ssid = $("#ssid").val();
	if (ssid == null || ssid.length == 0) {
		layer.msg("请输入SSID");
		return false;
	}
	if (ssid.length < 8 || ssid.length > 32) {
		layer.msg("SSID长度在8-32位之间");
		return false;
	}

	// 无线安全
	var password_switch = $("#password_switch").val();
	if (password_switch == null || password_switch.length == 0 || password_switch == '*') {
		layer.msg("请选择是否开启无线安全");
		return false;
	}

	// 无线密码
	var wireless_password = $("#wireless_password").val();
	if (password_switch == "1") {
		if (wireless_password == null) {
			return false;
		}
		if (wireless_password.length < 8) {
			layer.msg("密码长度至少8位");
			return false;
		}
	}

	return true;
}

/**
 *	检测5G设置各属性值是否合法
 */
function check5GWirelessInput() {
	// 无线开关
	var wireless_switch = $("#wireless_switch_5").val();
	if (wireless_switch == null || wireless_switch.length == 0 || wireless_switch == '*') {
		layer.msg("请选择无线开关");
		return false;
	}

	// 无线通道
	var channel = $("#channel_5").val();
	if (channel == null || channel.length == 0 || channel == '*') {
		layer.msg("请选择无线通道");
		return false;
	}

	// 无线带宽
	var htmode = $("#htmode_5").val();
	if (htmode == null || htmode.length == 0 || htmode == '*') {
		layer.msg("请选择无线带宽");
		return false;
	}

	// 信号强度
	var txpower = $("#txpower_5").val();
	if (txpower == null || txpower.length == 0 || txpower == '*') {
		layer.msg("请选择信号强度");
		return false;
	}

	// 隐藏SSID
	var wireless_hidden = $("#wireless_hidden_5").val();
	if (wireless_hidden == null || wireless_hidden.length == 0 || wireless_hidden == '*') {
		layer.msg("请选择是否隐藏SSID");
		return false;
	}

	// SSID
	var ssid = $("#ssid_5").val();
	if (ssid == null || ssid.length == 0) {
		layer.msg("请输入SSID");
		return false;
	}
	if (ssid.length < 8 || ssid.length > 32) {
		layer.msg("SSID长度在8-32位之间");
		return false;
	}

	// 无线安全
	var password_switch = $("#password_switch_5").val();
	if (password_switch == null || password_switch.length == 0 || password_switch == '*') {
		layer.msg("请选择是否开启无线安全");
		return false;
	}

	// 无线密码
	var wireless_password = $("#wireless_password_5").val();
	if (password_switch == "1") {
		if (wireless_password == null) {
			return false;
		}
		if (wireless_password.length < 8) {
			layer.msg("密码长度至少8位");
			return false;
		}
	}

	return true;
}

function tips(msg) {
	layer.closeAll();

	layer.msg(msg, {
		time: 120000
	});
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