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
    <title>TARS-服务部署</title>
    <link rel="stylesheet" href="../css/bootstrap.min.css">
    <link rel="stylesheet" href="../css/zTreeStyle.css">
    <link rel="stylesheet" href="../css/style.css">
    <!--[if lt IE 9]>
    <script src="http://cdn.bootcss.com/html5shiv/3.7.2/html5shiv.min.js"></script>
    <script src="http://cdn.bootcss.com/respond.js/1.4.2/respond.min.js"></script>
    <![endif]-->
    <script src="../js/jquery.min.js"></script>
    <script src="../js/bootstrap.min.js"></script>

    <script src="../js/common.js"></script>
    <script src="../js/index.js"></script>
    <style>
        .frame-sidebar{position:absolute;width:250px;left:0;top:65px;bottom:0;}
        .frame-primary{margin: 65px 0 0 265px;}
        .tab-pane iframe{width: 100%;height: 100%;}
    </style>
</head>
<body>
    <#include "include/header.ftl">
    <#include "include/sider2.ftl">

    <div class="frame-primary">
        <div class="container-fluid">
            <iframe class="frame" id="main_iframe" name="main_iframe" width="100%" frameborder="0" src="/server_deploy.html"></iframe>
        </div>
    </div>
</body>

<script>
    var ifr = $('.frame'),
            $window = $(window);
    ifr.height($(window).height()-75);

    $window.on('resize',function () {
        var t;
        t = setTimeout(function () {
            clearTimeout(t);
            ifr.height($window.height()-75);
        },1);
    });

    $('.operator').addClass('active');
    $('.index').removeClass('active');
</script>

</html>
