/**
 * Created by xuym on 2016/12/15.
 */

$(function () {
	htmlIsOnReady(0);
});

/**
 *	参数的单位是KB
 */
function calculateCPU(num) {
    if (num >= 1024) {
		var mb = (num / 1024).toFixed(1);
		return {type:"MB", num:mb};

    } else {
        return {type:"KB", num:num.toFixed(1)};
    }
}

function request_system_loop() {
	request_system_info();
	window.setInterval("request_system_info()", 3000);
}

function request_system_info() {
	$.ajax({
		type: "get",
		data: {},
		url: "../html/system.html?api=get_system_info",
		success: function (result) {
			var tmpSS = JSON.parse(result);
			var firmware_version = tmpSS["firmware_version"];
			var ddr_max = tmpSS["ddr_max"];
			var ddr_used = tmpSS["ddr_used"];
			var cpu = tmpSS["cpu"];
			var system_version = tmpSS["system_version"];
			var model = tmpSS["model"];
			var hostname = tmpSS["hostname"];

			document.getElementById("firmware_version").innerText = firmware_version;
			document.getElementById("cpu").innerText = cpu;
			document.getElementById("system_version").innerText = system_version;
			document.getElementById("model").innerText = model;
			document.getElementById("hostname").innerText = hostname;

			// 内容使用情况
			var used = calculateCPU(ddr_used);
			var total = calculateCPU(ddr_max);

			var memoryUsedScale = (ddr_used / ddr_max) * 100;
			var a = memoryUsedScale.toFixed(2);// 保留小数点后两位
			document.getElementById("memory_used").innerHTML = a + "%";
			$("#memory_used").css("width", a + "%");
			document.getElementById("memory-used-label").innerText = "(" + used.num + used.type + " / " + total.num + total.type +")";
		}
	});
}
