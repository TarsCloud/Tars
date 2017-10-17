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

tars.ServerFProxy = function () {
    this._name   = undefined;
    this._worker = undefined;
}

tars.ServerFProxy.prototype.setTimeout = function (iTimeout) {
    this._worker.timeout = iTimeout;
}

tars.ServerFProxy.prototype.getTimeout = function ( ) {
    return this._worker.timeout;
}


tars.ServerInfo = function() {
    this.application = "";
    this.serverName = "";
    this.pid = 0;
    this.adapter = "";
    this._classname = "tars.ServerInfo";
};
tars.ServerInfo._classname = "tars.ServerInfo";

tars.ServerInfo._write = function (os, tag, value) { os.writeStruct(tag, value); }
tars.ServerInfo._read  = function (is, tag, def) { return is.readStruct(tag, true, def); }
tars.ServerInfo._readFrom = function (is) {
    var tmp = new tars.ServerInfo();
    tmp.application = is.readString(0, true, "");
    tmp.serverName = is.readString(1, true, "");
    tmp.pid = is.readInt32(2, true, 0);
    tmp.adapter = is.readString(3, false, "");
    return tmp;
};
tars.ServerInfo.prototype._writeTo = function (os) {
    os.writeString(0, this.application);
    os.writeString(1, this.serverName);
    os.writeInt32(2, this.pid);
    os.writeString(3, this.adapter);
};
tars.ServerInfo.prototype._equal = function (anItem) {
    assert(false, 'this structure not define key operation');
}
tars.ServerInfo.prototype._genKey = function () {
    if (!this._proto_struct_name_) {
        this._proto_struct_name_ = 'STRUCT' + Math.random();
    }
    return this._proto_struct_name_;
}
tars.ServerInfo.prototype.toObject = function() { 
    var tmp = {};

    tmp.application = this.application;
    tmp.serverName = this.serverName;
    tmp.pid = this.pid;
    tmp.adapter = this.adapter;
    
    return tmp;
}
tars.ServerInfo.prototype.readFromObject = function(json) { 
    !json.hasOwnProperty("application") || (this.application = json.application);
    !json.hasOwnProperty("serverName") || (this.serverName = json.serverName);
    !json.hasOwnProperty("pid") || (this.pid = json.pid);
    !json.hasOwnProperty("adapter") || (this.adapter = json.adapter);
}
tars.ServerInfo.prototype.toBinBuffer = function () {
    var os = new TarsStream.OutputStream();
    this._writeTo(os);
    return os.getBinBuffer();
}
tars.ServerInfo.new = function () {
    return new tars.ServerInfo();
}
tars.ServerInfo.create = function (is) {
    return tars.ServerInfo._readFrom(is);
}

tars.ServerFProxy.prototype.keepAlive = function (serverInfo) {
    var _encode = function () { 
        var os = new TarsStream.OutputStream();
        os.writeStruct(1, serverInfo);
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

    return this._worker.tars_invoke('keepAlive', _encode(), arguments.length != 0?arguments[arguments.length - 1]:undefined).then(_decode, _error);
}

tars.ServerFProxy.prototype.reportVersion = function (app, serverName, version) {
    var _encode = function () { 
        var os = new TarsStream.OutputStream();
        os.writeString(1, app);
        os.writeString(2, serverName);
        os.writeString(3, version);
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

    return this._worker.tars_invoke('reportVersion', _encode(), arguments.length != 0?arguments[arguments.length - 1]:undefined).then(_decode, _error);
}



