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

var treeId = parent.tarsTree.treeId ? parent.tarsTree.treeId : getQueryStringArgs().treeId;
var params = treeId.split('.'),
    paramMap = {};
$.each(params,function (index,n) {
    paramMap[n.substring(0,1)] = n.substring(1);
});
var level = params[params.length-1].substring(0,1);

$(function () {

    if(treeId=='undefined'){
        $('#table-box').html('<div class="alert alert-info" role="alert">请先从左侧业务树选择一个服务</div>');
        $('#cur-status-box').html('<div class="alert alert-info" role="alert">请先从左侧业务树选择一个服务</div>');
    }else{
        getServiceList(treeId);


        if(level<=4){
            $('#cur-status-container').hide();
        }else{
            $('#cur-status-container').show();
            getServiceStatus(treeId);
        }
    }


    // 是否启用Set
    $('input[name="u_enable_set"]').change(function () {
        if($(this).val()=='true'){
            $('.set-control').removeAttr('disabled');
        }else{
            $('.set-control').attr('disabled','disabled');
        }
    });

    $('input[name=checkCmdRadio]').change(function () {
        $('#dlg-other-command').find('.form-control').attr('disabled','disabled');
        $(this).parents('.form-group').find('.form-control').removeAttr('disabled');
    });

    $('body').on('keydown','.num-input',function (e) {
        if(!(e.keyCode>47 && e.keyCode<59) && !(e.keyCode>95 && e.keyCode<106) && e.keyCode!=8 && e.keyCode!=46){
            return false;
        }
    });
});

/**
 * 获取服务列表
 * @param treeId {String} 树ID
 */
function getServiceList(treeId) {
    showLoading();
    $.getJSON('/pages/server/api/server_list?tree_node_id='+treeId,function (data) {
        removeLoading();
        var htmlStr = '';
        if(data.ret_code==200 && !data.sub_code) {
            $.each(data.data,function (index,n) {
                n.bak_flag = n.bak_flag.toString();
                n.enable_set = n.enable_set.toString();
            });
            htmlStr = template('seviceTpl', data);
            $('#table-box').html(htmlStr);
            $('#table').table();
            $('.get_curr_status').each(function () {
                var $self = $(this);
                getServiceCurrStatus($self.attr('data-serviceid'),$self);
            });
        }else if(data.ret_code==200 && data.sub_code){
            $('#table-box').html('<p class="text-danger tc">参数错误</p>');
        }else{
            $('#table-box').html('<div class="alert alert-danger" role="alert">加载出错：'+data.err_msg+'</div>');
        }
    });
}

/**
 * 获取服务实时状态
 * @param {String} id 服务ID
 * @param {Element} tar 目标
 */
function getServiceCurrStatus(id,tar) {
    $.getJSON('/pages/server/api/get_realtime_state?id='+id,function(data){
        if(data.ret_code==200){
            if(data.data.realtime_state=='inactive'){
                tar.html('<span class="label label-danger">inactive</span>');
            }else{
                tar.html('<span class="label label-success">active</span>');
            }
        }else{
            tar.html('<span class="label label-danger">inactive</span>');
        }
    });
}

/**
 * 获取服务日志列表
 * @param treeId {String} 服务ID
 */
function getServiceStatus(treeId) {
    $('#dyTable').bootstrapTable({
        url: '/pages/server/api/server_notify_list?tree_node_id='+treeId,
        method: 'get',
        cache: false,
        pageNumber: 1,
        pageSize: 10,
        striped: true,
        pagination: true,
        dataType:'json',
        sortable: true,
        sidePagination: "server",
        pageList: [10, 25, 50, 100],
        queryParams:function(params) {
            return {
                page_size : params.limit,
                cur_page: params.offset/10+1
            };
        },
        responseHandler:function (res) {
            if(res.ret_code==200 && res.sub_code){
                $('#dyTable').html('<tr><td><p class="text-danger tc">参数错误</p></td></tr>');
                return;
            }
            $.each(res.data,function(index,n){
                if(/^\[alarm\]\s(zombie process,no keep alive msg for 60 seconds|down, server is inactive|down,pid not exist|activating,pid not exist)/gi.test(n.result)){
                    n.result = '<span class="text-danger">'+n.result+'</span>';
                }
            });
            return {
                total :res.total,
                rows : res.data
            }
        },
        columns: [{
            field: 'notifytime',
            title: '时间'
        }, {
            field: 'server_id',
            title: '服务ID'
        }, {
            field: 'thread_id',
            title: '线程ID'
        }, {
            field: 'result',
            title: '结果'
        }]
    });
}

