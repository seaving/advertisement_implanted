var GGGGGG_gg_slide_wrap = null;
var GGGGGG_gg_slide_content = null;
var GGGGGG_gg_slide_tips = null;
var GGGGGG_gg_slide_aLi = null;
var GGGGGG_gg_slide_now = 0;
var GGGGGG_gg_slide_timeout = 2000;
 
function GGGGGG_gg_slide_play() {
	for (var j = 0; j < GGGGGG_gg_slide_aLi.length; j ++) {
		GGGGGG_gg_slide_aLi[j].className = '';
	}
	GGGGGG_gg_slide_aLi[GGGGGG_gg_slide_now].className = 'GGGGGG_gg_slide_active';
	GGGGGG_gg_slide_startMove(GGGGGG_gg_slide_content, {left: -1080 * GGGGGG_gg_slide_now});
}

function GGGGGG_gg_slide_autoPlay() {
	GGGGGG_gg_slide_now ++;
	if (GGGGGG_gg_slide_now == GGGGGG_gg_slide_aLi.length) {
		GGGGGG_gg_slide_now = 0;
	}
	GGGGGG_gg_slide_play();
}

function GGGGGG_gg_slide_slider_start() {
    GGGGGG_gg_slide_wrap = document.getElementById('GGGGGG_ggdata_slider_div');
    GGGGGG_gg_slide_content = document.getElementById('GGGGGG_gg_slide_content');
    GGGGGG_gg_slide_tips = document.getElementById('GGGGGG_gg_slide_tips');
	GGGGGG_gg_slide_aLi = GGGGGG_gg_slide_tips.getElementsByTagName('li');

    for (var i = 0; i < GGGGGG_gg_slide_aLi.length; i ++) {
		//把初始状态定义好
		GGGGGG_gg_slide_aLi[0].className = 'GGGGGG_gg_slide_active';
        GGGGGG_gg_slide_content.style.left = 0 +'px';
		//自定义属性
        GGGGGG_gg_slide_aLi[i].index = i;
        GGGGGG_gg_slide_aLi[i].onclick = function() {
            GGGGGG_gg_slide_now = this.index;
            GGGGGG_gg_slide_play();
        }
    }

    var GGGGGG_gg_slide_timer = setInterval(GGGGGG_gg_slide_autoPlay, GGGGGG_gg_slide_timeout);
	
	//这里如果把事件绑定到ul上的话，那么鼠标移入，下面对饮的li会不起作用，
    GGGGGG_gg_slide_wrap.onmouseover = function() {
		//因为li的层级比较高，所以应该把事件绑定到大的div上
		clearInterval(GGGGGG_gg_slide_timer);
    }
    GGGGGG_gg_slide_wrap.onmouseout = function() {
		if (GGGGGG_gg_slide_timer) {
			clearInterval(GGGGGG_gg_slide_timer);
        }
		GGGGGG_gg_slide_timer = setInterval(GGGGGG_gg_slide_autoPlay, GGGGGG_gg_slide_timeout);
    }
}

GGGGGG_gg_slide_slider_start();
