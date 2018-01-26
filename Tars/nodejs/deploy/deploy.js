#!/usr/bin/env node

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

'use strict'

var fs = require('fs');
var path = require('path');
var events = require('events');
var spawn = require('child_process').spawn;
var zlib = require('zlib');

var async = require('async');
var fse = require('fs-extra');
var fstream = require('fstream');
var tar = require('tar');

module.exports = exports = new events();

var tmpName = '';

var config = exports.config =  {
	exclude : ['.svn', '.git', '_svn', '_git', '.tgz', '_tmp_dir', '.idea'],
	level : 6,
	memLevel : 6,
	maxBuffer : 500 * 1024
};

var execNPM = function(command, cwd, options, cb) {
	fs.exists(path.join(cwd, 'package.json'), function(exists) {
		var npm;

		command = command.trim().split(' ');
		if (!exists && command.length <= 1) {
			return cb();
		}

		npm = spawn(process.platform === 'win32' ? 'npm.cmd' : 'npm', command, {cwd : cwd,  stdio: 'inherit'});

		npm.on('exit', function(code) {
			var err;

			if (code !== 0) {
				err = new Error('npm return code: "' + code + '"');
			}

			fs.unlink(path.join(cwd, 'npm-debug.log'), function() {
				cb(err);
			});
		});
	});
};

// 初始化目录结构
var mkdir = function(name, dir, cb) {
	exports.emit('progress:start', 'Creating directory structure');

	fs.stat(dir, function(err, stat) {
		tmpName = '_' + name + '_' + Date.now();

		if (err || !stat.isDirectory()) {
			cb(new Error('Not a directory'));
			return;
		}

		fs.mkdir(path.join(dir, tmpName), function(err) {
			if (err) {
				cb(err);
				return;
			}
			fs.readdir(dir, function(err, files) {
				if (err) {
					cb(err);
					return;
				}

				async.mapSeries([
					path.join(dir, tmpName, name),
					path.join(dir, tmpName, name, name),
					path.join(dir, tmpName, name, name, 'src'),
					path.join(dir, tmpName, name, name, 'tars_nodejs'),
					path.join(dir, tmpName, name, name, 'tars_nodejs', 'node-agent'),
					path.join(dir, tmpName, name, name, 'tars_nodejs', 'node-agent', 'node_modules')
				], fs.mkdir.bind(fs), function(err) {
					if (err) {
						cb(err);
						return;
					}

					async.map(files.filter(function(file) {
						return file !== tmpName;
					}).map(function(file) {
						return [path.join(dir, file), path.join(dir, tmpName, name, name, 'src', file)];
					}), function(item, cb) {
						fse.copy(item[0], item[1], cb);
					}, function(err) {
						if (err) {
							cb(err);
						} else {
							exports.emit('progress:end', 'Created directory');
							cb();
						}
					});
				
				});
			});
		});
	});
};

// 拷贝 node 可执行文件
var cp = function(name, dir, cb) {
	exports.emit('progress:start', 'Copying node exec file');

	fse.copy(process.execPath, path.join(dir, tmpName, name, name, 'tars_nodejs', 'node'), function(err) {
		if (err) {
			cb(err);
		} else {
			exports.emit('progress:end', 'Copied file');
			cb();
		}
	});
};

// 安装 node-agent
var install = function(name, dir, cb) {
	exports.emit('progress:start', 'Installing node-agent');

	var cwd = path.join(dir, tmpName, name, name, 'tars_nodejs', 'node-agent');

	execNPM('install --global-style @tars/node-agent', cwd, null, function(err, stdout, stderr) {
		if (err) {
			cb(err, stdout, stderr);
			return;
		}

		fs.exists(path.join(cwd, 'node_modules', '@tars', 'node-agent'), function(exists) {
			if (!exists) {
				cb(true, stdout, stderr);
				return;
			}

			fs.rename(path.join(cwd, 'node_modules', '@tars', 'node-agent'), cwd + '2', function(err) {
				if (err) {
					cb(true, stdout, stderr);
					return;
				}
				fse.remove(cwd, function(err) {
					if (err) {
						cb(true, stdout, stderr);
						return;
					}
					fs.rename(cwd + '2', cwd, function(err) {
						if (err) {
							cb(true, stdout, stderr);
						} else {
							exports.emit('progress:end', 'Installed node-agent');
							cb(null, stdout, stderr);
						}
					});
				});
			});
		});
	});
};