/**
 * 查看服务状态
 * @param serviceId {String} 服务ID
 */
function viewStatusOfServer(serviceId) {
    showLoading();
    var $dlg = $('#dlg-view-service');
    $.getJSON('/pages/server/api/send_command?server_ids='+serviceId+'&command=tars.viewstatus',function (data) {
        removeLoading();
        var htmlStr = '';
        if(data.ret_code==200 && data.data.length){
            htmlStr = '<h4>服务ID：'+serviceId+'</h4>';
            if(data.data[0].ret_code==0){
                htmlStr += '<div class="alert alert-success" role="alert">';
                htmlStr += data.data[0].err_msg.replace(/\n/g,'<br>').replace(/\s/g,'&nbsp;');
            }else{
                htmlStr = '<div class="alert alert-danger" role="alert">';
                htmlStr += data.data[0].err_msg.replace(/\n/g,'<br>').replace(/\s/g,'&nbsp;');
            }
            htmlStr += '</div>';
            $dlg.html(htmlStr);
        }else{
            $dlg.html('<div class="alert alert-danger" role="alert">系统出错：'+data.err_msg+'</div>');
        }
    });
    showDialog($dlg,'服务状态',null,800,400);
}

/**
 * 服务下线
 * @param serviceId {Number} 服务ID
 * @param settingState {String} 设置状态
 */
function undeployServer(serviceId,settingState) {
    var $ret = $('#dlg-ret');
    showLoading();
    $.getJSON('/pages/server/api/get_realtime_state?id='+serviceId,function(data){
        removeLoading();
        if(data.ret_code==200){
            if(data.data.realtime_state=='inactive' && settingState=='inactive'){
                $ret.html('您确定要下线此服务？');
                showDialog($ret,'服务下线',{
                    '取消':function () {
                        this.modal('hide');
                    },
                    '下线':function () {
                        var self = this;
                        showLoading();
                        var paramObj = {
                            serial  :   true,
                            items   :   [{server_id:serviceId,command :'undeploy_tars'}]
                        };
                        var param = $.stringify(paramObj);
                        $.ajax('/pages/server/api/add_task',{
                            contentType:'application/json',
                            method:'post',
                            data:param,
                            dataType:'json',
                            success:function (data) {
                                removeLoading();
                                if(data.ret_code==200){
                                    self.modal('hide');
                                    var $box = $('#dlg-undeploy');

                                    getTaskStatus(data.data.task_no,$box);
                                    showDialog($box,'任务状态',{
                                        '确定':function () {
                                            getServiceList(parent.tarsTree.treeId);
                                            this.modal('hide');
                                        }
                                    });
                                }else{
                                    $ret.html('<div class="alert alert-danger">下线失败</div>');
                                }
                            }
                        });
                    }
                });
            }else{
                $ret.html('运行中的服务无法下线');
                showDialog($ret,'服务下线');
            }
        }
    });
}

/**
 * 重启服务
 * @param serviceId {Number} 服务ID
 */
function restartServer(serviceId) {
    showLoading();
    var $box = $('#dlg-ret');
    var paramObj = {
        serial  :   true,
        items   :   [{server_id:serviceId,command :'restart'}]
    };
    var statusMap = {
        EM_I_NOT_START  : '未开始',
        EM_I_RUNNING  : '执行中',
        EM_I_SUCCESS  : '成功',
        EM_I_FAILED  : '失败',
        EM_I_CANCEL  : '取消',
        EM_I_PARIAL : '部分成功'
    };
    $.post('/pages/server/api/add_task',$.stringify(paramObj),function (data) {
        removeLoading();
        if(data.ret_code==200){
            getTaskStatus(data.data.task_no,$box);
        }else{
            $box.html('');
            closeErrorMsg();
            showErrorMsg($box,'重启失败:'+data.err_msg);
        }
    },'json');
    showDialog($box,'重启结果',{
        '确定':function () {
            this.modal('hide');
            getServiceList(parent.tarsTree.treeId);
        }
    });
}

