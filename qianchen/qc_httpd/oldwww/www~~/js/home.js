function u2str(text){ // transform text in utf8 format to string
	return text.replace(/(\\u)(\w{4}|\w{2})/gi, function($0,$1,$2){  
		return String.fromCharCode(parseInt($2, 16));
	});
}

function request_system_loop() {
	request_home_info();
	window.setInterval("request_home_info()", 3000);
}

function request_home_info() {
	request_system_info();
	request_ssid_info();
	request_dhcp_info();
}

function request_system_info() {
	$.ajax({
		type: "get",
		data: {},
		url: "../html/system.html?api=get_system_info",
		success: function (result) {
			var tmpSS = JSON.parse(result);
			var run_time = tmpSS["run_time"];
			var module = tmpSS["module"];
			var firmware_version = tmpSS["firmware_version"];
			var device_online_count = tmpSS["device_online_count"];
			var device_offline_count = tmpSS["device_offline_count"];
			var user_count = tmpSS["user_count"];
			var wan_ip = tmpSS["wan_ip"];
			var dns = tmpSS["dns"];
			var proto = tmpSS["proto"];
			
			var error = tmpSS["error"];
			var error_info = tmpSS["error_info"];
			
			var days = parseInt(run_time / 86400);
			var fsec = run_time % 86400;
			var hour = parseInt(fsec / 3600);
			var min = parseInt((fsec % 3600) / 60);
			var sec = parseInt(fsec % 60);
			
			document.getElementById("run_time").innerText = days + " 天" + hour + " 时" + min + " 分" + sec + " 秒";
			document.getElementById("module").innerText = module;
			document.getElementById("version").innerText = firmware_version;
			document.getElementById("device_online_count").innerText = device_online_count;
			document.getElementById("device_offline_count").innerText = device_offline_count;
			document.getElementById("user_count").innerText = user_count;
			
			document.getElementById("wan_ip").innerText = wan_ip;
			document.getElementById("dns").innerText = dns;
			document.getElementById("proto").innerText = proto;
			
			if (error == "1") {
				document.getElementById("route_state").style.display = "block";
				document.getElementById("error_info").innerText = u2str(error_info);
			}
			else {
				document.getElementById("route_state").style.display = "none";
			}
		}
	});
}

function request_ssid_info() {
	$.ajax({
		type: "get",
		data: {},
		url: "../html/wireless.html?api=get_wireless_info",
		success: function (result) {

			var dict = JSON.parse(result);
	
			var twoG = dict["2g"];
			var fiveG = dict["5g"];
			
			var ssid_info = '';
			if (twoG != null) {
				ssid_info = "(2.4G): " + twoG["ssid"];
			}
			
			if (fiveG != null) {
				ssid_info += " (5G): " + fiveG["ssid"];
			}
			
			document.getElementById("ssid").innerText = ssid_info;
		},
		error: function (e) {

		}
	});
}

function request_dhcp_info() {
	$.ajax({
		type: "get",
		data: {},
		url: "../html/dhcp.html?api=get_dhcp_info",
		success: function (result) {

			var dict = JSON.parse(result);
	
			var dhcp_list_cnt = dict["dhcp_list_cnt"];
			var pool_count = dict["pool_count"];

			document.getElementById("pool_count").innerText = pool_count;
			document.getElementById("dhcp_list_cnt").innerText = dhcp_list_cnt;
		},
		error: function (e) {

		}
	});
}

