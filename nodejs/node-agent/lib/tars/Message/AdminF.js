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
var TarsError  = require('@tars/rpc').error;

var tars = tars || {};
module.exports.tars = tars;


tars.AdminFImp = function () { 
    this._name   = undefined;
    this._worker = undefined;
}

tars.AdminFImp.prototype.initialize = function () { 
    //TODO:
}

tars.AdminFImp.prototype.onDispatch = function (current, FuncName, BinBuffer) { 
    switch (FuncName) {
        case 'tars_ping': {
                current.sendResponse = function (_ret) {
                        if (current.getRequestVersion() === TarsStream.Tup.TUP_SIMPLE || current.getRequestVersion() === TarsStream.Tup.TUP_COMPLEX){
                                var tup = new TarsStream.UniAttribute();
                                tup.tupVersion = current.getRequestVersion();
                                tup.writeInt32("", _ret);
                                current.doResponse(tup.encode());
                        }else{
                                var os = new TarsStream.OutputStream();
                                os.writeInt32(0, _ret);
                                current.doResponse(os.getBinBuffer());
                        }
                }
                        current.sendResponse(0);
                return TarsError.SUCCESS;
        }
        case 'shutdown' : {
            current.sendResponse = function () {

                current.doResponse(new TarsStream.BinBuffer());
            }

            this.shutdown(current);

            return TarsError.SUCCESS;
        }
        case 'notify' : {
            if (current.getRequestVersion() === TarsStream.Tup.TUP_SIMPLE || current.getRequestVersion() === TarsStream.Tup.TUP_COMPLEX) {
                var tup = new TarsStream.UniAttribute();
                tup.tupVersion = current.getRequestVersion();
                tup.decode(BinBuffer);
                var command = tup.readString("command");
            } else {
                var is = new TarsStream.InputStream(BinBuffer);
                var command = is.readString(1, true);
            }
            current.sendResponse = function (_ret) {
                if (current.getRequestVersion() === TarsStream.Tup.TUP_SIMPLE || current.getRequestVersion() === TarsStream.Tup.TUP_COMPLEX) {
                    var tup = new TarsStream.UniAttribute();
                    tup.tupVersion = current.getRequestVersion();
                    tup.writeString("", _ret);

                    current.doResponse(tup.encode());
                } else {
                    var os = new TarsStream.OutputStream();
                    os.writeString(0, _ret);

                    current.doResponse(os.getBinBuffer());
                }
            }

            this.notify(current, command);

            return TarsError.SUCCESS;
        }
    }
    return TarsError.SERVER.FUNC_NOT_FOUND;
}

tars.AdminFImp.prototype.shutdown = function (current) {
    //TODO:
    assert.fail('shutdown function not implemented');
}

tars.AdminFImp.prototype.notify = function (current, command) {
    //TODO:
    assert.fail('notify function not implemented');
}





