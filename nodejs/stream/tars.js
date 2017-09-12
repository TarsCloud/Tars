/**
 * KevinTian@tencent.com
 *
 * NodeJs版本编解码
 */
var assert = require("assert");
var util   = require("util");
var Tars    = module.exports = {};

/**
 * 异常类
 */
Tars.Exception = function ($code, $message) {
    this.code    = $code;
    this.message = $message;
}
util.inherits(Tars.Exception, Error);

Tars.EncodeException = function ($message) {
    this.code    = -2;
    this.message = $message;
}
util.inherits(Tars.EncodeException, Tars.Exception);

Tars.DecodeException = function ($message) {
    this.code    = -1;
    this.message = $message;
}
util.inherits(Tars.DecodeException, Tars.Exception);

Tars.DecodeMismatch = function ($message) {
    this.code    = -1;
    this.message = $message;
}
util.inherits(Tars.DecodeMismatch, Tars.DecodeException);

Tars.DecodeRequireNotExist = function ($message) {
    this.code    = -1;
    this.message = $message;
}
util.inherits(Tars.DecodeRequireNotExist, Tars.DecodeException);

Tars.DecodeInvalidValue = function ($message) {
    this.code    = -1;
    this.message = $message;
}
util.inherits(Tars.DecodeInvalidValue, Tars.DecodeException);

Tars.TupNotFoundKey = function ($message) {
    this.code    = -1;
    this.message = $message;
}
util.inherits(Tars.TupNotFoundKey, Tars.Exception);

/**
 * 编解码底层辅助类
 */
Tars.DataHelp = {
    EN_INT8         : 0,
    EN_INT16        : 1,
    EN_INT32        : 2,
    EN_INT64        : 3,
    EN_FLOAT        : 4,
    EN_DOUBLE       : 5,
    EN_STRING1      : 6,
    EN_STRING4      : 7,
    EN_MAP          : 8,
    EN_LIST         : 9,
    EN_STRUCTBEGIN  : 10,
    EN_STRUCTEND    : 11,
    EN_ZERO         : 12,
    EN_SIMPLELIST   : 13
};

Tars.Boolean = {
    _write      : function(os, tag, val) { return os.writeBoolean(tag, val);      },
    _read       : function(is, tag, def) { return is.readBoolean(tag, true, def); },
    _classname  : "bool"
};

Tars.Int8 = {
    _write      : function(os, tag, val) { return os.writeInt8(tag, val); },
    _read       : function(is, tag, def) { return is.readInt8(tag, true, def); },
    _classname  : "char"
};

Tars.Int16 = {
    _write      : function(os, tag, val) { return os.writeInt16(tag, val); },
    _read       : function(is, tag, def) { return is.readInt16(tag, true, def); },
    _classname  : "short"
};

Tars.Int32 = {
    _write      : function(os, tag, val) { return os.writeInt32(tag, val); },
    _read       : function(is, tag, def) { return is.readInt32(tag, true, def); },
    _classname  : "int32"
};

Tars.Int64 = {
    _write      : function(os, tag, val) { return os.writeInt64(tag, val); },
    _read       : function(is, tag, def) { return is.readInt64(tag, true, def); },
    _classname  : "int64"
};

Tars.UInt8 = {
    _write      : function(os, tag, val) { return os.writeInt16(tag, val); },
    _read       : function(is, tag, def) { return is.readInt16(tag, true, def); },
    _classname  : "short"
};

Tars.UInt16 = {
    _write      : function(os, tag, val) { return os.writeInt32(tag, val); },
    _read       : function(is, tag, def) { return is.readInt32(tag, true, def); },
    _classname  : "int32"
};

Tars.UInt32 = {
    _write      : function(os, tag, val) { return os.writeInt64(tag, val);      },
    _read       : function(is, tag, def) { return is.readInt64(tag, true, def); },
    _classname  : "int64"
};

Tars.Float = {
    _write      : function(os, tag, val) { return os.writeFloat(tag, val);      },
    _read       : function(is, tag, def) { return is.readFloat(tag, true, def); },
    _classname  : "float"
};

Tars.Double = {
    _write      : function(os, tag, val) { return os.writeDouble(tag, val);      },
    _read       : function(is, tag, def) { return is.readDouble(tag, true, def); },
    _classname  : "double"
};

Tars.String = {
    _write      : function(os, tag, val, bRaw) { return os.writeString(tag, val, bRaw);      },
    _read       : function(is, tag, def, bRaw) { return is.readString(tag, true, def, bRaw); },
    _classname  : "string"
};

Tars.Enum    = {
    _write      : function(os, tag, val) { return os.writeInt32(tag, val);        },
    _read       : function(is, tag, def) { return is.readInt32(tag, true, def);   },
    _classname  : "int32"
};

/**
 * Tars-List实现类
 */
var HeroList = function (proto) {
    this._proto     = proto;
    this.value      = new Array();
    this._classname = "List<" + proto._classname + ">";
};

HeroList.prototype._write  = function(os, tag, val) { return os.writeList(tag, val); };
HeroList.prototype._read   = function(is, tag, def) { return is.readList(tag, true, def); };
HeroList.prototype.new     = function() { return new HeroList(this._proto); };
HeroList.prototype.at      = function(index) { return this.value[index]; };
HeroList.prototype.push    = function(value) { this.value.push(value); };
HeroList.prototype.forEach = function(callback) {
    for (var i = 0; i < this.value.length; i++) {
        if (callback.call(null, this.value[i], i, this.value) == false) break;
    }
};
HeroList.prototype.toObject = function () {
    var tmp = [];
    for (var i = 0, len = this.value.length; i < len; i++){
        tmp.push(!this.value[i].toObject?this.value[i]:this.value[i].toObject());
    }

    return tmp;
};

HeroList.prototype.readFromObject = function (json) {
    var vsimple = !this._proto.create && !this._proto._vproto && !this._proto._proto && !this._proto.new;

    for (var i = 0, len = json.length; i < len; i++) {
        if (vsimple) {
            this.push(json[i]);
        } else {
            var temp = this._proto.new();
            temp.readFromObject(json[i]);
            this.push(temp);
        }
    }

    return this;
}

HeroList.prototype.__defineGetter__("length", function () { return this.value.length; });

Tars.List = function(proto) {
    return new HeroList(proto);
};

/**
 * Tars-Map实现类
 */
var MultiMap = function(kproto, vproto) {
    this._kproto    = kproto;
    this._vproto    = vproto;
    this.keys       = new Object();
    this.value      = new Object();
    this._classname = "map<" + kproto._classname + "," + vproto._classname + ">";
};

