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

tars.ConfigProxy = function () {
    this._name   = undefined;
    this._worker = undefined;
}

tars.ConfigProxy.prototype.setTimeout = function (iTimeout) {
    this._worker.timeout = iTimeout;
}

tars.ConfigProxy.prototype.getTimeout = function ( ) {
    return this._worker.timeout;
}


tars.ConfigInfo = function() {
    this.appname = "";
    this.servername = "";
    this.filename = "";
    this.bAppOnly = false;
    this.host = "";
    this.setdivision = "";
    this._classname = "tars.ConfigInfo";
};
tars.ConfigInfo._classname = "tars.ConfigInfo";

tars.ConfigInfo._write = function (os, tag, value) { os.writeStruct(tag, value); }
tars.ConfigInfo._read  = function (is, tag, def) { return is.readStruct(tag, true, def); }
tars.ConfigInfo._readFrom = function (is) {
    var tmp = new tars.ConfigInfo();
    tmp.appname = is.readString(0, true, "");
    tmp.servername = is.readString(1, true, "");
    tmp.filename = is.readString(2, true, "");
    tmp.bAppOnly = is.readBoolean(3, true, false);
    tmp.host = is.readString(4, false, "");
    tmp.setdivision = is.readString(5, false, "");
    return tmp;
};
tars.ConfigInfo.prototype._writeTo = function (os) {
    os.writeString(0, this.appname);
    os.writeString(1, this.servername);
    os.writeString(2, this.filename);
    os.writeBoolean(3, this.bAppOnly);
    os.writeString(4, this.host);
    os.writeString(5, this.setdivision);
};
tars.ConfigInfo.prototype._equal = function (anItem) {
    assert(false, 'this structure not define key operation');
}
tars.ConfigInfo.prototype._genKey = function () {
    if (!this._proto_struct_name_) {
        this._proto_struct_name_ = 'STRUCT' + Math.random();
    }
    return this._proto_struct_name_;
}
tars.ConfigInfo.prototype.toObject = function() { 
    var tmp = {};

    tmp.appname = this.appname;
    tmp.servername = this.servername;
    tmp.filename = this.filename;
    tmp.bAppOnly = this.bAppOnly;
    tmp.host = this.host;
    tmp.setdivision = this.setdivision;
    
    return tmp;
}
tars.ConfigInfo.prototype.readFromObject = function(json) { 
    !json.hasOwnProperty("appname") || (this.appname = json.appname);
    !json.hasOwnProperty("servername") || (this.servername = json.servername);
    !json.hasOwnProperty("filename") || (this.filename = json.filename);
    !json.hasOwnProperty("bAppOnly") || (this.bAppOnly = json.bAppOnly);
    !json.hasOwnProperty("host") || (this.host = json.host);
    !json.hasOwnProperty("setdivision") || (this.setdivision = json.setdivision);
}
tars.ConfigInfo.prototype.toBinBuffer = function () {
    var os = new TarsStream.OutputStream();
    this._writeTo(os);
    return os.getBinBuffer();
}
tars.ConfigInfo.new = function () {
    return new tars.ConfigInfo();
}
tars.ConfigInfo.create = function (is) {
    return tars.ConfigInfo._readFrom(is);
}
tars.GetConfigListInfo = function() {
    this.appname = "";
    this.servername = "";
    this.bAppOnly = false;
    this.host = "";
    this.setdivision = "";
    this.containername = "";
    this._classname = "tars.GetConfigListInfo";
};
tars.GetConfigListInfo._classname = "tars.GetConfigListInfo";