/**
 * 停止服务
 * @param serviceId {Number} 服务ID
 */
function stopServer(serviceId) {
    var $ret = $('#dlg-ret');
    $ret.html('您确定要停止此服务？');
    showDialog($ret,'停止服务',{
        '取消':function () {
            this.modal('hide');
        },
        '停止服务':function () {
            this.modal('hide');
            showLoading();
            var $box = $('#dlg-undeploy');
            var paramObj = {
                serial  :   true,
                items   :   [{server_id:serviceId,command :'stop'}]
            };
            $.post('/pages/server/api/add_task',$.stringify(paramObj),function (data) {
                removeLoading();
                if(data.ret_code==200){
                    getTaskStatus(data.data.task_no,$box);
                    showDialog($box,'停止服务结果',{
                        '确定':function () {
                            this.modal('hide');
                            location.reload();
                        }
                    });
                }else{
                    closeErrorMsg();
                    showErrorMsg($box,'停止服务失败:'+data.err_msg);
                    showDialog($box,'停止服务结果');
                }
            },'json');

        }
    });

}

/**
 * 编辑服务
 * @param {Number} serviceId 服务ID
 * @param {String} serverNmae 服务名称
 * @param {String} nodeName  部署节点
 * @param {Boolean} isBak  是否备机
 * @param {String} templateName 模板名称
 * @param {String} serverType  服务类型
 * @param {Boolean} enableSet  是否启用Set
 * @param {String} setName  Set名称
 * @param {String} setArea  Set区域
 * @param {String} setGroup Set组
 * @param {Number} asyncThreadNum  异步线程数
 * @param {String} basePath  缺省路径
 * @param {String} exePath  EXE路径
 * @param {String} startScriptPath  启动脚本
 * @param {String} stopScriptPath  停止脚本
 * @param {String} monitorScriptPath  监控脚本
 * @param {String} profile  私有模板
 */