MultiMap.prototype._write  = function(os, tag, val)   { return os.writeMap(tag, val);           };
MultiMap.prototype._read   = function(is, tag, def)   { return is.readMap(tag, true, def);      };
MultiMap.prototype.put     = function(key, value)     { this.insert(key, value);                };
MultiMap.prototype.set     = function(key, value)     { this.insert(key, value);                };
MultiMap.prototype.remove  = function(key)            { delete this.keys[key._genKey()]; delete this.value[key._genKey()]; };
MultiMap.prototype.size    = function()               { return Object.keys(this.keys || {}).length;};
MultiMap.prototype.has     = function(key)            { return this.keys.hasOwnProperty(key._genKey()); };
MultiMap.prototype.insert  = function(key, value)     {
    var keys = Object.keys(this.keys || {});
    for(var i = 0; i < keys.length; i++)
    {
        var anItem = keys[i];
        if (key._equal(this.keys[anItem])) {
            this.value[anItem] = value;
            return ;
        }
    }

    this.keys[key._genKey()]  = key;
    this.value[key._genKey()] = value;
};

MultiMap.prototype.get = function(key) {
    var keys = Object.keys(this.keys || {});
    for(var i = 0; i < keys.length; i++)
    {
        var anItem = keys[i];
        if (key._equal(this.keys[anItem])) {
            return this.value[anItem];
        }
    }
    return undefined;
};

MultiMap.prototype.clear   = function() {
    delete this.keys;
    delete this.value;
    this.keys  = new Object();
    this.value = new Object();
}

MultiMap.prototype.forEach = function(callback) {
    var keys = Object.keys(this.value || {});
    for(var i = 0; i < keys.length; i++)
    {
        var key = keys[i];
        if (callback.call(null, this.keys[key], this.value[key]) == false)
            break;
    }
};

MultiMap.prototype.toObject = function () {
    assert(false, "multimap has no toObject function");
}

MultiMap.prototype.readFromObject = function () {
    assert(false, "multimap has no toObject readFromObject");
}

var HeroMap = function (kproto, vproto) {
    this._kproto    = kproto;
    this._vproto    = vproto;
    this.value      = new Object();
    this._classname = "map<" + kproto._classname + "," + vproto._classname + ">";
}

HeroMap.prototype._write  = function(os, tag, val)   { return os.writeMap(tag, val);                       }
HeroMap.prototype._read   = function(is, tag, def)   { return is.readMap(tag, true, def);                  }
HeroMap.prototype.new     = function()               { return new HeroMap(this._kproto, this._vproto);     }
HeroMap.prototype.put     = function(key, value)     { this.insert(key, value);                            }
HeroMap.prototype.set     = function(key, value)     { this.insert(key, value);                            }
HeroMap.prototype.remove  = function(key)            { delete this.value[key];                             }
HeroMap.prototype.size    = function()               { return Object.keys(this.value || {}).length;        }
HeroMap.prototype.has     = function(key)            { return this.value.hasOwnProperty(key);              }
HeroMap.prototype.insert  = function(key, value)     { this.value[key] = value;                            }
HeroMap.prototype.get     = function(key)            { return this.value[key];                             }
HeroMap.prototype.clear   = function()               { delete this.value; this.value = new Object();       }
HeroMap.prototype.forEach = function(callback)       {
    var keys = Object.keys(this.value || {});
    for(var i = 0; i < keys.length; i++)
    {
        var key = keys[i];
        switch (this._kproto){
            case Tars.Int8:
            case Tars.Int16:
            case Tars.Int32:
            case Tars.Int64:
            case Tars.UInt8:
            case Tars.UInt16:
            case Tars.UInt32:
            case Tars.Float:
            case Tars.Double:
                key=Number(key);
                break;
        }
        if (callback.call(null, key, this.value[key]) == false)
            break;
    }
};

HeroMap.prototype.toObject = function () {
    var tmp = {};
    var keys = Object.keys(this.value || {});
    for(var i = 0; i < keys.length; i++)
    {
        var key = keys[i];
        tmp[key] = !this.value[key].toObject?this.value[key]:this.value[key].toObject();
    }

    return tmp;
};

HeroMap.prototype.readFromObject = function (json) {
    var vsimple = !this._vproto.create && !this._vproto._vproto && !this._vproto._proto;

    var keys = Object.keys(json || {});
    for(var i = 0; i < keys.length; i++)
    {
        var key = keys[i];
        if (vsimple) {
            this.insert(key, json[key]);
        } else {
            var temp = this._vproto.new();
            temp.readFromObject(json[key]);
            this.insert(key, temp);
        }
    }
};

Tars.Map = function(kproto, vproto) {
    if (kproto.prototype && kproto.prototype._equal) {
        return new MultiMap(kproto, vproto);
    }

    return new HeroMap(kproto, vproto);
}

/**
 * 适用于NodeJS支持的Buffer的实现类
 */
//升级Buffer内存分配方式，node 6.x支持Buffer.allocUnsafe(size)
Tars.createNodeBuffer=function(){};
Tars.initializeBufferCreator=function(){
    var nodeversion=process.version.match(/^v(\d)/)[1];
    //6.x+版本
    if(nodeversion>=6){
        this.createNodeBuffer=function(data){
            if(util.isNumber(data)){
                return Buffer.allocUnsafe(data);
            }
            if(Array.isArray(data) || Buffer.isBuffer(data) || typeof data==="string" || data instanceof ArrayBuffer){
                return Buffer.from(data);
            }
            throw new Error("can not create buffer by "+typeof data+" in nodejs v6+");
        }
    }
    //6.x以下版本
    else{
        this.createNodeBuffer=function(data){
            return new Buffer(data);
        }
    }
}
Tars.initializeBufferCreator();

Tars.BinBuffer = function (buffer) {
    if(!buffer)
    {
        buffer = Tars.createNodeBuffer([]);
    }
    this._buffer    = (buffer != undefined && buffer instanceof Buffer)?buffer:null;
    this._length    = (buffer != undefined && buffer instanceof Buffer)?buffer.length:0;
    this._capacity  = this._length;
    this._position  = 0;
}
Tars.BinBuffer._classname = "list<char>";

Tars.BinBuffer.prototype.__defineGetter__("length",   function () { return this._length;   });
Tars.BinBuffer.prototype.__defineGetter__("capacity", function () { return this._capacity; });
Tars.BinBuffer.prototype.__defineGetter__("position", function () { return this._position; });
Tars.BinBuffer.prototype.__defineSetter__("position", function (position) { this._position = position; });

Tars.BinBuffer._write = function (os, tag, val) {
    return os.writeBytes(tag, val);
}

Tars.BinBuffer._read = function (os, tag, def) {
    return os.readBytes(tag, true, def);
}

Tars.BinBuffer.new = function () {
    return new Tars.BinBuffer();
}

Tars.BinBuffer.prototype.reset = function () {
    this._length    = 0;
    this._position  = 0;
}

Tars.BinBuffer.prototype._allocate = function (byteLength) {
    if (this._capacity > this._position + byteLength) {
        return ;
    }

    this._capacity = Math.max(512, (this._position + byteLength) * 2);
    var temp = Tars.createNodeBuffer(this._capacity);
    if (this._buffer != null) {
        this._buffer.copy(temp, 0, 0, this._position);
        this._buffer = undefined;
    }

    this._buffer = temp;
}

