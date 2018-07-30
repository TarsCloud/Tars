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
@brief: rpc调用逻辑实现
'''

import time
import argparse

from __logger import tarsLogger
from __logger import initLog
from __trans import EndPointInfo
from __TimeoutQueue import TimeoutQueue
from __TimeoutQueue import QueueTimeout
from __trans import FDReactor
from __adapterproxy import AdapterProxyManager
from __servantproxy import ServantProxy
from exception import (TarsException)
from __async import AsyncProcThread


class Communicator:
    '''
    @brief: 通讯器，创建和维护ServantProxy、ObjectProxy、FDReactor线程和超时线程
    '''
    default_config = {'tars':
                      {'application':
                       {'client':
                        {'async-invoke-timeout': 20000,
                         'asyncthread': 0,
                         'locator': '',
                            'loglevel': 'error',
                            'logpath': 'tars.log',
                            'logsize': 15728640,
                            'lognum': 0,
                            'refresh-endpoint-interval': 60000,
                            'sync-invoke-timeout': 5000}}}}

    def __init__(self, config={}):
        tarsLogger.debug('Communicator:__init__')
        self.__terminate = False
        self.__initialize = False
        self.__objects = {}
        self.__servants = {}
        self.__reactor = None
        self.__qTimeout = None
        self.__asyncProc = None
        self.__config = Communicator.default_config.copy()
        self.__config.update(config)
        self.initialize()

    def __del__(self):
        tarsLogger.debug('Communicator:__del__')

    def initialize(self):
        '''
        @brief: 使用通讯器前必须先调用此函数
        '''
        tarsLogger.debug('Communicator:initialize')
        if self.__initialize:
            return
        logpath = self.getProperty('logpath')
        logsize = self.getProperty('logsize', int)
        lognum = self.getProperty('lognum', int)
        loglevel = self.getProperty('loglevel')
        initLog(logpath, logsize, lognum, loglevel)

        self.__reactor = FDReactor()
        self.__reactor.initialize()
        self.__reactor.start()

        self.__qTimeout = QueueTimeout()
        self.__qTimeout.setHandler(self.handleTimeout)
        self.__qTimeout.start()

        async_num = self.getProperty('asyncthread', int)
        self.__asyncProc = AsyncProcThread()
        self.__asyncProc.initialize(async_num)
        self.__asyncProc.start()

        self.__initialize = True

    def terminate(self):
        '''
        @brief: 不再使用通讯器需调用此函数释放资源
        '''
        tarsLogger.debug('Communicator:terminate')

        if not self.__initialize:
            return

        self.__reactor.terminate()
        self.__qTimeout.terminate()
        self.__asyncProc.terminate()

        for objName in self.__servants:
            self.__servants[objName]._terminate()

        for objName in self.__objects:
            self.__objects[objName].terminate()

        self.__objects = {}
        self.__servants = {}
        self.__reactor = None
        self.__initialize = False

    def parseConnAddr(self, connAddr):
        '''
        @brief: 解析connAddr字符串
        @param connAddr: 连接地址
        @type connAddr: str
        @return: 解析结果
        @rtype: dict, key是str，val里name是str，
                timeout是float，endpoint是EndPointInfo的list
        '''
        tarsLogger.debug('Communicator:parseConnAddr')
        connAddr = connAddr.strip()
        connInfo = {
            'name': '',
            'timeout': -1,
            'endpoint': []
        }
        if '@' not in connAddr:
            connInfo['name'] = connAddr
            return connInfo

        try:
            tks = connAddr.split('@')
            connInfo['name'] = tks[0]
            tks = tks[1].lower().split(':')
            parser = argparse.ArgumentParser(add_help=False)
            parser.add_argument('-h')
            parser.add_argument('-p')
            parser.add_argument('-t')
            for tk in tks:
                argv = tk.split()
                if argv[0] != 'tcp':
                    raise TarsException(
                        'unsupport transmission protocal : %s' % connInfo['name'])
                mes = parser.parse_args(argv[1:])
                try:
                    ip = mes.h if mes.h is not None else ''
                    port = int(mes.p) if mes.p is not None else '-1'
                    timeout = int(mes.t) if mes.t is not None else '-1'
                    connInfo['endpoint'].append(
                        EndPointInfo(ip, port, timeout))
                except Exception:
                    raise TarsException('Unrecognized option : %s' % mes)
        except TarsException:
            raise

        except Exception, exp:
            raise TarsException(exp)

        return connInfo

    def getReactor(self):
        '''
        @brief: 获取reactor
        '''
        return self.__reactor

    def getAsyncProc(self):
        '''
        @brief: 获取asyncProc
        '''
        return self.__asyncProc

    def getProperty(self, name, dt_type=str):
        '''
        @brief: 获取配置
        @param name: 配置名称
        @type name: str
        @param dt_type: 数据类型
        @type name: type
        @return: 配置内容
        @rtype: str
        '''
        try:
            ret = self.__config['tars']['application']['client'][name]
            ret = dt_type(ret)
        except:
            ret = Communicator.default_config['tars']['application']['client'][name]

        return ret

    def setProperty(self, name, value):
        '''
        @brief: 修改配置
        @param name: 配置名称
        @type propertys: str
        @param value: 配置内容
        @type propertys: str
        @return: 设置是否成功
        @rtype: bool
        '''
        try:
            self.__config['tars']['application']['client'][name] = value
            return True
        except:
            return False

    def setPropertys(self, propertys):
        '''
        @brief: 修改配置
        @param propertys: 配置集合
        @type propertys: map, key type: str, value type: str
        @return: 无
        @rtype: None
        '''
        pass

    def updateConfig(self):
        '''
        @brief: 重新设置配置
        '''

    def stringToProxy(self, servantProxy, connAddr):
        '''
        @brief: 初始化ServantProxy
        @param connAddr: 服务器地址信息
        @type connAddr: str
        @param servant: servant proxy
        @type servant: ServantProxy子类
        @return: 无
        @rtype: None
        @note: 如果connAddr的ServantObj连接过，返回连接过的ServantProxy
               如果没有连接过，用参数servant初始化，返回servant
        '''
        tarsLogger.debug('Communicator:stringToProxy')

        connInfo = self.parseConnAddr(connAddr)
        objName = connInfo['name']
        if objName in self.__servants:
            return self.__servants[objName]

        objectPrx = ObjectProxy()
        objectPrx.initialize(self, connInfo)

        servantPrx = servantProxy()
        servantPrx._initialize(self.__reactor, objectPrx)
        self.__objects[objName] = objectPrx
        self.__servants[objName] = servantPrx
        return servantPrx

    def handleTimeout(self):
        '''
        @brief: 处理超时事件
        @return: 无
        @rtype: None
        '''
        # tarsLogger.debug('Communicator:handleTimeout')
        for obj in self.__objects.itervalues():
            obj.handleQueueTimeout()


class ObjectProxy:
    '''
    @brief: 一个object name在一个Communicator里有一个objectproxy
            管理收发的消息队列
    '''
    DEFAULT_TIMEOUT = 3.0

    def __init__(self):
        tarsLogger.debug('ObjectProxy:__init__')
        self.__name = ''
        self.__timeout = ObjectProxy.DEFAULT_TIMEOUT
        self.__comm = None
        self.__epi = None
        self.__adpmanager = None
        self.__timeoutQueue = None
        # self.__adapter = None
        self.__initialize = False

    def __del__(self):
        tarsLogger.debug('ObjectProxy:__del__')

    def initialize(self, comm, connInfo):
        '''
        @brief: 初始化，使用ObjectProxy前必须调用
        @param comm: 通讯器
        @type comm: Communicator
        @param connInfo: 连接信息
        @type comm: dict
        @return: None
        @rtype: None
        '''
        if self.__initialize:
            return
        tarsLogger.debug('ObjectProxy:initialize')
        self.__comm = comm
        # async-invoke-timeout来设置队列时间
        async_timeout = self.__comm.getProperty(
            'async-invoke-timeout', float) / 1000
        self.__timeoutQueue = TimeoutQueue(async_timeout)

        self.__name = connInfo['name']

        self.__timeout = self.__comm.getProperty(
            'sync-invoke-timeout', float) / 1000

        # 通过Communicator的配置设置超时
        # 不再通过连接信息的-t来设置
        # if connInfo['timeout'] != -1:
        # self.__timeout = connInfo['timeout']
        eplist = connInfo['endpoint']

        self.__adpmanager = AdapterProxyManager()
        self.__adpmanager.initialize(comm, self, eplist)

        self.__initialize = True

    def terminate(self):
        '''
        @brief: 回收资源，不再使用ObjectProxy时调用
        @return: None
        @rtype: None
        '''
        tarsLogger.debug('ObjectProxy:terminate')
        self.__timeoutQueue = None
        self.__adpmanager.terminate()
        self.__initialize = False

    def name(self):
        '''
        @brief: 获取object name
        @return: object name
        @rtype: str
        '''
        return self.__name

    # def setTimeout(self, timeout):
        # '''
        # @brief: 设置超时
        # @param timeout: 超时时间，单位为s
        # @type timeout: float
        # @return: None
        # @rtype: None
        # '''
        # self.__timeout = timeout
        # self.__timeoutQueue.setTimeout(timeout)

    def timeout(self):
        '''
        @brief: 获取超时时间
        @return: 超时时间，单位为s
        @rtype: float
        '''
        return self.__timeout

    def getTimeoutQueue(self):
        '''
        @brief: 获取超时队列
        @return: 超时队列
        @rtype: TimeoutQueue
        '''
        return self.__timeoutQueue

    def handleQueueTimeout(self):
        '''
        @brief: 超时事件发生时处理超时事务
        @return: None
        @rtype: None
        '''
        # tarsLogger.debug('ObjectProxy:handleQueueTimeout')
        self.__timeoutQueue.timeout()

    def invoke(self, reqmsg):
        '''
        @brief: 远程过程调用
        @param reqmsg: 请求响应报文
        @type reqmsg: ReqMessage
        @return: 错误码
        @rtype:
        '''
        tarsLogger.debug('ObjectProxy:invoke, objname: %s, func: %s',
                         self.__name, reqmsg.request.sFuncName)
        # 负载均衡
        # adapter = self.__adpmanager.getNextValidProxy()
        adapter = self.__adpmanager.selectAdapterProxy(reqmsg)
        if not adapter:
            tarsLogger.error("invoke %s, select adapter proxy return None",
                             self.__name)
            return -2

        adapter.checkActive(True)
        reqmsg.adapter = adapter
        return adapter.invoke(reqmsg)

    # 弹出请求报文
    def popRequest(self):
        '''
        @brief: 返回消息队列里的请求响应报文，FIFO
                不删除TimeoutQueue里的数据，响应时要用
        @return: 请求响应报文
        @rtype: ReqMessage
        '''
        return self.__timeoutQueue.pop(erase=False)


if __name__ == '__main__':
    connAddr = "apptest.lightServer.lightServantObj@tcp -h 10.130.64.220 -p 10001 -t 10000"
    connAddr = 'MTT.BookMarksUnifyServer.BookMarksUnifyObj@tcp -h 172.17.149.77 -t 60000 -p 10023'
    comm = Communicator()
    comm.initialize()
    servant = ServantProxy()
    servant = comm.stringToProxy(connAddr, servant)
    print servant.tars_timeout()
    try:
        rsp = servant.tars_invoke(
            ServantProxy.TARSNORMAL, "test", '', ServantProxy.mapcls_context(), None)
        print 'Servant invoke success, request id: %d, iRet: %d' % (
            rsp.iRequestId, rsp.iRet)
    except Exception, msg:
        print msg
    finally:
        servant.tars_terminate()
    time.sleep(2)
    print 'app closing ...'
    comm.terminate()
    time.sleep(2)
    print 'cpp closed'
