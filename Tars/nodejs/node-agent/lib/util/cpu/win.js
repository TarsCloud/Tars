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

var wmic = require('wmic');

var physicalCores, totalCores;

exports.init = function(cb) {
	wmic.get_values('cpu', 'NumberOfCores, NumberOfLogicalProcessors', null, function(err, result) {
		if (err) {
			cb(new Error('exec wmic failed'));
			return;
		}

		physicalCores = parseInt(result.NumberOfCores);
		totalCores = parseInt(result.NumberOfLogicalProcessors);

		if (isNaN(physicalCores) || isNaN(totalCores) || physicalCores <= 0 || totalCores <= 0) {
			cb(new Error('parse cpuinfo data err'));
		} else {
			cb();
		}
	});	
};

exports.__defineGetter__('physicalCores', function() {
	assert(physicalCores > 0, 'init MUST BE called first!');

	return physicalCores;
});

exports.__defineGetter__('totalCores', function() {
	assert(totalCores > 0, 'init MUST BE called first!');

	return totalCores;
});