Tars.BinBuffer.prototype.replace = function (srcBuffer, offset, byteLength) {
    srcBuffer.copy(this._buffer, 0, offset, offset + byteLength);
}

Tars.BinBuffer.prototype.writeInt8 = function (value) {
    value = +value;
    this._allocate(1);
    this._buffer.writeInt8(value, this._position);
    this._position += 1;
    this._length = this._position;
}

Tars.BinBuffer.prototype.writeUInt8 = function (value) {
    value = +value;
    this._allocate(1);
    this._buffer.writeUInt8(value, this._position);
    this._position += 1;
    this._length = this._position;
}

Tars.BinBuffer.prototype.writeInt16 = function (value) {
    value = +value;
    this._allocate(2);
    this._buffer.writeInt16BE(value, this._position);
    this._position += 2;
    this._length = this._position;
}

Tars.BinBuffer.prototype.writeUInt16 = function (value) {
    value = +value;
    this._allocate(2);
    this._buffer.writeUInt16BE(value, this._position);
    this._position += 2;
    this._length = this._position;
}

Tars.BinBuffer.prototype.writeInt32 = function (value) {
    value = +value;
    this._allocate(4);
    this._buffer.writeInt32BE(value, this._position);
    this._position += 4;
    this._length = this._position;
}

Tars.BinBuffer.prototype.writeUInt32 = function (value) {
    value = +value;
    this._allocate(4);
    this._buffer.writeUInt32BE(value, this._position);
    this._position += 4;
    this._length = this._position;
}

Tars.BinBuffer.prototype.writeInt64  = function (value) {
    value = +value;
    if (value > 0){
        var H4 = Math.floor(value/4294967296);
        var L4 = value - H4 * 4294967296;
    }
    else {
        var H4 = Math.floor(value/4294967296);
        var L4 = value - H4 * 4294967296;
        H4 += 4294967296;
    }

    this._allocate(8);
    this._buffer.writeUInt32BE(H4, this._position);
    this._buffer.writeUInt32BE(L4, this._position + 4);
    this._position += 8;
    this._length = this._position;
}

Tars.BinBuffer.prototype.writeFloat = function (value) {
    this._allocate(4);
    this._buffer.writeFloatBE(value, this._position);
    this._position += 4;
    this._length = this._position;
}

Tars.BinBuffer.prototype.writeDouble = function (value) {
    this._allocate(8);
    this._buffer.writeDoubleBE(value, this._position);
    this._position += 8;
    this._length = this._position;
}

Tars.BinBuffer.prototype.writeString = function (value, ByteLength, bRaw) {
    if (bRaw != undefined && bRaw == true) {
        this._allocate(ByteLength);
        value.copy(this._buffer, this._position);
        this._position += ByteLength;
        this._length = this._position;
        return ;
    }

    var byteLength = ByteLength || Buffer.byteLength(value);
    this._allocate(byteLength);

    this._buffer.write(value, this._position, byteLength, "utf8");


    this._position += byteLength;
    this._length = this._position;
}

Tars.BinBuffer.prototype.writeBinBuffer = function (srcBinBuffer) {
    if (srcBinBuffer._length == 0 || srcBinBuffer._buffer == null) return ;

    this._allocate(srcBinBuffer.length);
    srcBinBuffer._buffer.copy(this._buffer, this._position, 0, srcBinBuffer._length);    
    this._position += srcBinBuffer._length;
    this._length = this._position;
}

Tars.BinBuffer.prototype.writeNodeBuffer = function (srcBuffer, offset, byteLength) {
    offset      = (offset == undefined)?0:offset;
    byteLength  = (byteLength == undefined)?srcBuffer.length:byteLength;

    this._allocate(byteLength);
    srcBuffer.copy(this._buffer, this._position, offset, offset + byteLength);
    this._length   += byteLength;
    this._position += byteLength;
}

Tars.BinBuffer.prototype.readInt8 = function () {
    return this._buffer.readInt8(this._position++);
}

Tars.BinBuffer.prototype.readInt16 = function () {
    this._position += 2;
    return this._buffer.readInt16BE(this._position - 2);
}

Tars.BinBuffer.prototype.readInt32 = function () {
    this._position += 4;
    return this._buffer.readInt32BE(this._position - 4);
}

Tars.BinBuffer.prototype.readUInt8 = function () {
    this._position += 1;
    return this._buffer.readUInt8(this._position - 1);
}

Tars.BinBuffer.prototype.readUInt16 = function () {
    this._position += 2;
    return this._buffer.readUInt16BE(this._position - 2);
}

Tars.BinBuffer.prototype.readUInt32 = function () {
    this._position += 4;
    return this._buffer.readUInt32BE(this._position - 4);
}

Tars.BinBuffer.prototype.readInt64 = function() {
    var H4 = this._buffer.readUInt32BE(this._position);
    var L4 = this._buffer.readUInt32BE(this._position + 4); 
    this._position += 8;

    if (H4 < 2147483648) {
        return H4 * 4294967296 + L4;
    } else {
        return -((4294967296 - L4) + 4294967296 * (4294967296 - 1 - H4));
    }
}

Tars.BinBuffer.prototype.readFloat = function() {
    this._position += 4;
    return this._buffer.readFloatBE(this._position - 4);
}

Tars.BinBuffer.prototype.readDouble = function() {
    this._position += 8;
    return this._buffer.readDoubleBE(this._position - 8);
}

Tars.BinBuffer.prototype.readString = function (byteLength, bRaw) {

    var temp = Tars.createNodeBuffer(byteLength);
    var ret;
    if (bRaw != undefined && bRaw == true) {
        this._buffer.copy(temp, 0, this._position, this._position + byteLength);
        this._position += byteLength;
        return temp;
    }

    this._buffer.copy(temp, 0, this._position, this._position + byteLength);
    this._position += byteLength;
    if(temp.length == 1)
    {
        if(temp[0] & 0x80)
        {
            ret = temp.toString("binary", 0, temp.length);
        }
        else
        {
            ret = temp.toString("utf8", 0, temp.length);
        }
    }
    else
    {
        ret = temp.toString("utf8", 0, temp.length);
    }

    return ret;
}

Tars.BinBuffer.prototype.readBinBuffer = function (byteLength, bReuse) {
    if (bReuse === true) {
        var temp = new Tars.BinBuffer();
        temp._buffer    = this._buffer.slice(this._position, this._position + byteLength);
        temp._length    = byteLength;
        temp._capacity  = byteLength;
        temp._position  = 0;

        return temp;
    }

    var temp = new Tars.BinBuffer();
    temp.writeNodeBuffer(this._buffer, this._position, byteLength);
    this._position += byteLength;
    return temp;
}

Tars.BinBuffer.prototype.toNodeBuffer = function () {
    var temp = Tars.createNodeBuffer(this._length);
    this._buffer.copy(temp, 0, 0, this._length);
    return temp; 
}

Tars.BinBuffer.prototype.toObject = function () {
    return this.toNodeBuffer();
}

Tars.BinBuffer.prototype.readFromObject = function (json) {
    this.writeNodeBuffer(Tars.createNodeBuffer(json.data||json));
    return this;
}

