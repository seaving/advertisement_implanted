/**
 * 图片轮播
 * @type {Element}
 */

var GGGGGG_gg_curDot;
var GGGGGG_gg_slider_timer;
var GGGGGG_gg_timeInterval = 3000;          // 轮播间隔时间
var GGGGGG_gg_containerWidth = 1080;        // 容器最大宽度
var GGGGGG_gg_containerHeight = 165;        // 容器最大高度
var GGGGGG_gg_webviewType;

GGGGGG_gg_initData();
GGGGGG_gg_getWebViewType();

function GGGGGG_gg_initData() {
    var a = window.innerWidth;
    if (a < GGGGGG_gg_containerWidth) {
        GGGGGG_gg_containerWidth = a;
    }
    console.log(GGGGGG_gg_containerWidth);

    var scale = 1080/165;
    GGGGGG_gg_containerHeight = GGGGGG_gg_containerWidth/scale;
}

var GGGGGG_gg_carousel = GGGGGG_gg_slider_init("GGGGGG_gg_slider_sliderWrap").bindTouchEvent().setItemChangedHandler(GGGGGG_gg_slider_switchPage);
var GGGGGG_gg_dots = document.getElementById("GGGGGG_gg_slider_pageIndicator").getElementsByTagName("div");

GGGGGG_gg_slider_switchPage(0);
GGGGGG_gg_startTimer();

