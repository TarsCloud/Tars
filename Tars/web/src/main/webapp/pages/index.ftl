<!--
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
-->

<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <meta http-equiv="X-UA-Compatible" content="IE=edge">
    <meta name="viewport" content="width=device-width, initial-scale=1">
    <title>TARS</title>
    <link rel="stylesheet" href="../css/bootstrap.min.css">
    <link rel="stylesheet" href="../css/zTreeStyle.css">
    <!--[if lt IE 9]>
    <script src="http://cdn.bootcss.com/html5shiv/3.7.2/html5shiv.min.js"></script>
    <script src="http://cdn.bootcss.com/respond.js/1.4.2/respond.min.js"></script>
    <![endif]-->
    <script src="../js/jquery.min.js"></script>
    <script src="../js/bootstrap.min.js"></script>
    <script src="../js/jquery.ztree.core.min.js"></script>
    <script src="../js/jquery.ztree.excheck.min.js"></script>

    <script src="../js/tars_tree.js"></script>
    <script src="../js/common.js"></script>
    <script src="../js/index.js"></script>
    <style>
        .frame-sidebar{position:absolute;width:240px;left:10px;top:65px;bottom:15px;overflow-y:auto;overflow-x: hidden;border:solid 1px #ddd;border-radius: 5px;}
        .frame-primary{margin: 65px 0 0 250px;}
        .tab-pane iframe{width: 100%;height: 100%;}
    </style>
</head>
<body>
    <#include "include/header.ftl">
    <#include "include/sider.ftl">

    <div class="frame-primary">
        <div class="container-fluid" id="tab_container">
            <ul class="nav nav-tabs" role="tablist" id="myTab">
                <li role="presentation" class="active"><a href="#home" role="tab" data-active="0" data-toggle="tab">服务管理</a></li>
                <li role="presentation"><a href="#publish" role="tab" data-active="1" data-toggle="tab">发布管理</a></li>
                <li role="presentation"><a href="#serviceCfg" role="tab" data-active="2" data-toggle="tab">服务配置</a></li>
                <li role="presentation"><a href="#serviceMonitor" role="tab" data-active="3" data-toggle="tab">服务监控</a></li>
                <li role="presentation"><a href="#propertyMonitor" role="tab" data-active="4" data-toggle="tab">特性监控</a></li>
            </ul>

            <div class="tab-content pt10">
                <div role="tabpanel" class="tab-pane active" id="home" data-url="server_list.html">
                    <iframe class="frame" width="100%" frameborder="0"></iframe>
                </div>
                <div role="tabpanel" class="tab-pane" id="publish" data-url="pub_manage.html">
                    <iframe  class="frame" width="100%" frameborder="0"></iframe>
                </div>
                <div role="tabpanel" class="tab-pane" id="serviceCfg" data-url="service_cfg.html">
                    <iframe class="frame" width="100%" frameborder="0"></iframe>
                </div>
                <div role="tabpanel" class="tab-pane" id="serviceMonitor" data-url="monitor.html">
                    <iframe class="frame" width="100%" frameborder="0"></iframe>
                </div>
                <div role="tabpanel" class="tab-pane" id="propertyMonitor" data-url="property_monitor.html">
                    <iframe class="frame" width="100%" frameborder="0"></iframe>
                </div>
            </div>
        </div>
    </div>
</body>

<script>
    var ifr = $('.frame'),
            $window = $(window);
    ifr.height($(window).height()-130);

    $window.on('resize',function () {
        var t;
        t = setTimeout(function () {
            clearTimeout(t);
            ifr.height($window.height()-130);
        },1);
    });

    var hashMap = gethashStringArgs(),
        tab = $('#myTab').find('a'),
        tabIndex = hashMap.tabIndex,
        tabUrl = hashMap.tabUrl,
        treeId = hashMap.treeId;
    tarsTree.activeTab=tabIndex;
    tarsTree.treeId = treeId;
    $(tab[tabIndex]).tab('show'); 
    var $activeTab = $('.tab-content').find('.active');
    $activeTab.find('.frame').attr('src',$activeTab.attr('data-url')+'?treeId='+tarsTree.treeId);

    $('#myTab a').click(function (e) {
        e.preventDefault();
        tarsTree.activeTab=$(this).attr('data-active');
        if(tarsTree.treeId){
            $(this).tab('show');
            $activeTab = $('.tab-content').find('.active');
            $activeTab.find('.frame').attr('src',$activeTab.attr('data-url')+'?treeId='+tarsTree.treeId);
        }
        location.hash='#tabUrl='+$activeTab.attr('data-url')+'&tabIndex='+tarsTree.activeTab+'&treeId='+tarsTree.treeId;
    });

    $('.index').addClass('active');
    $('.operator').removeClass('active');


    function gethashStringArgs(){
        var qs=(location.hash.length>0 ? location.hash.substring(1) : '');
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
</script>

</html>