Tars.BinBuffer.prototype.print = function() {
    var str  = "";
    for (var i = 0; i < this._length; i++) {
        str += (this._buffer[i] > 15?"":"0") + this._buffer[i].toString(16) + (((i+1)%16 == 0)?"\n":" ");
    }

    console.log(str.toUpperCase());
}

/**
 * Tars-输出编解码包裹类
 */
Tars.OutputStream = function () {
    this._binBuffer = new Tars.BinBuffer;
}

Tars.OutputStream.prototype._writeTo = function (tag, type) {
    if (tag < 15) {
        this._binBuffer.writeUInt8((tag << 4 & 0xF0) | type);
    }
    else {
        this._binBuffer.writeUInt16((0xF0 | type) << 8 | tag);
    }
}

Tars.OutputStream.prototype.setHeaderLength = function (value) {
    var position = this._binBuffer._position === 0?4:this._binBuffer._position;
    var length   = this._binBuffer._position === 0?4:this._binBuffer._length;

    this._binBuffer._position = 0;
    this._binBuffer.writeInt32(value);
    this._binBuffer._position = position;
    this._binBuffer._length   = length;
}

Tars.OutputStream.prototype.writeBoolean = function (tag, value) {
    this.writeInt8(tag, value == true ? 1 : 0);
}

Tars.OutputStream.prototype.writeInt8 = function (tag, value) {
    value = +value;
    if (value == 0) {
        this._writeTo(tag, Tars.DataHelp.EN_ZERO);
    }
    else {
        this._writeTo(tag, Tars.DataHelp.EN_INT8);
        this._binBuffer.writeInt8(value);
    }
}

Tars.OutputStream.prototype.writeInt16 = function (tag, value) {
    value = +value;
    if (value >= -128 && value <= 127) {
        this.writeInt8(tag, value);
    }
    else {
        this._writeTo(tag, Tars.DataHelp.EN_INT16);
        this._binBuffer.writeInt16(value);
    }
}

Tars.OutputStream.prototype.writeInt32 = function (tag, value) {
    value = +value;
    if (value >= -32768 && value <= 32767) {
        this.writeInt16(tag, value);
    }
    else {
        this._writeTo(tag, Tars.DataHelp.EN_INT32);
        this._binBuffer.writeInt32(value);
    }
}

Tars.OutputStream.prototype.writeInt64 = function (tag, value) {
    value = +value;
    if (value >= -2147483648 && value <= 2147483647) {
        this.writeInt32(tag, value);
    }
    else {
        this._writeTo(tag, Tars.DataHelp.EN_INT64);
        this._binBuffer.writeInt64(value);
    }
}

Tars.OutputStream.prototype.writeUInt8 = function (tag, value) {
    this.writeInt16(tag, value);
}

Tars.OutputStream.prototype.writeUInt16 = function (tag, value) {
    this.writeInt32(tag, value);
}

Tars.OutputStream.prototype.writeUInt32 = function (tag, value) {
    this.writeInt64(tag, value);
}

Tars.OutputStream.prototype.writeFloat = function (tag, value) {
    if (value == 0) {
        this._writeTo(tag, Tars.DataHelp.EN_ZERO);
    }
    else {
        this._writeTo(tag, Tars.DataHelp.EN_FLOAT);
        this._binBuffer.writeFloat(value);
    }
}

Tars.OutputStream.prototype.writeDouble = function (tag, value) {
    if (value == 0) {
        this._writeTo(tag, Tars.DataHelp.EN_ZERO);
    }
    else {
        this._writeTo(tag, Tars.DataHelp.EN_DOUBLE);
        this._binBuffer.writeDouble(value);
    }
}

Tars.OutputStream.prototype.writeStruct = function (tag, value) {
    if (value._writeTo == undefined) {
        throw Error("not defined writeTo Function");
    }

    this._writeTo(tag, Tars.DataHelp.EN_STRUCTBEGIN);
    value._writeTo(this);
    this._writeTo(0, Tars.DataHelp.EN_STRUCTEND);
}

Tars.OutputStream.prototype.writeString = function (tag, value, bRaw) {
    if (bRaw != undefined && bRaw == true) {
        var byteLength = value.length;
        if (byteLength > 255) {
            this._writeTo(tag, Tars.DataHelp.EN_STRING4);
            this._binBuffer.writeUInt32(byteLength);
        }
        else {
            this._writeTo(tag, Tars.DataHelp.EN_STRING1);
            this._binBuffer.writeUInt8(byteLength);
        }

        this._binBuffer.writeString(value, byteLength, bRaw);
        return ;
    }

    var byteLength = Buffer.byteLength(value, "utf8");
    if (byteLength > 255) {
        this._writeTo(tag, Tars.DataHelp.EN_STRING4);
        this._binBuffer.writeUInt32(byteLength);
    }
    else {
        this._writeTo(tag, Tars.DataHelp.EN_STRING1);
        this._binBuffer.writeUInt8(byteLength);
    }

    this._binBuffer.writeString(value, byteLength);
}

Tars.OutputStream.prototype.writeBytes = function (tag, value) {
    this._writeTo(tag, Tars.DataHelp.EN_SIMPLELIST);
    this._writeTo(0, Tars.DataHelp.EN_INT8);
    this.writeInt32(0, value.length);
    this._binBuffer.writeBinBuffer(value);
}

Tars.OutputStream.prototype.writeList = function (tag, value, bRaw) {
    this._writeTo(tag, Tars.DataHelp.EN_LIST);
    this.writeInt32(0, value.length);
    for (var i = 0, len = value.value.length; i < len; i++) {
        value._proto._write(this, 0, value.value[i], bRaw);
    }
}

Tars.OutputStream.prototype.writeMap  = function (tag, value) {
    this._writeTo(tag, Tars.DataHelp.EN_MAP);
    this.writeInt32(0, value.size());

    var self = this;
    value.forEach(function (key, val){
        value._kproto._write(self, 0, key);
        value._vproto._write(self, 1, val);
    });
}

Tars.OutputStream.prototype.getBinBuffer = function() {
    return this._binBuffer;
}

/**
 * Tars-输入编解码包裹类
 */
Tars.InputStream = function (binBuffer) {
    this._binBuffer = binBuffer;
    this._binBuffer._position = 0;
}

Tars.InputStream.prototype.setBinBuffer = function (binBuffer) {
    this._binBuffer = binBuffer;
    this._binBuffer._position = 0;
}

Tars.InputStream.prototype._readFrom = function () {
    var temp = this._binBuffer.readUInt8();
    var tag  = (temp & 0xF0) >> 4;
    var type = (temp & 0x0F);

    if (tag >= 15) tag = this._binBuffer.readUInt8();
    return {tag:tag, type:type};
}

Tars.InputStream.prototype._peekFrom = function () {
    var pos  = this._binBuffer._position;
    var head = this._readFrom();
    this._binBuffer.position = pos;

    return {tag:head.tag, type:head.type, size:(head.tag >= 15) ? 2 : 1};
}

