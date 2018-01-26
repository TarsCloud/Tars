/*
 * Tencent is pleased to support the open source community by making Tars available.
 *
 * Copyright (C) 2016 THL A29 Limited, a Tencent company. All rights reserved.
 *
 * Licensed under the BSD 3-Clause License (the "License"); you may not use this file except
 * in compliance with the License. You may obtain a copy of the License at
 *
 * https://opensource.org/licenses/BSD-3-Clause
 *
 * Unless required by applicable law or agreed to in writing, software distributed
 * under the License is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
 * CONDITIONS OF ANY KIND, either express or implied. See the License for the
 * specific language governing permissions and limitations under the License.
 */

$.ajaxSetup({
    contentType : 'application/json',
    cache       :   false
});
/**
 * 静态表格插件
 */
jQuery.fn.table = function () {
    var fn = {
        sortTable : function (table,col,desc) {
            var trs = $.makeArray(table.find('tbody').find('tr')),
                df;
            df = trs.sort(function (tr1,tr2) {
                if(parseInt(tr1.cells[col].innerHTML)){
                    return desc * (tr2.cells[col].innerHTML - tr1.cells[col].innerHTML);
                }else{
                    if(desc==1) {
                        return tr1.cells[col].innerHTML < tr2.cells[col].innerHTML;
                    }else if(desc==-1){
                        return tr1.cells[col].innerHTML > tr2.cells[col].innerHTML;
                    }
                }
            });
            table.find('tbody').html(df);
            var active = table.data('pageIdx') || 1;
            fn.showTr(table,20*(active-1),20*(active-1)+20);
        },
        showTr: function (table, begin, end) {
            var $tbody = table.find('tbody'),
                $tr = $tbody.find('tr'),
                arrTr = $.makeArray($tr);
            $tr.hide();
            $.each(arrTr.slice(begin,end),function (index,n) {
                $(n).show();
            });
        },
        paging: function (table) {
            var $tPage = table.find('.tpage'),
                $tbody = table.find('tbody'),
                trs = $.makeArray($tbody.find('tr')),
                len = Math.ceil(trs.length/20),
                pagingStr = '<ul class="pagination"><li><a href="#;">&laquo;</a></li>';
            for(var i=0;i<len;i++){
                pagingStr += '<li><a href="#;">'+(i+1)+'</a></li>';
            }
            pagingStr += '<li><a href="#;">&raquo;</a></li></ul>';
            $tPage.html(pagingStr);
            $tPage.find('li').eq(1).addClass('active');
            fn.showTr(table,0,20);

            var btnPage = $tPage.find('li'),
                btnLen = btnPage.length;
            btnPage.each(function (index) {
                $(this).click(function () {
                    if(index > 0 && index < btnLen-1){
                        $(this).siblings().removeClass('active');
                        $(this).addClass('active');
                        table.data('pageIdx',index);
                        fn.showTr(table,20*(index-1),20*(index-1)+19);
                    }else if(index==0){
                        var pIdx = table.data('pageIdx') || 1,
                            $active = $(this).siblings('.active');
                        if(pIdx>1){
                            table.data('pageIdx',pIdx-1);
                            $active.removeClass('active');
                            $active.prev().addClass('active');
                            fn.showTr(table,20*(pIdx-2),20*(pIdx-2)+19);
                        }
                    }else{
                        var pIdx = table.data('pageIdx') || 1,
                            $active = $(this).siblings('.active');
                        if(pIdx<btnLen-2){
                            table.data('pageIdx',pIdx+1);
                            $active.removeClass('active');
                            $active.next().addClass('active');
                            fn.showTr(table,20*pIdx,20*pIdx+19);
                        }
                    }
                });
            });
        }
    };

    return this.each(function(){
        var table = $(this),
            th = $(this).find('th');
        fn.paging(table);
        th.each(function (index) {
            var $this = $(this);
            if($this.hasClass('sort')){
                $this.append('<span></span>');
                $this.click(function () {
                    var desc = $(this).data('desc');
                    $this.siblings().find('span').removeAttr('class');
                    if(desc===1){
                        $this.data('desc',-1);
                        $this.html($this.text()+'<span class="desc"></span>');
                    }else if(desc===-1){
                        $this.data('desc',1);
                        $this.html($this.text()+'<span class="asc"></span>');
                    }else{
                        $this.data('desc',1);
                        $this.html($this.text()+'<span class="asc"></span>');
                        desc=-1;
                    }
                    fn.sortTable(table,index,desc);
                });
            }
        });
    });
};

