function action(action) {
	var element = document.getElementById("msg_count");
	if (element == null) {
		return;
	}
	
	if (action == 0) {
		element.style.display  = "none";
	}
	else if (action == 1) {
		element.style.display = ""
	}
	else {
		if (element.style.display == "none") {
			element.style.display = "";
		}
		else {
			element.style.display = "none";
		}
	}
}

function message() {
	
	//闪烁用到的定时器
	var flash_interval = null;
		
	this.flash = function () {
		action(2);
	}
	
	this.start = function () {
		if (flash_interval != null) {
			window.clearTimeout(flash_interval);
		}
		flash_interval = window.setInterval(this.flash, 500);
	}
	
	this.stop = function () {
		if (flash_interval != null) {
			window.clearTimeout(flash_interval);
		}
	}
}

var message = new message();
message.start();