Tars.InputStream.prototype._skipField = function (type) {
    switch (type) {
        case Tars.DataHelp.EN_INT8        : this._binBuffer._position += 1; break;
        case Tars.DataHelp.EN_INT16       : this._binBuffer._position += 2; break;
        case Tars.DataHelp.EN_INT32       : this._binBuffer._position += 4; break;
        case Tars.DataHelp.EN_INT64       : this._binBuffer._position += 8; break;
        case Tars.DataHelp.EN_FLOAT       : this._binBuffer._position += 4; break;
        case Tars.DataHelp.EN_DOUBLE      : this._binBuffer._position += 8; break;
        case Tars.DataHelp.EN_STRING1     : {
            var Length = this._binBuffer.readUInt8();
            this._binBuffer._position += Length;
            break;
        }
        case Tars.DataHelp.EN_STRING4     : {
            var Length = this._binBuffer.readUInt32();
            this._binBuffer._position += Length;
            break;
        }
        case Tars.DataHelp.EN_STRUCTBEGIN : this._skipToStructEnd(); break;
        case Tars.DataHelp.EN_STRUCTEND   :
        case Tars.DataHelp.EN_ZERO        : break;
        case Tars.DataHelp.EN_MAP         : {
            var size = this.readInt32(0, true);

            for (var i = 0; i < size * 2; ++i) {
                var head = this._readFrom();
                this._skipField(head.type);
            }
            
            break;
        }
        case Tars.DataHelp.EN_SIMPLELIST : {
            var head = this._readFrom();
            if (head.type != Tars.DataHelp.EN_INT8) {
                throw new Tars.DecodeInvalidValue("skipField with invalid type, type value: " + type + "," + head.type);
            }

            var Length = this.readInt32(0, true);
            this._binBuffer._position += Length;
            break;
        }
        case Tars.DataHelp.EN_LIST : {
            var size = this.readInt32(0, true);
            for (var i = 0; i < size; ++i) {
                var head = this._readFrom();
                this._skipField(head.type);
            }
            break;
        }
        default : throw new Tars.DecodeInvalidValue("skipField with invalid type, type value: " + type);
    }
}

Tars.InputStream.prototype._skipToStructEnd = function () {
    for ( ; ; ) {
        var head = this._readFrom();
        this._skipField(head.type);

        if (head.type == Tars.DataHelp.EN_STRUCTEND) {
            return;
        }
    }
}

Tars.InputStream.prototype._skipToTag = function (tag, require) {
    while (this._binBuffer._position < this._binBuffer._length) {
        var head = this._peekFrom();
        if (tag <= head.tag || head.type == Tars.DataHelp.EN_STRUCTEND) {
            if ((head.type === Tars.DataHelp.EN_STRUCTEND)?false:(tag === head.tag)) {
                return true;
            }
            break;
        }

        this._binBuffer._position += head.size;
        this._skipField(head.type);
    }

    if (require) throw new Tars.DecodeRequireNotExist("require field not exist, tag:" + tag);
    return false;
}

Tars.InputStream.prototype.readBoolean = function (tag, require, DEFAULT_VALUE) {
    return this.readInt8(tag, require, DEFAULT_VALUE) == 1 ? true : false;
}

Tars.InputStream.prototype.readInt8 = function (tag, require, DEFAULT_VALUE) {
    if (this._skipToTag(tag, require) == false) { return DEFAULT_VALUE; }

    var head = this._readFrom();
    switch (head.type) {
        case Tars.DataHelp.EN_ZERO: return 0;
        case Tars.DataHelp.EN_INT8: return this._binBuffer.readInt8();
    }

    throw new Tars.DecodeMismatch("read int8 type mismatch, tag:" + tag + ", get type:" + head.type);
}

Tars.InputStream.prototype.readInt16 = function (tag, require, DEFAULT_VALUE) {
    if (this._skipToTag(tag, require) == false) { return DEFAULT_VALUE; }

    var head = this._readFrom();
    switch (head.type) {
        case Tars.DataHelp.EN_ZERO   : return 0;
        case Tars.DataHelp.EN_INT8   : return this._binBuffer.readInt8();
        case Tars.DataHelp.EN_INT16  : return this._binBuffer.readInt16();
    }

    throw new Tars.DecodeMismatch("read int8 type mismatch, tag:" + tag + ", get type:" + head.type);
}

Tars.InputStream.prototype.readInt32 = function (tag, requrire, DEFAULT_VALUE) {
    if (this._skipToTag(tag, requrire) == false) { return DEFAULT_VALUE; }

    var head = this._readFrom();
    switch (head.type) {
        case Tars.DataHelp.EN_ZERO   : return 0;
        case Tars.DataHelp.EN_INT8   : return this._binBuffer.readInt8();
        case Tars.DataHelp.EN_INT16  : return this._binBuffer.readInt16();
        case Tars.DataHelp.EN_INT32  : return this._binBuffer.readInt32();
    }

    throw new Tars.DecodeMismatch("read int8 type mismatch, tag:" + tag + ", get type:" + head.type);
}

Tars.InputStream.prototype.readInt64 = function (tag, require, DEFAULT_VALUE) {
    if (this._skipToTag(tag, require) == false) { return DEFAULT_VALUE; }

    var head = this._readFrom();
    switch (head.type) {
        case Tars.DataHelp.EN_ZERO   : return 0;
        case Tars.DataHelp.EN_INT8   : return this._binBuffer.readInt8();
        case Tars.DataHelp.EN_INT16  : return this._binBuffer.readInt16();
        case Tars.DataHelp.EN_INT32  : return this._binBuffer.readInt32();
        case Tars.DataHelp.EN_INT64  : return this._binBuffer.readInt64();
    }
    
    throw new Tars.DecodeMismatch("read int64 type mismatch, tag:" + tag + ", get type:" + head.type);
}

Tars.InputStream.prototype.readFloat = function (tag, require, DEFAULT_VALUE) {
    if (this._skipToTag(tag, require) == false) { return DEFAULT_VALUE; }

    var head = this._readFrom();
    switch (head.type) {
        case Tars.DataHelp.EN_ZERO   : return 0;
        case Tars.DataHelp.EN_FLOAT  : return this._binBuffer.readFloat();
    }
    
    throw new Tars.DecodeMismatch("read float type mismatch, tag:" + tag + ", get type:" + head.type);
}

Tars.InputStream.prototype.readDouble = function (tag, require, DEFAULT_VALUE) {
    if (this._skipToTag(tag, require) == false) { return DEFAULT_VALUE; }

    var head = this._readFrom();
    switch (head.type) {
        case Tars.DataHelp.EN_ZERO    : return 0;
        case Tars.DataHelp.EN_DOUBLE  : return this._binBuffer.readDouble();
    }

    throw new Tars.DecodeMismatch("read double type mismatch, tag:" + tag + ", get type:" + head.type);
}

Tars.InputStream.prototype.readUInt8 = function (tag, require, DEFAULT_VALUE)  {
    return this.readInt16(tag, require, DEFAULT_VALUE);
}

