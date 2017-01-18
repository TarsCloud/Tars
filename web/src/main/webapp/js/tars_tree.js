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

var tarsTree = {};
tarsTree.activeTab = 0;

$(function () {
    $.getJSON('/pages/tree',function (data) {
        if(data.ret_code==200 && data.data){
            var treeId = location.hash.split('treeId=');
            data.data[0].open = true;
            if(treeId.length && treeId[0]){
                showTree(data.data,$('#tree-container'),'radio',location.hash.split('treeId=')[1]);
            }else{
                showTree(data.data,$('#tree-container'),'radio');
                var treeObj = $.fn.zTree.getZTreeObj("filetree");

                var nodes = treeObj.getNodes();
                if (nodes.length>0) {
                    treeObj.selectNode(nodes[1]);
                }
            }
        }else{
            $('#tree-container').html('<div class="alert alert-danger" role="alert"><strong>业务树加载出错!</strong> '+data.err_msg+'</div>');
        }
    });
});


tarsTree.zTreeClick=function(e,treeId, treeNode){
    //tarsTree.zTreeObj.expandNode(treeNode);
    var id = treeNode.pid.replace(/^\d/,'')+'.'+treeNode.name,
        $panel = $('#tab_container').find('.tab-pane'),
        index = parseInt(tarsTree.activeTab) || 0,
        url = $panel.eq(index).attr('data-url');

    var params = treeNode.id.split('.'),
        level = params[params.length-1].substring(0,1);
    var tabs = $('#myTab').find('li');
    switch(parseInt(level)){
        case 1 :
            tabs.removeClass('hide');
            tabs.not(tabs.eq(0)).not(tabs.eq(2)).addClass('hide');
            tabs.eq(2).find('a').text('应用配置');
            break;
        case 2 :
        case 3 :
            tabs.removeClass('hide');
            tabs.not(tabs.eq(0)).addClass('hide');
            break;
        case 4 :
            tabs.removeClass('hide');
            tabs.not(tabs.eq(0)).not(tabs.eq(2)).addClass('hide');
            tabs.eq(2).find('a').text('Set配置');
            break;
        case 5:
            tabs.removeClass('hide');
            tabs.eq(2).find('a').text('服务配置');
    }
    location.hash = 'tabUrl='+url+'&tabIndex='+parseInt(tarsTree.activeTab)+'&treeId='+treeNode.id;   //url
    tarsTree.treeId = treeNode.id;
    getActiveTab(treeNode.id);
    tarsTree.treeName = id;
    $panel.eq(index).find('iframe')[0].src = '/'+url+'?treeId='+treeNode.id;
    //$panel.eq(0).find('iframe')[0].src = '/server_list.html?treeId='+treeNode.id;
};

tarsTree.setting = {
    data: {
        simpleData: {
            enable: false,
            idKey: "id",
            pIdKey: "pid",
            rootPId:'root'
        }
    },
    async: {
        enable:false
    },
    check: {
        //enable: true
        //chkStyle: "radio"
    },
    view: {
        selectedMulti: false,
        dblClickExpand: false,
        showIcon:false
    },
    callback: {
        onClick: tarsTree.zTreeClick
    }
};

/** 弹出框的点击事件
 *  @param data {Array} 业务树数据
 *  @param treeContainer {Element} 业务树容器
 *  @param checkType {String} 选择类型
 *  @param selectedData {Array} 已选数据
 *  @param callback {Function} 回调函数
 */
function showTree(data,treeContainer,checkType,selectedData,callback) {
    var $treeObj;
    tarsTree.setting.check.enable = (checkType =='check');
    tarsTree.treeContainer = treeContainer;
    tarsTree.checkType = checkType;
    tarsTree.callback = callback || function(){};
    $treeObj = treeContainer.find("#filetree");
    if(data.length){
        $.fn.zTree.init($treeObj, tarsTree.setting,data);
        tarsTree.zTreeObj = $.fn.zTree.getZTreeObj('filetree');
        if(selectedData){
            if(checkType!='check'){
                var node = tarsTree.zTreeObj.getNodeByParam('id',selectedData,null);
                tarsTree.zTreeObj.selectNode(node);
            }else{
                var dataArr = selectedData.split(','),
                    checkedNodes = [];
                $.each(dataArr,function(index,n){
                    checkedNodes.push(tarsTree.zTreeObj.getNodeByParam('id',n,null));
                });
                $.each(checkedNodes,function(index,n){
                    tarsTree.zTreeObj.checkNode(n,true,false);
                    tarsTree.zTreeObj.selectNode(n);
                });
            }
        }
    }
}

function getActiveTab(treeId) {
    var tabs = $('#myTab').find('li');
    $.each(tabs,function (index,n) {
        if($(n).hasClass('active') && $(n).hasClass('hide')){
            tabs.eq(0).find('a').tab('show');
            $('#tab_container').find('.tab-pane').eq(0).find('iframe')[0].src = '/server_list.html?treeId='+treeId;
            location.hash = 'tabUrl=server_list.html&tabIndex=0&treeId='+treeId;
            tarsTree.activeTab=0;
        }
    });
}

