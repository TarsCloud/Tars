var TarsServer = require("../../../../protal.js").server;
var TarsDemo   = require("./NodeTarsImp.js").tars;

//设置监听端口的属性

var option = new TarsServer.BindAdapterOption();
option.endpoint  = "tcp -h 127.0.0.1 -p 14002 -t 10000";
option.name      = "TarsDemo.NodeTarsServer.NodeTarsObjAdapter";
option.servant   = "TarsDemo.NodeTarsServer.NodeTarsObj";
option.handleImp = TarsDemo.NodeTarsImp;

var svr = TarsServer.createServer();
svr.start(option);
console.log("server started.");