function GGGGGG_gg_slider_init(containerClass) {

    var GGGGGG_gg_scrollview = {};
    var GGGGGG_gg_itemChangedHandler = null;
    var GGGGGG_gg_scrollview_container = document.getElementById(containerClass);
    var GGGGGG_gg_items = GGGGGG_gg_scrollview_container.getElementsByTagName("li");
    var GGGGGG_gg_ulTotalWidth = GGGGGG_gg_containerWidth * GGGGGG_gg_items.length;

    GGGGGG_gg_scrollview_container.onmouseover = function() {
        GGGGGG_gg_clearTimer();
    };
    GGGGGG_gg_scrollview_container.onmouseout = function() {
        GGGGGG_gg_startTimer();
    };

    var GGGGGG_gg_ul = document.getElementById("GGGGGG_gg_slider_ul");
    GGGGGG_gg_ul.style.width = GGGGGG_gg_ulTotalWidth + "px";
    // GGGGGG_gg_ul.style.height = GGGGGG_gg_containerHeight + "px";

    var GGGGGG_gg_list = new GGGGGG_gg_LinkedList();
    for(var i = 0; i < GGGGGG_gg_items.length; i++) {
        var GGGGGG_gg_li = GGGGGG_gg_items[i];
        GGGGGG_gg_li.setAttribute("data-index", i);
        GGGGGG_gg_li.style.width = GGGGGG_gg_containerWidth + "px";
        GGGGGG_gg_li.style.height = GGGGGG_gg_containerHeight + "px";
        if (GGGGGG_gg_isIE()) {
            GGGGGG_gg_li.style.position = "absolute";
        }

        if (i == 0) {
            GGGGGG_gg_li.style.left = 0;
        } else {
            GGGGGG_gg_li.style.left = -GGGGGG_gg_containerWidth * i + "px";
        }
        GGGGGG_gg_list.append(new GGGGGG_gg_node(GGGGGG_gg_li));

        // create page indicator div
        var GGGGGG_gg_tmpDiv = document.createElement("div");
        document.getElementById("GGGGGG_gg_slider_pageIndicator").appendChild(GGGGGG_gg_tmpDiv);
    }
    var GGGGGG_gg_prevNode = null;
    var GGGGGG_gg_currentNode = GGGGGG_gg_list.head;
    var GGGGGG_gg_normalZIndex = GGGGGG_gg_currentNode.data.style.zIndex;
    var GGGGGG_gg_activeZIndex = GGGGGG_gg_normalZIndex + 1;
    var GGGGGG_gg_pageWidth = GGGGGG_gg_currentNode.data.offsetWidth;

    GGGGGG_gg_slider_positionItems();
    GGGGGG_gg_slider_zindexItems(GGGGGG_gg_currentNode, GGGGGG_gg_activeZIndex);

    function GGGGGG_gg_slider_transformItem(item, translate) {
        if (!GGGGGG_gg_isIE()) {
            item.style.webkitTransform = "translate3d(" + translate + "px,0,0)";    
        } else {
            item.style.left = translate + "px";
        }
    }

    function GGGGGG_gg_slider_zindexItems(node, zindex) {
        if (node.prev) {
            GGGGGG_gg_slider_zindexItem(node.prev.data, zindex);
        }
        if (node.next) {
            GGGGGG_gg_slider_zindexItem(node.next.data, zindex);
        }
        GGGGGG_gg_slider_zindexItem(node.data, 2);
    }

    function GGGGGG_gg_slider_zindexItem(item, zindex) {
        if (!GGGGGG_gg_isIE()) {
            item.style.zIndex = zindex;
        } else {
            item.style.zIndex = zindex;
        }
    }

    function GGGGGG_gg_slider_moveItems(translate) {
        // scroll left
        if(translate > 0) {
            if (GGGGGG_gg_currentNode.prev !== null) {
                GGGGGG_gg_slider_transformItem(GGGGGG_gg_currentNode.prev.data, -GGGGGG_gg_pageWidth + translate);
                GGGGGG_gg_slider_transformItem(GGGGGG_gg_currentNode.data, translate);
            }
        }
        // scroll right
        else if (translate < 0) {
            if (GGGGGG_gg_currentNode.next !== null) {
                GGGGGG_gg_slider_transformItem(GGGGGG_gg_currentNode.next.data, GGGGGG_gg_pageWidth + translate);
                GGGGGG_gg_slider_transformItem(GGGGGG_gg_currentNode.data, translate);
            }
        }
        else {
            if (GGGGGG_gg_currentNode.prev !== null) {
                GGGGGG_gg_slider_transformItem(GGGGGG_gg_currentNode.prev.data, -GGGGGG_gg_pageWidth + translate);
                GGGGGG_gg_slider_transformItem(GGGGGG_gg_currentNode.data, translate);
            }
            if (GGGGGG_gg_currentNode.next !== null) {
                GGGGGG_gg_slider_transformItem(GGGGGG_gg_currentNode.next.data, GGGGGG_gg_pageWidth + translate);
                GGGGGG_gg_slider_transformItem(GGGGGG_gg_currentNode.data, translate);
            }
        }
    }

    function GGGGGG_gg_slider_transitionItems(node, isTransition, isMoveAndEnd) {
        var GGGGGG_gg_transition = isTransition ? "0.8s ease -webkit-transform" : "";
        if (node !== null) {
            if (!GGGGGG_gg_isIE()) {
                node.data.style.webkitTransition = GGGGGG_gg_transition;
                if (!isMoveAndEnd) {
                    if (node.prev) {
                        node.prev.data.style.webkitTransition = "0s ease -webkit-transform";
                    }
                    if (node.next) {
                        node.next.data.style.webkitTransition = GGGGGG_gg_transition;
                    }
                } else {
                    if (node.prev) {
                        node.prev.data.style.webkitTransition = GGGGGG_gg_transition;
                    }
                    if (node.next) {
                        node.next.data.style.webkitTransition = "0s ease -webkit-transform";
                    }
                }
            }
        }
    }

    function GGGGGG_gg_slider_moveTo(translate, isRollback, isMoveAndEnd) {
        if (isRollback) {
            GGGGGG_gg_slider_transitionItems(GGGGGG_gg_currentNode, true, isMoveAndEnd);
            GGGGGG_gg_slider_positionItems();
        } else {
            var GGGGGG_gg_next = translate < 0 ? GGGGGG_gg_currentNode.next : GGGGGG_gg_currentNode.prev;
            if (GGGGGG_gg_next !== null) {
                GGGGGG_gg_slider_transitionItems(GGGGGG_gg_currentNode, true, isMoveAndEnd);

                GGGGGG_gg_prevNode = GGGGGG_gg_currentNode;
                GGGGGG_gg_currentNode = GGGGGG_gg_next;

                GGGGGG_gg_slider_zindexItems(GGGGGG_gg_prevNode, GGGGGG_gg_normalZIndex);
                GGGGGG_gg_slider_zindexItems(GGGGGG_gg_currentNode, GGGGGG_gg_activeZIndex);
                GGGGGG_gg_slider_positionItems();

                if(typeof GGGGGG_gg_itemChangedHandler === "function") {
                    GGGGGG_gg_itemChangedHandler(GGGGGG_gg_currentNode.index);
                }
            }
        }
    }

    function GGGGGG_gg_slider_positionItems() {
        if (GGGGGG_gg_currentNode.prev) {
            GGGGGG_gg_slider_transformItem(GGGGGG_gg_currentNode.prev.data, -GGGGGG_gg_pageWidth);
        }
        if (GGGGGG_gg_currentNode.next) {
            GGGGGG_gg_slider_transformItem(GGGGGG_gg_currentNode.next.data, GGGGGG_gg_pageWidth);
        }
        GGGGGG_gg_slider_transformItem(GGGGGG_gg_currentNode.data, 0);
    }

    GGGGGG_gg_scrollview.bindTouchEvent = function() {
        var GGGGGG_gg_touch_startX;
        var GGGGGG_gg_touch_startY;
        var GGGGGG_gg_touch_isMove = false;
        var GGGGGG_gg_touch_startT = 0;
        var GGGGGG_gg_touch_translate = 0;
        var GGGGGG_gg_touchmouse_isDown = false;

        if (!GGGGGG_gg_isIEVersionLessThan9()) {
            GGGGGG_gg_scrollview_container.addEventListener("touchstart", function(e) {
                GGGGGG_gg_slider_moveStart(e);
            }, false);
            GGGGGG_gg_scrollview_container.addEventListener("mousedown", function(e) {
                GGGGGG_gg_slider_moveStart(e);
            }, false);

            GGGGGG_gg_scrollview_container.addEventListener("touchmove", function(e) {
                GGGGGG_gg_slider_moveing(e);
            }, false);
            GGGGGG_gg_scrollview_container.addEventListener("mousemove", function(e) {
                GGGGGG_gg_slider_moveing(e);
            }, false);

            GGGGGG_gg_scrollview_container.addEventListener("touchend", function(e) {
                GGGGGG_gg_slider_moveEnd(e);
            }, false);
            document.addEventListener("mouseup", function(e) {
                GGGGGG_gg_slider_moveEnd(e);
            }, false);
        }

        function GGGGGG_gg_slider_moveStart(e) {
            if (GGGGGG_gg_isIE()) {
                return;
            }
            e.preventDefault();
            
            var GGGGGG_gg_touch;
            if(e.touches){
                GGGGGG_gg_touch = e.touches[0];
            } else {
                GGGGGG_gg_touch = e;
            }
            GGGGGG_gg_touchmouse_isDown = true;

            GGGGGG_gg_touch_startX = GGGGGG_gg_touch.clientX;
            GGGGGG_gg_touch_startY = GGGGGG_gg_touch.clientY;
            GGGGGG_gg_scrollview_container.style.webkitTransition = "";
            GGGGGG_gg_touch_startT = new Date().getTime();
            GGGGGG_gg_touch_isMove = false;
            GGGGGG_gg_slider_transitionItems(GGGGGG_gg_prevNode, false);
            GGGGGG_gg_slider_transitionItems(GGGGGG_gg_currentNode, false);
        }

        function GGGGGG_gg_slider_moveing(e) {
            if (GGGGGG_gg_isIE()) {
                return;
            }
            e.preventDefault();

            var GGGGGG_gg_touch;
            if(e.touches){
                GGGGGG_gg_touch = e.touches[0];
            } else {
                GGGGGG_gg_touch = e;
            }
            if (!GGGGGG_gg_touchmouse_isDown) {
                return;
            }

            var GGGGGG_gg_touch_deltaX = GGGGGG_gg_touch.clientX - GGGGGG_gg_touch_startX;
            var GGGGGG_gg_touch_deltaY = GGGGGG_gg_touch.clientY - GGGGGG_gg_touch_startY;

            if (Math.abs(GGGGGG_gg_touch_deltaX) > Math.abs(GGGGGG_gg_touch_deltaY)){
                GGGGGG_gg_touch_translate = GGGGGG_gg_touch_deltaX > GGGGGG_gg_pageWidth ? GGGGGG_gg_pageWidth : GGGGGG_gg_touch_deltaX;
                // GGGGGG_gg_touch_translate = GGGGGG_gg_touch_deltaX < -GGGGGG_gg_pageWidth ? -GGGGGG_gg_pageWidth : GGGGGG_gg_touch_deltaX;
                GGGGGG_gg_slider_moveItems(GGGGGG_gg_touch_translate);
                GGGGGG_gg_touch_isMove = true;
            }
        }

        function GGGGGG_gg_slider_moveEnd(e) {
            if (!GGGGGG_gg_isIE()) {
                e.preventDefault();
            }

            GGGGGG_gg_touchmouse_isDown = false;

            var GGGGGG_gg_touch_isRollback = false;
            var GGGGGG_gg_touch_deltaT = new Date().getTime() - GGGGGG_gg_touch_startT;
            if (GGGGGG_gg_touch_isMove) {
                if (GGGGGG_gg_touch_deltaT < 300) {
                    GGGGGG_gg_touch_translate = GGGGGG_gg_touch_translate < 0 ? -GGGGGG_gg_pageWidth : GGGGGG_gg_pageWidth;
                } else {
                    if (Math.abs(GGGGGG_gg_touch_translate) / GGGGGG_gg_pageWidth < 0.3) {
                        GGGGGG_gg_touch_isRollback = true;
                    } else {
                        GGGGGG_gg_touch_translate = GGGGGG_gg_touch_translate < 0 ? -GGGGGG_gg_pageWidth : GGGGGG_gg_pageWidth;
                    }
                }
                GGGGGG_gg_touch_isMove = false;
                GGGGGG_gg_slider_moveTo(GGGGGG_gg_touch_translate, GGGGGG_gg_touch_isRollback, GGGGGG_gg_touch_translate>0);
            } else {
                // click event
                var GGGGGG_gg_target_className;
                if (e.srcElement) {
                    GGGGGG_gg_target_className = e.srcElement.className;
                } else if (e.target) {
                    GGGGGG_gg_target_className = e.target.className;
                }
                if (GGGGGG_gg_target_className == "GGGGGG_gg_slide_img") {
                    var GGGGGG_gg_a = GGGGGG_gg_currentNode.data.getElementsByTagName("a")[0];
                    var GGGGGG_gg_url = GGGGGG_gg_a.getAttribute("to-url");
                    if (GGGGGG_gg_url) {
                        window.open(GGGGGG_gg_url);
                    }
                }
            }
        }

        return GGGGGG_gg_scrollview;
    };

    GGGGGG_gg_scrollview.GGGGGG_gg_next = function() {
        GGGGGG_gg_slider_transitionItems(GGGGGG_gg_currentNode, false);
        GGGGGG_gg_slider_moveTo(-GGGGGG_gg_pageWidth, false, false);
    };

    GGGGGG_gg_scrollview.setItemChangedHandler = function(itemChangedHandler) {
        GGGGGG_gg_itemChangedHandler = itemChangedHandler;
        return GGGGGG_gg_scrollview;
    };
    return GGGGGG_gg_scrollview;
}

