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

var treeId = parent.tarsTree.treeId ? parent.tarsTree.treeId : getQueryStringArgs().treeId,
    params = treeId.split('.'),
    paramMap = {};
var gData = {};
$.each(params,function (index,n) {
    paramMap[n.substring(0,1)] = n.substring(1);
});
gData.level = params[params.length-1].substring(0,1);
gData.application = paramMap[1];
gData.set_name = paramMap[2];
gData.set_area = paramMap[3];
gData.set_group = paramMap[4];
gData.server_name = paramMap[5];

$(function () {

    if(gData.level==1){
        getCfgList({level:1,application:gData.application});
    }else{
        getCfgList({level:gData.level,application:gData.application,server_name:gData.server_name,set_name:gData.set_name,set_area:gData.set_area,set_group:gData.set_group});
        if(gData.level==4){
            $('#ref-container').hide();
            $('#node-container').hide();
        }else{
            $('#ref-container').show();
            $('#node-container').show();
        }

        $('#cfg-box').on('click','.chks',function () {
            $('#ref-container').show();
            $('#node-container').show();
            getFileList(this.value);
            getNodeList({level:gData.level,application:gData.application,server_name:gData.server_name,set_name:gData.set_name,set_area:gData.set_area,set_group:gData.set_group,config_id:this.value});
        });
        $('#cfg-box').on('click','.cfg_row',function () {
            var chk = $(this).find('.chks')[0];
            chk.checked=true;
            $('#ref-container').show();
            $('#node-container').show();
            getFileList(chk.value);
            getNodeList({level:gData.level,application:gData.application,server_name:gData.server_name,set_name:gData.set_name,set_area:gData.set_area,set_group:gData.set_group,config_id:chk.value});
        });
    }

    $('#push-btn').click(function () {
        var $chks = $('#node-table').find('.chks:checked'),
            ids=[];
        $chks.each(function () {
            ids.push(this.value);
        });
        pushCfg(ids.join(';'));
    });

});




/**
 * 获取服务配置列表
 * @param {Object} param 参数对象
 */
function getCfgList(param) {
    $.each(param,function (key,value) {
        if(!value) {
            delete param[key];
        }
    });
    param = $.param(param);
    $.getJSON('/pages/server/api/config_file_list?' + param,function (data) {
        var htmlStr = '';
        if(data.ret_code==200 && data.sub_code!=400){
            htmlStr = template('cfg-tpl',data);
            $('#cfg-box').html(htmlStr);
            $('.chks').eq(0).click();
        }else if(data.ret_code==200 && data.sub_code==400){
            $('#cfg-box').html('<div class="alert alert">'+data.err_msg+'</div>');
        }else{
            $('#cfg-box').html('<div class="empty">系统出错！</div>');
        }
    });
}


/**
 * 获取引用文件列表
 * @param {Number} id 配置文件ID
 */
function getFileList(id) {
    $.getJSON('/pages/server/api/config_ref_list?config_id=' + id,function (data) {
        var htmlStr = '';
        if(data.ret_code==200 && data.data.length){
            htmlStr = template('file-tpl',data);
            $('#file-box').html(htmlStr);
        }else{
            $('#file-box').html('<div class="empty">没有文件！</div>');
        }
    });
}

/**
 * 添加引用文件
 */
function addCfgRef() {
    var $ret = $('#dlg-ret');
    var cfgId = $('#cfg-table').find('.chks:checked').val();
    if(!cfgId){
        $ret.html('<div class="alert alert-danger">请先选择服务配置</div>');
        showDialog($ret,'添加引用文件');
        return;
    }
    param = $.param({application:gData.application,config_id:cfgId});
    $.getJSON('/pages/server/api/unused_config_file_list?' + param,function (data) {
        var htmlStr = '<div class="form-group"><label class="col-sm-2 control-label">引用文件<span class="text-danger">*</span></label>';
        htmlStr += '<div class="col-sm-10"><select class="form-control" id="ref-select">';
        if(data.ret_code==200 && data.sub_code!=400){
            $.each(data.data,function (index,n) {
                htmlStr += '<option value="'+n.id+'">'+n.filename+'</option>';
            });
            htmlStr += '</select></div></div>';
            $ret.html(htmlStr);
        }else{
            $ret.html('<div class="empty">没有文件</div>');
        }
    });
    closeErrorMsg();
    showDialog($ret,'添加引用文件',{
        '取消':function () {
            this.modal('hide');
        },
        '添加':function () {
            var cfgId = $('#cfg-table').find('.chks:checked').val(),
                self = this;
            if(!cfgId){
                showErrorMsg($ret,'请先选择服务配置文件');
                return;
            }
            showLoading();
            $.getJSON('/pages/server/api/add_config_ref?config_id='+cfgId+'&reference_id='+$('#ref-select').val(),function (data) {
                removeLoading();
                if(data.ret_code==200){
                    self.modal('hide');
                    getFileList(cfgId);
                }else{
                    showErrorMsg($ret,'添加引用文件出错');
                }
            });
        }
    });
}

