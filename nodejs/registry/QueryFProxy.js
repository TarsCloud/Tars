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
var _TARS_MODULE_A_ = require('./EndpointFTars.js');

var tars = tars || {};
module.exports.tars = tars;

tars.QueryFProxy = function () {
    this._name   = undefined;
    this._worker = undefined;
}

tars.QueryFProxy.prototype.setTimeout = function (iTimeout) {
    this._worker.timeout = iTimeout;
}

tars.QueryFProxy.prototype.getTimeout = function ( ) {
    return this._worker.timeout;
}


tars.QueryFProxy.prototype.findObjectById = function (id) {
    var _encode = function () { 
        var os = new TarsStream.OutputStream();
        os.writeString(1, id);
        return os.getBinBuffer();
    }

    var _decode = function (data) {
        try {
            var response = {arguments:{}};
            var is = new TarsStream.InputStream(data.response.sBuffer);

            response.costtime = data.request.costtime;
            response.return   = is.readList(0, true, TarsStream.List(_TARS_MODULE_A_.tars.EndpointF));

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

    return this._worker.tars_invoke('findObjectById', _encode(), arguments.length != 0?arguments[arguments.length - 1]:undefined).then(_decode, _error);
}

tars.QueryFProxy.prototype.findObjectById4All = function (id) {
    var _encode = function () { 
        var os = new TarsStream.OutputStream();
        os.writeString(1, id);
        return os.getBinBuffer();
    }

    var _decode = function (data) {
        try {
            var response = {arguments:{}};
            var is = new TarsStream.InputStream(data.response.sBuffer);

            response.costtime = data.request.costtime;
            response.return   = is.readInt32(0, true, TarsStream.Int32);
            response.arguments.activeEp = is.readList(2, true, TarsStream.List(_TARS_MODULE_A_.tars.EndpointF));
            response.arguments.inactiveEp = is.readList(3, true, TarsStream.List(_TARS_MODULE_A_.tars.EndpointF));

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

    return this._worker.tars_invoke('findObjectById4All', _encode(), arguments.length != 0?arguments[arguments.length - 1]:undefined).then(_decode, _error);
}

tars.QueryFProxy.prototype.findObjectById4Any = function (id) {
    var _encode = function () { 
        var os = new TarsStream.OutputStream();
        os.writeString(1, id);
        return os.getBinBuffer();
    }

    var _decode = function (data) {
        try {
            var response = {arguments:{}};
            var is = new TarsStream.InputStream(data.response.sBuffer);

            response.costtime = data.request.costtime;
            response.return   = is.readInt32(0, true, TarsStream.Int32);
            response.arguments.activeEp = is.readList(2, true, TarsStream.List(_TARS_MODULE_A_.tars.EndpointF));
            response.arguments.inactiveEp = is.readList(3, true, TarsStream.List(_TARS_MODULE_A_.tars.EndpointF));

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

    return this._worker.tars_invoke('findObjectById4Any', _encode(), arguments.length != 0?arguments[arguments.length - 1]:undefined).then(_decode, _error);
}

tars.QueryFProxy.prototype.findObjectByIdInSameGroup = function (id) {
    var _encode = function () { 
        var os = new TarsStream.OutputStream();
        os.writeString(1, id);
        return os.getBinBuffer();
    }

    var _decode = function (data) {
        try {
            var response = {arguments:{}};
            var is = new TarsStream.InputStream(data.response.sBuffer);

            response.costtime = data.request.costtime;
            response.return   = is.readInt32(0, true, TarsStream.Int32);
            response.arguments.activeEp = is.readList(2, true, TarsStream.List(_TARS_MODULE_A_.tars.EndpointF));
            response.arguments.inactiveEp = is.readList(3, true, TarsStream.List(_TARS_MODULE_A_.tars.EndpointF));

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

    return this._worker.tars_invoke('findObjectByIdInSameGroup', _encode(), arguments.length != 0?arguments[arguments.length - 1]:undefined).then(_decode, _error);
}

tars.QueryFProxy.prototype.findObjectByIdInSameSet = function (id, setId) {
    var _encode = function () { 
        var os = new TarsStream.OutputStream();
        os.writeString(1, id);
        os.writeString(2, setId);
        return os.getBinBuffer();
    }

    var _decode = function (data) {
        try {
            var response = {arguments:{}};
            var is = new TarsStream.InputStream(data.response.sBuffer);

            response.costtime = data.request.costtime;
            response.return   = is.readInt32(0, true, TarsStream.Int32);
            response.arguments.activeEp = is.readList(3, true, TarsStream.List(_TARS_MODULE_A_.tars.EndpointF));
            response.arguments.inactiveEp = is.readList(4, true, TarsStream.List(_TARS_MODULE_A_.tars.EndpointF));

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

    return this._worker.tars_invoke('findObjectByIdInSameSet', _encode(), arguments.length != 0?arguments[arguments.length - 1]:undefined).then(_decode, _error);
}

tars.QueryFProxy.prototype.findObjectByIdInSameStation = function (id, sStation) {
    var _encode = function () { 
        var os = new TarsStream.OutputStream();
        os.writeString(1, id);
        os.writeString(2, sStation);
        return os.getBinBuffer();
    }

    var _decode = function (data) {
        try {
            var response = {arguments:{}};
            var is = new TarsStream.InputStream(data.response.sBuffer);

            response.costtime = data.request.costtime;
            response.return   = is.readInt32(0, true, TarsStream.Int32);
            response.arguments.activeEp = is.readList(3, true, TarsStream.List(_TARS_MODULE_A_.tars.EndpointF));
            response.arguments.inactiveEp = is.readList(4, true, TarsStream.List(_TARS_MODULE_A_.tars.EndpointF));

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

    return this._worker.tars_invoke('findObjectByIdInSameStation', _encode(), arguments.length != 0?arguments[arguments.length - 1]:undefined).then(_decode, _error);
}



