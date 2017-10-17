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

import threading
import socket
import select
import os
import time
import sys
import getopt
import logging

from __logger       import tarsLogger
from __logger       import initLog
from __util         import util;
from __trans        import TcpTransceiver
from __trans        import EndPointInfo
from __packet       import RequestPacket
from __packet       import ResponsePacket
from __TimeoutQueue import ReqMessage
from __TimeoutQueue import TimeoutQueue
from __TimeoutQueue import QueueTimeout
from __trans        import FDReactor
#from __async        import AsyncProcThread
from exception      import *

class ServantProxy(object):
    '''
    @brief: 1、远程对象的本地代理
            2、同名servant在一个通信器中最多只有一个实例
            3、防止和用户在Tars中定义的函数名冲突，接口以tars_开头
    '''

    # 服务器响应的错误码
    TARSSERVERSUCCESS       = 0    #服务器端处理成功
    TARSSERVERDECODEERR     = -1   #服务器端解码异常
    TARSSERVERENCODEERR     = -2   #服务器端编码异常
    TARSSERVERNOFUNCERR     = -3   #服务器端没有该函数
    TARSSERVERNOSERVANTERR  = -4   #服务器端五该Servant对象
    TARSSERVERRESETGRID     = -5   #服务器端灰度状态不一致
    TARSSERVERQUEUETIMEOUT  = -6   #服务器队列超过限制
    TARSASYNCCALLTIMEOUT    = -7   #异步调用超时
    TARSPROXYCONNECTERR     = -8   #proxy链接异常
    TARSSERVERUNKNOWNERR    = -99  #服务器端未知异常

    TARSVERSION = 1
    TUPVERSION = 2
    TUPVERSION2 = 3

    TARSNORMAL = 0
    TARSONEWAY = 1

    TARSMESSAGETYPENULL = 0
    TARSMESSAGETYPEHASH = 1
    TARSMESSAGETYPEGRID = 2
    TARSMESSAGETYPEDYED = 4
    TARSMESSAGETYPESAMPLE = 8
    TARSMESSAGETYPEASYNC = 16

    mapcls_context = util.mapclass(util.string, util.string)

    def __init__(self):
        tarsLogger.debug('ServantProxy:__init__')
        self.__reactor = None
        self.__object = None
        self.__initialize = False

    def __del__(self):
        tarsLogger.debug('ServantProxy:__del__')

    def _initialize(self, reactor, obj):
        '''
        @brief: 初始化函数，需要调用才能使用ServantProxy
        @param reactor: 网络管理的reactor实例
        @type reactor: FDReactor
        @return: None
        @rtype: None
        '''
        tarsLogger.debug('ServantProxy:_initialize')

        assert(reactor and obj)
        if self.__initialize:
            return
        self.__reactor = reactor
        self.__object = obj
        self.__initialize = True

    def _terminate(self):
        '''
        @brief: 不再使用ServantProxy时调用，会释放相应资源
        @return: None
        @rtype: None
        '''
        tarsLogger.debug('ServantProxy:_terminate')
        self.__object = None
        self.__reactor = None
        self.__initialize = False

    def tars_name(self):
        '''
        @brief: 获取ServantProxy的名字
        @return: ServantProxy的名字
        @rtype: str
        '''
        return self.__object.name()

    def tars_timeout(self):
        '''
        @brief: 获取超时时间，单位是ms
        @return: 超时时间
        @rtype: int
        '''
        return int(self.__timeout() * 1000)

    def tars_ping(self):
        pass

    #def tars_initialize(self):
        #pass

    #def tars_terminate(self):
        #pass

    def tars_invoke(self, cPacketType, sFuncName, sBuffer, context, status):
        '''
        @brief: TARS协议同步方法调用
        @param cPacketType: 请求包类型
        @type cPacketType: int
        @param sFuncName: 调用函数名
        @type sFuncName: str
        @param sBuffer: 序列化后的发送参数
        @type sBuffer: str
        @param context: 上下文件信息
        @type context: ServantProxy.mapcls_context
        @param status: 状态信息
        @type status:
        @return: 响应报文
        @rtype: ResponsePacket
        '''
        tarsLogger.debug('ServantProxy:tars_invoke, func: %s', sFuncName)
        req = RequestPacket()
        req.iVersion = ServantProxy.TARSVERSION
        req.cPacketType = cPacketType
        req.iMessageType = ServantProxy.TARSMESSAGETYPENULL
        req.iRequestId = 0;
        req.sServantName = self.tars_name()
        req.sFuncName = sFuncName
        req.sBuffer = sBuffer
        req.iTimeout= self.tars_timeout()

        reqmsg = ReqMessage()
        reqmsg.type = ReqMessage.SYNC_CALL
        reqmsg.servant = self
        reqmsg.lock = threading.Condition()
        reqmsg.request = req
        reqmsg.begtime = time.time()

        rsp = None
        try:
            rsp = self.__invoke(reqmsg)
        except TarsSyncCallTimeoutException, msg:
            if reqmsg.adapter:
                reqmsg.adapter.finishInvoke(True)
            raise
        except TarsException:
            raise
        except:
            raise TarsException('ServantProxy::tars_invoke excpetion')

        if reqmsg.adapter:
            reqmsg.adapter.finishInvoke(False)

        return rsp

    def tars_invoke_async(self, cPacketType, sFuncName, sBuffer,
                         context, status, callback):
        '''
        @brief: TARS协议同步方法调用
        @param cPacketType: 请求包类型
        @type cPacketType: int
        @param sFuncName: 调用函数名
        @type sFuncName: str
        @param sBuffer: 序列化后的发送参数
        @type sBuffer: str
        @param context: 上下文件信息
        @type context: ServantProxy.mapcls_context
        @param status: 状态信息
        @type status:
        @param callback: 异步调用回调对象
        @type callback: ServantProxyCallback的子类
        @return: 响应报文
        @rtype: ResponsePacket
        '''
        tarsLogger.debug('ServantProxy:tars_invoke')
        req = RequestPacket()
        req.iVersion = ServantProxy.TARSVERSION
        req.cPacketType = cPacketType if callback else ServantProxy.TARSONEWAY
        req.iMessageType = ServantProxy.TARSMESSAGETYPENULL
        req.iRequestId = 0;
        req.sServantName = self.tars_name()
        req.sFuncName = sFuncName
        req.sBuffer = sBuffer
        req.iTimeout= self.tars_timeout()

        reqmsg = ReqMessage()
        reqmsg.type = ReqMessage.ASYNC_CALL if callback else ReqMessage.ONE_WAY
        reqmsg.callback = callback
        reqmsg.servant = self
        reqmsg.request = req
        reqmsg.begtime = time.time()

        rsp = None
        try:
            rsp = self.__invoke(reqmsg)
        except TarsException:
            raise
        except:
            raise TarsException('ServantProxy::tars_invoke excpetion')

        if reqmsg.adapter:
            reqmsg.adapter.finishInvoke(False)

        return rsp

    def __timeout(self):
        '''
        @brief: 获取超时时间，单位是s
        @return: 超时时间
        @rtype: float
        '''
        return self.__object.timeout()

    def __invoke(self, reqmsg):
        '''
        @brief: 远程过程调用
        @param reqmsg: 请求数据
        @type reqmsg: ReqMessage
        @return: 调用成功或失败
        @rtype: bool
        '''
        tarsLogger.debug('ServantProxy:invoke, func: %s', reqmsg.request.sFuncName)
        ret = self.__object.invoke(reqmsg)
        if ret == -2:
            errmsg = ('ServantProxy::invoke fail, no valid servant,' +
                      ' servant name : %s, function name : %s' %
                      (reqmsg.request.sServantName,
                       reqmsg.request.sFuncName))
            raise TarsException(errmsg)
        if ret == -1:
            errmsg = ('ServantProxy::invoke connect fail,' +
                      ' servant name : %s, function name : %s, adapter : %s' %
                      (reqmsg.request.sServantName,
                       reqmsg.request.sFuncName,
                       reqmsg.adapter.getEndPointInfo()))
            raise TarsException(errmsg)
        elif ret != 0:
            errmsg = ('ServantProxy::invoke unknown fail, ' +
                      'Servant name : %s, function name : %s' %
                      (reqmsg.request.sServantName,
                       reqmsg.request.sFuncName))
            raise TarsException(errmsg)

        if reqmsg.type == ReqMessage.SYNC_CALL:
            reqmsg.lock.acquire()
            reqmsg.lock.wait(self.__timeout())
            reqmsg.lock.release()

            if not reqmsg.response:
                errmsg = ('ServantProxy::invoke timeout: %d, servant name'
                          ': %s, adapter: %s, request id: %d' % (
                              self.tars_timeout(),
                              self.tars_name(),
                              reqmsg.adapter.trans().getEndPointInfo(),
                              reqmsg.request.iRequestId))
                raise TarsSyncCallTimeoutException(errmsg)
            elif reqmsg.response.iRet == ServantProxy.TARSSERVERSUCCESS:
                return reqmsg.response
            else:
                errmsg = 'servant name: %s, function name: %s' % (
                         self.tars_name(), reqmsg.request.sFuncName)
                self.tarsRaiseException(reqmsg.response.iRet, errmsg)

    def _finished(self, reqmsg):
        '''
        @brief: 通知远程过程调用线程响应报文到了
        @param reqmsg: 请求响应报文
        @type reqmsg: ReqMessage
        @return: 函数执行成功或失败
        @rtype: bool
        '''
        tarsLogger.debug('ServantProxy:finished')
        if not reqmsg.lock:
            return False
        reqmsg.lock.acquire()
        reqmsg.lock.notifyAll()
        reqmsg.lock.release()
        return True

    def tarsRaiseException(self, errno, desc):
        '''
        @brief: 服务器调用失败，根据服务端给的错误码抛出异常
        @param errno: 错误码
        @type errno: int
        @param desc: 错误描述
        @type desc: str
        @return: 没有返回值，函数会抛出异常
        @rtype:
        '''
        if errno == ServantProxy.TARSSERVERSUCCESS:
            return

        elif errno == ServantProxy.TARSSERVERDECODEERR:
            raise TarsServerDecodeException(
                "server decode exception: errno: %d, msg: %s" % (errno, desc))

        elif errno == ServantProxy.TARSSERVERENCODEERR:
            raise TarsServerEncodeException(
                "server encode exception: errno: %d, msg: %s" % (errno, desc))

        elif errno == ServantProxy.TARSSERVERNOFUNCERR:
            raise TarsServerNoFuncException(
                "server function mismatch exception: errno: %d, msg: %s" % (errno, desc))

        elif errno == ServantProxy.TARSSERVERNOSERVANTERR:
            raise TarsServerNoServantException(
                "server servant mismatch exception: errno: %d, msg: %s" % (errno, desc))

        elif errno == ServantProxy.TARSSERVERRESETGRID:
            raise TarsServerResetGridException(
                "server reset grid exception: errno: %d, msg: %s" % (errno, desc))

        elif errno == ServantProxy.TARSSERVERQUEUETIMEOUT:
            raise TarsServerQueueTimeoutException(
                "server queue timeout exception: errno: %d, msg: %s" % (errno, desc))

        elif errno == ServantProxy.TARSPROXYCONNECTERR:
            raise TarsServerQueueTimeoutException(
                "server connection lost: errno: %d, msg: %s" % (errno, desc))

        else:
            raise TarsServerUnknownException(
                "server unknown exception: errno: %d, msg: %s" % (errno, desc))


