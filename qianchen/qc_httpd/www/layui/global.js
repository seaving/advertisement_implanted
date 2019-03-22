/**

 layui官网

*/

layui.define(['code', 'element', 'table', 'util'], function(exports){
  var $ = layui.jquery
  ,element = layui.element
  ,layer = layui.layer
  ,form = layui.form
  ,util = layui.util
  ,device = layui.device();


  //阻止IE7以下访问
  if(device.ie && device.ie < 8){
    layer.alert('~很抱歉^_^ 您当前使用的是古老的 IE'+ device.ie + ', 请用其他浏览器打开~');
  }

  layer.ready(function(){
    var local = layui.data('layui');

    layui.data('layui', {
      key: 'notice_20171212'
      ,remove: true
    });
    return;
  });
  
  //窗口scroll
  ;!function(){
    var main = $('.site-tree').parent(), scroll = function(){
      var stop = $(window).scrollTop();

      if($(window).width() <= 750) return;
      var bottom = $('.footer').offset().top - $(window).height();
      if(stop > 61 && stop < bottom){
        if(!main.hasClass('site-fix')){
          main.addClass('site-fix');
        }
        if(main.hasClass('site-fix-footer')){
          main.removeClass('site-fix-footer');
        }
      } else if(stop >= bottom) {
        if(!main.hasClass('site-fix-footer')){
          main.addClass('site-fix site-fix-footer');
        }
      } else {
        if(main.hasClass('site-fix')){
          main.removeClass('site-fix').removeClass('site-fix-footer');
        }
      }
      stop = null;
    };
    scroll();
    $(window).on('scroll', scroll);
  }();

  //示例页面滚动
  $('.site-demo-body').on('scroll', function(){
    var elemDate = $('.layui-laydate')
    ,elemTips = $('.layui-table-tips');
    if(elemDate[0]){
      elemDate.each(function(){
        var othis = $(this);
        if(!othis.hasClass('layui-laydate-static')){
          othis.remove();
        }
      });
      $('input').blur();
    }
    if(elemTips[0]) elemTips.remove();

    if($('.layui-layer')[0]){
      layer.closeAll('tips');
    }
  });
  
  //代码修饰
  layui.code({
    elem: 'pre'
  });

  //手机设备的简单适配
  var treeMobile = $('.site-tree-mobile')
  ,shadeMobile = $('.site-mobile-shade')

  treeMobile.on('click', function(){
    $('body').addClass('site-mobile');
  });

  shadeMobile.on('click', function(){
    $('body').removeClass('site-mobile');
  });

  exports('global', {});
});