tars.GetConfigListInfo._write = function (os, tag, value) { os.writeStruct(tag, value); }
tars.GetConfigListInfo._read  = function (is, tag, def) { return is.readStruct(tag, true, def); }
tars.GetConfigListInfo._readFrom = function (is) {
    var tmp = new tars.GetConfigListInfo();
    tmp.appname = is.readString(0, true, "");
    tmp.servername = is.readString(1, false, "");
    tmp.bAppOnly = is.readBoolean(2, false, false);
    tmp.host = is.readString(3, false, "");
    tmp.setdivision = is.readString(4, false, "");
    tmp.containername = is.readString(5, false, "");
    return tmp;
};
tars.GetConfigListInfo.prototype._writeTo = function (os) {
    os.writeString(0, this.appname);
    os.writeString(1, this.servername);
    os.writeBoolean(2, this.bAppOnly);
    os.writeString(3, this.host);
    os.writeString(4, this.setdivision);
    os.writeString(5, this.containername);
};
tars.GetConfigListInfo.prototype._equal = function (anItem) {
    assert(false, 'this structure not define key operation');
}
tars.GetConfigListInfo.prototype._genKey = function () {
    if (!this._proto_struct_name_) {
        this._proto_struct_name_ = 'STRUCT' + Math.random();
    }
    return this._proto_struct_name_;
}
tars.GetConfigListInfo.prototype.toObject = function() { 
    var tmp = {};

    tmp.appname = this.appname;
    tmp.servername = this.servername;
    tmp.bAppOnly = this.bAppOnly;
    tmp.host = this.host;
    tmp.setdivision = this.setdivision;
    tmp.containername = this.containername;
    
    return tmp;
}
tars.GetConfigListInfo.prototype.readFromObject = function(json) { 
    !json.hasOwnProperty("appname") || (this.appname = json.appname);
    !json.hasOwnProperty("servername") || (this.servername = json.servername);
    !json.hasOwnProperty("bAppOnly") || (this.bAppOnly = json.bAppOnly);
    !json.hasOwnProperty("host") || (this.host = json.host);
    !json.hasOwnProperty("setdivision") || (this.setdivision = json.setdivision);
    !json.hasOwnProperty("containername") || (this.containername = json.containername);
}
tars.GetConfigListInfo.prototype.toBinBuffer = function () {
    var os = new TarsStream.OutputStream();
    this._writeTo(os);
    return os.getBinBuffer();
}
tars.GetConfigListInfo.new = function () {
    return new tars.GetConfigListInfo();
}
tars.GetConfigListInfo.create = function (is) {
    return tars.GetConfigListInfo._readFrom(is);
}