# 因为循环import的问题只能放这里，不能放文件开始处
from __async        import AsyncProcThread
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
                            'loglevel': 'ERROR',
                            'logpath': 'tars.log',
                            'logsize': 15728640,
                            'lognum': 0,
                            'refresh-endpoint-interval': 60000,
                            'sync-invoke-timeout': 5000}}}}

    def __init__(self, config = {}):
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
        self.initialize();

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
        loglevel = getattr(logging, loglevel.upper(), None)
        loglevel = loglevel if loglevel else logging.ERROR

        initLog(logpath, logsize, lognum)
        tarsLogger.setLevel(loglevel)

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
            'name' : '',
            'timeout' : -1,
            'endpoint' : []
        }
        if '@' not in connAddr:
            connInfo['name'] = connAddr
            return connInfo

        try:
            tks = connAddr.split('@')
            connInfo['name'] = tks[0]
            tks = tks[1].lower().split(':')
            for tk in tks:
                argv = tk.split()
                if argv[0] != 'tcp':
                    raise TarsException('unsupport transmission protocal : %s' % args[0])
                opts, args = getopt.getopt(argv[1:], "h:p:t:")
                ip, port = '', -1
                for opt, arg in opts:
                    if opt == '-h':
                        ip = arg
                    elif opt == '-p':
                        port = int(arg)
                    elif opt == '-t':
                        connInfo['timeout'] = float(arg) / 1000.0
                    else:
                        raise TarsException('unkown option : %s' % opt)
                connInfo['endpoint'].append(EndPointInfo(ip, port))

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

        objectPrx = ObjectProxy();
        objectPrx.initialize(self, connInfo)

        servantPrx = servantProxy();
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
        #tarsLogger.debug('Communicator:handleTimeout')
        for obj in self.__objects.itervalues():
            obj.handleQueueTimeout()


