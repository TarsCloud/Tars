/**
 * Created by czzou on 2017/2/28.
 */
var http = require('http');
var net  = require("net");

//依次启动：server.node.1=>server.node.tup.tupproxyserver=>client.node.tup.tupproxyserver
var server = http.createServer((req, res) => {
    "use strict";
    var bufferArr=[],totalLength=0;
    req.on('data',(buffer)=>{
        bufferArr.push(buffer);
        totalLength+=buffer.length;
    });
    req.on('end',()=>{
        var data = Buffer.concat(bufferArr, totalLength);
        console.log(' proxy work done ');
        tupRequest(data,(result)=>{
            res.write(result);
        });
    });
});

server.on('clientError', (err, socket) => {
    socket.end('HTTP/1.1 400 Bad Request\r\n\r\n');
});
server.listen(14001,()=>{
    "use strict";
    console.log(' tup proxy http server is listening on port',server.address().port);
});

function tupRequest(data,callback){
    "use strict";
    var socket = new net.Socket();
    socket.connect(14002, "127.0.0.1");

    socket.on("connect", function () {
        socket.write(data);
    });

    socket.on("data", function(data) {
        callback(data);
    });

    socket.on("close", function() {
        console.log("peer endpoint close this connection");
    });

    socket.on("error", function() {
        console.log("connection error");
    });
}

