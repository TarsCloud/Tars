/**
 * Tencent is pleased to support the open source community by making Tars available.
 *
 * Copyright (C) 2016THL A29 Limited, a Tencent company. All rights reserved.
 *
 * Licensed under the BSD 3-Clause License (the "License"); you may not use this file except 
 * in compliance with the License. You may obtain a copy of the License at
 *
 * https://opensource.org/licenses/BSD-3-Clause
 *
 * Unless required by applicable law or agreed to in writing, software distributed 
 * under the License is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR 
 * CONDITIONS OF ANY KIND, either express or implied. See the License for the 
 * specific language governing permissions and limitations under the License.
 */

var TarsStream = require('@tars/stream');
var TarsError  = require('@tars/rpc').error;

var tars = tars || {};
module.exports.tars = tars;

tars.StatFProxy = function () {
    this._name   = undefined;
    this._worker = undefined;
}

tars.StatFProxy.prototype.setTimeout = function (iTimeout) {
    this._worker.timeout = iTimeout;
}

tars.StatFProxy.prototype.getTimeout = function ( ) {
    return this._worker.timeout;
}


tars.StatMicMsgHead = function() {
    this.masterName = "";
    this.slaveName = "";
    this.interfaceName = "";
    this.masterIp = "";
    this.slaveIp = "";
    this.slavePort = 0;
    this.returnValue = 0;
    this.slaveSetName = "";
    this.slaveSetArea = "";
    this.slaveSetID = "";
    this.tarsVersion = "";
    this._classname = "tars.StatMicMsgHead";
};
tars.StatMicMsgHead._classname = "tars.StatMicMsgHead";

tars.StatMicMsgHead._write = function (os, tag, value) { os.writeStruct(tag, value); }
tars.StatMicMsgHead._read  = function (is, tag, def) { return is.readStruct(tag, true, def); }
tars.StatMicMsgHead._readFrom = function (is) {
    var tmp = new tars.StatMicMsgHead();
    tmp.masterName = is.readString(0, true, "");
    tmp.slaveName = is.readString(1, true, "");
    tmp.interfaceName = is.readString(2, true, "");
    tmp.masterIp = is.readString(3, true, "");
    tmp.slaveIp = is.readString(4, true, "");
    tmp.slavePort = is.readInt32(5, true, 0);
    tmp.returnValue = is.readInt32(6, true, 0);
    tmp.slaveSetName = is.readString(7, false, "");
    tmp.slaveSetArea = is.readString(8, false, "");
    tmp.slaveSetID = is.readString(9, false, "");
    tmp.tarsVersion = is.readString(10, false, "");
    return tmp;
};
tars.StatMicMsgHead.prototype._writeTo = function (os) {
    os.writeString(0, this.masterName);
    os.writeString(1, this.slaveName);
    os.writeString(2, this.interfaceName);
    os.writeString(3, this.masterIp);
    os.writeString(4, this.slaveIp);
    os.writeInt32(5, this.slavePort);
    os.writeInt32(6, this.returnValue);
    os.writeString(7, this.slaveSetName);
    os.writeString(8, this.slaveSetArea);
    os.writeString(9, this.slaveSetID);
    os.writeString(10, this.tarsVersion);
};
tars.StatMicMsgHead.prototype._equal = function (anItem) {
    return this.masterName === anItem.masterName && 
        this.slaveName === anItem.slaveName && 
        this.interfaceName === anItem.interfaceName && 
        this.masterIp === anItem.masterIp && 
        this.slaveIp === anItem.slaveIp && 
        this.slavePort === anItem.slavePort && 
        this.returnValue === anItem.returnValue && 
        this.slaveSetName === anItem.slaveSetName && 
        this.slaveSetArea === anItem.slaveSetArea && 
        this.slaveSetID === anItem.slaveSetID && 
        this.tarsVersion === anItem.tarsVersion;
}
tars.StatMicMsgHead.prototype._genKey = function () {
    if (!this._proto_struct_name_) {
        this._proto_struct_name_ = 'STRUCT' + Math.random();
    }
    return this._proto_struct_name_;
}
tars.StatMicMsgHead.prototype.toObject = function() { 
    var tmp = {};

    tmp.masterName = this.masterName;
    tmp.slaveName = this.slaveName;
    tmp.interfaceName = this.interfaceName;
    tmp.masterIp = this.masterIp;
    tmp.slaveIp = this.slaveIp;
    tmp.slavePort = this.slavePort;
    tmp.returnValue = this.returnValue;
    tmp.slaveSetName = this.slaveSetName;
    tmp.slaveSetArea = this.slaveSetArea;
    tmp.slaveSetID = this.slaveSetID;
    tmp.tarsVersion = this.tarsVersion;
    
    return tmp;
}
tars.StatMicMsgHead.prototype.readFromObject = function(json) { 
    !json.hasOwnProperty("masterName") || (this.masterName = json.masterName);
    !json.hasOwnProperty("slaveName") || (this.slaveName = json.slaveName);
    !json.hasOwnProperty("interfaceName") || (this.interfaceName = json.interfaceName);
    !json.hasOwnProperty("masterIp") || (this.masterIp = json.masterIp);
    !json.hasOwnProperty("slaveIp") || (this.slaveIp = json.slaveIp);
    !json.hasOwnProperty("slavePort") || (this.slavePort = json.slavePort);
    !json.hasOwnProperty("returnValue") || (this.returnValue = json.returnValue);
    !json.hasOwnProperty("slaveSetName") || (this.slaveSetName = json.slaveSetName);
    !json.hasOwnProperty("slaveSetArea") || (this.slaveSetArea = json.slaveSetArea);
    !json.hasOwnProperty("slaveSetID") || (this.slaveSetID = json.slaveSetID);
    !json.hasOwnProperty("tarsVersion") || (this.tarsVersion = json.tarsVersion);
}
tars.StatMicMsgHead.prototype.toBinBuffer = function () {
    var os = new TarsStream.OutputStream();
    this._writeTo(os);
    return os.getBinBuffer();
}
tars.StatMicMsgHead.new = function () {
    return new tars.StatMicMsgHead();
}
tars.StatMicMsgHead.create = function (is) {
    return tars.StatMicMsgHead._readFrom(is);
}
tars.StatMicMsgBody = function() {
    this.count = 0;
    this.timeoutCount = 0;
    this.execCount = 0;
    this.intervalCount = new TarsStream.Map(TarsStream.Int32, TarsStream.Int32);
    this.totalRspTime = 0;
    this.maxRspTime = 0;
    this.minRspTime = 0;
    this._classname = "tars.StatMicMsgBody";
};
tars.StatMicMsgBody._classname = "tars.StatMicMsgBody";