function updateServer(serviceId,serverNmae,nodeName,isBak,templateName,serverType,enableSet,setName,setArea,setGroup,asyncThreadNum,basePath,exePath,startScriptPath,stopScriptPath,monitorScriptPath) {
    var $box = $('#dlg-update-service');
    $box.find('#u-service-name').html(serverNmae);
    $box.find('#u-node-name').html(nodeName);
    $box.find('input[name="is_bak"][value="'+isBak+'"]')[0].checked = true;
    $box.find('#u-server-type').val(serverType);
    $box.find('input[name="u_enable_set"][value="'+enableSet+'"]')[0].checked = true;
    $box.find('#u-set-name').val(setName);
    $box.find('#u-set-area').val(setArea);
    $box.find('#u-set-group').val(setGroup);
    $box.find('#u-async-thread').val(asyncThreadNum);
    $box.find('#u-base-path').val(basePath);
    $box.find('#u-exe-path').val(exePath);
    $box.find('#u-start-path').val(startScriptPath);
    $box.find('#u-stop-path').val(stopScriptPath);
    $box.find('#u-monitor-path').val(monitorScriptPath);
    if(enableSet=='true'){
        $('.set-control').removeAttr('disabled');
    }else{
        $('.set-control').attr('disabled','disabled');
    }
    getTemplateList(function (data) {
        $box.find('#u-template-name').html(data).val(templateName);
    });
    getServer(serviceId,function (data) {
        $box.find('#u-profile').val(data.profile);
    });
    closeErrorMsg();
    showDialog($box,'编辑服务',{
        '取消':function () {
            this.modal('hide');
        },
        '确定':function () {
            var paramObj = {
                id              :   serviceId,
                isBak           :   $box.find('input[name="is_bak"]:checked').val(),
                template_name   :   $box.find('#u-template-name').val(),
                server_type     :   $box.find('#u-server-type').val(),
                enable_set      :   "true" == $box.find('input[name="u_enable_set"]:checked').val(),
                set_name        :   $.trim($box.find('#u-set-name').val().toLowerCase()),
                set_area        :   $.trim($box.find('#u-set-area').val().toLowerCase()),
                set_group       :   $.trim($box.find('#u-set-group').val()),
                async_thread_num:   $.trim($box.find('#u-async-thread').val()),
                base_path       :   $.trim($box.find('#u-base-path').val()),
                exe_path        :   $.trim($box.find('#u-exe-path').val()),
                start_script_path:  $.trim($box.find('#u-start-path').val()),
                stop_script_path:   $.trim($box.find('#u-stop-path').val()),
                monitor_script_path: $.trim($box.find('#u-monitor-path').val()),
                profile         :   $.trim($box.find('#u-profile').val())
            },
            self = this,
            regPath = /^\/?([a-zA-Z0-9_.-]+\/)+[a-zA-Z0-9_.-]+?$/i;
            if(paramObj.enable_set){
                if(!paramObj.set_name || !paramObj.set_area || !paramObj.set_group){
                    showErrorMsg($box,'所有打星的为必填项');
                    return;
                }
                if(!/^[a-z]+$/.test(paramObj.set_name)){
                    showErrorMsg($box,'Set名只能为英文字母，且必须小写');
                    return;
                }
                if(!/^[a-z]+$/.test(paramObj.set_area)){
                    showErrorMsg($box,'Set区域只能为英文字母，且必须小写');
                    return;
                }
                if(!/^(\d+|\*)$/.test(paramObj.set_group)){
                    showErrorMsg($box,'Set组必须为数字或*号');
                    return;
                }
            }else{
                delete paramObj.set_name;
                delete paramObj.set_area;
                delete paramObj.set_group;
            }
            if(paramObj.base_path && !regPath.test(paramObj.base_path)){
                showErrorMsg($box,'缺省路径格式错误，必须为绝对路径');
                return;
            }
            if(paramObj.exe_path && !regPath.test(paramObj.exe_path)){
                showErrorMsg($box,'EXE路径格式错误，必须为绝对路径');
                return;
            }
            if(paramObj.start_script_path && !regPath.test(paramObj.start_script_path)){
                showErrorMsg($box,'启动脚本格式错误，必须为绝对路径');
                return;
            }
            if(paramObj.stop_script_path && !regPath.test(paramObj.stop_script_path)){
                showErrorMsg($box,'停止脚本格式错误，必须为绝对路径');
                return;
            }
            if(paramObj.monitor_script_path && !regPath.test(paramObj.monitor_script_path)){
                showErrorMsg($box,'监控脚本格式错误，必须为绝对路径');
                return;
            }
            showLoading();
            var param = $.stringify(paramObj);

            $.ajax('/pages/server/api/update_server',{
                contentType:'application/json',
                method:'post',
                data:param,
                dataType:'json',
                success:function (data) {
                    removeLoading();
                    self.modal('hide');
                    if(data.ret_code==200){
                        var $msg = $('#dlg-ret');
                        $msg.html('<div class="alert alert-success" role="alert">服务更新成功！</div>');
                        showDialog($msg,'信息提示');
                        getServiceList(parent.tarsTree.treeId);
                    }else{
                        showErrorMsg($box,'服务更新失败');
                    }
                }
            });
        }
    },800);
}

/**
 * 获取模板名称
 * @param {Function} callback  获取数据后的回调函数
 * @return {String}  html      返回获取到的下拉列表
 */
function getTemplateList(callback) {
    $.getJSON('/pages/server/api/template_name_list',function (data) {
        if(data.ret_code==200){
            var htmlStr = '';
            $.each(data.data,function (index, n) {
                htmlStr += '<option value="'+n+'">'+n+'</option>';
            });
            callback(htmlStr);
        }
    });
}

/**
 * 获取服务详情
 * @param {Number}  id      服务ID
 */
