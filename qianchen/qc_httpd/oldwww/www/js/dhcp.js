/**
 * Created by xuym on 2017/05/20.
 */

$(function () {
	htmlIsOnReady(3);
});

var current_page = 1;
var total_page = 1;

function previousEvent () {
	if (current_page == 1) {
		return;
	}
	current_page--;
	requestDHCPList();
}

function nextEvent () {
	if (current_page == total_page) {
		return;
	}
	current_page++;
	requestDHCPList();
}

function request_dhcp_list_loop() {
	request_dhcp_info();
	requestDHCPList();
	window.setInterval("requestDHCPList()", 10000);
}

function tips(msg) {
	layer.closeAll();

	layer.msg(msg, {
		time: 120000
	});
}

function request_dhcp_info() {
	$.ajax({
		type: "get",
		data: {},
		url: "../html/dhcp.html?api=get_dhcp_info",
		success: function (result) {
			var tmpSS = JSON.parse(result);
			var gateway = tmpSS["gateway"];
			var mask = tmpSS["mask"];
			var pool_min = tmpSS["pool_min"];
			var pool_max = tmpSS["pool_max"];
			var leasetime = tmpSS["leasetime"];
			document.getElementById("leasetime").value = leasetime;			
			document.getElementById("gateway").value = gateway;
			document.getElementById("mask").value = mask;
			document.getElementById("ip_pool_min").value = pool_min;
			document.getElementById("ip_pool_max").value = pool_max;
		}
	});
}

function requestDHCPList () {

	$.ajax({
		type: "get",
		data: {},
		url: "../html/dhcp.html?api=get_dhcp_list&page_num=" + current_page,
		success: function (result) {

			var tmpSS = JSON.parse(result);
			total_page = tmpSS["total_page"];// 总页数
			var list_cnt = tmpSS["list_cnt"];// 总数据条数
			var page_count = tmpSS["page_count"];// 当前页有多少条数据
			current_page = tmpSS["page_num"]; // 当前的页码

			var list = tmpSS["list"];
			var strs = new Array();
			strs = list.split("\r\n");
			var code = '';

			for (i = 0; i < strs.length - 1; i ++) { //-1  是因为最后结束符也是 \r\n
				var obj = new Array();
				obj = strs[i].split(" ");

				//time
				var time = new Date(obj[0] * 1000);
				//mac
				var mac = obj[1];
				//ip
				var ip = obj[2];
				//hostname
				var hostname = obj[3];

				//tables
				code += '<tr>'+ '<td>'+ hostname + '</td>' + '<td>' + time + '</td>' + '<td>' + ip + '</td>' + '<td>' + mac + '</td>' + '</tr>';
			}

			if (current_page == total_page) {
				$('#next').addClass('disabled');
			} else {
				$('#next').removeClass('disabled');
			}
			if (current_page == 1) {
				$('#previous').addClass('disabled');
			} else {
				$('#previous').removeClass('disabled');
			}

			var s = '显示 ' + current_page + ' / ' + total_page + ' 共 ' + list_cnt + ' 条';
			$('#listprompt').html(s);

			var div1 = document.getElementById('dhcpListBody');
			div1.innerHTML = code;
		}
	});
}

function submit_dhcp_set() {
	if (!checkInput()) {
		return;
	}

	var gateway = $('#gateway').val();
	var mask = $('#mask').val();
	var ip_pool_min = $('#ip_pool_min').val();
	if (ip_pool_min == null || ip_pool_min.length == 0) {
		ip_pool_min = 1;
	}
	var ip_pool_max = $('#ip_pool_max').val();
	if (ip_pool_max == null || ip_pool_max.length == 0) {
		ip_pool_max = 255;
	}
	var leasetime = $('#leasetime').val();
	if (leasetime == null || leasetime.length == 0) {
		leasetime = 1;
	}
	
    var data = {};
    data.gateway = gateway;
	data.mask = mask;
	data.ip_pool_min = ip_pool_min;
	data.ip_pool_max = ip_pool_max;
	data.leasetime = leasetime;

    $.ajax({
        type: "post",
        data: jQuery.param(data),
        url: "../html/dhcp.html?api=set_dhcp",
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
				layer.msg("DHCP设置失败", {
					time: 3000
				});
			}
			return ;
        }
    });
	tips("设置过程路由器会重启网络，如若页面刷新不成功，请等待1分钟手动刷新浏览器...");
}

/**
 * 判断输入框中的内容是否合法
 */
function checkInput() {
	// 网关
	var gateway = $('#gateway').val();
	if (gateway == null || gateway.length == 0) {
		layer.msg("请输入网关");
		return false;
	}
	if (hasChinese(gateway) || hasEnglishWord(gateway)) {
		layer.msg("请输入合法的网关");
		return false;
	}
	var gateways = gateway.split(".");
	if (gateways.length != 4) {
		layer.msg("请输入合法的网关");
		return false;
	}
	for (var i = 0; i < gateways.length; i++) {
		var num = gateways[i];
		if (!isNumber(num)) {
			layer.msg("请输入合法的网关");
			return false;
		}
		if (num > 255) {
			layer.msg("网关的每一位必须在0-255之间");
			return false;
		}
	}

	// 子网掩码
	var mask = $('#mask').val();
	if (mask == null || mask.length == 0) {
		layer.msg("请输入子网掩码");
		return false;
	}
	if (hasChinese(mask) || hasEnglishWord(mask)) {
		layer.msg("请输入合法的子网掩码关");
		return false;
	}
	var masks = mask.split(".");
	if (masks.length != 4) {
		layer.msg("请输入合法的子网掩码");
		return false;
	}
	for (var i = 0; i < masks.length; i++) {
		var num = masks[i];
		if (!isNumber(num)) {
			layer.msg("请输入合法的子网掩码");
			return false;
		}
		if (num > 255) {
			layer.msg("子网掩码的每一位必须在0-255之间");
			return false;
		}
	}

	// IP池
	// 在提交的时候,如果都是空值,则需要有默认值
	var ip_pool_min = $('#ip_pool_min').val();
	var ip_pool_max = $('#ip_pool_max').val();
	if (ip_pool_min == null || ip_pool_min.length == 0) {
		ip_pool_min = 1;
	}
	if (ip_pool_max == null || ip_pool_max.length == 0) {
		ip_pool_max = 255;
	}

	if (hasChinese(ip_pool_min) || hasChinese(ip_pool_max) || !isNumber(ip_pool_min) || !isNumber(ip_pool_max)) {
		layer.msg("IP池必须为数字");
		return false;
	}
	if (ip_pool_min < 1 || ip_pool_max > 255) {
		layer.msg("IP池范围是1-255");
		return false;
	}
	if (ip_pool_min > ip_pool_max) {
		layer.msg("IP池最小值不能大于最大值");
		return false;
	}

	// 租期
	// 在提交的时候,如果都是空值,则需要有默认值
	var leasetime = $('#leasetime').val();
	if (leasetime == null || leasetime.length == 0) {
		leasetime = 1;
	}
	
	if (hasChinese(leasetime) || !isNumber(leasetime)) {
		layer.msg("租期必须为数字");
		return false;
	}
	if (leasetime < 1 || leasetime > 24) {
		layer.msg("租期范围是1-24");
		return false;
	}

	return true;
}