Tars.InputStream.prototype.readUInt16 = function (tag, require, DEFAULT_VALUE) {
    return this.readInt32(tag, require, DEFAULT_VALUE);
}

Tars.InputStream.prototype.readUInt32 = function (tag, require, DEFAULT_VALUE) {
    return this.readInt64(tag, require, DEFAULT_VALUE);
}

Tars.InputStream.prototype.readString = function (tag, require, DEFAULT_VALUE, bRaw) {
    if (this._skipToTag(tag, require) == false) { return DEFAULT_VALUE; }

    var head = this._readFrom();
    if (head.type == Tars.DataHelp.EN_STRING1) {
        return this._binBuffer.readString(this._binBuffer.readUInt8(), bRaw);
    }

    if (head.type == Tars.DataHelp.EN_STRING4) {
        return this._binBuffer.readString(this._binBuffer.readUInt32(), bRaw);
    }

    throw new Tars.DecodeMismatch("read 'string' type mismatch, tag: " + tag + ", get type: " + head.type + ".");
}

Tars.InputStream.prototype.readStruct = function (tag, require, TYPE_T) {
    if (this._skipToTag(tag, require) == false) { return  new TYPE_T(); }

    var head = this._readFrom();
    if (head.type != Tars.DataHelp.EN_STRUCTBEGIN) {
        throw new Tars.DecodeMismatch("read struct type mismatch, tag: " + tag + ", get type:" + head.type);
    }

    var temp = TYPE_T._readFrom(this);
    this._skipToStructEnd();
    return temp;
}

Tars.InputStream.prototype.readBytes  = function(tag, require, TYPE_T, bRaw) {
    if (this._skipToTag(tag, require) == false) { return new TYPE_T(); }

    var head = this._readFrom();
    if (head.type == Tars.DataHelp.EN_SIMPLELIST) {
        var temp = this._readFrom();
        if (temp.type != Tars.DataHelp.EN_INT8) {
            throw new Tars.DecodeMismatch("type mismatch, tag:" + tag + ",type:" + head.type + "," + temp.type);
        }

        var size = this.readInt32(0, true);
        if (size < 0) {
            throw new Tars.DecodeInvalidValue("invalid size, tag:" + tag + ",type:" + head.type + "," + temp.type);
        }

        var bytes = this._binBuffer.readBinBuffer(size, bRaw);
        bytes.position = 0;
        return bytes;
    }

    throw new Tars.DecodeMismatch("type mismatch, tag:" + tag + ",type:" + head.type);
}

Tars.InputStream.prototype.readList = function(tag, require, TYPE_T, bRaw) {
    if (this._skipToTag(tag, require) == false) { return TYPE_T; }

    var head = this._readFrom();
    if (head.type != Tars.DataHelp.EN_LIST) {
        throw new Tars.DecodeMismatch("read 'vector' type mismatch, tag: " + tag + ", get type: " + head.type);
    }

    var size = this.readInt32(0, true);
    if (size < 0) {
        throw new Tars.DecodeInvalidValue("invalid size, tag: " + tag + ", type: " + head.type + ", size: " + size);
    }

    var TEMP = new Tars.List(TYPE_T._proto);
    for (var i = 0; i < size; ++i) {
        TEMP.value.push(TEMP._proto._read(this, 0, TEMP._proto, bRaw));
    }
    return TEMP;
}

Tars.InputStream.prototype.readMap = function(tag, require, TYPE_T) {
    if (this._skipToTag(tag, require) == false) { return TYPE_T; }

    var head = this._readFrom();
    if (head.type != Tars.DataHelp.EN_MAP) {
        throw new Tars.DecodeMismatch("read 'map' type mismatch, tag: " + tag + ", get type: " + head.type);
    }

    var size = this.readInt32(0, true);
    if (size < 0) {
        throw new Tars.DecodeMismatch("invalid map, tag: " + tag + ", size: " + size);
    }

    var TEMP = new Tars.Map(TYPE_T._kproto, TYPE_T._vproto);
    for (var i = 0; i < size; i++) {
        var key = TEMP._kproto._read(this, 0, TEMP._kproto);
        var val = TEMP._vproto._read(this, 1, TEMP._vproto);
        TEMP.insert(key, val);
    }

    return TEMP;
}

/**
 * Tup包裹类
 */
Tars.UniAttribute = function () {
    this._data = new Tars.Map(Tars.String, Tars.BinBuffer);
    this._mmap = new Tars.Map(Tars.String, Tars.Map(Tars.String, Tars.BinBuffer));
    this._buff = new Tars.OutputStream();
    this._temp = new Tars.InputStream(new Tars.BinBuffer());
    this._iver = Tars.UniAttribute.Tup_SIMPLE;

    this.__defineGetter__("tupVersion", function() { return this._iver; });
    this.__defineSetter__("tupVersion", function(value) { this._iver = value; });
}

Tars.UniAttribute.TUP_COMPLEX = 2;
Tars.UniAttribute.TUP_SIMPLE  = 3;

Tars.UniAttribute.prototype._getkey = function(name, DEFAULT_VALUE, TYPE_T, FUNC) {
    if (this._iver == Tars.UniAttribute.TUP_SIMPLE) {
        var binBuffer = this._data.get(name);
        if (binBuffer == undefined && DEFAULT_VALUE == undefined) {
            throw new Tars.TupNotFoundKey("UniAttribute not found key:" + name);
        }
        if (binBuffer == undefined && DEFAULT_VALUE != undefined) {
            return DEFAULT_VALUE;
        }
    } else {
        var item = this._mmap.get(name);
        if (item == undefined && DEFAULT_VALUE == undefined) {
            throw new Tars.TupNotFoundKey("UniAttribute not found key:" + name);
        }
        if (item == undefined && DEFAULT_VALUE != undefined) {
            return DEFAULT_VALUE;
        }

        var binBuffer = item.get(TYPE_T._classname);
        if (binBuffer == undefined) {
            throw new Tars.TupNotFoundKey("UniAttribute type match fail,key:" + name + ",type:" + TYPE_T._classname);
        }
    }

    this._temp.setBinBuffer(binBuffer);
    return FUNC.call(this._temp, 0, true, TYPE_T)
};

Tars.UniAttribute.prototype._setkey = function(name, value, TYPE_T, FUNC) {
    this._buff._binBuffer.reset();
    FUNC.call(this._buff, 0, value);

    if (this._iver == Tars.UniAttribute.TUP_SIMPLE) {
        this._data.set(name, new Tars.BinBuffer(this._buff.getBinBuffer().toNodeBuffer()));
    } else {
        var temp = new Tars.Map(Tars.String, Tars.BinBuffer);
        temp.set(TYPE_T._classname, new Tars.BinBuffer(this._buff.getBinBuffer().toNodeBuffer()));
        this._mmap.set(name, temp);
    }
};

