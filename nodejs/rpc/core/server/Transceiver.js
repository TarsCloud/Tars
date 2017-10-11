/**
 * 服务端网络传输层代理类
 */
///////////////////////////////////////////////TCP网络传输类////////////////////////////////////////////////////////////
var TCPTransceiver = function () {
    this.adapter     = undefined;
    this.socket      = undefined;
    this.stream      = undefined;
    this.netthread   = undefined;
    this.uid         = 0;
    this._socket_evt = undefined;
    this._iTimeout   = 60000;
};
module.exports.TCPTransceiver = TCPTransceiver;

TCPTransceiver.prototype.initialize = function () {
    var self = this;

    self._socket_evt = {
        close   : function()        { self.close(); },
        error   : function()        { self.close(); },
        timeout : function ()       { self.close(); },
        data    : function(data)    { self.stream.feed(data); },
        message : function(request) { self.adapter.dispatch(self, request); }
    };

    self.socket.on("data",    self._socket_evt.data);
    self.socket.on("error",   self._socket_evt.error);
    self.socket.on("close",   self._socket_evt.close);
    self.socket.on("timeout", self._socket_evt.timeout);
    self.stream.on("message", self._socket_evt.message);

    self.socket.setTimeout(this._iTimeout);
};

TCPTransceiver.prototype.close = function () { //主动或者发生错误之后，关闭本连接
    if (this.socket) {
        try
        {
            this.netthread.remove(this.uid);

            this.socket.removeListener("error",     this._socket_evt.error);
            this.socket.removeListener("close",     this._socket_evt.close);
            this.socket.removeListener("data",      this._socket_evt.data);
            this.socket.removeListener("timeout",   this._socket_evt.timeout);
            this.stream.removeListener("message",   this._socket_evt.message);
            this.socket.destroy();

            this.socket = undefined;
        }catch(e){
            console.error('[TCPTransceiver.close] ' + e.message);
        }
    }
};

TCPTransceiver.prototype.send = function ($protoMessage) {
    if(this.socket)
        this.socket.write(this.stream.compose($protoMessage));
};

TCPTransceiver.prototype.setTimeout = function ($iTimeout) { //设置服务端连接空闲时间
    this._iTimeout = $iTimeout;
};

///////////////////////////////////////////////UDP网络传输类////////////////////////////////////////////////////////////
var UDPTransceiver = function () {
    this.adapter   = undefined;
    this.socket    = undefined;
    this.stream    = undefined;
    this.endpoint  = undefined;

    this._iTimeout = 60000;
};
module.exports.UDPTransceiver = UDPTransceiver;

UDPTransceiver.prototype.doRequest = function ($msg) {
    var self = this;

    self.stream.reset();
    self.stream.on("message", function (request) { self.adapter.dispatch(self, request); });
    self.stream.feed($msg);
};

UDPTransceiver.prototype.send = function ($protoMessage) {
    var NodeBuffer = this.stream.compose($protoMessage);

    this.socket.send(NodeBuffer, 0, NodeBuffer.length, this.endpoint.iPort, this.endpoint.sHost);
};

UDPTransceiver.prototype.setTimeout = function ($iTimeout) { //设置服务端连接空闲时间
    this._iTimeout = $iTimeout;
};

UDPTransceiver.prototype.close = function () {

};
