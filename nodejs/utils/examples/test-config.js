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

var Configure = require('../index').Config;

var configure = new Configure();
configure.parseFile('./config.conf', 'utf8');

var data = configure.data;

console.log('### get: tars.application.server.localip: ', configure.get('tars.application.server.localip'));

console.log('### getDomain: tars.application.server: ', configure.getDomain('tars.application.server'));
console.log('### getDomainValue: tars.application.server: ', configure.getDomainValue('tars.application.server'));

console.log('=== data ===', data.tars.application.server);