Tars.UniAttribute.prototype.decode = function(binBuffer) {
    var is = new Tars.InputStream(binBuffer);
    if (this._iver == Tars.UniAttribute.TUP_SIMPLE) {
        this._data.clear();
        this._data = is.readMap(0, true, Tars.Map(Tars.String, Tars.BinBuffer));
    } else {
        this._mmap.clear();
        this._mmap = is.readMap(0, true, Tars.Map(Tars.String, Tars.Map(Tars.String, Tars.BinBuffer)));
    }
};

Tars.UniAttribute.prototype.encode = function() {
    var os = new Tars.OutputStream();
    os.writeMap(0, this._iver == Tars.UniAttribute.TUP_SIMPLE?this._data:this._mmap);
    return os.getBinBuffer();
};

Tars.UniAttribute.prototype.writeBoolean = function(name, value) { this._setkey(name, value, Tars.Boolean, this._buff.writeBoolean); };
Tars.UniAttribute.prototype.writeInt8    = function(name, value) { this._setkey(name, value, Tars.Int8, this._buff.writeInt8);       };
Tars.UniAttribute.prototype.writeUInt8   = function(name, value) { this._setkey(name, value, Tars.UInt8, this._buff.writeUInt8);     };
Tars.UniAttribute.prototype.writeInt16   = function(name, value) { this._setkey(name, value, Tars.Int16, this._buff.writeInt16);     };
Tars.UniAttribute.prototype.writeUInt16  = function(name, value) { this._setkey(name, value, Tars.UInt16, this._buff.writeUInt16);   };
Tars.UniAttribute.prototype.writeInt32   = function(name, value) { this._setkey(name, value, Tars.Int32, this._buff.writeInt32);     };
Tars.UniAttribute.prototype.writeUInt32  = function(name, value) { this._setkey(name, value, Tars.UInt32, this._buff.writeUInt32);   };
Tars.UniAttribute.prototype.writeInt64   = function(name, value) { this._setkey(name, value, Tars.Int64, this._buff.writeInt64);     };
Tars.UniAttribute.prototype.writeFloat   = function(name, value) { this._setkey(name, value, Tars.Float, this._buff.writeFloat);     };
Tars.UniAttribute.prototype.writeDouble  = function(name, value) { this._setkey(name, value, Tars.Double, this._buff.writeDouble);   };
Tars.UniAttribute.prototype.writeBytes   = function(name, value) { this._setkey(name, value, Tars.BinBuffer, this._buff.writeBytes); };
Tars.UniAttribute.prototype.writeString  = function(name, value) { this._setkey(name, value, Tars.String, this._buff.writeString);   };
Tars.UniAttribute.prototype.writeStruct  = function(name, value) { this._setkey(name, value, value, this._buff.writeStruct);        };
Tars.UniAttribute.prototype.writeList    = function(name, value) { this._setkey(name, value, value, this._buff.writeList);          };
Tars.UniAttribute.prototype.writeMap     = function(name, value) { this._setkey(name, value, value, this._buff.writeMap);           };

Tars.UniAttribute.prototype.readBoolean  = function(name, DEFAULT_VALUE) { return this._getkey(name, DEFAULT_VALUE, Tars.Boolean,    this._temp.readBoolean);     };
Tars.UniAttribute.prototype.readInt8     = function(name, DEFAULT_VALUE) { return this._getkey(name, DEFAULT_VALUE, Tars.Int8,       this._temp.readInt8);        };
Tars.UniAttribute.prototype.readUInt8    = function(name, DEFAULT_VALUE) { return this._getkey(name, DEFAULT_VALUE, Tars.UInt8,      this._temp.readUInt8);       };
Tars.UniAttribute.prototype.readInt16    = function(name, DEFAULT_VALUE) { return this._getkey(name, DEFAULT_VALUE, Tars.Int16,      this._temp.readInt16);       };
Tars.UniAttribute.prototype.readUInt16   = function(name, DEFAULT_VALUE) { return this._getkey(name, DEFAULT_VALUE, Tars.UInt16,     this._temp.readUInt16);      };
Tars.UniAttribute.prototype.readInt32    = function(name, DEFAULT_VALUE) { return this._getkey(name, DEFAULT_VALUE, Tars.Int32,      this._temp.readInt32);       };
Tars.UniAttribute.prototype.readUInt32   = function(name, DEFAULT_VALUE) { return this._getkey(name, DEFAULT_VALUE, Tars.UInt32,     this._temp.readUInt32);      };
Tars.UniAttribute.prototype.readInt64    = function(name, DEFAULT_VALUE) { return this._getkey(name, DEFAULT_VALUE, Tars.Int64,      this._temp.readInt64);       };
Tars.UniAttribute.prototype.readFloat    = function(name, DEFAULT_VALUE) { return this._getkey(name, DEFAULT_VALUE, Tars.Float,      this._temp.readFloat);       };
Tars.UniAttribute.prototype.readDouble   = function(name, DEFAULT_VALUE) { return this._getkey(name, DEFAULT_VALUE, Tars.Double,     this._temp.readDouble);      };
Tars.UniAttribute.prototype.readBytes    = function(name, DEFAULT_VALUE) { return this._getkey(name, DEFAULT_VALUE, Tars.BinBuffer,  this._temp.readBytes);       };
Tars.UniAttribute.prototype.readString   = function(name, DEFAULT_VALUE) { return this._getkey(name, DEFAULT_VALUE, Tars.String,     this._temp.readString);      };
Tars.UniAttribute.prototype.readStruct   = function(name, TYPE_T, DEFAULT_VALUE) { return this._getkey(name, DEFAULT_VALUE, TYPE_T, this._temp.readStruct);      };
Tars.UniAttribute.prototype.readList     = function(name, TYPE_T, DEFAULT_VALUE) { return this._getkey(name, DEFAULT_VALUE, TYPE_T, this._temp.readList);        };
Tars.UniAttribute.prototype.readMap      = function(name, TYPE_T, DEFAULT_VALUE) { return this._getkey(name, DEFAULT_VALUE, TYPE_T, this._temp.readMap);         };

Tars.Tup = function () {
    this._iVersion     = 0;
    this._cPacketType  = 0;
    this._iMessageType = 0;
    this._iRequestId   = 0;
    this._sServantName = '';
    this._sFuncName    = '';
    this._binBuffer    = new Tars.BinBuffer();
    this._iTimeout     = 0;
    this._context      = new Tars.Map(Tars.String, Tars.String);
    this._status       = new Tars.Map(Tars.String, Tars.String);
    this._attribute    = new Tars.UniAttribute();

    this.__defineGetter__("servantName", function() { return this._sServantName; });
    this.__defineSetter__("servantName", function(value) { this._sServantName = value; });
    this.__defineGetter__("funcName",    function() { return this._sFuncName; });
    this.__defineSetter__("funcName",    function(value) { this._sFuncName = value; });
    this.__defineGetter__("requestId",   function() { return this._iRequestId; });
    this.__defineSetter__("requestId",   function(value) { this._iRequestId = value; });
    this.__defineGetter__("tupVersion",  function() { return this._attribute.tupVersion; });
    this.__defineSetter__("tupVersion",  function(value) { this._attribute.tupVersion = value; });
};

