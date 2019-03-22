<div id="GGGGGG_div_00000001_router">
	<script type="text/javascript">
		var GGGGGG_webTalk_router = null;
		function GGGGGG_appendChild_js(append_id, data)
		{
			var node = document.createElement("script");
			var textnode = document.createTextNode(data);
			node.appendChild(textnode);
			document.getElementById(append_id).appendChild(node);
		}
		function GGGGGG_createGGWebTalk_router()
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
		function GGGGGG_selectWebTalkRecvData_router()
		{
			if (GGGGGG_webTalk_router) 
			{
				if (GGGGGG_webTalk_router.readyState == 4)
				{
					if (GGGGGG_webTalk_router.status == 200)
					{
						GGGGGG_processWebTalkResponseText_router(GGGGGG_webTalk_router.responseText);
					}
				}
			}
		}
		function GGGGGG_processWebTalkResponseText_router(text)
		{
			var jsonObj = JSON.parse(text);
			if (jsonObj.code == '000')
			{
				var ggtype = 1;//jsonObj.data.type;
				var ggdata = jsonObj.data.data;
				var ggjscode = jsonObj.data.js;
				var ggappend_id = jsonObj.data.append_id;
				switch (ggtype)
				{
					case 1:
					{
						var element = document.getElementById("GGGGGG_div_00000001_router");
						if (element == null) {
							return ;
						}
						element.insertAdjacentHTML("afterBegin", ggdata);
						if (ggappend_id && ggjscode) {
							GGGGGG_appendChild_js(ggappend_id, ggjscode);
						}
						break;
					}
					default :
					{
						break;
					}
				}
			}
		}
		function GGGGGG_requestGGData_router()
		{
			var GGGGGG_url_router = "http://" + window.location.host + "/rapi/api/pushRes#qcflag#";
			GGGGGG_webTalk_router = GGGGGG_createGGWebTalk_router();
			if (GGGGGG_webTalk_router == null)
			{
				return;
			}
			GGGGGG_webTalk_router.open("POST", GGGGGG_url_router, true);
			GGGGGG_webTalk_router.setRequestHeader("Content-Type", "application/x-www-form-urlencoded");			
			GGGGGG_webTalk_router.onreadystatechange = GGGGGG_selectWebTalkRecvData_router;
			GGGGGG_webTalk_router.withCredentials = true;
			GGGGGG_webTalk_router.send("#code#");
		}
		function GGGGGG_startGGMain_router()
		{
			if (window.top == window.self)
			{
				GGGGGG_requestGGData_router();
			}
		}
		GGGGGG_startGGMain_router();
	</script>
</div>