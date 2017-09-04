var TarsClient  = require("../../../../protal.js").client;
var TARS = require("./NodeSecondProxy.js").tars;

var prx = TarsClient.stringToProxy(TARS.NodeSecondProxy, "TARS.NodeSecondServer.NodeSecondObj@tcp -h 127.0.0.1 -p 14003 -t 10000");

var success = function (result) {
    console.log("result.response.costtime:",             result.response.costtime);
    console.log("result.response.return: ",              result.response.return);
    console.log("result.response.arguments.sUserName:",  result.response.arguments.sUserName);
}

var error = function (result) {
    console.log("result.error: ",   result.response.error.code);
    console.log("result.message: ", result.response.error.message);
}

prx.yourName("czzou").then(success, error).done();