function getServer(id,callback) {
    $.getJSON('/pages/server/api/server?id='+id,function (data) {
        if(data.ret_code==200){
            callback(data.data);
        }
    });
}

/**
 * 显示Servant详情
 * @param {Number}  id          服务ID
 * @param {String}  serverName  服务名称
 * @param {String}  nodeName    节点名称
 */
function showAdapter(id,serverName,nodeName) {
    var $box = $('#dlg-servant');
    $box.data('serverName',serverName);
    $box.data('serverId',id);
    $box.data('nodeName',nodeName);
    showDialog($box,'Servant管理',null,1000);
    getAdapterList(id,function (data) {
        var htmlStr = template('servant-tpl',data);
        $box.find('#servant-box').html(htmlStr);
    });
}

/**
 * 获取Servant列表
 * @param {Number}  id      服务ID
 */
function getAdapterList(id,callback) {
    $.getJSON('/pages/server/api/adapter_conf_list?id='+id,function (data) {
        if(data.ret_code==200){
            callback(data);
        }
    });
}

/**
 * 添加Servant
 * @param {Number}  id      服务ID
 */
function addAdapter() {
    var $box = $('#dlg-add-servant'),
        serverName = $('#dlg-servant').data('serverName'),
        appName = parent.tarsTree.treeId.split('.')[0].substring(1);
    closeErrorMsg();
    $('#a-server-name').html(appName+'·'+serverName);
    $('#a-servant-name').val('');
    $('#a-thread-num').val('');
    $('#a-end-point').val('');
    $('#a-max-connections').val(200000);
    $('#a-queuecap').val(10000);
    $('#a-queuetimeout').val(60000);
    $('#a-allow-ip').val('');
    $('#a-handlegroup').val('');
    showDialog($box,'添加Servant',{
        '取消':function(){
            this.modal('hide');
        },
        '确定':function () {
            var self = this,
                paramObj = {
                    application         :   appName,
                    server_name         :   serverName,
                    node_name           :   $('#dlg-servant').data('nodeName'),
                    endpoint            :   $.trim($('#a-end-point').val()),
                    servant             :   appName+'.'+serverName+'.'+$.trim($('#a-servant-name').val()),
                    thread_num          :   $.trim($('#a-thread-num').val()),
                    max_connections     :   $.trim($('#a-max-connections').val()),
                    queuecap            :   $.trim($('#a-queuecap').val()),
                    queuetimeout        :   $.trim($('#a-queuetimeout').val()),
                    allow_ip            :   $.trim($('#a-allow-ip').val()),
                    protocol            :   $('input[name="a-protocol"]:checked').val(),
                    handlegroup         :   $.trim($('#a-handlegroup').val())
                },regIP = /^(\d{1,2}|1\d\d|2[0-4]\d|25[0-5])\.(\d{1,2}|1\d\d|2[0-4]\d|25[0-5])\.(\d{1,2}|1\d\d|2[0-4]\d|25[0-5])\.(\d{1,2}|1\d\d|2[0-4]\d|25[0-5])$/;
            if(!paramObj.servant || !paramObj.endpoint){
                showErrorMsg($box,'OBJ名和绑定地址为必填项!!!');
                return;
            }
            if(!/^[a-zA-Z]+$/.test($.trim($('#a-servant-name').val()))){
                showErrorMsg($box,'OBJ名称必须为全英文');
                return;
            }
            if(!/^\d+$/g.test(paramObj.thread_num) || parseInt(paramObj.thread_num)<1){
                showErrorMsg($box,'线程数必须为数字,且大于0');
                return;
            }
            if(paramObj.endpoint){
                var tmp = paramObj.endpoint.split(/\s-/);
                var regProtocol = /^tcp|udp|ssl$/gi,
                    regHost = /^h\s(\d{1,2}|1\d\d|2[0-4]\d|25[0-5])\.(\d{1,2}|1\d\d|2[0-4]\d|25[0-5])\.(\d{1,2}|1\d\d|2[0-4]\d|25[0-5])\.(\d{1,2}|1\d\d|2[0-4]\d|25[0-5])$/gi,
                    regT = /^t\s([1-9]|[1-9]\d+)$/gi,
                    regPort = /^p\s\d{4,5}$/,
                    regAuth = /^e\s\d$/, // optional
                    check=true;
                if(regProtocol.test(tmp[0])){
                    var flag = 0;
                    for(var i=1;i<tmp.length;i++){
                        if(regHost.test(tmp[i]) || regT.test(tmp[i]) || regPort.test(tmp[i]) || regAuth.test(tmp[i])){
                            flag++;
                        }else{
                            flag--;
                        }
                    }
                    if(flag>=3){
                        for(var i=1;i<tmp.length;i++){
                            if(regPort.test(tmp[i])){
                                var port = tmp[i].substring(2);
                                if(port<0 || port>65535){
                                    check = false;
                                }
                            }
                        }
                    }else{
                        check = false;
                    }
                }else{
                    check = false;
                }
                if(!check){
                    showErrorMsg($box,'绑定地址填写错误!!!  <br>绑定地址格式：以tcp 、ssl 或者 udp开头，有 -h -t -p 三个必选参数和-e可选参数， -p 0-65535  -t 大于 0  -h ip格式 -e 0-1');
                    return;
                }
            }
            if(paramObj.allow_ip){
                var tmp = paramObj.allow_ip.split(/[;:]/);
                for(var i=0;i<tmp.length;i++){
                    if(!regIP.test(tmp[i])){
                        showErrorMsg($box,'IP格式错误，请重新填写。');
                        return;
                    }
                }
            }
            showLoading();
            var param = $.stringify(paramObj);
            $.ajax('/pages/server/api/add_adapter_conf',{
                contentType:'application/json',
                method:'post',
                data:param,
                dataType:'json',
                success:function (data) {
                    removeLoading();
                    if(data.ret_code==200){
                        self.modal('hide');
                        getAdapterList($('#dlg-servant').data('serverId'),function (data) {
                            var htmlStr = template('servant-tpl',data);
                            $('#dlg-servant').find('#servant-box').html(htmlStr);
                        });
                    }else{
                        showErrorMsg($box,'Servant添加失败');
                    }
                }
            });
        }
    },800);
}

