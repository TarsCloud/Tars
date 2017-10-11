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

'use strict';

var util = require("util");
var events = require("events");

var TarsServer = require('@tars/rpc').server;
var AdminF = require('./AdminFImp');

function tarsMessage() {
    events.EventEmitter.call(this);
}

util.inherits(tarsMessage, events.EventEmitter);

tarsMessage.prototype.startServer = function(name, localString) {
	var servantName = util.format('%s.%s', name, 'AdminObj'), ths = this;

	if (this._server) {
		return;
	}

	AdminF.tars.AdminFImp.prototype._mesgEvents = ths;

	this._server = TarsServer.createServer(AdminF.tars.AdminFImp);
	this._server.start({
		name : servantName + 'Adapter',
		servant : servantName,
		endpoint : localString,
		maxconns : 200000,
		protocol : 'tars'
	});
};

tarsMessage.prototype.destroy = function() {
	if (this._server) {
		this._server.stop();
		this._server = undefined;
	}
	delete AdminF.tars.AdminFImp.prototype._mesgEvents;
};

module.exports = exports = new tarsMessage();