/**
 * 将JSON对象转换成JSON字符串
 * @param obj {Object} JSON对象
 * @return json {String} 返回
 */
jQuery.extend({
    stringify  : function stringify(obj) {
        if ("JSON" in window) {
            return JSON.stringify(obj);
        }
        var t = typeof (obj);
        if (t != "object" || obj === null) {
            // simple data type
            if (t == "string") obj = '"' + obj + '"';
            return String(obj);
        } else {
            // recurse array or object
            var n, v, json = [], arr = (obj && obj.constructor == Array);

            for (n in obj) {
                v = obj[n];
                t = typeof(v);
                if (obj.hasOwnProperty(n)) {
                    if (t == "string") v = '"' + v + '"'; else if (t == "object" && v !== null) v = jQuery.stringify(v);
                    json.push((arr ? "" : '"' + n + '":') + String(v));
                }
            }
            return (arr ? "[" : "{") + String(json) + (arr ? "]" : "}");
        }
    }
});

// 生成加载动画
function showLoading(){
    var htmlStr = '<div id="mask" class="mask"><div class="loading_wrap"><img src="/img/loading2.gif"><p>玩命加载中</p></div></div>';
    $('body').append(htmlStr);
    $('#mask').height($(window).height() + $(window).scrollTop());
    $(window).bind('resize',function(){
        $('#mask').height($(window).height() + $(window).scrollTop());
    });
}

// 移除加载动画
function removeLoading(){
    var $mask = $('#mask');
    if($mask.length){
        $mask.remove();
        $(window).unbind('resize');
    }
}


/**
 * 封装bootstrap的模态框
 * @param tar {Object} 弹出框对象
 * @param title {String} 弹出框标题
 * @param btns {Object} 弹出框按钮
 * @param width {Number} 弹出框宽度
 * @param height {Number} 弹出框高度
 */
function showDialog(tar,title,btns,width,height) {
    var $modal = tar.parents('.modal');
    $modal.off('click');
    tar.show();
    width = width || 600;
    height = height +'px' || auto;
    if(tar.parents('.modal').length){
        tar.parents('.modal').show();
        $modal.find('.modal-title').html(title);
    }else{
        var dlgStr = '<div class="modal fade modal1" tabindex="-1" role="dialog" aria-labelledby="myLargeModalLabel" aria-hidden="true">';
        dlgStr += '<div class="modal-dialog" style="width:'+width+'px"><div class="modal-content"><div class="modal-header">';
        dlgStr += '<button type="button" class="close" data-dismiss="modal"><span aria-hidden="true">×</span><span class="sr-only">Close</span></button>';
        dlgStr += '<h4 class="modal-title" id="mySmallModalLabel">'+title+'</h4></div><div class="modal-body" style="overflow: auto;height: '+height+'"></div>';
        dlgStr += '<div class="modal-footer"></div></div></div></div>';
        //$('body').append(dlgStr);
        $modal = $(dlgStr);
        $modal.find('.modal-body').html(tar);
    }
    if(!$.isEmptyObject(btns)){
        var btnStr = '';
        $.each(btns,function (key,value) {
            if(key=='取消'){
                btnStr += '<button type="button" class="btn btn-sm btn-default dlg-btn">'+key+'</button>';
            }else{
                btnStr += '<button type="button" class="btn btn-sm btn-primary dlg-btn">'+key+'</button>';
            }
        });
        $modal.find('.modal-footer').html(btnStr);
        $($modal).on('click','.dlg-btn',function () {
            btns[$(this).text()].call($modal);
        });
    }else{
        $modal.find('.modal-footer').html('');
    }
    $modal.modal({backdrop: 'static'});
}

