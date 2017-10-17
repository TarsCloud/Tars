var Tars=require("../../../protal.js");
var TarsServer      = Tars.server;
var Protocol = Tars.rpcJson.server;
var Handle      = require("./EchoHandle.js").EchoHandle;


var option = new TarsServer.BindAdapterOption();
option.endpoint     = "tcp -h 127.0.0.1 -p 12306 -t 10000";
option.protocolName = "json";
option.protocol     = Protocol;
option.handleImp    = Handle;

var svr = TarsServer.createServer();
svr.start(option);
console.log("server started.");
