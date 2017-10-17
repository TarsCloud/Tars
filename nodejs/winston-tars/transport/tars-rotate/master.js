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

var path = require('path');

var TarsRotateCore = require('./core');

var timerlist = {};

exports.start = function(filename, interval, maxFiles, maxSize, concatStr) {
	if (!timerlist[filename]) {
		timerlist[filename] = setTimeout(function() {
			var dirname = path.dirname(filename), 
				extname = path.extname(filename), 
				basename = path.basename(filename, extname);

			TarsRotateCore.rotate(dirname, basename, extname, filename, maxFiles, maxSize, concatStr);
			delete timerlist[filename];
		}, interval);
	}
};

exports.close = function() {
	Object.getOwnPropertyNames(timerlist).forEach(function(filename) {
		clearTimeout(timerlist[filename]);
	});
	timerlist = {};
};