/**
 * 使用JSON数据格式作为RPC协议的协议解析器
 * kevintian@tencent.com
 * 2014-11-18
 */
var EventEmitter = require("events").EventEmitter;
var util         = require("util");

var stream = function () {
    EventEmitter.call(this);
    this._data = undefined;
    this._name = "rpc-json";
}
util.inherits(stream, EventEmitter);

stream.prototype.__defineGetter__("name", function () { return this._name; });

module.exports = stream;

/**
 * 根据传入数据进行打包的方法
 * @param request
 * request.iRequestId : 框架生成的请求序列号
 * request.sFuncName  : 函数名称
 * request.Arguments  : 函数的参数列表
 */
stream.prototype.compose = function ($protoMessage) {
    var str = JSON.stringify({
        iResultCode : $protoMessage.iResultCode,
        sResultDesc : $protoMessage.sResultDesc,
        iRequestId  : $protoMessage.origin.iRequestId,
        sFuncName   : $protoMessage.origin.sFuncName,
        Arguments   : $protoMessage.appBuffer
    });

    var len = 4 + Buffer.byteLength(str);
    var buf = new Buffer(len);
    buf.writeUInt32BE(len, 0);
    buf.write(str, 4);

    return buf;
}

/**
 * 网络收取包之后，填入数据判断是否完整包
 * @param data
 */
var ProtoMessageRequest = function () {
    this.origin       = undefined;
    this.sFuncName    = undefined;
    this.iResultCode  =  0;
    this.sResultDesc  = "";
}

stream.prototype.feed = function (data) {
    var BinBuffer = data;
    if (this._data != undefined) {
        var temp = new Buffer(this._data.length + data.length);
        this._data.copy(temp, 0);
        data.copy(temp, this._data.length);
        this._data = undefined;
        BinBuffer = temp;
    }

    for (var pos = 0; pos < BinBuffer.length; ) {
        if (BinBuffer.length - pos < 4) {
            break;
        }
        var Length = BinBuffer.readUInt32BE(pos);
        if (pos + Length > BinBuffer.length) {
            break;
        }

        var result  = JSON.parse(BinBuffer.slice(pos + 4, pos + Length).toString());
        var request = new ProtoMessageRequest();
        request.iResultCode = 0;
        request.sResultDesc = "";
        request.sFuncName   = result.sFuncName;
        request.origin      = {
            sFuncName  : result.sFuncName,
            iRequestId : result.iRequestId,
            Arguments  : result.Arguments
        };

        this.emit("message", request);
        pos += Length;
    }

    if (pos != BinBuffer.length) {
        this._data = new Buffer(BinBuffer.length - pos);
        BinBuffer.copy(this._data, 0, pos);
    }
}

/**
 * 重置当前协议解析器
 */
stream.prototype.reset = function () {
    delete this._data;
    this._data = undefined;
}
