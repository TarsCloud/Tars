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

var assert = require('assert');

var Base = function(logPattern, name, interval, timePattern) {
	if (!(this instanceof Base)) {
		return new Base(interval, logPattern, timePattern);
	}

	this.logPattern = logPattern;
	this.name = name;

	if (interval && timePattern) { 
		this.interval = parseInt(interval);
		assert(this.interval > 0, 'interval must be large than 0');

		this.timePattern = timePattern;
	}
};

var LogByDay = function(interval, pattern) {
	return new Base(pattern || '%Y%m%d', 'day', interval || 1, '%Y%m%d');
};

var LogByHour = function(interval, pattern) {
	assert(!interval || (interval < 24 && interval > 0), 'interval can\'t be large than 24, less than 0');
	
	return new Base(pattern || '%Y%m%d%H', 'hour', interval || 1, '%Y%m%d%H');
};

var LogByMinute = function(interval, pattern) {
	assert(!interval || interval < 60 && interval > 0, 'interval can\'t be large than 60, less than 0');
	return new Base(pattern || '%Y%m%d%H%M', 'minute', interval || 10, '%Y%m%d%H%M');
};

var LogByCustom = function(pattern) {
	assert(pattern, 'pattern not defined');
	return new Base(pattern, 'custom');
};

exports.LogByDay = LogByDay;
exports.LogByHour = LogByHour;
exports.LogByMinute = LogByMinute;
exports.LogByCustom = LogByCustom;