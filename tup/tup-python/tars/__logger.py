#!/usr/bin/env python
# -*- coding: utf-8 -*-

# filename: __logger.py

# Tencent is pleased to support the open source community by making Tars available.
#
# Copyright (C) 2016THL A29 Limited, a Tencent company. All rights reserved.
#
# Licensed under the BSD 3-Clause License (the "License"); you may not use this file except 
# in compliance with the License. You may obtain a copy of the License at
#
# https://opensource.org/licenses/BSD-3-Clause
#
# Unless required by applicable law or agreed to in writing, software distributed 
# under the License is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR 
# CONDITIONS OF ANY KIND, either express or implied. See the License for the 
# specific language governing permissions and limitations under the License.
#

'''
@version: 0.01
@brief: 日志模块
'''

# 仅用于调试

import logging
from logging.handlers import RotatingFileHandler

tarsLogger = logging.getLogger('TARS client')
#console = logging.StreamHandler()
#console.setLevel(logging.DEBUG)
#filelog = logging.FileHandler('tars.log')
#filelog.setLevel(logging.DEBUG)
#formatter = logging.Formatter('%(asctime)s | %(levelname)8s | [%(name)s] %(message)s', '%Y-%m-%d %H:%M:%S')
#console.setFormatter(formatter)
#filelog.setFormatter(formatter)
#tarsLogger.addHandler(console)
#tarsLogger.addHandler(filelog)
#tarsLogger.setLevel(logging.DEBUG)
#tarsLogger.setLevel(logging.INFO)
#tarsLogger.setLevel(logging.ERROR)

def initLog(logpath, logsize, lognum):
    handler = RotatingFileHandler(filename=logpath, maxBytes=logsize,
                                  backupCount=lognum)
    formatter = logging.Formatter('%(asctime)s | %(levelname)8s | [%(name)s] %(message)s', '%Y-%m-%d %H:%M:%S')
    handler.setFormatter(formatter)
    tarsLogger.addHandler(handler)

if __name__ == '__main__':
    tarsLogger.debug('debug log')
    tarsLogger.info('info log')
    tarsLogger.warning('warning log')
    tarsLogger.error('error log')
    tarsLogger.critical('critical log')