class AdapterProxy:
    '''
    @brief: 每一个Adapter管理一个服务端端口的连接，数据收发
    '''
    def __init__(self):
        tarsLogger.debug('AdapterProxy:__init__')
        self.__closeTrans = False
        self.__trans = None
        self.__object = None
        self.__reactor = None
        self.__lock = None
        self.__asyncProc = None

    def __del__(self):
        tarsLogger.debug('AdapterProxy:__del__')

    def initialize(self, endPointInfo, objectProxy, reactor, asyncProc):
        '''
        @brief: 初始化
        @param endPointInfo: 连接对端信息
        @type endPointInfo: EndPointInfo
        @type objectProxy: ObjectProxy
        @type reactor: FDReactor
        @type asyncProc: AsyncProcThread
        '''
        tarsLogger.debug('AdapterProxy:initialize')
        self.__closeTrans = False
        self.__trans = TcpTransceiver(endPointInfo)
        self.__object = objectProxy
        self.__reactor = reactor
        self.__lock = threading.Lock()
        self.__asyncProc = asyncProc

    def terminate(self):
        '''
        @brief: 关闭
        '''
        tarsLogger.debug('AdapterProxy:terminate')
        self.setCloseTrans(True)

    def trans(self):
        '''
        @brief: 获取传输类
        @return: 负责网络传输的trans
        @rtype: Transceiver
        '''
        return self.__trans

    def invoke(self, reqmsg):
        '''
        @brief: 远程过程调用处理方法
        @param reqmsg: 请求响应报文
        @type reqmsg: ReqMessage
        @return: 错误码：0表示成功，-1表示连接失败
        @rtype: int
        '''
        tarsLogger.debug('AdapterProxy:invoke')
        assert(self.__trans)

        if (not self.__trans.hasConnected() and
            not self.__trans.isConnecting):
            # -1表示连接失败
            return -1

        reqmsg.request.iRequestId = self.__object.getTimeoutQueue().generateId()
        self.__object.getTimeoutQueue().push(reqmsg, reqmsg.request.iRequestId)

        self.__reactor.notify(self)

        return 0

    def finished(self, rsp):
        '''
        @brief: 远程过程调用返回处理
        @param rsp: 响应报文
        @type rsp: ResponsePacket
        @return: 函数是否执行成功
        @rtype: bool
        '''
        tarsLogger.debug('AdapterProxy:finished')
        reqmsg = self.__object.getTimeoutQueue().pop(rsp.iRequestId)
        if not reqmsg:
            tarsLogger.error(
                'finished, can not get ReqMessage, may be timeout, id: %d',
                rsp.iRequestId)
            return False

        reqmsg.response = rsp
        if reqmsg.type == ReqMessage.SYNC_CALL:
            return reqmsg.servant._finished(reqmsg)
        elif reqmsg.callback:
            self.__asyncProc.put(reqmsg)
            return True

        tarsLogger.error('finished, adapter proxy finish fail, id: %d, ret: %d',
                        rsp.iRequestId, rsp.iRet)
        return False

    # 检测连接是否失败，失效时重连
    def checkActive(self, forceConnect = False):
        '''
        @brief: 检测连接是否失效
        @param forceConnect: 是否强制发起连接，为true时不对状态进行判断就发起连接
        @type forceConnect: bool
        @return: 连接是否有效
        @rtype: bool
        '''
        tarsLogger.debug('AdapterProxy:checkActive')
        self.__lock.acquire()
        tarsLogger.info('checkActive, %s, forceConnect: %s',
                       self.__trans.getEndPointInfo(), forceConnect)

        if not self.__trans.isConnecting() and not self.__trans.hasConnected():
            self.doReconnect()

        self.__lock.release()
        return self.__trans.isConnecting() or self.__trans.hasConnected()

    def doReconnect(self):
        '''
        @brief: 重新发起连接
        @return: None
        @rtype: None
        '''
        tarsLogger.debug('AdapterProxy:doReconnect')
        assert(self.__trans)

        self.__trans.reInit()
        tarsLogger.info('doReconnect, connect: %s, fd:%d',
                       self.__trans.getEndPointInfo(),
                       self.__trans.getFd())

        self.__reactor.registerAdapter(self, select.EPOLLIN | select.EPOLLOUT)

    def sendRequest(self):
        '''
        @brief: 把队列中的请求放到Transceiver的发送缓存里
        @return: 放入缓存的数据长度
        @rtype: int
        '''
        tarsLogger.debug('AdapterProxy:sendRequest')
        if not self.__trans.hasConnected():
            return False

        reqmsg = self.__object.popRequest()
        blen = 0
        while reqmsg:
            reqmsg.adapter = self
            buf = reqmsg.packReq()
            self.__trans.writeToSendBuf(buf)
            tarsLogger.info('sendRequest, id: %d, len: %d',
                           reqmsg.request.iRequestId, len(buf))
            blen += len(buf)
            # 合并一次发送的包 最大合并至8k 提高异步时客户端效率?
            if (self.__trans.getEndPointInfo().getConnType() == EndPointInfo.SOCK_UDP
                or blen > 8192):
                break
            reqmsg = self.__object.popRequest()

        return blen

    def finishConnect(self):
        '''
        @brief: 使用的非阻塞socket连接不能立刻判断是否连接成功，
                在epoll响应后调用此函数处理connect结束后的操作
        @return: 是否连接成功
        @rtype: bool
        '''
        tarsLogger.debug('AdapterProxy:finishConnect')
        success = True
        errmsg = ''
        try:
            ret = self.__trans.getSock().getsockopt(
                socket.SOL_SOCKET, socket.SO_ERROR)
            if ret:
                success = False
                errmsg = os.strerror(ret)
        except Exception, msg:
            errmsg = msg
            success = False

        if not success:
            self.__reactor.unregisterAdapter(
                self, socket.EPOLLIN | socket.EPOLLOUT)
            self.__trans.close()
            self.__trans.setConnFailed()
            tarsLogger.error(
                'AdapterProxy finishConnect, exception: %s, error: %s',
                self.__trans.getEndPointInfo(), errmsg)
            return False
        self.__trans.setConnected()
        self.__reactor.notify(self)
        tarsLogger.info('AdapterProxy finishConnect, connect %s success',
                       self.__trans.getEndPointInfo())
        return True

    def finishInvoke(self, isTimeout):
        pass

    # 弹出请求报文
    def popRequest(self):
        pass

    def shouldCloseTrans(self):
        '''
        @brief: 是否设置关闭连接
        @return: 关闭连接的flag的值
        @rtype: bool
        '''
        return self.__closeTrans

    def setCloseTrans(self, closeTrans):
        '''
        @brief: 设置关闭连接flag的值
        @param closeTrans: 是否关闭连接
        @type closeTrans: bool
        @return: None
        @rtype: None
        '''
        self.__closeTrans = closeTrans


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
        #self.__adapter = None
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
        async_timeout = self.__comm.getProperty('async-invoke-timeout', float) / 1000
        self.__timeoutQueue = TimeoutQueue(async_timeout)

        self.__name = connInfo['name']

        self.__timeout = self.__comm.getProperty('sync-invoke-timeout', float) / 1000

        # 通过Communicator的配置设置超时
        # 不再通过连接信息的-t来设置
        #if connInfo['timeout'] != -1:
            #self.__timeout = connInfo['timeout']
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

    #def setTimeout(self, timeout):
        #'''
        #@brief: 设置超时
        #@param timeout: 超时时间，单位为s
        #@type timeout: float
        #@return: None
        #@rtype: None
        #'''
        #self.__timeout = timeout
        #self.__timeoutQueue.setTimeout(timeout)

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
        #tarsLogger.debug('ObjectProxy:handleQueueTimeout')
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
        adapter = self.__adpmanager.getNextValidProxy()
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
        return self.__timeoutQueue.pop(erase = False)

