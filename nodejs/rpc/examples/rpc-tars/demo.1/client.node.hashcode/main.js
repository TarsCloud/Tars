var TarsClient  = require("../../../../protal.js").client;
var TarsProxy = require("./NodeTarsProxy").tars;

//使用配置文件初始化通信器
TarsClient.initialize("./config.conf");
//生成代理类
//var prx = TarsClient.stringToProxy(TARS.NodeTarsProxy, TarsClient.configure.get("main.DevServer"));        //客户端和服务端都部署在IDC机房。客户端通过主控，查询活动列表，然后调用服务端
//var prx = TarsClient.stringToProxy(TARS.NodeTarsProxy, TarsClient.configure.get("main.ProxyServer"));      //客户端部署在本地，服务端部署在IDC机房。客户端通过“特别代理”，连接IDC机器
var servant=TarsClient.configure.get("main.LocalServer");
var prx = TarsClient.stringToProxy(TarsProxy.NodeTarsProxy, servant);      //客户端和服务端都部署在本地。客户端直连本地服务
var index=0;
//生成回调类
var success = function (result) {
    console.log("remote server endpoint:",             result.request.RemoteEndpoint.toString());   //本次调用对应的服务端地址，类型为['tars-utils'].Endpoint
    console.log("result.response.costtime:",           result.response.costtime);                   //本地调用的耗时，单位为毫秒
    console.log("result.response.return: ",            result.response.return);                     //函数的返回值（在tars文件中定义）
    console.log("result.response.arguments.sValue1:",  result.response.arguments.sValue1);          //函数的out参数（在tars文件中定义）
    console.log("result.response.arguments.sValue1:",  result.response.arguments.sValue2);          //函数的out参数（在tars文件中定义）
}
var error = function (result) {
    console.log("result.response.error.code: ",        result.response.error.code,"  index:",index);                 //本次调用失败的错误代码
    //console.log("result.response.error.message: ",     result.response.error.message);              //本次调用失败的错误描述
}
prx.setTimeout(3000);
setInterval(function(){
    prx.getUsrName(`czzou_${index++}`,{hashCode:1}).then(success, error);
},3000);

