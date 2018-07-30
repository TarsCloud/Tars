#!/usr/bin/env python
# -*- coding: utf-8 -*-
# filename: __rpc.py

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
@brief: 异步rpc实现
'''

import threading
import Queue
from __logger import tarsLogger
from __packet import ResponsePacket
from __servantproxy import ServantProxy


class AsyncProcThread:
    '''
    @brief: 异步调用线程管理类
    '''

    def __init__(self):
        tarsLogger.debug('AsyncProcThread:__init__')
        self.__initialize = False
        self.__runners = []
        self.__queue = None
        self.__nrunner = 0
        self.__popTimeout = 0.1

    def __del__(self):
        tarsLogger.debug('AsyncProcThread:__del__')

    def initialize(self, nrunner=3):
        '''
        @brief: 使用AsyncProcThread前必须先调用此函数
        @param nrunner: 异步线程个数
        @type nrunner: int
        @return: None
        @rtype: None
        '''
        tarsLogger.debug('AsyncProcThread:initialize')
        if self.__initialize:
            return
        self.__nrunner = nrunner
        self.__queue = Queue.Queue()
        self.__initialize = True

    def terminate(self):
        '''
        @brief: 关闭所有异步线程
        @return: None
        @rtype: None
        '''
        tarsLogger.debug('AsyncProcThread:terminate')

        for runner in self.__runners:
            runner.terminate()

        for runner in self.__runners:
            runner.join()
        self.__runners = []

    def put(self, reqmsg):
        '''
        @brief: 处理数据入队列
        @param reqmsg: 待处理数据
        @type reqmsg: ReqMessage
        @return: None
        @rtype: None
        '''
        tarsLogger.debug('AsyncProcThread:put')
        # 异步请求超时
        if not reqmsg.response:
            reqmsg.response = ResponsePacket()
            reqmsg.response.iVerson = reqmsg.request.iVerson
            reqmsg.response.cPacketType = reqmsg.request.cPacketType
            reqmsg.response.iRequestId = reqmsg.request.iRequestId
            reqmsg.response.iRet = ServantProxy.TARSASYNCCALLTIMEOUT

        self.__queue.put(reqmsg)

    def pop(self):
        '''
        @brief: 处理数据出队列
        @return: ReqMessage
        @rtype: ReqMessage
        '''
        # tarsLogger.debug('AsyncProcThread:pop')
        ret = None
        try:
            ret = self.__queue.get(True, self.__popTimeout)
        except Queue.Empty:
            pass
        return ret

    def start(self):
        '''
        @brief: 启动异步线程
        @return: None
        @rtype: None
        '''
        tarsLogger.debug('AsyncProcThread:start')
        for i in xrange(self.__nrunner):
            runner = AsyncProcThreadRunner()
            runner.initialize(self)
            runner.start()
            self.__runners.append(runner)


class AsyncProcThreadRunner(threading.Thread):
    '''
    @brief: 异步调用线程
    '''

    def __init__(self):
        tarsLogger.debug('AsyncProcThreadRunner:__init__')
        super(AsyncProcThreadRunner, self).__init__()
        # threading.Thread.__init__(self)
        self.__terminate = False
        self.__initialize = False
        self.__procQueue = None

    def __del__(self):
        tarsLogger.debug('AsyncProcThreadRunner:__del__')

    def initialize(self, queue):
        '''
        @brief: 使用AsyncProcThreadRunner前必须调用此函数
        @param queue: 有pop()的类，用于提取待处理数据
        @type queue: AsyncProcThread
        @return: None
        @rtype: None
        '''
        tarsLogger.debug('AsyncProcThreadRunner:initialize')
        self.__procQueue = queue

    def terminate(self):
        '''
        @brief: 关闭线程
        @return: None
        @rtype: None
        '''
        tarsLogger.debug('AsyncProcThreadRunner:terminate')
        self.__terminate = True

    def run(self):
        '''
        @brief: 线程启动函数，执行异步调用
        '''
        tarsLogger.debug('AsyncProcThreadRunner:run')
        while not self.__terminate:
            if self.__terminate:
                break
            reqmsg = self.__procQueue.pop()
            if not reqmsg or not reqmsg.callback:
                continue

            if reqmsg.adapter:
                succ = reqmsg.response.iRet == ServantProxy.TARSSERVERSUCCESS
                reqmsg.adapter.finishInvoke(succ)

            try:
                reqmsg.callback.onDispatch(reqmsg)
            except Exception, msg:
                tarsLogger.error('AsyncProcThread excepttion: %s', msg)

        tarsLogger.debug('AsyncProcThreadRunner:run finished')


class ServantProxyCallback(object):
    '''
    @brief: 异步回调对象基类
    '''

    def __init__(self):
        tarsLogger.debug('ServantProxyCallback:__init__')

    def onDispatch(reqmsg):
        '''
        @brief: 分配响应报文到对应的回调函数
        @param queue: 有pop()的类，用于提取待处理数据
        @type queue: AsyncProcThread
        @return: None
        @rtype: None
        '''
        raise NotImplementedError()
