/**
 * 客户端网络传输类
 */
var net      = require("net");
var dgram    = require("dgram");
var Endpoint = require("@tars/utils").Endpoint;

var Transceiver = function () {

};
Transceiver.ES_UNCONNECTED = 1;  //没有连接
Transceiver.ES_CONNECTING  = 2;  //正在连接中
Transceiver.ES_CONNECTED   = 3;  //已经连接上

/////////////////////////////////////////////TCP协议套接口类////////////////////////////////////////////////////////////
var TCPTransceiver = function ($adapter, $endpoint) {
    this._adapter       = $adapter;
    this._endpoint      = $endpoint;

    this._stream        = undefined; //协议解析器的实例
    this._socket        = undefined;
    this._status        = Transceiver.ES_UNCONNECTED;
    this._localEndpoint = undefined;
    this._socket_evt    = undefined;
};
module.exports.TCPTransceiver = TCPTransceiver;

//定义TCP接口的属性
//当socket存在，并且处于已连接状态时，才去获取本地端口
TCPTransceiver.prototype.__defineGetter__("LocalEndpoint", function () {
    if (this._localEndpoint === undefined && this._socket && this._status == Transceiver.ES_CONNECTED) {
        this._localEndpoint = new Endpoint();
        this._localEndpoint.sProtocol = "tcp";
        this._localEndpoint.sHost     = this._socket.localAddress;
        this._localEndpoint.iPort     = this._socket.localPort;
    }

    return this._localEndpoint;
});

//重新连接
TCPTransceiver.prototype.reconnect = function () {
    var self = this;

    self._stream = new self._adapter._worker._protocol();
    self._socket = new net.Socket();
    self._status = Transceiver.ES_CONNECTING;
    self._socket.connect(self._endpoint.iPort, self._endpoint.sHost);

    self._socket_evt = {
        connect : function()     { self._status = Transceiver.ES_CONNECTED; self._adapter.doInvoke();  },
        close   : function()     { self.close(); },
        error   : function()     { self.close(); },
        data    : function(data) { self._stream.feed(data); }
    };

    self._socket.on("connect", self._socket_evt.connect);
    self._socket.on("data",    self._socket_evt.data);
    self._socket.on("error",   self._socket_evt.error);
    self._socket.on("close",   self._socket_evt.close);
    self._stream.on("message", function($protoMessage) { self._adapter.doResponse($protoMessage); });
};

//发送未经编码的请求
TCPTransceiver.prototype.sendRequest = function ($protoMessage) {
    if($protoMessage && this._endpoint && this._endpoint.setId)
    {
        $protoMessage.setId = this._endpoint.setId;
    }

    if (this._status === Transceiver.ES_CONNECTED) {
        this._socket.write(this._stream.compose($protoMessage));
        return true;
    }

    return false;
};

TCPTransceiver.prototype.hasConnected = function () { return this._status === Transceiver.ES_CONNECTED;  };
TCPTransceiver.prototype.isConnecting = function () { return this._status === Transceiver.ES_CONNECTING; };
TCPTransceiver.prototype.isValid      = function () { return this._status !== Transceiver.ES_UNCONNECTED;};

//关闭当前网络套接字
TCPTransceiver.prototype.close = function () {
    this._status = Transceiver.ES_UNCONNECTED;
    this._stream = undefined;

    if (this._socket) {
        this._socket.removeListener("connect", this._socket_evt.connect);
        this._socket.removeListener("error", this._socket_evt.error);
        this._socket.removeListener("close", this._socket_evt.close);
        this._socket.removeListener("data", this._socket_evt.data);
        this._socket.destroy();
        this._socket = undefined;
    }

    //原来的地址已经失效，现在清空，下次调用接口时重新获取
    this._localEndpoint = undefined;
};

/////////////////////////////////////////////UDP协议套接口类////////////////////////////////////////////////////////////
var UDPTransceiver = function ($adapter, $endpoint) {
    this._adapter       = $adapter;
    this._endpoint      = $endpoint;
    this._stream        = undefined;
    this._socket        = undefined;
    this._status        = Transceiver.ES_UNCONNECTED;
    this._localEndpoint = undefined;
};
module.exports.UDPTransceiver = UDPTransceiver;

UDPTransceiver.prototype.__defineGetter__("LocalEndpoint", function () {
    if (this._localEndpoint === undefined && this._socket && this._status == Transceiver.ES_CONNECTED) {
        this._localEndpoint = new Endpoint();
        this._localEndpoint.sProtocol = "udp";
        this._localEndpoint.sHost     = this._socket.address().address;
        this._localEndpoint.iPort     = this._socket.address().port;
    }

    return this._localEndpoint;
});

//启动传输
UDPTransceiver.prototype.reconnect = function () {
    var self = this;

    self._stream = new self._adapter._worker._protocol();
    self._socket = dgram.createSocket("udp4");
    self._socket.on("message", function ($msg) { self._stream.reset(); self._stream.feed($msg);    });
    self._stream.on("message", function ($protoMessage) { self._adapter.doResponse($protoMessage); });
    self._status = Transceiver.ES_CONNECTED;
};

UDPTransceiver.prototype.sendRequest = function ($protoMessage) {
    var NodeBuffer = this._stream.compose($protoMessage);

    this._socket.send(NodeBuffer, 0, NodeBuffer.length, this._endpoint.iPort, this._endpoint.sHost);

    return true;
};

UDPTransceiver.prototype.hasConnected = function () { return this._status === Transceiver.ES_CONNECTED;  };
UDPTransceiver.prototype.isConnecting = function () { return this._status === Transceiver.ES_CONNECTING; };
UDPTransceiver.prototype.isValid      = function () { return this._status !== Transceiver.ES_UNCONNECTED;};

//关闭当前网络套接字
UDPTransceiver.prototype.close = function () {
    this._status = Transceiver.ES_UNCONNECTED;
    this._stream = undefined;

    this._socket.close();

    //原来的地址已经失效，现在清空，下次调用接口时重新获取
    this._localEndpoint = undefined;
};