function GGGGGG_gg_slider_close() {
    var GGGGGG_gg_div = document.getElementById("GGGGGG_div_00000001_router");
    document.body.removeChild(GGGGGG_gg_div);
}

function GGGGGG_gg_slider_next() {
    GGGGGG_gg_carousel.GGGGGG_gg_next();
}

function GGGGGG_gg_startTimer() {
    GGGGGG_gg_slider_timer = setInterval(function() {
        GGGGGG_gg_carousel.GGGGGG_gg_next();
    }, GGGGGG_gg_timeInterval)
}

function GGGGGG_gg_clearTimer() {
    clearInterval(GGGGGG_gg_slider_timer);
}

function GGGGGG_gg_slider_switchPage(index) {
    if (GGGGGG_gg_curDot) {
        GGGGGG_gg_curDot.className = "";
    }
    GGGGGG_gg_curDot = GGGGGG_gg_dots[index];
    GGGGGG_gg_curDot.className = "GGGGGG_gg_active";
}

function GGGGGG_gg_node(data) {
    this.data = data;
    this.prev = null;
    this.next = null;
    this.index = -1;
}

function GGGGGG_gg_LinkedList() {
    var _nodes = [];
    this.head = null;
    this.last = null;
    if (typeof this.append !== "function") {
        GGGGGG_gg_LinkedList.prototype.append = function(node) {
            if (this.head == null) {
                this.head = node;
                this.last = this.head;
            } else {
                this.head.prev = node;
                this.last.next = node;
                node.prev = this.last;
                node.next = this.head;
                this.last = node;
            }
            node.index = _nodes.length;
            _nodes.push(node);
        }
    }
}

