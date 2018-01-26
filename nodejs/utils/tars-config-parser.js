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

var fs = require("fs");

function isType(type) {
    return function(obj) {
        return Object.prototype.toString.call(obj) === "[object " + type + "]"
    }
};

var isObject = isType("Object");
var isString = isType("String");
var isArray = isType('Array');

var Configure = function() {
    this._data = { };
    this._file = undefined;

    this.__defineGetter__("json", function() {
        return cloneObj(this._data, 'tars_config_line_vector_value');
    });
    this.__defineGetter__("data", function() {
        return cloneObj(this._data, 'tars_config_line_vector_value');
    });
};

function cloneObj(obj, filterKey) {
    var result = {};
    if (obj && isObject(obj)) {
        var keys = Object.keys(obj);
        for (var i = 0; i < keys.length; i++) {
            if (filterKey != keys[i]) {
                if (isObject(obj[keys[i]])) {
                    result[keys[i]] = cloneObj(obj[keys[i]], filterKey);
                } else {
                    result[keys[i]] = obj[keys[i]];
                }
            }
        }
    }
    return result;
};

Configure.prototype.parseText = function(sText) {
    var arr   = sText.split(/\r\n|\r|\n/); //行划分
    var stack = [this._data];
    var key = '', value= '';

    for (var i = 0, len = arr.length; i < len; i++) {
        var line = arr[i].replace(/^[\s\t ]+|[\s\t ]+$/g, '');
        if (line.length == 0 || line[0] == "#") {
            continue;
        }

        // 当前域下面的值,例如：格式为a="b=2"或者a='b=2'或者a=b=2应该解析为key为a，value为b=2
        if (line[0] != "<") {
            var index = line.indexOf('=');
            if (index == -1) {
                key = line;
                value = undefined;
            } else {
                key = line.slice(0, index);
                value = line.slice(index + 1);
                value = value.replace(/^[\s\t ]+|[\s\t ]+$/g, '');
            }

            key = key.replace(/^[\s\t ]+|[\s\t ]+$/g, '');

            stack[stack.length - 1][key] = value;
            if (!stack[stack.length - 1]['tars_config_line_vector_value']) {
                stack[stack.length - 1]['tars_config_line_vector_value'] = [];
            }
            stack[stack.length - 1]['tars_config_line_vector_value'].push(line);

            continue;
        }

        //当前行为域的标识
        if (line[line.length - 1] != '>') {
            //域标识符有开头但没有结尾，则分析错误
            this.data = {};
            return false;
        }

        //当前行为域的结束
        if (line[1] == "/") {
            stack.pop();
            continue;
        }

        //当前行为域的开始
        key         = line.substring(1, line.length - 1);
        var parent  = stack[stack.length - 1];

        if (parent.hasOwnProperty(key)) {
            //在当前域中已经有相同名字的域
            stack.push(parent[key]);
        } else {
            parent[key] = {};
            stack.push(parent[key]);
        }
    }
    return true;
};

Configure.prototype.parseFile = function(sFilePath, encoding, callback) {
    callback = typeof(encoding) === "function" ? encoding : callback;
    encoding = typeof(encoding) === "string"   ? encoding : "utf8";
    var error = {
        code : 0,
        message : "",
        exception : undefined
    };

    try {
        if (callback === undefined) {
            this.parseText(fs.readFileSync(sFilePath, encoding));
        } else {
            this.asyncParseFile(sFilePath, encoding, callback);
        }
    } catch (e) {
        error.code      = -1;
        error.message   = "read file fault";
        error.exception = e;
    }

    this._file = sFilePath;
    if (callback && error.code != 0) {
        callback.call(null, error, this);
    }
    return error.code === 0;
};

Configure.prototype.asyncParseFile = function(sFilePath, encoding, callback) {
    var self = this;
    fs.readFile(sFilePath, encoding, function(err, data) {
        if (err) {
            throw err;
        }

        self.parseText(data);
        self._file = sFilePath;
        callback.call(null, {
            code : 0,
            message : "success",
            exception : undefined
        }, self);
    });
};

Configure.prototype.get = function(key, DEFAULT_VALUE, notClone) {
    key = key.replace(/[\s\t ]+/g, '');
    key = key.replace(/\.{2,}/g, '.');
    var ret;

    var paths = [];
    var s = 0, i = 0, finding = false, len = 0;
    for (i = 0, finding = false, len = key.length; i < len; i++) {
        if (key[i] == '.' && !finding) {
            paths.push(key.substring(s, i));
            s = i + 1;
            continue;
        }

        if (key[i] == '<') {
            s = i;
            finding = true;
            continue;
        }

        if (key[i] == '>') {
            paths.push(key.substring(s + 1, i));
            s = i + 1;
            if (key[i + 1] == '.') {
                s++;
                i++;
            }
            finding = false;
        }
    }

    if (s < key.length) {
        paths.push(key.substr(s));
    }

    var parent = this._data;

    for (i = 0, len = paths.length; i < len; i++) {
        if (!parent.hasOwnProperty(paths[i])) {
            ret = DEFAULT_VALUE;
            break;
        }

        if (i == len - 1) {
            ret =  parent[paths[i].indexOf('<') == -1 ? paths[i] : substr(1, paths[i].length - 2)];
            break;
        }

        parent = parent[paths[i]];

        if (parent && isArray(parent)) {
            if (parent.length > 0) {
                parent = parent[parent.length - 1];
            }
        }
    }

    // 如果出现相同的key，后面的覆盖前面的值
    if (isArray(ret)) {
        if (ret.length > 0) {
            ret = ret[ret.length - 1];
        }
    }

    if (isObject(ret) && !notClone) {
        ret = cloneObj(ret, 'tars_config_line_vector_value');
    }

    return ret;
};

// 获取key所对应的对象中的类型为对象的属性名数组（非递归）
Configure.prototype.getDomain = function(key, DEFAULT_VALUE) {
    var domain = this.get(key, undefined);
    if (domain == undefined) {
        return DEFAULT_VALUE;
    }

    var maps = [];
    if (domain instanceof Object) {
        for (var key in domain) {
            if (domain[key] instanceof Object && key !== 'tars_config_line_vector_value') {
                maps.push(key);
            }
        }
    }

    return maps;
};

Configure.prototype.getDomainLine = function(key, DEFAULT_VALUE) {
    var obj = this.get(key, '', true);
    var ret = [];
    var result;
    if (obj && isObject(obj)) {
        result = obj['tars_config_line_vector_value'];
        if (isArray(result)) {
            ret = result;
        }
    }

    if (ret.length == 0) {
        ret = DEFAULT_VALUE;
    }

    return ret;
};

// 获取key所对应的对象中的类型为对象的属性值数组（非递归）
Configure.prototype.getDomainValue = function(key, DEFAULT_VALUE) {
    var arr = [];
    var keys = this.getDomain(key, []);
    for (var i = 0; i < keys.length; i++) {
        var k = keys[i];
        var v = this.get(key + '.<' + k + '>', undefined);
        if (v) {
            arr.push(v);
        }
    }

    return arr;
};

module.exports = Configure;