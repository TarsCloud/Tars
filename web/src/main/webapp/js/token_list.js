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

var args = getQueryStringArgs();
var treeId = parent.tarsTree.treeId ? parent.tarsTree.treeId : getQueryStringArgs().treeId;
var params = treeId.split('.'),
    paramMap = {};
$.each(params, function (index, n) {
    paramMap[n.substring(0, 1)] = n.substring(1);
});
window.application = paramMap[1];
window.serverName = paramMap[5];

getTokenList(application, serverName);
getObjList(application, serverName);

$('#btn-add-token').click(function(){
    addToken();
});

/**
 * 获取token列表
 * @param {String} application
 * @param {String} serverName
 */
function getTokenList(application, serverName) {
    showLoading();
    var paramObj = {
        application: application || '',
        server_name: serverName || '',
    };
    $.getJSON('/pages/server/api/get_tokens', paramObj, function (data) {
        removeLoading();
        if (data.ret_code == 200) {
            var htmlStr = template('token-tpl', data);
            $('#token-list').html(htmlStr);
            $('#token-table').table();
        }
    });
}

/**
 * 解除token
 * @param {String} sobj obj名
 * @param {String} obj 主调服务
 */
function deleteToken(sObj, obj) {
    var $box = $('#dlg-ret');
    $box.html('您确定要解除此token?');
    showDialog($box, '删除Servant', {
        '取消': function () {
            this.modal('hide');
        },
        '确定': function () {
            showLoading();
            var self = this;
            var arr = obj.split('.');
            var application = arr[0] || '';
            var serverName = arr[1] || '';
            $.getJSON('/pages/server/api/delete_token?application=' + application + '&server_name=' + serverName + '&obj_name=' + sObj + '&_=' + Math.random(), function (data) {
                removeLoading();
                if (data.ret_code == 200) {
                    self.modal('hide');
                    getTokenList(window.application, window.serverName);
                }
            });
        }
    }, 800);
}

/**
 * 获取obj列表
 * @param {String} application
 * @param {String} serverName
 */
function getObjList(application, serverName){
    $.getJSON('/pages/server/api/get_obj_list?application=' + application + '&server_name=' + serverName + '&_=' + Math.random(), function (data) {
        if (data.ret_code == 200) {
            data = data.data || [];
            var options = '';
            for(var i = 0; i< data.length; i++){
                options+='<option value="'+data[i]+'">'+data[i]+'</option>'
            }
            $('#a-obj-name').html(options);
        }
    });
}



/**
 * 新增token框
 * @param {String} sobj obj名
 * @param {String} obj 主调服务
 */
function addToken(){
    var $box = $('#dlg-add-token');
    $('#a-obj-name').val('');
    $('#a-application-server-name').val('');
    showDialog($box,'新增token',{
        '取消':function(){
            this.modal('hide');
        },
        '确定':function () {
            var self = this;
            var objName = $('#a-obj-name').val();
            var applicationServerName = $('#a-application-server-name').val();
            if(!objName){
                showErrorMsg($box,'obj名不能为空');
                return;
            }
            if(!applicationServerName){
                showErrorMsg($box,'主调名不能为空');
                return;
            }
            var arr = applicationServerName.split('.');
            if(arr.length != 2){
                showErrorMsg($box,'主调名格式不正确，必须为“应用名.服务名”');
                return;
            }
            $.getJSON('/pages/server/api/add_token?application=' + arr[0] + '&server_name=' + arr[1] + '&obj_name=' + objName + '&_=' + Math.random(), function (data) {
                removeLoading();
                if (data.ret_code == 200) {
                    self.modal('hide');
                    var $msg = $('#dlg-ret');
                    $msg.html('<div class="alert alert-success" role="alert">新增token成功！</div>');
                    showDialog($msg, '信息提示');
                    getTokenList(application, serverName);
                }
            });
        }
    },800);
}
