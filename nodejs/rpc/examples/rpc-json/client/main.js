var Tars=require("../../../protal.js");
var TarsClient = Tars.client;
var Protocol = Tars.rpcJson.client;
var ServantProxy = Tars.ServantProxy;

var prx = TarsClient.stringToProxy(ServantProxy, "test@tcp -h 127.0.0.1 -p 12306 -t 60000");
prx.setProtocol(Protocol);

var success = function (result) {
    console.log("result.response.costtime:",    result.response.costtime);
    console.log("result.response.appBuffer",    result.response.appBuffer);
}

var error = function (result) {
    console.log("result.response.error.code:",    result.response.error.code);
    console.log("result.response.error.message:", result.response.error.message);
}

//第一种使用方法
prx.rpc.createFunc("echo");
prx.rpc.echo("TENCENT", "MIG", {a : 1, b : 2}).then(success, error).done();

//第二种使用方法
prx.rpc_call(1, "echo", ["TENCENT", "MIG", {a : 1, b : 2}]).then(success, error).done();