/**
 * 编辑Servant
 * @param {Number}  id      ServantID
 * @param {Element} tar     目标链接
 */
function updateAdapter(id,tar,protocol,handlegroup,allowip) {
    var $box = $('#dlg-add-servant'),
        serverName = $('#dlg-servant').data('serverName'),
        $tr = $(tar).parents('tr');
    var servantNameArr = $tr.find('.servant').text().split('.');
    $('#a-servant-name').val(servantNameArr[servantNameArr.length-1]);
    $('#a-end-point').val($tr.find('.end-point').text());
    $('#a-thread-num').val($tr.find('.thread-num').text());
    $('#a-max-connections').val($tr.find('.max-connections').text());
    $('#a-queuecap').val($tr.find('.queuecap').text());
    $('#a-queuetimeout').val($tr.find('.queuetimeout').text());
    $('#a-allow-ip').val(allowip);
    $('input[name="a-protocol"][value="'+protocol+'"]')[0].checked=true;
    $('#a-handlegroup').val(handlegroup);
    closeErrorMsg();
    showDialog($box,'编辑Servant',{
        '取消':function () {
            this.modal('hide');
        },
        '确定':function () {
            var serverName = $('#dlg-servant').data('serverName'),
                self = this,
                paramObj = {
                    id                  :   id,
                    endpoint            :   $.trim($('#a-end-point').val()),
                    servant             :   paramMap[1]+'.'+serverName+'.'+$.trim($('#a-servant-name').val()),
                    thread_num          :   $.trim($('#a-thread-num').val()),
                    max_connections     :   $.trim($('#a-max-connections').val()),
                    queuecap            :   $.trim($('#a-queuecap').val()),
                    queuetimeout        :   $.trim($('#a-queuetimeout').val()),
                    allow_ip            :   $.trim($('#a-allow-ip').val()),
                    protocol            :   $('input[name="a-protocol"]:checked').val(),
                    handlegroup         :   $.trim($('#a-handlegroup').val())
                },regIP = /^(\d{1,2}|1\d\d|2[0-4]\d|25[0-5])\.(\d{1,2}|1\d\d|2[0-4]\d|25[0-5])\.(\d{1,2}|1\d\d|2[0-4]\d|25[0-5])\.(\d{1,2}|1\d\d|2[0-4]\d|25[0-5])$/;
            if(!paramObj.servant || !paramObj.endpoint){
                showErrorMsg($box,'OBJ名和绑定地址为必填项!!!');
                return;
            }
            if(!/^[a-zA-Z]+$/.test($.trim($('#a-servant-name').val()))){
                showErrorMsg($box,'OBJ名称必须为全英文');
                return;
            }
            if(!/^\d+$/g.test(paramObj.thread_num) || parseInt(paramObj.thread_num)<1){
                showErrorMsg($box,'线程数必须为数字,且大于0');
                return;
            }
            if(paramObj.endpoint){
                var tmp = paramObj.endpoint.split(/\s-/);
                var regProtocol = /^tcp|udp|ssl$/gi,
                    regHost = /^h\s(\d{1,2}|1\d\d|2[0-4]\d|25[0-5])\.(\d{1,2}|1\d\d|2[0-4]\d|25[0-5])\.(\d{1,2}|1\d\d|2[0-4]\d|25[0-5])\.(\d{1,2}|1\d\d|2[0-4]\d|25[0-5])$/gi,
                    regT = /^t\s([1-9]|[1-9]\d+)$/gi,
                    regPort = /^p\s\d{4,5}$/,
                    regAuth = /^e\s\d$/, // optional
                    check=true;
                if(regProtocol.test(tmp[0])){
                    var flag = 0;
                    for(var i=1;i<tmp.length;i++){
                        if(regHost.test(tmp[i]) || regT.test(tmp[i]) || regPort.test(tmp[i]) || regAuth.test(tmp[i])){
                            flag++;
                        }else{
                            flag--;
                        }
                    }
                    if(flag>=3){
                        for(var i=1;i<tmp.length;i++){
                            if(regPort.test(tmp[i])){
                                var port = tmp[i].substring(2);
                                if(port<0 || port>65535){
                                    check = false;
                                }
                            }
                        }
                    }else{
                        check = false;
                    }
                }else{
                    check = false;
                }
                if(!check){
                    showErrorMsg($box,'绑定地址填写错误!!!  <br>绑定地址格式：以tcp 、ssl 或者 udp开头，有 -h -t -p 三个必选参数和-e可选参数， -p 0-65535  -t 大于 0  -h ip格式 -e 0-1');
                    return;
                }
            }
            if(paramObj.allow_ip){
                var tmp = paramObj.allow_ip.split(/[;:]/);
                for(var i=0;i<tmp.length;i++){
                    if(!regIP.test(tmp[i])){
                        showErrorMsg($box,'IP格式错误，请重新填写。');
                        return;
                    }
                }
            }
            showLoading();
            var param = $.stringify(paramObj);
            $.ajax('/pages/server/api/update_adapter_conf',{
                contentType:'application/json',
                method:'post',
                data:param,
                dataType:'json',
                success:function (data) {
                    removeLoading();
                    if(data.ret_code==200){
                        self.modal('hide');
                        getAdapterList($('#dlg-servant').data('serverId'),function (data) {
                            var htmlStr = template('servant-tpl',data);
                            $('#dlg-servant').find('#servant-box').html(htmlStr);
                        });
                    }else{
                        showErrorMsg($box,'Servant更新失败');
                    }
                }
            });
        }
    },800);
}