// 获取查询字符串
function getQueryStringArgs(){
    var qs=(location.search.length>0 ? location.search.substring(1) : '');
    var args={};
    var items=qs.split('&');
    var item=null,name=null,value=null;
    for(var i=0;i<items.length;i++){
        item=items[i].split('=');
        name=decodeURIComponent(item[0]);
        value=decodeURIComponent(item[1]);
        args[name]=value;
    }
    return args;
}

/**
 * 显示错误信息
 * @param tar {Object} 错误信息显示的指定对象
 * @param msg {String} 信息内容
 */
function showErrorMsg(tar,msg) {
    if($('.err_msg_box').length){
        closeErrorMsg();
    }
    var msgStr = '<div class="alert alert-danger err_msg_box" role="alert"><button type="button" class="close" data-dismiss="alert"><span aria-hidden="true">×</span><span class="sr-only">Close</span></button><h5><strong>错误提示!!!</strong></h5><p>'+msg+'</p></div>';
    tar.prepend(msgStr);
}

function closeErrorMsg() {
    $('.err_msg_box').remove();
}


/**
 * 表格全选
 * @param {Element} tar 全选目标控件
 */
function checkAll(tar) {
    var chks = $(tar).parents('table').find('.chks');
    if(tar.checked){
        //chks.attr('checked','checked');
        chks.each(function () {
            this.checked=true;
        });
    }else{
        chks.removeAttr('checked');
    }
}

/**
 * 拉取任务实时状态
 * @param {Number} taskId  任务ID
 * @param {Element} tar    目标元素
 */
function getTaskStatus(taskId,tar) {
    var status = ['EM_T_NOT_START','EM_T_RUNNING','EM_T_SUCCESS','EM_T_FAILED','EM_T_CANCEL','EM_T_PARIAL'],t;
    var statusMap = {
        EM_T_NOT_START  : '未开始',
        EM_T_RUNNING  : '执行中',
        EM_T_SUCCESS  : '成功',
        EM_T_FAILED  : '失败',
        EM_T_CANCEL  : '取消',
        EM_T_PARIAL : '部分成功'
    };
    t && clearTimeout(t);
    var f = function () {
        var htmlStr = '<img src="/img/loading.gif">';
        $.getJSON('/pages/server/api/task?task_no='+taskId,function (data) {
            var tableStr = '<table  class="table"><caption>任务ID：'+taskId+'</caption><thead><tr><th>节点</th><th>状态</th></tr></thead><tbody>';
            if(data.ret_code==200){
                try{
                    $.each(data.data.items,function (index,n) {
                        var statusStr = '<span class="ml10"><img src="/img/loading.gif"></span>' + statusMap[status[data.data.status]];
                        if(n.status==2){
                            statusStr = '<span class="text-success">'+statusMap[status[data.data.status]]+'</span>';
                            clearTimeout(t);
                        }else if(n.status==3){
                            statusStr = '<span class="text-danger">'+statusMap[status[data.data.status]]+'</span>';
                            clearTimeout(t);
                        }
                        tableStr += '<tr><td>'+n.node_name+'</td><td class="curr-status">'+statusStr+'</td></tr>';
                    });
                    /*htmlStr ='<span class="ml10"><img src="/img/loading.gif"></span>' + statusMap[status[data.data.status]];
                    if(data.data.status==2){
                        htmlStr = '<span class="text-success">'+statusMap[status[data.data.status]]+'</span>';
                        clearTimeout(t);
                    }else if(data.data.status==3){
                        htmlStr = '<span class="text-danger">'+statusMap[status[data.data.status]]+'</span>';
                        clearTimeout(t);
                    }*/
                    tableStr += '</tbody></table>';
                    htmlStr = tableStr;
                }catch(err){
                    htmlStr = '<span class="text-danger">返回的状态码错误，状态码：'+data.data.status+'</span>';
                    clearTimeout(t);
                }
            }else{
                htmlStr = '<span class="text-danger">系统出错！！！</span>';
                clearTimeout(t);
            }
            tar.html(htmlStr);
        });
        t = setTimeout(f,3000);
    };
    f();
}