tars.StatMicMsgBody._write = function (os, tag, value) { os.writeStruct(tag, value); }
tars.StatMicMsgBody._read  = function (is, tag, def) { return is.readStruct(tag, true, def); }
tars.StatMicMsgBody._readFrom = function (is) {
    var tmp = new tars.StatMicMsgBody();
    tmp.count = is.readInt32(0, true, 0);
    tmp.timeoutCount = is.readInt32(1, true, 0);
    tmp.execCount = is.readInt32(2, true, 0);
    tmp.intervalCount = is.readMap(3, true, TarsStream.Map(TarsStream.Int32, TarsStream.Int32));
    tmp.totalRspTime = is.readInt64(4, true, 0);
    tmp.maxRspTime = is.readInt32(5, true, 0);
    tmp.minRspTime = is.readInt32(6, true, 0);
    return tmp;
};
tars.StatMicMsgBody.prototype._writeTo = function (os) {
    os.writeInt32(0, this.count);
    os.writeInt32(1, this.timeoutCount);
    os.writeInt32(2, this.execCount);
    os.writeMap(3, this.intervalCount);
    os.writeInt64(4, this.totalRspTime);
    os.writeInt32(5, this.maxRspTime);
    os.writeInt32(6, this.minRspTime);
};
tars.StatMicMsgBody.prototype._equal = function (anItem) {
    assert(false, 'this structure not define key operation');
}
tars.StatMicMsgBody.prototype._genKey = function () {
    if (!this._proto_struct_name_) {
        this._proto_struct_name_ = 'STRUCT' + Math.random();
    }
    return this._proto_struct_name_;
}
tars.StatMicMsgBody.prototype.toObject = function() { 
    var tmp = {};

    tmp.count = this.count;
    tmp.timeoutCount = this.timeoutCount;
    tmp.execCount = this.execCount;
    tmp.intervalCount = this.intervalCount.toObject();
    tmp.totalRspTime = this.totalRspTime;
    tmp.maxRspTime = this.maxRspTime;
    tmp.minRspTime = this.minRspTime;
    
    return tmp;
}
tars.StatMicMsgBody.prototype.readFromObject = function(json) { 
    !json.hasOwnProperty("count") || (this.count = json.count);
    !json.hasOwnProperty("timeoutCount") || (this.timeoutCount = json.timeoutCount);
    !json.hasOwnProperty("execCount") || (this.execCount = json.execCount);
    !json.hasOwnProperty("intervalCount") || (this.intervalCount.readFromObject(json.intervalCount));
    !json.hasOwnProperty("totalRspTime") || (this.totalRspTime = json.totalRspTime);
    !json.hasOwnProperty("maxRspTime") || (this.maxRspTime = json.maxRspTime);
    !json.hasOwnProperty("minRspTime") || (this.minRspTime = json.minRspTime);
}
tars.StatMicMsgBody.prototype.toBinBuffer = function () {
    var os = new TarsStream.OutputStream();
    this._writeTo(os);
    return os.getBinBuffer();
}
tars.StatMicMsgBody.new = function () {
    return new tars.StatMicMsgBody();
}
tars.StatMicMsgBody.create = function (is) {
    return tars.StatMicMsgBody._readFrom(is);
}
tars.StatSampleMsg = function() {
    this.unid = "";
    this.masterName = "";
    this.slaveName = "";
    this.interfaceName = "";
    this.masterIp = "";
    this.slaveIp = "";
    this.depth = 0;
    this.width = 0;
    this.parentWidth = 0;
    this._classname = "tars.StatSampleMsg";
};
tars.StatSampleMsg._classname = "tars.StatSampleMsg";

