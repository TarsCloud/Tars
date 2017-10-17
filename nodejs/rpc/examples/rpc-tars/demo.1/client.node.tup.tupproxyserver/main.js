var http = require("http");
var TarsStream = require('@tars/stream');
var TARS = require("./NodeTars.js").tars;

//依次启动：server.node.1=>server.node.tup.tupproxyserver=>client.node.tup.tupproxyserver
//组织请求数据
var tup = new TarsStream.Tup();
tup.tupVersion  = TarsStream.Tup.TUP_COMPLEX;
tup.servantName = "node.nodetarsserver";   //该servantName是在TupProxyServer中注册的名字
tup.funcName    = "getall";               //该funcName是服务端真实的函数名称

var stUser = new TARS.User_t();
stUser.id    = 10000;
stUser.score = 88888;
stUser.name  = "ForTupProxyTest";

tup.writeStruct("stUser", stUser);

//发送请求数据
var binBuffer = tup.encode();
var postData  = binBuffer.toNodeBuffer();

var options = {
    hostname : '127.0.0.1',    //该地址为TupProxyServer的地址
    port     : 14001,               //该端口为TupProxyServer的端口
    path     : '/',
    method   : 'POST',
    headers  : {
        'Content-Type': 'application/octet-stream',
        'Content-Length': postData.length
    }
};

var req = http.request(options, function(res) {
    res.on('data', function (chunk) {
        var data = new TarsStream.BinBuffer(new Buffer(chunk));
        var tup  = new TarsStream.Tup();
        tup.decode(data);

        var stResult = tup.readStruct("stResult", TARS.Result_t);
        console.log("result.response.return: ",             tup.readInt32(""));
        console.log("result.response.arguments.stResult:",  stResult.id, stResult.iLevel);
    });
});

req.on('error', function(e) {
    console.log('problem with request: ' + e.message);
});

req.write(postData);
req.end();
