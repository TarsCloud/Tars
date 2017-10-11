var net  = require("net");
var TarsStream = require('@tars/stream');
var TARS = require("./NodeTars.js").tars;

var socket = new net.Socket();
socket.connect(14002, "127.0.0.1");

socket.on("connect", function () {
    var tup = new TarsStream.Tup();
    tup.tupVersion  = TarsStream.Tup.TUP_COMPLEX;
    tup.servantName = "TARS.NodeTarsServer.NodeTarsObj";
    tup.funcName    = "getUsrName";

    tup._iRequestId = 123456;

    tup.writeString("sUsrName", "czzou@tencent.com");
    tup.writeString("sValue1",  "czzou1");
    tup.writeString("sValue2",  "czzou2");

    var buf = tup.encode();

    socket.write(buf.toNodeBuffer());
});

socket.on("data", function(data) {
    var tup  = new TarsStream.Tup();
    tup.decode(new TarsStream.BinBuffer(new Buffer(data)));

    var iTarsResultCode = tup.getTarsResultCode();
    if (iTarsResultCode !== 0) {
        console.log("TARS ERROR CODE:",    tup.getTarsResultCode());
        console.log("TARS ERROR MESSAGE:", tup.getTarsResultDesc());
    } else {
        console.log("result.response.return:",            tup.readInt32(""));
        console.log("result.response.arguments.sValue1:", tup.readString("sValue1"));
        console.log("result.response.arguments.sValue2:", tup.readString("sValue2"));
    }
});

socket.on("close", function() {
    console.log("peer endpoint close this connection");
});

socket.on("error", function() {
    console.log("connection error");
});