Tars.Tup.TUP_COMPLEX = Tars.UniAttribute.TUP_COMPLEX; //复杂TUP协议
Tars.Tup.TUP_SIMPLE  = Tars.UniAttribute.TUP_SIMPLE;  //精简TUP协议

Tars.Tup.prototype._writeTo = function() {
    var os = new Tars.OutputStream(); 
    os._binBuffer.writeInt32(0);
    os.writeInt16  (1,  this._attribute.tupVersion);
    os.writeInt8   (2,  this._cPacketType);
    os.writeInt32  (3,  this._iMessageType);
    os.writeInt32  (4,  this._iRequestId);
    os.writeString (5,  this._sServantName);
    os.writeString (6,  this._sFuncName);
    os.writeBytes  (7,  this._binBuffer);
    os.writeInt32  (8,  this._iTimeout);
    os.writeMap    (9,  this._context);
    os.writeMap    (10, this._status);

    var pos = os._binBuffer._position;
    var len = os._binBuffer._length;

    os._binBuffer._position = 0;
    os._binBuffer.writeInt32(os._binBuffer._length);
    os._binBuffer._length   = len;
    os._binBuffer._position = pos;

    return os.getBinBuffer();
}

Tars.Tup.prototype._readFrom = function(is) {
    this._iVersion     = is.readInt16(1, true);
    this._cPacketType  = is.readInt8(2, true);
    this._iMessageType = is.readInt32(3, true);
    this._iRequestId   = is.readInt32(4, true);
    this._sServantName = is.readString(5, true);
    this._sFuncName    = is.readString(6, true);
    this._binBuffer    = is.readBytes(7, true);
    this._iTimeout     = is.readInt32(8, true);
    this._context      = is.readMap(9, false, Tars.Map(Tars.String, Tars.String));
    this._status       = is.readMap(10, false, Tars.Map(Tars.String, Tars.String));

    this._attribute.tupVersion = this._iVersion;
}

Tars.Tup.prototype.encode = function() {
    this._binBuffer = this._attribute.encode();
    return this._writeTo();
}

Tars.Tup.prototype.decode = function (binBuffer) {
    var is  = new Tars.InputStream(binBuffer);
    var len = is._binBuffer.readInt32();
    if (len < 4) {
        throw Error("packet length too short");
    }
    this._readFrom(is);
    this._attribute.decode(this._binBuffer);
}

Tars.Tup.prototype.getTarsResultCode = function () {
    var code = this._status.get("STATUS_RESULT_CODE");
    return code === undefined?0:parseInt(code);
}

Tars.Tup.prototype.getTarsResultDesc = function () {
    var desc = this._status.get("STATUS_RESULT_DESC");
    return desc === undefined?"":desc;
}

Tars.Tup.prototype.writeBoolean = function(name, value) { this._attribute.writeBoolean(name, value); }
Tars.Tup.prototype.writeInt8    = function(name, value) { this._attribute.writeInt8(name, value);    }
Tars.Tup.prototype.writeUInt8   = function(name, value) { this._attribute.writeUInt8(name, value);   }
Tars.Tup.prototype.writeInt16   = function(name, value) { this._attribute.writeInt16(name, value);   }
Tars.Tup.prototype.writeUInt16  = function(name, value) { this._attribute.writeUInt16(name, value);  }
Tars.Tup.prototype.writeInt32   = function(name, value) { this._attribute.writeInt32(name, value);   }
Tars.Tup.prototype.writeUInt32  = function(name, value) { this._attribute.writeUInt32(name, value);  }
Tars.Tup.prototype.writeInt64   = function(name, value) { this._attribute.writeInt64(name, value);   }
Tars.Tup.prototype.writeFloat   = function(name, value) { this._attribute.writeFloat(name, value);   }
Tars.Tup.prototype.writeDouble  = function(name, value) { this._attribute.writeDouble(name, value);  }
Tars.Tup.prototype.writeBytes   = function(name, value) { this._attribute.writeBytes(name, value);   }
Tars.Tup.prototype.writeString  = function(name, value) { this._attribute.writeString(name, value);  }
Tars.Tup.prototype.writeStruct  = function(name, value) { this._attribute.writeStruct(name, value);  }
Tars.Tup.prototype.writeList    = function(name, value) { this._attribute.writeList(name, value);    }
Tars.Tup.prototype.writeMap     = function(name, value) { this._attribute.writeMap(name, value);     }

Tars.Tup.prototype.readBoolean  = function(name, DEFAULT_VALUE)         { return this._attribute.readBoolean(name, DEFAULT_VALUE);        }
Tars.Tup.prototype.readInt8     = function(name, DEFAULT_VALUE)         { return this._attribute.readInt8(name, DEFAULT_VALUE);           }
Tars.Tup.prototype.readUInt8    = function(name, DEFAULT_VALUE)         { return this._attribute.readUInt8(name, DEFAULT_VALUE);          }
Tars.Tup.prototype.readInt16    = function(name, DEFAULT_VALUE)         { return this._attribute.readInt16(name, DEFAULT_VALUE);          }
Tars.Tup.prototype.readUInt16   = function(name, DEFAULT_VALUE)         { return this._attribute.readUInt16(name, DEFAULT_VALUE);         }
Tars.Tup.prototype.readInt32    = function(name, DEFAULT_VALUE)         { return this._attribute.readInt32(name, DEFAULT_VALUE);          }
Tars.Tup.prototype.readUInt32   = function(name, DEFAULT_VALUE)         { return this._attribute.readUInt32(name, DEFAULT_VALUE);         }
Tars.Tup.prototype.readInt64    = function(name, DEFAULT_VALUE)         { return this._attribute.readInt64(name, DEFAULT_VALUE);          }
Tars.Tup.prototype.readFloat    = function(name, DEFAULT_VALUE)         { return this._attribute.readFloat(name, DEFAULT_VALUE);          }
Tars.Tup.prototype.readDouble   = function(name, DEFAULT_VALUE)         { return this._attribute.readDouble(name, DEFAULT_VALUE);         }
Tars.Tup.prototype.readBytes    = function(name, DEFAULT_VALUE)         { return this._attribute.readBytes(name, DEFAULT_VALUE);          }
Tars.Tup.prototype.readString   = function(name, DEFAULT_VALUE)         { return this._attribute.readString(name, DEFAULT_VALUE);         }
Tars.Tup.prototype.readStruct   = function(name, TYPE_T, DEFAULT_VALUE) { return this._attribute.readStruct(name, TYPE_T, DEFAULT_VALUE); }
Tars.Tup.prototype.readList     = function(name, TYPE_T, DEFAULT_VALUE) { return this._attribute.readList(name, TYPE_T, DEFAULT_VALUE);   }
Tars.Tup.prototype.readMap      = function(name, TYPE_T, DEFAULT_VALUE) { return this._attribute.readMap(name, TYPE_T, DEFAULT_VALUE);    }
