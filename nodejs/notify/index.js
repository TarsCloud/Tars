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

var assert = require('assert');

var TarsClient = require('@tars/rpc').Communicator.New();
var TarsConfigure = require('@tars/utils').Config;
var Notify = require('./NotifyFProxy');

var client;
var appName, serverName, setName;
var emptyfn = function() {};

var LEVEL = Notify.tars.NOTIFYLEVEL;

var init = function(tarsConfig) {
	var config;

	assert(tarsConfig || process.env.TARS_CONFIG, 'TARS_CONFIG is not in env and tarsConfig not defined');

	if (tarsConfig instanceof TarsConfigure) {
		config = tarsConfig;
	} else {
		if (typeof tarsConfig === 'string') {
			TarsClient.initialize(tarsConfig);
			config = new TarsConfigure();
			config.parseFile(tarsConfig);
		} else if (process.env.TARS_CONFIG) {
			config = new TarsConfigure();
			config.parseFile(process.env.TARS_CONFIG);
		}
	}

	assert(config, 'tarsConfig not instanceof (@tars/utils).Config');

	assert(config.get('tars.application.server.app') && 
		config.get('tars.application.server.server'), 'app & server not found in configure file');

	appName = config.get('tars.application.server.app');
	assert(appName, 'app not found in TARS_CONFIG(tars.application.server.app)');

	serverName = config.get('tars.application.server.server');
	assert(serverName, 'server not found in TARS_CONFIG(tars.application.server.server)');

	if (config.get('tars.application.enableset', 'n').toLowerCase() === 'y') {
		setName = config.get('tars.application.setdivision');
		assert(setName !== '..', 'setdivision format is incorrect in TARS_CONFIG(tars.application.setdivision)');
	}

	client = TarsClient.stringToProxy(Notify.tars.NotifyProxy, config.get('tars.application.server.notify', 'tars.tarsnotify.NotifyObj'));
};

exports.init = init;

exports.report = function(message, id) {
	var info, tid = id;

	if (!client) {
		init();
	}

	assert(typeof message === 'string', 'message param must be string');

	if (!tid && typeof tid !== 'string') {
		tid = process.pid;
	}

	info = new Notify.tars.ReportInfo;
	info.eType = Notify.tars.ReportType.REPORT;
	info.sApp = appName;
	info.sServer = serverName;
	info.sMessage = message;
	info.sThreadId = tid + '';

	if (setName) {
		info.sSet = setName;
	}

	client.reportNotifyInfo(info).catch(emptyfn);
};

exports.notify = function(message, level, id) {
	var info, tid = id;

	if (!client) {
		init();
	}

	assert(typeof message === 'string', 'message param must be string');

	if (!tid && typeof tid !== 'string') {
		tid = process.pid;
	}

	info = new Notify.tars.ReportInfo;
	info.eType = Notify.tars.ReportType.NOTIFY;
	info.sApp = appName;
	info.sServer = serverName;
	info.sMessage = message;
	info.sThreadId = tid + '';

	if (setName) {
		info.sSet = setName;
	}

	if (typeof level === 'number') {
		info.eLevel = level;
	} else {
		info.eLevel = LEVEL.NOTIFYNORMAL;
	}

	client.reportNotifyInfo(info).catch(emptyfn);
};

exports.LEVEL = LEVEL;