// 安装 src 中的依赖项
var init = function(name, dir, cb) {
	exports.emit('progress:start', 'Installing dependency');

	var cwd = path.join(dir, tmpName, name, name, 'src');
	
	fs.exists(path.join(cwd, 'package.json'), function(exists) {
		if (!exists) {
			exports.emit('progress:end', 'Not found package.json');
			return cb();
		}

		execNPM('install --production', cwd, null, function(err, stdout, stderr) {
			if (!err) {
				exports.emit('progress:end', 'Installed dependency');
			}

			cb(err, stdout, stderr);
		});
	});
};

// 重新编译
var rebuild = function(name, dir, cb) {
	exports.emit('progress:start', 'Building C/C++ modules');

	var cwd = path.join(dir, tmpName, name, name, 'src');
	execNPM('rebuild', cwd, null, function(err, stdout, stderr) {
		if (err) {
			cb(err, stdout, stderr);
			return;
		}

		fs.exists(path.join(cwd, 'binding.gyp'), function(exists) {
			var gyp;

			if (!exists) {
				exports.emit('progress:end', 'Not found C/C++ modules');
				return cb();
			}

			gyp = spawn(process.platform === 'win32' ? 'node-gyp.cmd' : 'node-gyp', ['rebuild'], {cwd : cwd,  stdio: 'inherit'});

			gyp.stdout.pipe(process.stdout);
			gyp.stderr.pipe(process.stderr);

			gyp.on('exit', function(code) {
				var err;

				if (code !== 0) {
					err = new Error('node-gyp return code: "' + code + '"');
				} else {
					exports.emit('progress:end', 'Built C/C++ modules');
				}

				cb(err);
			});
		});
	});
};

// 检查依赖状态
var check = function(name, dir, cb) {
	exports.emit('progress:start', 'Checking outdated dependency');

	execNPM('outdated', path.join(dir, tmpName, name, name, 'src'), null, function(err, stdout, stderr) {
		if (!err) {
			exports.emit('progress:end', 'Checked outdated dependency');
		}

		cb(err, stdout, stderr);
	});
};

// 生成tar.gz, tgz包
var pack = function(name, dir, cb) {
	exports.emit('progress:start', 'Making deploy package');

	var dirDest = fs.createWriteStream(path.join(dir, name + '.tgz')),
		packer = tar.Pack({
			noProprietary: true
		}),
		gzip = zlib.createGzip({
			level: config.level,
			memLevel: config.memLevel
		}),
		reader = fstream.Reader({
			path: path.join(dir, tmpName, name),
			type: "Directory",
			filter : function(entry) {
				if (entry.props.Directory || path.extname(entry.props.basename) === '') {
					entry.props.mode = 493; // 0755
				}

				return !config.exclude.some(function(name) {
					if (entry.props.basename.indexOf(name) !== -1) {
						return entry.props.basename.indexOf(name) === entry.props.basename.length - name.length;
					} else {
						return false;
					}
				});
			}
		}),
		complete = function(err) {
			if (!err) {
				exports.emit('progress:end', 'Made deploy package');
			}

			cb(err);
		};

	reader.on('error', complete);
	packer.on('error', complete);
	gzip.on('error', complete);
	dirDest.on('error', complete);

	dirDest.on('close', complete);

	reader.pipe(packer).pipe(gzip).pipe(dirDest);
};

// 删除临时文件
var clean = function(name, dir, cb) {
	exports.emit('progress:start', 'Cleaning temp files');
	fse.remove(path.join(dir, tmpName), function(err) {
		if (!err) {
			exports.emit('progress:end', 'Cleaned temp files');
		}

		if (typeof cb === 'function') {
			cb(err);
		}
	});
};

exports.STEP_COUNT = 8;

exports.make = function(name, dir) {
	var wrapper = function(fn) {
		return function(callback) {
			fn(name, dir, callback);
		};
	};

	async.series([mkdir, cp, install, init, rebuild, check, pack, clean].map(function(fn) {
		return wrapper(fn);
	}), function(err, results) {
		if (err) {
			exports.emit('error', err);
			clean(name, dir);
		} else {
			exports.emit('done', path.join(dir, name + '.tgz'));
		}
	});
};