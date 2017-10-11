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

var util = require('util');
var path = require('path');

var readInstalled = require("read-installed");

exports.list = function(cb) {
	readInstalled(path.join(__dirname, '../../'), {depth : 1}, function(err, data) {
		if (err) {
			cb(err);
			return;
		}

		cb(null, Object.getOwnPropertyNames(data.dependencies).map(function(name) {
			var deps = data.dependencies[name], depsStr;

			if (deps.dependencies) {
				depsStr = Object.getOwnPropertyNames(deps.dependencies).map(function(name) {
					return name + '@' + deps.dependencies[name].version;
				}).join(', ');
			}

			return util.format('%s@%s%s', name, deps.version, depsStr ? ' (' + depsStr + ')' : ''); 
		}).join(', '));
	});
};