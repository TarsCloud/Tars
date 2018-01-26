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
$.getJSON('/DBServer/getService.json',function(data){
    tarsTree.serviceData = data;
});
$.getJSON('/DBServer/getModule.json',function(data){
    tarsTree.moduleData = data;
});
$.getJSON('/DBServer/getUsedService.json',function(data){
    tarsTree.usedServiceData = data;
});
tarsTree.zTreeClick=function(e,treeId, treeNode){
    tarsTree.zTreeObj.expandNode(treeNode);
    if(!treeNode.isParent && treeNode.level==1 || (tarsTree.checkType=='radio' && treeNode.level==2)){
        tarsTree.input.val(treeNode.name).attr('data-value',treeNode.id);
        tarsTree.callback.call(tarsTree.input);
        $("#bussiness_tree").hide();
    }

};
tarsTree.zTreeCheck=function(){
    tarsTree.checkNodes();
};

tarsTree.setting = {
    data: {
        simpleData: {
            enable: true,
            idKey: "id",
            pIdKey: "pId"
        }
    },
    async: {
        enable:true
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
        onClick: tarsTree.zTreeClick,
        onCheck: tarsTree.zTreeCheck
    }
};

tarsTree.hide = function(){
    tarsTree.tree_container.hide();
    $(document).unbind('click',tarsTree.hide);
    tarsTree.input.unbind('blur',tarsTree.clearValue);
};

tarsTree.checkNodes = function(){
    var nodes = tarsTree.zTreeObj.getCheckedNodes(true);
    if(nodes.length){
        tarsTree.checkedNodes = nodes;
        var selectedNodes = [],
            selectedNodeIds = [];
        $.each(nodes,function(index,n){
            if(n.level==2){
                selectedNodes.push(n.name);
                selectedNodeIds.push(n.id);
            }
        });
        tarsTree.input.val(selectedNodes.join(',')).attr('data-value',selectedNodeIds.join(','));
    }else{
        tarsTree.input.val('').removeAttr('data-value');
    }
};

tarsTree.clearValue = function(){
    if(tarsTree.input.val()==''){
        tarsTree.input.removeAttr('data-value');
    }
};

/*弹出框的点击事件
    * data:数据
    * combo_input:选择的值将填入的文本框
    * checkType:树类型，单选还是多选
*/
function showTree(data,combo_input,checkType,selectedData,callback){
    tarsTree.input = combo_input;
    //tarsTree.setting.async.url = url;
    var tree_container = $("#bussiness_tree"),
        $treeObj;
    tree_container.show();
    if(checkType!='check'){
        $(".fix_div").hide();
        tarsTree.setting.check.enable = false;
    }else{
        $(".fix_div").hide();
        tarsTree.setting.check.enable = true;
    }
    combo_input.after(tree_container);
    tree_container.width(combo_input.width());
    tarsTree.tree_container = tree_container;
    tarsTree.checkType = checkType;
    tarsTree.callback = callback || function(){};
    $treeObj = $("#filetree");
    if(tarsTree[data]){
        $.fn.zTree.init($treeObj, tarsTree.setting,tarsTree[data]);
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
    $(document).bind('click',tarsTree.hide);
    tarsTree.tree_container.bind('click',function(e){
        e.stopPropagation();
    });
    tarsTree.input.bind('blur',tarsTree.clearValue);
}


// 业务树搜索功能
function searchService(els,value,e){
    e.stopPropagation();
    var $treeObj = $('#filetree'),
        treeObj = $.fn.zTree.getZTreeObj("filetree");
    tarsTree = tarsTree || {};
    if($.trim(value)){
        if(!treeObj){
            showTree('serviceData',$(els).siblings('input'),'radio',$(els).siblings('input').attr('data-value'));
            return;
        }
        $.fn.zTree.init($treeObj, tarsTree.setting,searchTreeData(treeObj.getNodes(),$.trim(value)));
        $(els).siblings('.tree_combo').show();
        $(document).bind('click',tarsTree.hide);
        $(els).siblings('.tree_combo').bind('click',function(e){
            e.stopPropagation();
        });
    }else{
        if(els.id=='moduleId'){
            showTree('usedServiceData',$(els).siblings('input'),'radio');
        }else{
            showTree('serviceData',$(els).siblings('input'),'radio');
        }

    }
}

// 模块树搜索功能
function searchModule(els,value,e){
    e.stopPropagation();
    var $treeObj = $('#filetree');
    value = $.trim(value);
    tarsTree = tarsTree || {};
    if(value){
        $.getJSON('/DBServer/getModule.json?keyWord='+value,function(data){
            $.fn.zTree.init($treeObj, tarsTree.setting,data);
            $(els).siblings('.tree_combo').show();
            $(document).bind('click',tarsTree.hide);
            $(els).siblings('.tree_combo').bind('click',function(e){
                e.stopPropagation();
            });
        });
    }else{
        showTree('moduleData',$(els).siblings('input'),'check');
    }

}

// 搜索树
function searchTreeData(arr, key){
    key = key.toLowerCase();
    //递归查找
    var treeData = (function(arr, key, pNode){
        var args = arguments;
        pNode = pNode || {};
        if(!pNode.children){
            pNode.children = [];
        }
        for(var i = 0, len = arr.length; i<len; i++){
            if(arr[i].name && arr[i].name.toLowerCase().indexOf(key)>-1){
                pNode.children.push(arr[i]);
            }else{
                var children = arr[i].children;
                if(children && children.length){
                    delete(arr[i].children);
                    args.callee(children, key, arr[i]);
                    if(arr[i].children && arr[i].children.length){
                        pNode.children.push(arr[i]);
                    }
                }
            }
        }
        return pNode.children;
    })($.extend(true, [], arr), key, {});

    //给所有节点加上open参数
    return (function(arr){
        for(var i = 0, len=arr.length;i<len;i++){
            arr[i].open = true;
            if(arr[i].children && arr[i].children.length > 0){
                arguments.callee(arr[i].children);
            }
        }
        return arr;
    })(treeData);
}