/**
 * 节点配置列表添加引用文件
 * @param {Element} tar 添加引用文件按钮本身
 */
function addCfgRefNode(tar) {
    var $ret = $('#dlg-ref-list');
    var cfgId = $('#cfg-table').find('.chks:checked').val();
    if(!cfgId){
        $ret.html('<div class="alert alert-danger">请先选择服务配置</div>');
        showDialog($ret,'添加引用文件');
        return;
    }
    //param = $.param({level:1,application:gData.application});
    param = $.param({application:gData.application,config_id:$(tar).data('value')});
    $.getJSON('/pages/server/api/unused_config_file_list?' + param,function (data) {
        var htmlStr = '<div class="form-group"><label class="col-sm-2 control-label">引用文件<span class="text-danger">*</span></label>';
        htmlStr += '<div class="col-sm-10"><select class="form-control" id="ref-select">';
        if(data.ret_code==200 && data.sub_code!=400){
            $.each(data.data,function (index,n) {
                htmlStr += '<option value="'+n.id+'">'+n.filename+'</option>';
            });
            htmlStr += '</select></div></div>';
            $ret.html(htmlStr);
        }else{
            $ret.html('<div class="empty">没有文件</div>');
        }
    });
    closeErrorMsg();
    showDialog($ret,'添加引用文件',{
        '取消':function () {
            this.modal('hide');
        },
        '添加':function () {
            var cfgId = $(tar).data('value'),
                self = this;
            showLoading();
            $.getJSON('/pages/server/api/add_config_ref?config_id='+cfgId+'&reference_id='+$('#ref-select').val(),function (data) {
                removeLoading();
                if(data.ret_code==200){
                    self.modal('hide');
                    getFileRefNode(cfgId);
                }else{
                    showErrorMsg($ret,'添加引用文件出错');
                }
            });
        }
    });
}


/**
 * 删除引用文件
 * @param {Number} id 引用文件ID
 */
function delCfgRef(id) {
    $('#dlg-ret').html('您确定要删除这个引用文件吗？');
    closeErrorMsg();
    showDialog($('#dlg-ret'),'删除引用文件',{
        '取消':function () {
            this.modal('hide');
        },
        '删除':function () {
            var self = this;
            showLoading();
            $.getJSON('/pages/server/api/delete_config_ref?id='+id,function (data) {
                removeLoading();
                if(data.ret_code==200){
                    self.modal('hide');
                    getFileList($('#cfg-table').find('.chks:checked').val());
                }else{
                    showErrorMsg($('#dlg-ret'),'删除引用文件出错');
                }
            });
        }
    });
}

/**
 * 查看引用文件内容
 * @param {Number} id 引用文件ID
 */
function viewCfgRef(id) {
    var $ret = $('#dlg-ret');
    showLoading();
    $.getJSON('/pages/server/api/view_config_ref?id='+id,function (data) {
        removeLoading();
        if(data.ret_code==200){
            $ret.html('<pre>'+data.data.replace(/\</g,'&lt;').replace(/\>/g,'&gt;')+'</pre>');
        }else{
            $ret.html('<div class="alert-danger">查看引用文件内容出错</div>');
        }
    });
    showDialog($ret,'查看引用文件内容');
}

/**
 * 查看引用文件历史记录
 * @param {Number} id 引用文件ID
 */
function viewHistoryRef(id) {
    var $ret = $('#dlg-ret');
    showLoading();
    $.getJSON('/pages/server/api/view_config_ref?id='+id,function (data) {
        removeLoading();
        if(data.ret_code==200){
            $ret.html('<pre>'+data.data.replace(/\</g,'&lt;').replace(/\>/g,'&gt;')+'</pre>');
        }else{
            $ret.html('<div class="alert-danger">查看引用文件内容出错</div>');
        }
    });
    showDialog($ret,'查看引用文件内容');
}

/**
 * 获取节点配置列表
 * @param {Object} param 参数对象
 */
