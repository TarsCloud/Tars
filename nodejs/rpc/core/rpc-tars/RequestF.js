
var TarsStream= require('@tars/stream');


var tars = tars || {};
module.exports.tars = tars;

tars.RequestPacket = function() {
    this.iVersion = 0;
    this.cPacketType = 0;
    this.iMessageType = 0;
    this.iRequestId = 0;
    this.sServantName = "";
    this.sFuncName = "";
    this.sBuffer = new TarsStream.BinBuffer();
    this.iTimeout = 0;
    this.context = new TarsStream.Map(TarsStream.String, TarsStream.String);
    this.status = new TarsStream.Map(TarsStream.String, TarsStream.String);
};
tars.RequestPacket._write = function (os, tag, value) { os.writeStruct(tag, value); }
tars.RequestPacket._read  = function (is, tag, def) { return is.readStruct(tag, true, def); }
tars.RequestPacket._readFrom = function (is) {
    var tmp = new tars.RequestPacket();
    tmp.iVersion = is.readInt16(1, true, TarsStream.Int16);
    tmp.cPacketType = is.readInt8(2, true, TarsStream.Int8);
    tmp.iMessageType = is.readInt32(3, true, TarsStream.Int32);
    tmp.iRequestId = is.readInt32(4, true, TarsStream.Int32);
    tmp.sServantName = is.readString(5, true, TarsStream.String);
    tmp.sFuncName = is.readString(6, true, TarsStream.String);
    tmp.sBuffer = is.readBytes(7, true, TarsStream.BinBuffer);
    tmp.iTimeout = is.readInt32(8, true, TarsStream.Int32);
    tmp.context = is.readMap(9, true, TarsStream.Map(TarsStream.String, TarsStream.String));
    tmp.status = is.readMap(10, true, TarsStream.Map(TarsStream.String, TarsStream.String));
    return tmp;
};
tars.RequestPacket.prototype._writeTo = function (os) {
    os.writeInt16(1, this.iVersion);
    os.writeInt8(2, this.cPacketType);
    os.writeInt32(3, this.iMessageType);
    os.writeInt32(4, this.iRequestId);
    os.writeString(5, this.sServantName);
    os.writeString(6, this.sFuncName);
    os.writeBytes(7, this.sBuffer);
    os.writeInt32(8, this.iTimeout);
    os.writeMap(9, this.context);
    os.writeMap(10, this.status);
};
tars.RequestPacket.prototype.__equal__ = function (anItem) {
    return anItem.iVersion === this.iVersion
    && anItem.cPacketType === this.cPacketType
    && anItem.iMessageType === this.iMessageType
    && anItem.iRequestId === this.iRequestId
    && anItem.sServantName === this.sServantName
    && anItem.sFuncName === this.sFuncName
    && anItem.sBuffer === this.sBuffer
    && anItem.iTimeout === this.iTimeout
    && anItem.context === this.context
    && anItem.status === this.status;
}
tars.RequestPacket.prototype.toString = function () {
    if (!this._proto_struct_name_) {
        this._proto_struct_name_ = 'STRUCT' + Math.random();
    }
    return this._proto_struct_name_;
}
tars.RequestPacket.prototype.toBinBuffer = function () {
    var os = new TarsStream.OutputStream();
    this._writeTo(os);
    return os.getBinBuffer();
}
tars.RequestPacket.create = function (is) {
    return tars.RequestPacket._readFrom(is);
}
tars.ResponsePacket = function() {
    this.iVersion = 0;
    this.cPacketType = 0;
    this.iRequestId = 0;
    this.iMessageType = 0;
    this.iRet = 0;
    this.sBuffer = new TarsStream.BinBuffer();
    this.status = new TarsStream.Map(TarsStream.String, TarsStream.String);
    this.sResultDesc = "";
    this.context = new TarsStream.Map(TarsStream.String, TarsStream.String);
};
tars.ResponsePacket._write = function (os, tag, value) { os.writeStruct(tag, value); }
tars.ResponsePacket._read  = function (is, tag, def) { return is.readStruct(tag, true, def); }
tars.ResponsePacket._readFrom = function (is) {
    var tmp = new tars.ResponsePacket();
    tmp.iVersion = is.readInt16(1, true, TarsStream.Int16);
    tmp.cPacketType = is.readInt8(2, true, TarsStream.Int8);
    tmp.iRequestId = is.readInt32(3, true, TarsStream.Int32);
    tmp.iMessageType = is.readInt32(4, true, TarsStream.Int32);
    tmp.iRet = is.readInt32(5, true, TarsStream.Int32);
    tmp.sBuffer = is.readBytes(6, true, TarsStream.BinBuffer);
    tmp.status = is.readMap(7, true, TarsStream.Map(TarsStream.String, TarsStream.String));
    tmp.sResultDesc = is.readString(8, false, TarsStream.String);
    tmp.context = is.readMap(9, false, TarsStream.Map(TarsStream.String, TarsStream.String));
    return tmp;
};
tars.ResponsePacket.prototype._writeTo = function (os) {
    os.writeInt16(1, this.iVersion);
    os.writeInt8(2, this.cPacketType);
    os.writeInt32(3, this.iRequestId);
    os.writeInt32(4, this.iMessageType);
    os.writeInt32(5, this.iRet);
    os.writeBytes(6, this.sBuffer);
    os.writeMap(7, this.status);
    os.writeString(8, this.sResultDesc);
    os.writeMap(9, this.context);
};
tars.ResponsePacket.prototype.__equal__ = function (anItem) {
    return anItem.iVersion === this.iVersion
    && anItem.cPacketType === this.cPacketType
    && anItem.iRequestId === this.iRequestId
    && anItem.iMessageType === this.iMessageType
    && anItem.iRet === this.iRet
    && anItem.sBuffer === this.sBuffer
    && anItem.status === this.status
    && anItem.sResultDesc === this.sResultDesc
    && anItem.context === this.context;
}
tars.ResponsePacket.prototype.toString = function () {
    if (!this._proto_struct_name_) {
        this._proto_struct_name_ = 'STRUCT' + Math.random();
    }
    return this._proto_struct_name_;
}
tars.ResponsePacket.prototype.toBinBuffer = function () {
    var os = new TarsStream.OutputStream();
    this._writeTo(os);
    return os.getBinBuffer();
}
tars.ResponsePacket.create = function (is) {
    return tars.ResponsePacket._readFrom(is);
}


