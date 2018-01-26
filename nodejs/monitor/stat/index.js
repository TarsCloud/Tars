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

var TarsRpc = require('@tars/rpc');
var TarsConfigure = require('@tars/utils').Config;
var TarsStream = require('@tars/stream');

var StatF = require('./StatFProxy');

var TYPE = {
	SUCCESS : 'success',
	ERROR : 'error',
	TIMEOUT : 'timeout'
};

var emptyfn = function() {};

var increaseMap = function(map, key) {
	map.set(key, map.get(key) + 1);
};

var data = {}, timer_id, client;

var ranges = [5, 10, 50, 100, 200, 500, 1000, 2000, 3000];

var ReportObj = function(headers) {
	var ths = this;

	this._StatMicMsgHead = new StatF.tars.StatMicMsgHead();
	this._StatMicMsgHead.masterName = headers.masterName;
	this._StatMicMsgHead.slaveName = headers.slaveName;
	this._StatMicMsgHead.interfaceName = headers.interfaceName;
	this._StatMicMsgHead.masterIp = headers.masterIp;
	this._StatMicMsgHead.slaveIp = headers.slaveIp;
	this._StatMicMsgHead.slavePort = headers.slavePort;
	if (isFinite(headers.returnValue)) {
		this._StatMicMsgHead.returnValue = parseInt(headers.returnValue);
	} else {
		this._StatMicMsgHead.returnValue = 0;
	}

	if (typeof headers.slaveSetName === 'string') {
		this._StatMicMsgHead.slaveSetName = headers.slaveSetName;
	}
	if (typeof headers.slaveSetArea === 'string') {
		this._StatMicMsgHead.slaveSetArea = headers.slaveSetArea;
	}
	if (typeof headers.slaveSetID === 'string') {
		this._StatMicMsgHead.slaveSetID = headers.slaveSetID;
	}
	if (typeof headers.masterSetInfo === 'string') {
		this._StatMicMsgHead.sMasterSetInfo = headers.masterSetInfo;
	}
	this._StatMicMsgHead.iStatVer = 2;

	this._StatMicMsgBody = new StatF.tars.StatMicMsgBody();
	this._StatMicMsgBody.count = 0
	this._StatMicMsgBody.timeoutCount = 0;
	this._StatMicMsgBody.execCount = 0;

	this._StatMicMsgBody.intervalCount = new TarsStream.Map(TarsStream.Int32, TarsStream.Int32);
	ranges.forEach(function(range) {
		ths._StatMicMsgBody.intervalCount.put(range, 0);
	});
	this._StatMicMsgBody.totalRspTime = 0;
	this._StatMicMsgBody.maxRspTime = 0;
	this._StatMicMsgBody.minRspTime = 0;
 
	this._bFromClient = headers.bFromClient;
};
ReportObj.prototype.add = function(type, time) {
	var ths = this;

	switch(type) {
		case TYPE.SUCCESS : {
			this._StatMicMsgBody.count += 1;
			break;
		}
		case TYPE.TIMEOUT : {
			this._StatMicMsgBody.timeoutCount += 1;
			return;
		}
		default:
		case TYPE.ERROR : {
			this._StatMicMsgBody.execCount += 1;
			return;
		}
	}

	if (ranges.some(function(range) {
		if (time <= range) {
			increaseMap(ths._StatMicMsgBody.intervalCount, range);
			return true;
		}
	})) {
		increaseMap(ths._StatMicMsgBody.intervalCount, ranges[ranges.length - 1]);
	}

	this._StatMicMsgBody.maxRspTime = Math.max(this._StatMicMsgBody.maxRspTime, time);
	this._StatMicMsgBody.minRspTime = Math.min(this._StatMicMsgBody.minRspTime, time);
	this._StatMicMsgBody.totalRspTime += time;
};

var task = function() {
	var msgList = {
		fromClient : new TarsStream.Map(StatF.tars.StatMicMsgHead, StatF.tars.StatMicMsgBody),
		fromServer : new TarsStream.Map(StatF.tars.StatMicMsgHead, StatF.tars.StatMicMsgBody)
	};

	if (!client) {
		client = TarsRpc.client.stringToProxy(StatF.tars.StatFProxy, exports.StatObj);
	}

	Object.getOwnPropertyNames(data).forEach(function(key) {
		msgList[data[key]._bFromClient ? 'fromClient' : 'fromServer'].put(data[key]._StatMicMsgHead, data[key]._StatMicMsgBody);
	});

	data = {};
	timer_id = undefined;

	if (msgList.fromClient.size() > 0) {
		client.reportMicMsg(msgList.fromClient, true, {packetType : 1}).catch(emptyfn);
	}

	if (msgList.fromServer.size() > 0) {
		client.reportMicMsg(msgList.fromServer, false, {packetType : 1}).catch(emptyfn);
	}
};

exports.StatObj = 'tars.tarsstat.StatObj';
exports.reportIntervalMin = 10 * 1000;
exports.reportInterval = 10 * 1000;

exports.init = function(obj) {
	var TarsConfig, reportInterval;

	obj = obj || process.env.TARS_CONFIG;

	if (obj) {
		if (typeof obj === 'string') {
			TarsRpc.client.initialize(obj);
			TarsConfig = new TarsConfigure();
			TarsConfig.parseFile(obj);
			obj = TarsConfig.json;
		}

		if (typeof obj === 'object') {
			exports.StatObj = obj.tars.application.client.stat;
			reportInterval = parseInt(obj.tars.application.client['report-interval']);
			if (!isNaN(reportInterval) && reportInterval >= exports.reportIntervalMin) {
				exports.reportInterval = reportInterval;
			}
		}
	}
};

exports.report = function(headers, type, timeout) {
	var key;

	if (type === TYPE.SUCCESS) {
		assert(Number.isFinite(timeout) && timeout >= 0, 'timeout is Not a Number or less then 0');
	}

	key = Object.getOwnPropertyNames(headers).map(function(key) {
		return headers[key];
	}).join('.');
	
	data[key] = data[key] || new ReportObj(headers);
	data[key].add(type, timeout);

	if (!timer_id) {
		timer_id = setTimeout(task, exports.reportInterval);
	}

	return data[key];
};

exports.stop = function() {
	if (timer_id) {
		clearTimeout(timer_id);
		timer_id = undefined;
		data = {};
	}
};

exports.TYPE = TYPE;