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

var Endpoint = function (sHost, iPort) {
    this.sProtocol  = "";
    this.sHost      = sHost ? sHost : "";
    this.iPort      = iPort ? iPort :  0;
    this.iTimeout   = 0;
};

// 输出Endpoint信息
Endpoint.prototype.toString = function () {
    return (this.sProtocol + " -h " + this.sHost + " -p " + this.iPort + " -t " + this.iTimeout).replace(/^[\s\t ]+|[\s\t ]+$/g, '');
};

// 拷贝Endpoint实例
Endpoint.prototype.copy = function () {
    var temp = new Endpoint();
    temp.sProtocol  = this.sProtocol;
    temp.sHost      = this.sHost;
    temp.iPort      = this.iPort;
    temp.iTimeout   = this.iTimeout;

    return temp;
};

// 从字符串中解析出Endpoint信息
Endpoint.parse = function(desc) {
    var endpoint = new Endpoint();

    var items = desc.split(" ");
    var protocol = items[0].toLowerCase();
    if (protocol != "http" && protocol != "tcp" && protocol != "udp") {
        return undefined;
    }
    endpoint.sProtocol = protocol;

    for (var ii = 1; ii < items.length; ii++) {
        if (items[ii] === "-h") {
            endpoint.sHost = items[++ii];
            continue;
        }
        if (items[ii] === "-p") {
            endpoint.iPort = parseInt(items[++ii]);
            continue;
        }
        if (items[ii] === "-t") {
            endpoint.iTimeout = parseInt(items[++ii]);
            continue;
        }
    }

    return endpoint;
};

module.exports  = Endpoint;
