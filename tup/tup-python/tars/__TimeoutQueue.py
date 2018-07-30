#!/usr/bin/env python
# -*- coding: utf-8 -*-
# filename: __timeQueue.py

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
@brief: 请求响应报文和超时队列
'''

import threading
import time
import struct

from __logger import tarsLogger
from __tars import TarsInputStream
from __tars import TarsOutputStream
from __packet import RequestPacket
from __packet import ResponsePacket
from __util import (NewLock, LockGuard)


class ReqMessage:
    '''
    @brief: 请求响应报文，保存一个请求响应所需要的数据
    '''
    SYNC_CALL = 1
    ASYNC_CALL = 2
    ONE_WAY = 3

    def __init__(self):
        self.type = ReqMessage.SYNC_CALL
        self.servant = None
        self.lock = None
        self.adapter = None
        self.request = None
        self.response = None
        self.callback = None
        self.begtime = None
        self.endtime = None
        self.isHash = False
        self.isConHash = False
        self.hashCode = 0

    def packReq(self):
        '''
        @brief: 序列化请求报文
        @return: 序列化后的请求报文
        @rtype: str
        '''
        if not self.request:
            return ''
        oos = TarsOutputStream()
        RequestPacket.writeTo(oos, self.request)
        reqpkt = oos.getBuffer()
        plen = len(reqpkt) + 4
        reqpkt = struct.pack('!i', plen) + reqpkt
        return reqpkt

    @staticmethod
    def unpackRspList(buf):
        '''
        @brief: 解码响应报文
        @param buf: 多个序列化后的响应报文数据
        @type buf: str
        @return: 解码出来的响应报文和解码的buffer长度
        @rtype: rsplist: 装有ResponsePacket的list
                unpacklen: int
        '''
        rsplist = []
        if not buf:
            return rsplist

        unpacklen = 0
        buf = buffer(buf)
        while True:
            if len(buf) - unpacklen < 4:
                break
            packsize = buf[unpacklen: unpacklen+4]
            packsize, = struct.unpack_from('!i', packsize)
            if len(buf) < unpacklen + packsize:
                break

            ios = TarsInputStream(buf[unpacklen+4: unpacklen+packsize])
            rsp = ResponsePacket.readFrom(ios)
            rsplist.append(rsp)
            unpacklen += packsize

        return rsplist, unpacklen

# 超时队列，加锁，线程安全


class TimeoutQueue:
    '''
    @brief: 超时队列，加锁，线程安全
            可以像队列一样FIFO，也可以像字典一样按key取item
    @todo: 限制队列长度
    '''

    def __init__(self, timeout=3):
        self.__uniqId = 0
        # self.__lock = threading.Lock()
        self.__lock = NewLock()
        self.__data = {}
        self.__queue = []
        self.__timeout = timeout

    def getTimeout(self):
        '''
        @brief: 获取超时时间，单位为s
        @return: 超时时间
        @rtype: float
        '''
        return self.__timeout

    def setTimeout(self, timeout):
        '''
        @brief: 设置超时时间，单位为s
        @param timeout: 超时时间
        @type timeout: float
        @return: None
        @rtype: None
        '''
        self.__timeout = timeout

    def size(self):
        '''
        @brief: 获取队列长度
        @return: 队列长度
        @rtype: int
        '''
        # self.__lock.acquire()
        lock = LockGuard(self.__lock)
        ret = len(self.__data)
        # self.__lock.release()
        return ret

    def generateId(self):
        '''
        @brief: 生成唯一id，0 < id < 2 ** 32
        @return: id
        @rtype: int
        '''
        # self.__lock.acquire()
        lock = LockGuard(self.__lock)
        ret = self.__uniqId
        ret = (ret + 1) % 0x7FFFFFFF
        while ret <= 0:
            ret = (ret + 1) % 0x7FFFFFFF
        self.__uniqId = ret
        # self.__lock.release()
        return ret

    def pop(self, uniqId=0, erase=True):
        '''
        @brief: 弹出item
        @param uniqId: item的id，如果为0，按FIFO弹出
        @type uniqId: int
        @param erase: 弹出后是否从字典里删除item
        @type erase: bool
        @return: item
        @rtype: any type
        '''
        ret = None

        # self.__lock.acquire()
        lock = LockGuard(self.__lock)

        if not uniqId:
            if len(self.__queue):
                uniqId = self.__queue.pop(0)
        if uniqId:
            if erase:
                ret = self.__data.pop(uniqId, None)
            else:
                ret = self.__data.get(uniqId, None)

        # self.__lock.release()

        return ret[0] if ret else None

    def push(self, item, uniqId):
        '''
        @brief: 数据入队列，如果队列已经有了uniqId，插入失败
        @param item: 插入的数据
        @type item: any type
        @return: 插入是否成功
        @rtype: bool
        '''
        begtime = time.time()
        ret = True
        # self.__lock.acquire()
        lock = LockGuard(self.__lock)

        if uniqId in self.__data:
            ret = False
        else:
            self.__data[uniqId] = [item, begtime]
            self.__queue.append(uniqId)
        # self.__lock.release()
        return ret

    def peek(self, uniqId):
        '''
        @brief: 根据uniqId获取item，不会删除item
        @param uniqId: item的id
        @type uniqId: int
        @return: item
        @rtype: any type
        '''
        # self.__lock.acquire()
        lock = LockGuard(self.__lock)

        ret = self.__data.get(uniqId, None)
        # self.__lock.release()
        if not ret:
            return None
        return ret[0]

    def timeout(self):
        '''
        @brief: 检测是否有item超时，如果有就删除
        @return: None
        @rtype: None
        '''
        endtime = time.time()
        # self.__lock.acquire()
        lock = LockGuard(self.__lock)

        # 处理异常情况，防止死锁
        try:
            new_data = {}
            for uniqId, item in self.__data.iteritems():
                if endtime - item[1] < self.__timeout:
                    new_data[uniqId] = item
                else:
                    tarsLogger.debug(
                        'TimeoutQueue:timeout remove id : %d' % uniqId)
            self.__data = new_data
        finally:
            # self.__lock.release()
            pass


class QueueTimeout(threading.Thread):
    """
    超时线程，定时触发超时事件
    """

    def __init__(self, timeout=0.1):
        # threading.Thread.__init__(self)
        tarsLogger.debug('QueueTimeout:__init__')
        super(QueueTimeout, self).__init__()
        self.timeout = timeout
        self.__terminate = False
        self.__handler = None
        self.__lock = threading.Condition()

    def terminate(self):
        tarsLogger.debug('QueueTimeout:terminate')
        self.__terminate = True
        self.__lock.acquire()
        self.__lock.notifyAll()
        self.__lock.release()

    def setHandler(self, handler):
        self.__handler = handler

    def run(self):
        while not self.__terminate:
            try:
                self.__lock.acquire()
                self.__lock.wait(self.timeout)
                self.__lock.release()
                if self.__terminate:
                    break
                self.__handler()
            except Exception, msg:
                tarsLogger.error('QueueTimeout:run exception : %s', msg)

        tarsLogger.debug('QueueTimeout:run finished')


if __name__ == '__main__':
    pass