from QueryF import QueryFProxy
from QueryF import QueryFPrxCallback

class QueryRegisterCallback(QueryFPrxCallback):
    def __init__(self, adpManager):
        self.__adpManager = adpManager
        #super(QueryRegisterCallback, self).__init__()
        QueryFPrxCallback.__init__(self)

    def callback_findObjectById4All(self, ret, activeEp, inactiveEp):
        eplist = [EndPointInfo(x.host, x.port)
                  for x in activeEps if ret == 0 and x.istcp]
        self.__adpManager.setEndpoints(eplist)

    def callback_findObjectById4All_exception(self, ret):
        tarsLogger.error('callback_findObjectById4All_exception ret: %d', ret)

class AdapterProxyManager:
    '''
    @brief: 管理Adapter
    '''
    def __init__(self):
        tarsLogger.debug('AdapterProxyManager:__init__')
        self.__comm = None
        self.__object = None
        self.__adps = {}
        self.__lock = None
        self.__isDirectProxy = True
        self.__lastFreshTime = 0
        self.__queryRegisterCallback = QueryRegisterCallback(self)

    def initialize(self, comm, object, eplist):
        '''
        @brief: 初始化
        '''
        tarsLogger.debug('AdapterProxyManager:initialize')
        self.__comm = comm
        self.__object = object
        self.__lock = threading.Lock()

        self.__isDirectProxy = len(eplist) > 0
        if self.__isDirectProxy:
            self.setEndpoints(eplist)
        else:
            self.refreshEndpoints()

    def terminate(self):
        '''
        @brief: 释放资源
        '''
        tarsLogger.debug('AdapterProxyManager:terminate')
        self.__lock.acquire()
        for ep, epinfo in self.__adps.iteritems():
            epinfo[1].terminate()
        self.__adps = {}
        self.__lock.release()

    def refreshEndpoints(self):
        '''
        @brief: 刷新服务器列表
        @return: 新的服务列表
        @rtype: EndPointInfo列表
        '''
        tarsLogger.debug('AdapterProxyManager:refreshEndpoints')
        if self.__isDirectProxy:
            return

        interval = self.__comm.getProperty('refresh-endpoint-interval', float) / 1000
        locator = self.__comm.getProperty('locator')

        if '@' not in locator:
            raise TarsRegistryException('locator is not valid: ' + locator)

        now = time.time()
        last = self.__lastFreshTime
        epSize = len(self.__adps)
        if last + interval < now or ( epSize <= 0 and last + 2 < now):
            queryFPrx = self.__comm.stringToProxy(QueryFProxy, locator)
            if epSize == 0 or last == 0:
                ret, activeEps, inactiveEps = (
                       queryFPrx.findObjectById4All(self.__object.name()))
                # 目前只支持TCP
                eplist = [EndPointInfo(x.host, x.port)
                          for x in activeEps if ret == 0 and x.istcp]
                self.setEndpoints(eplist)
            else:
                queryFPrx.async_findObjectById4All(self.__queryRegisterCallback,
                          self.__object.name())
            self.__lastFreshTime = now

    def getEndpoints(self):
        '''
        @brief: 获取可用服务列表 如果启用分组,只返回同分组的服务端ip
        @return: 
        @rtype: EndPointInfo列表
        '''
        tarsLogger.debug('AdapterProxyManager:getEndpoints')
        self.__lock.acquire()
        ret = [x[1][0] for x in self.__adps.iteritems()]
        self.__lock.release()

        return ret

    def setEndpoints(self, eplist):
        '''
        @brief: 设置服务端信息
        '''
        tarsLogger.debug('AdapterProxyManager:setEndpoints')
        adps = {}
        comm = self.__comm
        self.__lock.acquire()
        for ep in eplist:
            epstr = str(ep)
            if epstr in self.__adps:
                adps[epstr] = self.__adps[epstr]
                continue
            adapter = AdapterProxy()
            adapter.initialize(ep, self.__object,
                                   comm.getReactor(), comm.getAsyncProc())
            adps[str(ep)] = [ep, adapter, 0]

        self.__adps, adps = adps, self.__adps
        self.__lock.release()

        for ep in adps:
            if ep not in self.__adps:
                adps[ep][1].terminate()

    def getNextValidProxy(self):
        '''
        @brief: 刷新本地缓存列表，如果服务下线了，要求删除本地缓存
        @return: 
        @rtype: EndPointInfo列表
        @todo: 优化负载均衡算法
        '''
        tarsLogger.debug('AdapterProxyManager:getNextValidProxy')
        minEp, miniInfo = '', [None, None, 0]
        self.refreshEndpoints()
        self.__lock.acquire()
        for epstr, adpinfo in self.__adps.iteritems():
            if adpinfo[2] >= miniInfo[2]:
                minEp, miniInfo = epstr, adpinfo
        miniInfo[2] += 1
        self.__lock.release()

        return miniInfo[1]

if __name__ == '__main__':
    connAddr = "apptest.lightServer.lightServantObj@tcp -h 10.130.64.220 -p 10001 -t 10000"
    connAddr = 'MTT.BookMarksUnifyServer.BookMarksUnifyObj@tcp -h 172.17.149.77 -t 60000 -p 10023'
    comm = Communicator()
    comm.initialize()
    servant = ServantProxy()
    servant = comm.stringToProxy(connAddr, servant)
    print servant.tars_timeout()
    try:
        rsp = servant.tars_invoke(ServantProxy.TARSNORMAL, "test", '', ServantProxy.mapcls_context(), None)
        print 'Servant invoke success, request id: %d, iRet: %d' % (rsp.iRequestId, rsp.iRet)
    except Exception, msg:
        print msg
    finally:
        servant.tars_terminate()
    time.sleep(2)
    print 'app closing ...'
    comm.terminate()
    time.sleep(2)
    print 'cpp closed'