/**
 * 删除Servant
 * @param {Number}  id      AdapterID
 */
function deleteAdapter(id){
    var $box = $('#dlg-ret');
    $box.html('您确定要删除这个Servant?');
    showDialog($box,'删除Servant',{
        '取消':function(){
            this.modal('hide');
        },
        '确定':function () {
            showLoading();
            var self = this;
            $.getJSON('/pages/server/api/delete_adapter_conf?id='+id,function(data){
                removeLoading();
                if(data.ret_code==200){
                    self.modal('hide');
                    var $dlg = $('#dlg-servant');
                    getAdapterList($dlg.data('serverId'),function (data) {
                        var htmlStr = template('servant-tpl',data);
                        $dlg.find('#servant-box').html(htmlStr);
                    });
                }
            });
        }
    },800);
}

/**
 * 发布服务
 */
function deployService() {
    var $box = $('#dlg-patch-server');
    $.getJSON('/pages/server/api/server_list?tree_node_id='+parent.tarsTree.treeId,function (data) {
        var htmlStr = template('patch-service-tpl',data);
        $box.find('#patch-server-list').html(htmlStr);
    });
    showDialog($box,'发布服务',null,800);
}

/**
 * 打开上传发布包弹出框
 * @param {String} appName      应用名
 * @param {String} serverName   服务名
 */