function getNodeList(param) {
    $.each(param,function (key,value) {
        if(!value) {
            delete param[key];
        }
    });
    param = $.param(param);
    $.getJSON('/pages/server/api/node_config_file_list?' + param,function (data) {
        var htmlStr = '';
        if(data.ret_code==200 && data.data){
            htmlStr = template('node-tpl',data);
            $('#node-box').html(htmlStr);
        }else{
            $('#node-box').html('<div class="tc">没有数据</div>');
        }
    });
}

/**
 * 查看合并后内容
 * @param {Number} id 配置文件ID
 */
function viewMergeCfg(id) {
    var $ret = $('#dlg-ret');
    $.getJSON('/pages/server/api/merged_node_config?id=' + id,function (data) {
        if(data.ret_code==200){
            $ret.html('<pre>'+data.data.replace(/\</g,'&lt;').replace(/\>/g,'&gt;')+'</pre>');
        }else{
            $ret.html('<div class="alert alert-danger">加载合并后配置文件出错！！！</div>');
        }
    });
    showDialog($ret,'查看合并后内容');
}

/**
 * 添加配置
 */
function addCfg() {
    var $box = $('#dlg-add-cfg');
    if(gData.level==4){
        $box.find('#add-cfg-filename').hide();
        $box.find('#add-cfg-filename-select').show();
        $.getJSON('/pages/server/api/config_file_list?level=1&application='+gData.application,function (data) {
            if(data.ret_code==200){
                var htmlStr = '';
                $.each(data.data,function (index,n) {
                    htmlStr += '<option value="'+n.filename+'">'+n.filename+'</option>';
                });
                $box.find('#add-cfg-filename-select').html(htmlStr);
            }
        });
    }
    closeErrorMsg();
    showDialog($box,'添加配置',{
        '取消':function () {
            this.modal('hide');
        },
        '确定':function () {
            var paramObj = {
                level       :   gData.level,
                application :   gData.application,
                server_name :   gData.server_name,
                set_name    :   gData.set_name,
                set_area    :   gData.set_area,
                set_group   :   gData.set_group,
                //node_name   :   '10.133.80.142',
                filename    :   $.trim($('#add-cfg-filename').val()),
                config      :   $.trim($('#add-cfg-filecontent').val())
            },self = this;
            if(gData.level==4){
                paramObj.filename = $('#add-cfg-filename-select').val();
            }
            if(!paramObj.filename){
                showErrorMsg($('#err-msg'),'文件名不能为空');
                return;
            }
            if(!/^([a-zA-Z0-9._-])+$/g.test(paramObj.filename)){
                showErrorMsg($('#err-msg'),'文件名只能包含英文字母或数字、点、下划线、短横线');
                return;
            }
            if(!paramObj.config){
                showErrorMsg($('#err-msg'),'文件内容不能为空');
                return;
            }
            var param = $.stringify(paramObj);
            showLoading();
            $.ajax('/pages/server/api/add_config_file',{
                contentType:'application/json',
                method:'post',
                data:param,
                dataType:'json',
                success:function (data) {
                    removeLoading();
                    var $msg = $('#dlg-ret');
                    if(data.ret_code==200){
                        self.modal('hide');
                        $msg.html('<div class="alert alert-success" role="alert">配置文件添加成功！</div>');
                        showDialog($msg,'信息提示',{'确定':function () {
                            location.reload();
                        }});
                        getCfgList({level:gData.level,application:gData.application,server_name:gData.server_name,set_name:gData.set_name,set_area:gData.set_area,set_group:gData.set_group});
                    }else{
                        showErrorMsg($('#err-msg'),'配置文件添加失败');
                    }
                }
            });
        }
    });
}

/**
 * 获取配置文件内容
 * @param {Number} id       配置列表ID
 * @param {Function} callback 回调函数
 */
function getFileConfig(id,callback) {
    $.getJSON('/pages/server/api/config_file?id='+id,function (data) {
        if(data.ret_code==200){
            callback(data.data.config);
        }else{
            showErrorMsg($('#dlg-update-cfg'),'获取配置文件内容失败');
        }
    });
}

/**
 * 修改配置
 * @param {Number} id       配置列表ID
 * @param {String} filename 文件名
 */
function updateCfg(id,filename) {
    $('#update-cfg-filename').val(filename);
    var $content = $('#update-cfg-filecontent');
    getFileConfig(id,function (data) {
        $content.html(data);
    });
    showDialog($('#dlg-update-cfg'),'修改配置',{
        '取消':function () {
            this.modal('hide');
        },
        '修改':function () {
            var self = this,
                content = $.trim($('#update-cfg-filecontent').val());
            showLoading();
            $.ajax('/pages/server/api/update_config_file',{
                contentType:'application/json',
                method:'post',
                data:$.stringify({id:id,config:content,reason:$.trim($('#reason').val())}),
                dataType:'json',
                success:function (data) {
                    removeLoading();
                    if(data.ret_code==200){
                        self.modal('hide');
                        getCfgList({level:gData.level,application:gData.application,server_name:gData.server_name,set_name:gData.set_name,set_area:gData.set_area,set_group:gData.set_group});
                    }else{
                        showErrorMsg($('#err-msg2'),'配置文件修改失败');
                    }
                }
            });
        }
    });
}

