/**
 * Created by xuym on 2017/5/15.
 */


/**
 *	修改密码请求
 */
function resetPwdRequest() {
    if (!checkResetPwdInput()) {
        return;
    }

    var oldPwd = $("#oldPwd").val();
    var newPwd = $("#newPwd").val();
    var repeatNewPwd = $("#repeatNewPwd").val();

    var data = {};
    data.oldPwd = oldPwd;
    data.newPwd = newPwd;
    data.repeatNewPwd = repeatNewPwd;

    $.ajax({
        type: "POST",
        dataType: "html",
        url: "../html/resetpwd.html?api=set_pwd",
        data: jQuery.param(data),
        success: function (result) {
            var tmpSS = JSON.parse(result);
            var res = tmpSS["res"];
            if (res == 'success') {
                layer.msg("密码修改成功");
                window.location.href = "login.html";
            } else {
                layer.msg("密码修改失败");
            }
        }
    })
}

/**
 *	设置密码请求
 */
function setPwdRequest() {
    if (!checkSetPwdInput()) {
        return;
    }

    var newPwd = $("#newPwd").val();
    var repeatNewPwd = $("#repeatNewPwd").val();

    var data = {};
    data.newPwd = newPwd;
    data.repeatNewPwd = repeatNewPwd;

    $.ajax({
        type: "POST",
        dataType: "html",
        url: "../html/setpwd.html?api=modify_pwd",
        data: jQuery.param(data),
        success: function (result) {
            var tmpSS = JSON.parse(result);
            var res = tmpSS["res"];
            if (res == 'success') {
                layer.msg("设置密码成功");
                window.location.href = "login.html";
            } else {
                layer.msg("设置密码失败");
            }
        }
    })
}

/**
 *	检测登录时输入框内容是否合法
 */
function checkLoginInput() {
    var pwd = $("#password").val();
    if (pwd.length < 8 || pwd.length > 16) {
        layer.msg("密码在8-16位之间");
        return false;
    }
    else if (pwd.indexOf("&") > 0) {
        layer.msg("密码不能包含特殊符号");
        return false;
    }
    else {
        return true;
    }
}

/**
 *	检测修改密码时输入框内容是否合法
 */
function checkResetPwdInput() {
    var oldPwd = $("#oldPwd").val();
    var newPwd = $("#newPwd").val();
    var repeatNewPwd = $("#repeatNewPwd").val();
    if (oldPwd.length < 8 || oldPwd.length > 16 || newPwd.length < 8 || repeatNewPwd.length < 8
        || newPwd.length > 16 || repeatNewPwd.length > 16) {
        layer.msg("密码在8-16位之间");
        return false;
    }
    else if (newPwd != repeatNewPwd) {
        layer.msg("两次新密码输入的不一致");
        return false;
    }
    else if (newPwd.indexOf("&") > 0 || repeatNewPwd.indexOf("&") > 0) {
        layer.msg("密码不能包含特殊符号");
        return false;
    }
    else {
        return true;
    }
}

/**
 *	检测设置密码时输入框内容是否合法
 */
function checkSetPwdInput() {
    var newPwd = $("#newPwd").val();
    var repeatNewPwd = $("#repeatNewPwd").val();
    if (newPwd.length < 8 || repeatNewPwd.length < 8 || newPwd.length > 16 || repeatNewPwd.length > 16) {
        layer.msg("密码在8-16位之间");
        return false;
    }
    else if (newPwd != repeatNewPwd) {
        layer.msg("两次新密码输入的不一致");
        return false;
    }
    else if (newPwd.indexOf("&") > 0 || repeatNewPwd.indexOf("&") > 0) {
        layer.msg("密码不能包含特殊符号");
        return false;
    }
    else {
        return true;
    }
}