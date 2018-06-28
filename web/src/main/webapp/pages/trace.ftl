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
    <link rel="stylesheet" href="../css/bootstrap_table.min.css">
    <link rel="stylesheet" href="../css/bootstrap_datepicker.min.css">
    <link rel="stylesheet" href="../css/style.css">

    <!--[if lt IE 9]>
    <script src="http://cdn.bootcss.com/html5shiv/3.7.2/html5shiv.min.js"></script>
    <script src="http://cdn.bootcss.com/respond.js/1.4.2/respond.min.js"></script>
    <![endif]-->
    <script src="../js/jquery.min.js"></script>
    <script src="../js/bootstrap.min.js"></script>
    <script src="../js/chart.min.js"></script>
    <script src="../js/template.js"></script>
    <script src="../js/bootstrap_table.min.js"></script>
    <script src="../js/bootstrap_datepicker.min.js"></script>
    <script src="../js/common.js"></script>
    <style>
        .charts_container{overflow:hidden;width: 1200px;margin: 10px auto;}
        .charts_container .chart_box{float:left;width:600px; height:300px;margin-bottom:10px;}
        .chart_box canvas{margin:0 10px;}
        .tool_bar{border:solid 1px #9ed0ff;margin-bottom: 10px;}
        .tool_bar a{display: inline-block;padding:5px;}
    </style>
</head>
<body>
<iframe id="frame"  name="frame" frameborder="0"  marginwidth="0" marginheight="0"></iframe>
</body>
<script>
    var newUrl ="${trace_url}/zipkin/?serviceName=app.server"
    var myDate = new Date(),
        preDate = new Date(new Date().setDate(myDate.getDate()-1)),
        gData = {};

    var treeId = parent.tarsTree.treeId ? parent.tarsTree.treeId : getQueryStringArgs().treeId,
            params = treeId.split('.'),
            paramMap = {};
    $.each(params,function (index,n) {
        paramMap[n.substring(0,1)] = n.substring(1);
    });
    gData.level = params[params.length-1].substring(0,1);
    gData.application = paramMap[1];
    gData.set_name = paramMap[2];
    gData.set_area = paramMap[3];
    gData.set_group = paramMap[4];
    gData.server_name = paramMap[5];
	newUrl = newUrl.replace("app.server", (gData.application+"."+gData.server_name).toLocaleLowerCase());
	console.log(" url is   " +newUrl )
	var iframe = window.frames['frame'];
 	$('iframe').attr('src',newUrl);  
 	
 	 var ifm= document.getElementById("frame");
	ifm.height=document.documentElement.clientHeight-10;
	ifm.width=document.documentElement.clientWidth;
</script>
</html>
