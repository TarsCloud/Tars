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

var common = require('winston/lib/winston/common');

var strftime = require('strftime');

exports.Detail = function(opt) {
	var separ = '|';

	opt = opt || {};

	if (opt.separ) {
		separ = opt.separ;
	}

	return function(options) {
		var meta = options.meta, output = '';

		if (options.prefix) {
			output += options.prefix + separ;
		}

		output += strftime('%Y-%m-%d %H:%M:%S') + separ;

		if (meta && meta.pid) {
			output += meta.pid + separ;
			delete meta.pid;
		} else {
			output += process.pid + separ;
		}

		output += options.level.toUpperCase() + separ;

		if (meta && meta.hasOwnProperty('lineno')) {
			if (typeof meta.lineno === 'string' && meta.lineno !== '') {
				output += meta.lineno + separ;
			}
			delete meta.lineno;
		}

		output += options.message;

		if (meta) {
			if (meta instanceof Error && meta.stack) {
				meta = meta.stack;
			}

			if (typeof meta !== 'object') {
				output += separ + meta;
			} else if (Object.keys(meta).length > 0) {
				output += separ + common.serialize(meta);
			}
		}

		return output;
	};
};

exports.Simple = function(opt) {
	var separ = '|';

	opt = opt || {};

	if (opt.separ) {
		separ = opt.separ;
	}

	return function(options) {
		var meta = options.meta, output = '';

		if (options.prefix) {
			output += options.prefix + separ;
		}

		output += strftime('%Y-%m-%d %H:%M:%S') + separ;

		output += options.message;

		if (meta) {
			if (meta.hasOwnProperty('pid')) {
				delete meta.pid;
			}
			if (meta.hasOwnProperty('lineno')) {
				delete meta.lineno;
			}

			if (meta instanceof Error && meta.stack) {
				meta = meta.stack;
			}

			if (typeof meta !== 'object') {
				output += separ + meta;
			} else if (Object.keys(meta).length > 0) {
				output += separ + common.serialize(meta);
			}
		}

		return output;
	};
};