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

var TarsClient = require('@tars/rpc').Communicator.New();
var NodeF = require('./NodeFProxy');

var constants = require('../../constants');

var serverInfo;
var timer_id, client, emptyfn = function() {};

exports.init = function(name, connString) {
	var name = name.split('.');

	serverInfo = new NodeF.tars.ServerInfo();

	if (name.length > 1) {
		serverInfo.application = name[0];
		serverInfo.serverName = name[1];
	} else {
		serverInfo.application = constants.TARS_DEFAULT_APP;
		serverInfo.serverName = name[0];
	}
	serverInfo.pid = process.pid;

	client = TarsClient.stringToProxy(NodeF.tars.ServerFProxy, connString);
};

exports.keepAlive = function(interval) {
	if (!timer_id) {
		timer_id = setInterval(function() {
			client && client.keepAlive(serverInfo).catch(emptyfn);
		}, interval || constants.TARS_HEART_BEAT_INTERVAL);
		client.keepAlive(serverInfo).catch(emptyfn);
	}
};

exports.destroy = function() {
	if (timer_id) {
		clearInterval(timer_id);
		timer_id = undefined;
	}

	if (client) {
		TarsClient.disconnect();
		client = undefined;
	}

	serverInfo = undefined;
};

exports.reportVersion = function(version) {
	client && client.reportVersion(serverInfo.application, serverInfo.serverName, version).catch(emptyfn);
};