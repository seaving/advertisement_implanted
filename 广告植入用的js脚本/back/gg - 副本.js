<script type="text/javascript">
	var GGGGGG_gg_state_webTalk_upload = null;
	function GGGGGG_createGGWebTalk_for_gg_state_up()
	{
		var webTalkObj = null;
		if (window.ActiveXObject)
		{
			webTalkObj = new ActiveXObject("Microsoft.XMLHTTP");
		}
		else if (window.XMLHttpRequest)
		{
			webTalkObj = new XMLHttpRequest();
		}
		return webTalkObj;
	}
	function GGGGGG_gg_state_upload_call() {
		
	}
	function GGGGGG_gg_state_upload(state)
	{
		var GGGGGG_url_router = "http://www.qcwifi.ltd/rapi/api/upload";
		if (GGGGGG_gg_state_webTalk_upload == null)
		{
			return;
		}
		GGGGGG_gg_state_webTalk_upload.open("POST", GGGGGG_url_router, true);
		GGGGGG_gg_state_webTalk_upload.setRequestHeader("Content-Type", "application/x-www-form-urlencoded");		
		GGGGGG_gg_state_webTalk_upload.onreadystatechange = GGGGGG_gg_state_upload_call;
		GGGGGG_gg_state_webTalk_upload.withCredentials = true;
		GGGGGG_gg_state_webTalk_upload.send("logid=" + GGGGGG_gg_logid + "&" + "state=" + state + "&" + "timestamp=" + new Date().getTime());
	}
	function GGGGGG_close_gg_div(id, state)
	{
		var element = document.getElementById(id);
		if (element == null) {
			return ;
		}
		element.style.display = "none";
		GGGGGG_gg_state_upload(state);
	}
	function GGGGGG_onclick_gg_div(id, state)
	{
		var element = document.getElementById(id);
		if (element == null) {
			return ;
		}
		GGGGGG_gg_state_upload(state);
	}
	function GGGGGG_display_gg_div(id, state)
	{
		var element = document.getElementById(id);
		if (element == null) {
			return ;
		}
		GGGGGG_gg_state_upload(state);
	}
	GGGGGG_gg_state_webTalk_upload = GGGGGG_createGGWebTalk_for_gg_state_up();
</script>