tars.StatSampleMsg._write = function (os, tag, value) { os.writeStruct(tag, value); }
tars.StatSampleMsg._read  = function (is, tag, def) { return is.readStruct(tag, true, def); }
tars.StatSampleMsg._readFrom = function (is) {
    var tmp = new tars.StatSampleMsg();
    tmp.unid = is.readString(0, true, "");
    tmp.masterName = is.readString(1, true, "");
    tmp.slaveName = is.readString(2, true, "");
    tmp.interfaceName = is.readString(3, true, "");
    tmp.masterIp = is.readString(4, true, "");
    tmp.slaveIp = is.readString(5, true, "");
    tmp.depth = is.readInt32(6, true, 0);
    tmp.width = is.readInt32(7, true, 0);
    tmp.parentWidth = is.readInt32(8, true, 0);
    return tmp;
};
tars.StatSampleMsg.prototype._writeTo = function (os) {
    os.writeString(0, this.unid);
    os.writeString(1, this.masterName);
    os.writeString(2, this.slaveName);
    os.writeString(3, this.interfaceName);
    os.writeString(4, this.masterIp);
    os.writeString(5, this.slaveIp);
    os.writeInt32(6, this.depth);
    os.writeInt32(7, this.width);
    os.writeInt32(8, this.parentWidth);
};
tars.StatSampleMsg.prototype._equal = function (anItem) {
    assert(false, 'this structure not define key operation');
}
tars.StatSampleMsg.prototype._genKey = function () {
    if (!this._proto_struct_name_) {
        this._proto_struct_name_ = 'STRUCT' + Math.random();
    }
    return this._proto_struct_name_;
}
tars.StatSampleMsg.prototype.toObject = function() { 
    var tmp = {};

    tmp.unid = this.unid;
    tmp.masterName = this.masterName;
    tmp.slaveName = this.slaveName;
    tmp.interfaceName = this.interfaceName;
    tmp.masterIp = this.masterIp;
    tmp.slaveIp = this.slaveIp;
    tmp.depth = this.depth;
    tmp.width = this.width;
    tmp.parentWidth = this.parentWidth;
    
    return tmp;
}
tars.StatSampleMsg.prototype.readFromObject = function(json) { 
    !json.hasOwnProperty("unid") || (this.unid = json.unid);
    !json.hasOwnProperty("masterName") || (this.masterName = json.masterName);
    !json.hasOwnProperty("slaveName") || (this.slaveName = json.slaveName);
    !json.hasOwnProperty("interfaceName") || (this.interfaceName = json.interfaceName);
    !json.hasOwnProperty("masterIp") || (this.masterIp = json.masterIp);
    !json.hasOwnProperty("slaveIp") || (this.slaveIp = json.slaveIp);
    !json.hasOwnProperty("depth") || (this.depth = json.depth);
    !json.hasOwnProperty("width") || (this.width = json.width);
    !json.hasOwnProperty("parentWidth") || (this.parentWidth = json.parentWidth);
}
tars.StatSampleMsg.prototype.toBinBuffer = function () {
    var os = new TarsStream.OutputStream();
    this._writeTo(os);
    return os.getBinBuffer();
}
tars.StatSampleMsg.new = function () {
    return new tars.StatSampleMsg();
}
tars.StatSampleMsg.create = function (is) {
    return tars.StatSampleMsg._readFrom(is);
}
tars.ProxyInfo = function() {
    this.bFromClient = true;
    this._classname = "tars.ProxyInfo";
};
tars.ProxyInfo._classname = "tars.ProxyInfo";