/**
 * 删除配置
 * @param {Number} id       配置列表ID
 */
function deleteCfg(id) {
    $('#dlg-ret').html('您确定要删除这条配置吗？');
    showDialog($('#dlg-ret'),'删除配置',{
        '取消':function () {
            this.modal('hide');
        },
        '删除':function () {
            var self = this;
            showLoading();
            $.getJSON('/pages/server/api/delete_config_file?id='+id,function (data) {
                removeLoading();
                if(data.ret_code==200){
                    self.modal('hide');
                    location.reload();
                }else{
                    showErrorMsg($('#dlg-ret'),'删除失败');
                }
            });
        }
    });
}

/**
 * 查看配置文件内容
 * @param {Number} id       配置列表ID
 */
function viewCfg(id) {
    var $dlgRet = $('#dlg-ret');
    getFileConfig(id,function (data) {
        if(data || data==''){
            $dlgRet.html('<pre>'+data.replace(/\</g,'&lt;').replace(/\>/g,'&gt;')+'</pre>');
        }else{
            $dlgRet.html('<pre></pre>');
        }
    });
    showDialog($dlgRet,'查看配置文件内容');
}

/**
 * 查看配置文件变更记录
 * @param {Number} id       配置列表ID
 */
function viewHistoryCfg(id) {
    var $dlgHisList = $('#dlg-history-list');
    $dlgHisList.off('click');
    $.getJSON('/pages/server/api/config_file_history_list?config_id='+id,function (data) {
        var htmlStr = '';
        if(data.ret_code==200){
            htmlStr = template('cfg-history-tpl',data);
            $dlgHisList.html(htmlStr);
            $dlgHisList.append('<pre id="content-box"></pre>');
            $dlgHisList.on('click','.viewContent',function () {
                $('#content-box').html('<img src="/img/loading.gif">');
                $.getJSON('/pages/server/api/config_file_history?id='+$(this).data('id'),function (data) {
                    if(data.ret_code==200){
                        $('#content-box').html(data.data.content);
                    }else{
                        $('#content-box').html('加载文件内容出错');
                    }
                });
            });
        }
    });
    showDialog($dlgHisList,'查看配置文件变更记录');
}

/**
 * 管理引用文件
 *  @param {Number} id       节点列表ID
 */
function manageRefCfg(id) {
    var $fileBox = $('#dlg-file');
    $fileBox.find('button').attr('data-value',id);
    getFileRefNode(id);
    showDialog($fileBox,'管理引用文件',null,800);
}

/**
 * 获取节点配置引用文件列表
 *  @param {Number} id       节点列表ID
 */
function getFileRefNode(id) {
    var $fileList = $('#dlg-file-box');
    $.getJSON('/pages/server/api/config_ref_list?config_id=' + id,function (data) {
        var htmlStr = '';
        if(data.ret_code==200 && data.data.length){
            htmlStr = template('file-tpl',data);
            $fileList.html(htmlStr);
        }else{
            $fileList.html('<div class="empty">没有文件！</div>');
        }
    });
}

/**
 * PUSH配置文件
 *  @param {String} ids     节点集合的字符串，以分号分隔
 */
function pushCfg(ids) {
    closeErrorMsg();
    if(!ids){
        showErrorMsg($('#node-box'),'请选择节点');
        return;
    }
    showLoading();
    $.getJSON('/pages/server/api/push_config_file?config_ids='+ids,function (data) {
        removeLoading();
        var $ret = $('#dlg-ret');
        if(data.ret_code==200){
            var htmlStr = '<table class="table table-hover table-bordered"><thead><tr><th>服务</th><th>结果</th></tr></thead><tbody>',
                status = 'text-success';
            $.each(data.data,function (index,n) {
                status = n.ret_code == 0 ? 'text-success' : 'text-danger';

                htmlStr += '<tr><td>'+n.server_name+'</td><td class="'+status+'">'+n.err_msg+'</td></tr>';
            });
            htmlStr += '</tbody></table>';
            $ret.html(htmlStr);
        }else{
            $ret.html('<div class="alert-danger">系统出错！！！</div>');
        }
        showDialog($ret,'PUSH结果');
    });
}

