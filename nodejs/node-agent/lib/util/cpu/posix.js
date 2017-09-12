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
var fs = require('fs');

var cpuinfo = [];

exports.init = function(cb) {
	fs.readFile('/proc/cpuinfo', {encoding : 'ascii'}, function(err, data) {
		var result = {};

		if (err) {
			cb(new Error('get /proc/cpuinfo failed'));
			return;
		}

		data.split('\n').forEach(function(line) {
			line = line.replace(/\t/g, '');
			
			var parts = line.split(':');
			if (parts.length === 2) {
				result[parts[0].replace(/\s/g, '_')] = parts[1].trim().split(' ', 1)[0];
			}

			if(line.length < 1) {
				cpuinfo.push(result);
				result = {};
			}
		});

		cpuinfo.pop();

		if (cpuinfo.length === 0) {
			cb(new Error('parse cpuinfo data err'));
		} else {
			cb();
		}
	});
};

exports.__defineGetter__('physicalCores', function() {
	var count = 0, phyidList = [];

	assert(cpuinfo.length !== 0, 'init MUST BE called first!');

	cpuinfo.forEach(function(info) {
		if (phyidList.indexOf(info['physical_id']) === -1) {
			count += parseInt(info['cpu_cores']);
			phyidList.push(info['physical_id']);
		}
	});

	return count;
});

exports.__defineGetter__('totalCores', function() {
	assert(cpuinfo.length !== 0, 'init MUST BE called first!');

	return cpuinfo.length;
});