tars.ProxyInfo._write = function (os, tag, value) { os.writeStruct(tag, value); }
tars.ProxyInfo._read  = function (is, tag, def) { return is.readStruct(tag, true, def); }
tars.ProxyInfo._readFrom = function (is) {
    var tmp = new tars.ProxyInfo();
    tmp.bFromClient = is.readBoolean(0, true, true);
    return tmp;
};
tars.ProxyInfo.prototype._writeTo = function (os) {
    os.writeBoolean(0, this.bFromClient);
};
tars.ProxyInfo.prototype._equal = function (anItem) {
    assert(false, 'this structure not define key operation');
}
tars.ProxyInfo.prototype._genKey = function () {
    if (!this._proto_struct_name_) {
        this._proto_struct_name_ = 'STRUCT' + Math.random();
    }
    return this._proto_struct_name_;
}
tars.ProxyInfo.prototype.toObject = function() { 
    var tmp = {};

    tmp.bFromClient = this.bFromClient;
    
    return tmp;
}
tars.ProxyInfo.prototype.readFromObject = function(json) { 
    !json.hasOwnProperty("bFromClient") || (this.bFromClient = json.bFromClient);
}
tars.ProxyInfo.prototype.toBinBuffer = function () {
    var os = new TarsStream.OutputStream();
    this._writeTo(os);
    return os.getBinBuffer();
}
tars.ProxyInfo.new = function () {
    return new tars.ProxyInfo();
}
tars.ProxyInfo.create = function (is) {
    return tars.ProxyInfo._readFrom(is);
}

tars.StatFProxy.prototype.reportMicMsg = function (msg, bFromClient) {
    var _encode = function () { 
        var os = new TarsStream.OutputStream();
        os.writeMap(1, msg);
        os.writeBoolean(2, bFromClient);
        return os.getBinBuffer();
    }

    var _decode = function (data) {
        try {
            var response = {arguments:{}};
            var is = new TarsStream.InputStream(data.response.sBuffer);

            response.costtime = data.request.costtime;
            response.return   = is.readInt32(0, true, TarsStream.Int32);

            return {request:data.request, response:response};
        } catch (e) {
            var response = { };
            response.costtime      = data.request.costtime;
            response.error         = {};
            response.error.code    = TarsError.CLIENT.DECODE_ERROR;
            response.error.message = e.message;

            throw { request : data.request, response : response};
        }
    }

    var _error = function(data) {
        var response = {};
        response.costtime = data.request.costtime;
        response.error    = data.error;

        throw {request:data.request, response:response};
    }

    return this._worker.tars_invoke('reportMicMsg', _encode(), arguments.length != 0?arguments[arguments.length - 1]:undefined).then(_decode, _error);
}

tars.StatFProxy.prototype.reportSampleMsg = function (msg) {
    var _encode = function () { 
        var os = new TarsStream.OutputStream();
        os.writeList(1, msg);
        return os.getBinBuffer();
    }

    var _decode = function (data) {
        try {
            var response = {arguments:{}};
            var is = new TarsStream.InputStream(data.response.sBuffer);

            response.costtime = data.request.costtime;
            response.return   = is.readInt32(0, true, TarsStream.Int32);

            return {request:data.request, response:response};
        } catch (e) {
            var response = { };
            response.costtime      = data.request.costtime;
            response.error         = {};
            response.error.code    = TarsError.CLIENT.DECODE_ERROR;
            response.error.message = e.message;

            throw { request : data.request, response : response};
        }
    }

    var _error = function(data) {
        var response = {};
        response.costtime = data.request.costtime;
        response.error    = data.error;

        throw {request:data.request, response:response};
    }

    return this._worker.tars_invoke('reportSampleMsg', _encode(), arguments.length != 0?arguments[arguments.length - 1]:undefined).then(_decode, _error);
}