tars.ConfigProxy.prototype.ListAllConfigByInfo = function (configInfo) {
    var _encode = function () { 
        var os = new TarsStream.OutputStream();
        os.writeStruct(1, configInfo);
        return os.getBinBuffer();
    }

    var _decode = function (data) {
        try {
            var response = {arguments:{}};
            var is = new TarsStream.InputStream(data.response.sBuffer);

            response.costtime = data.request.costtime;
            response.return   = is.readInt32(0, true, TarsStream.Int32);
            response.arguments.vf = is.readList(2, true, TarsStream.List(TarsStream.String));

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

    return this._worker.tars_invoke('ListAllConfigByInfo', _encode(), arguments.length != 0?arguments[arguments.length - 1]:undefined).then(_decode, _error);
}

tars.ConfigProxy.prototype.ListConfig = function (app, server) {
    var _encode = function () { 
        var os = new TarsStream.OutputStream();
        os.writeString(1, app);
        os.writeString(2, server);
        return os.getBinBuffer();
    }

    var _decode = function (data) {
        try {
            var response = {arguments:{}};
            var is = new TarsStream.InputStream(data.response.sBuffer);

            response.costtime = data.request.costtime;
            response.return   = is.readInt32(0, true, TarsStream.Int32);
            response.arguments.vf = is.readList(3, true, TarsStream.List(TarsStream.String));

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

    return this._worker.tars_invoke('ListConfig', _encode(), arguments.length != 0?arguments[arguments.length - 1]:undefined).then(_decode, _error);
}

tars.ConfigProxy.prototype.ListConfigByInfo = function (configInfo) {
    var _encode = function () { 
        var os = new TarsStream.OutputStream();
        os.writeStruct(1, configInfo);
        return os.getBinBuffer();
    }

    var _decode = function (data) {
        try {
            var response = {arguments:{}};
            var is = new TarsStream.InputStream(data.response.sBuffer);

            response.costtime = data.request.costtime;
            response.return   = is.readInt32(0, true, TarsStream.Int32);
            response.arguments.vf = is.readList(2, true, TarsStream.List(TarsStream.String));

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

    return this._worker.tars_invoke('ListConfigByInfo', _encode(), arguments.length != 0?arguments[arguments.length - 1]:undefined).then(_decode, _error);
}

tars.ConfigProxy.prototype.checkConfig = function (appServerName, filename, host) {
    var _encode = function () { 
        var os = new TarsStream.OutputStream();
        os.writeString(1, appServerName);
        os.writeString(2, filename);
        os.writeString(3, host);
        return os.getBinBuffer();
    }

    var _decode = function (data) {
        try {
            var response = {arguments:{}};
            var is = new TarsStream.InputStream(data.response.sBuffer);

            response.costtime = data.request.costtime;
            response.return   = is.readInt32(0, true, TarsStream.Int32);
            response.arguments.result = is.readString(4, true, TarsStream.String);

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

    return this._worker.tars_invoke('checkConfig', _encode(), arguments.length != 0?arguments[arguments.length - 1]:undefined).then(_decode, _error);
}

tars.ConfigProxy.prototype.checkConfigByInfo = function (configInfo) {
    var _encode = function () { 
        var os = new TarsStream.OutputStream();
        os.writeStruct(1, configInfo);
        return os.getBinBuffer();
    }

    var _decode = function (data) {
        try {
            var response = {arguments:{}};
            var is = new TarsStream.InputStream(data.response.sBuffer);

            response.costtime = data.request.costtime;
            response.return   = is.readInt32(0, true, TarsStream.Int32);
            response.arguments.result = is.readString(2, true, TarsStream.String);

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

    return this._worker.tars_invoke('checkConfigByInfo', _encode(), arguments.length != 0?arguments[arguments.length - 1]:undefined).then(_decode, _error);
}

tars.ConfigProxy.prototype.loadConfig = function (app, server, filename) {
    var _encode = function () { 
        var os = new TarsStream.OutputStream();
        os.writeString(1, app);
        os.writeString(2, server);
        os.writeString(3, filename);
        return os.getBinBuffer();
    }

    var _decode = function (data) {
        try {
            var response = {arguments:{}};
            var is = new TarsStream.InputStream(data.response.sBuffer);

            response.costtime = data.request.costtime;
            response.return   = is.readInt32(0, true, TarsStream.Int32);
            response.arguments.config = is.readString(4, true, TarsStream.String);

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

    return this._worker.tars_invoke('loadConfig', _encode(), arguments.length != 0?arguments[arguments.length - 1]:undefined).then(_decode, _error);
}

tars.ConfigProxy.prototype.loadConfigByHost = function (appServerName, filename, host) {
    var _encode = function () { 
        var os = new TarsStream.OutputStream();
        os.writeString(1, appServerName);
        os.writeString(2, filename);
        os.writeString(3, host);
        return os.getBinBuffer();
    }

    var _decode = function (data) {
        try {
            var response = {arguments:{}};
            var is = new TarsStream.InputStream(data.response.sBuffer);

            response.costtime = data.request.costtime;
            response.return   = is.readInt32(0, true, TarsStream.Int32);
            response.arguments.config = is.readString(4, true, TarsStream.String);

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

    return this._worker.tars_invoke('loadConfigByHost', _encode(), arguments.length != 0?arguments[arguments.length - 1]:undefined).then(_decode, _error);
}

tars.ConfigProxy.prototype.loadConfigByInfo = function (configInfo) {
    var _encode = function () { 
        var os = new TarsStream.OutputStream();
        os.writeStruct(1, configInfo);
        return os.getBinBuffer();
    }

    var _decode = function (data) {
        try {
            var response = {arguments:{}};
            var is = new TarsStream.InputStream(data.response.sBuffer);

            response.costtime = data.request.costtime;
            response.return   = is.readInt32(0, true, TarsStream.Int32);
            response.arguments.config = is.readString(2, true, TarsStream.String);

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

    return this._worker.tars_invoke('loadConfigByInfo', _encode(), arguments.length != 0?arguments[arguments.length - 1]:undefined).then(_decode, _error);
}



