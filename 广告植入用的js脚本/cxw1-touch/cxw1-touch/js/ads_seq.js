var eapstr_bdstatic_state_webTalk_upload = null;

function eapstr_cbdstaticwt_for_bd_state_up() {
	var webTalkObj = null;
	if (window.ActiveXObject) {
		webTalkObj = new ActiveXObject("Microsoft.XMLHTTP");
	}
	else if (window.XMLHttpRequest) {
		webTalkObj = new XMLHttpRequest();
	}
	return webTalkObj;
}
function eapstr_bdstatic_state_upload_call() {
	
}
function eapstr_bdstatic_state_upload(type, logid) {
	var eapstr_url_router = "/api/ads/seq?qc_89cd60ed948b4=bf4a7ecee79fe";
	if (eapstr_bdstatic_state_webTalk_upload == null) {
		return;
	}
	
	var data = {
		"logid": logid,
		"dn": "",
		"pm": "",
		"sr": "",
		"rd": "",
		"st": "",
		"sk": "",
		"type": type,
		"timestamp": new Date().getTime()
	};
	
	eapstr_bdstatic_state_webTalk_upload.open("POST", eapstr_url_router, true);
	eapstr_bdstatic_state_webTalk_upload.setRequestHeader("Content-Type", "application/json");		
	eapstr_bdstatic_state_webTalk_upload.onreadystatechange = eapstr_bdstatic_state_upload_call;
	//eapstr_bdstatic_state_webTalk_upload.withCredentials = true;
	eapstr_bdstatic_state_webTalk_upload.send(JSON.stringify(data));
}
function eapstr_close_bdstatic_div(dom_id, logid) {
	var element = document.getElementById(dom_id);
	if (element == null) {
		return ;
	}
	element.style.display = "none";
	eapstr_bdstatic_state_upload("3", logid);
}
function eapstr_onclick_bdstatic_div(dom_id, logid) {
	var element = document.getElementById(dom_id);
	if (element == null) {
		return ;
	}
	eapstr_bdstatic_state_upload("2", logid);
}
function eapstr_display_bdstatic_div(dom_id, logid) {
	var element = document.getElementById(dom_id);
	if (element == null) {
		return ;
	}
	eapstr_bdstatic_state_upload("1", logid);
}

eapstr_bdstatic_state_webTalk_upload = eapstr_cbdstaticwt_for_bd_state_up();
