/**
 * 使用tars编码数据格式作为RPC协议的客户端协议解析器
 */
var EventEmitter = require("events").EventEmitter;
var util         = require("util");
var TarsStream          = require("@tars/stream");
var TarsPacket    = require("./RequestF.js").tars;

var stream = function () {
    EventEmitter.call(this);
    this._name = "tars";
    this._data = undefined;
}
util.inherits(stream, EventEmitter);

stream.prototype.__defineGetter__("name", function () { return this._name; });

module.exports = stream;

/**
 * 根据传入数据进行打包的方法
 * @param request
 * request.iRequestId : 框架生成的请求序列号
 * request.sFuncName  : 函数名称
 * request.sBuffer    : 函数传入的打包数据
 */
stream.prototype.compose = function ($protoMessage) {
    var option = {};
    var packetType = 0;
    if($protoMessage && $protoMessage.property)
    {
        option = $protoMessage.property;
        if(option && option.packetType && option.packetType === 1)
            packetType = 1;
    }

    var req = new TarsPacket.RequestPacket();
    req.iVersion        = 1;
    req.cPacketType     = packetType;
    req.iMessageType    = 0;
    req.iRequestId      = $protoMessage.iRequestId;
    req.sServantName    = $protoMessage.sServantName;
    req.sFuncName       = $protoMessage.sFuncName;
    req.sBuffer         = $protoMessage.appBuffer;

    //设置染色以及context等属性
    if ($protoMessage.property && $protoMessage.property.dyeing && $protoMessage.property.dyeing.dyeing) {
        req.iMessageType = req.iMessageType | 0x04;
        req.status.insert("STATUS_DYED_KEY", ($protoMessage.property.dyeing.key || '').toString());
    }
    if ($protoMessage.property && $protoMessage.property.context) {
        for (var key in $protoMessage.property.context) {
            req.context.insert(key, ($protoMessage.property.context[key] || '').toString());
        }
    }

    //对请求结构体打包
    var os = new TarsStream.OutputStream();
    os.setHeaderLength(0);
    req._writeTo(os);
    os.setHeaderLength(os.getBinBuffer().length);

    return os.getBinBuffer().toNodeBuffer();
}

/**
 * 网络收取包之后，填入数据判断是否完整包
 */
var ProtoMessageResponse = function () {
    this.origin       = undefined;   //原始的请求结构体,从协议中解出来的第一层结构体
    this.iRequestId   = 0;
    this.iResultCode  = 0;           //tars框架的错误代码
    this.sResultDesc  = "";          //tars框架的错误消息
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

        var is      = new TarsStream.InputStream(new TarsStream.BinBuffer(BinBuffer.slice(pos + 4, pos + Length)));
        var message = new ProtoMessageResponse();
        message.origin      = TarsPacket.ResponsePacket._readFrom(is);
        message.iRequestId  = message.origin.iRequestId;
        message.iResultCode = message.origin.iRet;
        message.sResultDesc = message.origin.sResultDesc;

        this.emit("message", message);
        pos += Length;
    }

    if (pos != BinBuffer.length) {
        this._data = new Buffer(BinBuffer.length - pos);
        BinBuffer.copy(this._data, 0, pos);
    }
}

stream.prototype.reset = function () {
    delete this._data;
    this._data = undefined;
}
