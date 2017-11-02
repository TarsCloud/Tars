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

var fs = require('fs'),
	path = require('path');

var async = require('async');

exports.rotate = function(dirname, basename, extname, filename, maxFiles, maxSize, concatStr, cb) {
	fs.stat(filename, function(err, stats) {
		if (err) {
			if (err.code !== 'ENOENT') {
				cb && cb(err);
				return;
			}
		}

		if (stats && stats.size > maxSize) {
			fs.unlink(path.join(dirname, basename + '_' + (maxFiles - 1) + extname), function() {
				var i, list = [];
				for (i = maxFiles - 2; i > 0; i -= 1) {
					list.push({
						oldPath : path.join(dirname, basename + concatStr + i + extname),
						newPath : path.join(dirname, basename + concatStr + (i + 1) + extname)
					});
				}

				async.eachSeries(list, function(item, callback) {
					fs.rename(item.oldPath, item.newPath, function() {
						callback();
					});
				}, function() {
					fs.rename(filename, path.join(dirname, basename + concatStr + '1' + extname), function() {
						cb && cb(null, true);
					});
				});
			});
		} else {
			cb && cb(null, false);
		}
	});
};