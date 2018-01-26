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

var PropertyF = require('./PropertyFProxy');

exports.PropertyObj = 'tars.tarsproperty.PropertyObj';
exports.reportInterval = 10 * 1000;

var emptyfn = function() {};

var data = {}, timer_id, client, msgHeaders;

var ReportObj = function(headers, policies) {
	var ths = this;

	this.policies = Array.isArray(policies) ? policies : [policies];
	this.StatPropMsgHead = new PropertyF.tars.StatPropMsgHead();
	Object.getOwnPropertyNames(headers).forEach(function(name) {
		ths.StatPropMsgHead[name] = headers[name];
	});
};
ReportObj.prototype.report = function(value) {
	this.policies.forEach(function(policy) {
		policy.add(value);
	});

	if (!timer_id) {
		timer_id = setTimeout(task, exports.reportInterval);
	}
};
ReportObj.prototype.get = function() {
	var StatPropMsgBody = new PropertyF.tars.StatPropMsgBody();

	this.policies.forEach(function(policy) {
		var value = policy.get(), StatPropInfo;
		if (value !== undefined) {
			StatPropInfo = new PropertyF.tars.StatPropInfo();
			StatPropInfo.policy = policy.name;
			StatPropInfo.value = value.toString();
			StatPropMsgBody.vInfo.push(StatPropInfo);
		}
	});

	if (StatPropMsgBody.vInfo.length > 0) {
		return {
			StatPropMsgHead : this.StatPropMsgHead,
			StatPropMsgBody : StatPropMsgBody
		};
	} else {
		return null;
	}
};

var task = function() {
	var statmsg = new TarsStream.Map(PropertyF.tars.StatPropMsgHead, PropertyF.tars.StatPropMsgBody);

	if (!client) {
		client = TarsRpc.client.stringToProxy(PropertyF.tars.PropertyFProxy, exports.PropertyObj);
	}

	Object.getOwnPropertyNames(data).map(function(key) {
		var obj = data[key].get();
		if (obj) {
			statmsg.put(obj.StatPropMsgHead, obj.StatPropMsgBody);
		}
	});

	if (statmsg.size() > 0) {
		client.reportPropMsg(statmsg, {packetType : 1}).catch(emptyfn);
	}

	timer_id = undefined;
};

var init = function(obj) {
	var setdivision, tarsConfig;

	obj = obj || process.env.TARS_CONFIG;

	assert(obj, 'TARS_CONFIG is not in env and init argument is neither an Object nor a String.');
	
	if (typeof obj === 'string' && obj !== '') {
		TarsRpc.client.initialize(obj);
		tarsConfig = new TarsConfigure();
		tarsConfig.parseFile(obj);
	} else {
		tarsConfig = obj;
	}

	if (typeof tarsConfig === 'object') {
		exports.PropertyObj = tarsConfig.get('tars.application.client.property', exports.PropertyObj);

		if (!isNaN(parseInt(tarsConfig.get('tars.application.client.report-interval')))) {
			exports.reportInterval = parseInt(tarsConfig.get('tars.application.client.report-interval'));
		}

		msgHeaders = {
			moduleName : tarsConfig.get('tars.application.client.modulename', 'NO_MODULE_NAME'),
			ip : tarsConfig.get('tars.application.server.localip', '127.0.0.1'),
			iPropertyVer : 2
		};

		setdivision = tarsConfig.get('tars.application.setdivision');

		if (tarsConfig.get('tars.application.enableset', '').toLowerCase() === 'y' && setdivision && typeof setdivision === 'string') {
			setdivision = setdivision.split('.');
			if (setdivision.length >= 3) {
				msgHeaders.setName = setdivision[0];
				msgHeaders.setArea = setdivision[1];
				msgHeaders.setID = setdivision.slice(2).join('.');

				msgHeaders.moduleName += '.' + msgHeaders.setName + msgHeaders.setArea + msgHeaders.setID;
			}
		}
	} else {
		msgHeaders = {
			moduleName : 'NO_MODULE_NAME',
			ip : '127.0.0.1',
			iPropertyVer : 2
		};
	}
};

exports.init = init;

exports.create = function(obj, policies) {
	var headers = {}, key;

	if (!msgHeaders) {
		init();
	}

	Object.getOwnPropertyNames(msgHeaders).forEach(function(name) {
		headers[name] = msgHeaders[name];
	});

	if (typeof obj === 'string') {
		headers.propertyName = obj;
	} else {
		Object.getOwnPropertyNames(obj).forEach(function(name) {
			headers[name] = obj[name];
		});
	}

	headers.propertyName = headers.propertyName || 'NO_PROPERTY_NAME';

	key = Object.getOwnPropertyNames(headers).map(function(name) {
		return headers[name];
	}).join('.');
	
	data[key] = data[key] || new ReportObj(headers, policies);

	return data[key];
};

exports.POLICY = require('../policy');