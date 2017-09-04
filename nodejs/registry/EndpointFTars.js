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

var assert     = require('assert');
var TarsStream = require('@tars/stream');


var tars = tars || {};
module.exports.tars = tars;

tars.EndpointF = function() {
    this.host = "";
    this.port = 0;
    this.timeout = 0;
    this.istcp = 0;
    this.grid = 0;
    this.groupworkid = 0;
    this.grouprealid = 0;
    this.setId = "";
    this.qos = 0;
    this.bakFlag = 0;
    this.weight = 0;
    this.weightType = 0;
    this._classname = "tars.EndpointF";
};
tars.EndpointF._classname = "tars.EndpointF";

tars.EndpointF._write = function (os, tag, value) { os.writeStruct(tag, value); }
tars.EndpointF._read  = function (is, tag, def) { return is.readStruct(tag, true, def); }
tars.EndpointF._readFrom = function (is) {
    var tmp = new tars.EndpointF();
    tmp.host = is.readString(0, true, "");
    tmp.port = is.readInt32(1, true, 0);
    tmp.timeout = is.readInt32(2, true, 0);
    tmp.istcp = is.readInt32(3, true, 0);
    tmp.grid = is.readInt32(4, true, 0);
    tmp.groupworkid = is.readInt32(5, false, 0);
    tmp.grouprealid = is.readInt32(6, false, 0);
    tmp.setId = is.readString(7, false, "");
    tmp.qos = is.readInt32(8, false, 0);
    tmp.bakFlag = is.readInt32(9, false, 0);
    tmp.weight = is.readInt32(11, false, 0);
    tmp.weightType = is.readInt32(12, false, 0);
    return tmp;
};
tars.EndpointF.prototype._writeTo = function (os) {
    os.writeString(0, this.host);
    os.writeInt32(1, this.port);
    os.writeInt32(2, this.timeout);
    os.writeInt32(3, this.istcp);
    os.writeInt32(4, this.grid);
    os.writeInt32(5, this.groupworkid);
    os.writeInt32(6, this.grouprealid);
    os.writeString(7, this.setId);
    os.writeInt32(8, this.qos);
    os.writeInt32(9, this.bakFlag);
    os.writeInt32(11, this.weight);
    os.writeInt32(12, this.weightType);
};
tars.EndpointF.prototype._equal = function (anItem) {
    return this.host === anItem.host && 
        this.port === anItem.port && 
        this.timeout === anItem.timeout && 
        this.istcp === anItem.istcp && 
        this.grid === anItem.grid && 
        this.qos === anItem.qos && 
        this.weight === anItem.weight && 
        this.weightType === anItem.weightType;
}
tars.EndpointF.prototype._genKey = function () {
    if (!this._proto_struct_name_) {
        this._proto_struct_name_ = 'STRUCT' + Math.random();
    }
    return this._proto_struct_name_;
}
tars.EndpointF.prototype.toObject = function() { 
    var tmp = {};

    tmp.host = this.host;
    tmp.port = this.port;
    tmp.timeout = this.timeout;
    tmp.istcp = this.istcp;
    tmp.grid = this.grid;
    tmp.groupworkid = this.groupworkid;
    tmp.grouprealid = this.grouprealid;
    tmp.setId = this.setId;
    tmp.qos = this.qos;
    tmp.bakFlag = this.bakFlag;
    tmp.weight = this.weight;
    tmp.weightType = this.weightType;
    
    return tmp;
}
tars.EndpointF.prototype.readFromObject = function(json) { 
    !json.hasOwnProperty("host") || (this.host = json.host);
    !json.hasOwnProperty("port") || (this.port = json.port);
    !json.hasOwnProperty("timeout") || (this.timeout = json.timeout);
    !json.hasOwnProperty("istcp") || (this.istcp = json.istcp);
    !json.hasOwnProperty("grid") || (this.grid = json.grid);
    !json.hasOwnProperty("groupworkid") || (this.groupworkid = json.groupworkid);
    !json.hasOwnProperty("grouprealid") || (this.grouprealid = json.grouprealid);
    !json.hasOwnProperty("setId") || (this.setId = json.setId);
    !json.hasOwnProperty("qos") || (this.qos = json.qos);
    !json.hasOwnProperty("bakFlag") || (this.bakFlag = json.bakFlag);
    !json.hasOwnProperty("weight") || (this.weight = json.weight);
    !json.hasOwnProperty("weightType") || (this.weightType = json.weightType);
}
tars.EndpointF.prototype.toBinBuffer = function () {
    var os = new TarsStream.OutputStream();
    this._writeTo(os);
    return os.getBinBuffer();
}
tars.EndpointF.new = function () {
    return new tars.EndpointF();
}
tars.EndpointF.create = function (is) {
    return tars.EndpointF._readFrom(is);
}