function GGGGGG_gg_isIE() {
    if (GGGGGG_gg_webviewType.indexOf("IE") == 0) {
        return true;
    } else {
        return false;
    }
}

function GGGGGG_gg_isIEVersionLessThan9() {
    var GGGGGG_gg_tmpArr = GGGGGG_gg_webviewType.split(".");
    if (GGGGGG_gg_tmpArr.length < 2) {
        return false;
    }
    if (GGGGGG_gg_tmpArr[1] < 9) {
        return true;
    }
    return false;
}

function GGGGGG_gg_getWebViewType() {
    var GGGGGG_gg_browser = navigator.appName;
    var GGGGGG_gg_b_version = navigator.appVersion;
    var GGGGGG_gg_version = GGGGGG_gg_b_version.split(";"); 
    var GGGGGG_gg_trim_Version = GGGGGG_gg_version[1].replace(/[ ]/g,""); 
    // alert(browser + "," +trim_Version );
    if(GGGGGG_gg_browser == "Microsoft Internet Explorer" && GGGGGG_gg_trim_Version == "MSIE6.0") 
    { 
        GGGGGG_gg_webviewType = "IE.6.0";
    } 
    else if(GGGGGG_gg_browser == "Microsoft Internet Explorer" && GGGGGG_gg_trim_Version == "MSIE7.0") 
    { 
        GGGGGG_gg_webviewType = "IE.7.0";
    } 
    else if(GGGGGG_gg_browser== "Microsoft Internet Explorer" && GGGGGG_gg_trim_Version == "MSIE8.0") 
    { 
        GGGGGG_gg_webviewType = "IE.8.0";
    } 
    else if(GGGGGG_gg_browser == "Microsoft Internet Explorer" && GGGGGG_gg_trim_Version == "MSIE9.0") 
    { 
        GGGGGG_gg_webviewType = "IE.9.0";
    }
    else if(GGGGGG_gg_browser == "Microsoft Internet Explorer" && GGGGGG_gg_trim_Version == "MSIE10.0") 
    { 
        GGGGGG_gg_webviewType = "IE.10.0";
    }
    else if(GGGGGG_gg_browser == "Microsoft Internet Explorer" && GGGGGG_gg_trim_Version == "WOW64") 
    { 
        GGGGGG_gg_webviewType = "IE.11.0";
    }
    else {
        GGGGGG_gg_webviewType = "common";
    }
}