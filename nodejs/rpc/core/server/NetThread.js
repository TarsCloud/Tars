var net   = require("net");
var dgram = require("dgram");
var Endpoint = require("@tars/utils").Endpoint;
var TCPTransceiver = require("./Transceiver.js").TCPTransceiver;
var UDPTransceiver = require("./Transceiver.js").UDPTransceiver;

///////////////////////////////////////////////监听端口上的连接管理类///////////////////////////////////////////////////
var NetThread = function ($adapter, $endpoint, $stream) {
    this._adapter     = $adapter;
    this._endpoint    = $endpoint;
    this._stream      = $stream;
    this._uid         = 10000;
    this._listener    = undefined;
    this._connections = {};
};
module.exports.NetThread = NetThread;

//关闭所有的连接
NetThread.prototype.stop = function () {
    for (var key in this._connections) {
        this._connections[key].close();

        delete this._connections[key];
    }

    this._listener.close();
};

//从队列中删除某个连接
NetThread.prototype.remove = function ($uid) {
    delete this._connections[$uid];
};

NetThread.prototype.initialize = function () {
    var self = this;

    if (self._endpoint.sProtocol === "tcp") { //TCP的服务
        self._listener = net.createServer();
        self._listener.listen(self._endpoint.iPort, self._endpoint.sHost);
        self._listener.on("connection", function (socket) {
            var conn = new TCPTransceiver();
            conn.socket    = socket;
            conn.adapter   = self._adapter;
            conn.netthread = self;
            conn.uid       = self._uid++;
            conn.stream    = new self._stream();

            conn.setTimeout(self._endpoint.iTimeout);
            conn.initialize();

            self._connections[conn.uid] = conn;
        });
    } else { //UDP的服务
        self._listener = dgram.createSocket("udp4");
        self._listener.bind(self._endpoint.iPort, self._endpoint.sHost);
        self._listener.on("message", function (msg, rinfo){
            var connection = new UDPTransceiver();
            connection.socket   = self._listener;
            connection.adapter  = self._adapter;
            connection.stream   = new self._stream();
            connection.endpoint = new Endpoint(rinfo.address, rinfo.port);

            connection.doRequest(msg);
        });
    }
};