function openUploadDlg(appName,serverName) {
    var $box = $('#dlg-upload-patch');
    $box.find('#up-application').val(appName);
    $box.find('#up-server-name').val(serverName);
    showDialog($box,'上传发布包');
}

/**
 * 获取发布版本列表
 * @param {Element} tar         下拉框
 * @param {String} appName      应用名
 * @param {String} serverName   服务名
 */
function getPatchList(tar,appName,serverName) {
    $.getJSON('/pages/server/api/server_patch_list?application='+appName+'&module_name='+serverName,function (data) {
        if(data.ret_code==200 && data.data.length){
            var htmlStr = '';
            $.each(data.data,function (index,n) {
                htmlStr += '<option value="'+n.id+'">'+ n.tgz + ' | ' + n.posttime +'</option>';
            });
            $(tar).html(htmlStr);
        }
    });
}

/**
 * 打开更多命令弹出框
 * @param {String} application 应用名
 * @param {String} serverName  服务名
 * @param {String} serverId    服务ID
 */
function openCmdDlg(application,serverName,serverId) {
    var $box = $('#dlg-other-command');
    $.getJSON('/pages/server/api/config_file_list?level=5&application='+application+'&server_name='+serverName,function (data) {
        removeLoading();
        var htmlStr = '';
        if(data.ret_code==200 && !data.sub_code) {
            $.each(data.data,function (index,n) {
                 htmlStr += '<option value="'+n.filename+'">'+n.filename+'</option>';
            });
            $box.find('select[name=pushFile]').html(htmlStr);
        }
    });
    showDialog($box,'更多命令',{
        '取消':function () {
            this.modal('hide');
        },
        '提交':function () {
            var commandType = $('input[name="checkCmdRadio"]:checked').val(),
                command = '';
            switch (commandType){
                case 'setLog' :
                    command = $('select[name=setLog]').val();
                    break;
                case 'pushFile' :
                    command = 'tars.loadconfig ' + $('select[name=pushFile]').val();
                    break;
                case 'customCmd' :
                    command = $('input[name=customCmd]').val();
                    break;
                case 'viewLink' :
                    command = 'tars.connection';
                    break;
            }
            showLoading();
            $.getJSON('/pages/server/api/send_command?server_ids='+serverId+'&command='+command,function (data) {
                removeLoading();
                var $dlg = $('#dlg-ret');
                if(data.ret_code==200 && data.data.length){
                    htmlStr = '<h4>服务ID：'+serverId+'</h4>';
                    if(data.data[0].ret_code==0){
                        htmlStr += '<div class="alert alert-success" role="alert">';
                        htmlStr += data.data[0].err_msg.replace(/\n/g,'<br>');
                    }else{
                        htmlStr = '<div class="alert alert-danger" role="alert">';
                        htmlStr += data.data[0].err_msg.replace(/\n/g,'<br>');
                    }
                    htmlStr += '</div>';
                    $dlg.html(htmlStr);
                }else{
                    $dlg.html('<div class="alert alert-danger" role="alert">系统出错！！！</div>');
                }
                showDialog($dlg,'执行结果',null,800);
            });
        }
    });
}

