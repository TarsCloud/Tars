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

var tarsNotify = require('@tars/notify');

var initialized = false;

var init = function(config) {
	if (config) {
		tarsNotify.init(config);
		initialized = true;
	}
};

var report = function(level, message, id) {
	if (initialized) {
		if (level === tarsNotify.LEVEL.NOTIFYERROR) {
			message = '[alarm] ' + message;
		}
		tarsNotify.report(message, id);
	}
};

exports.init = init;
exports.report = {
	error : function(message, id) {
		report(tarsNotify.LEVEL.NOTIFYERROR, message, id);
	},
	info : function(message, id) {
		report(tarsNotify.LEVEL.NOTIFYNORMAL, message, id);
	}
};