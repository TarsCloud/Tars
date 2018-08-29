#!/usr/bin/env python
# -*- coding: utf-8 -*-
# filename: __servantproxy.py

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
@brief: rpc抽离出servantproxy
'''
import threading
import time

from __logger import tarsLogger
from __util import util
from __packet import RequestPacket
# from __packet import ResponsePacket
from __TimeoutQueue import ReqMessage
import exception
from exception import TarsException


class ServantProxy(object):
    '''
    @brief: 1、远程对象的本地代理
            2、同名servant在一个通信器中最多只有一个实例
            3、防止和用户在Tars中定义的函数名冲突，接口以tars_开头
    '''

    # 服务器响应的错误码
    TARSSERVERSUCCESS = 0  # 服务器端处理成功
    TARSSERVERDECODEERR = -1  # 服务器端解码异常
    TARSSERVERENCODEERR = -2  # 服务器端编码异常
    TARSSERVERNOFUNCERR = -3  # 服务器端没有该函数
    TARSSERVERNOSERVANTERR = -4  # 服务器端五该Servant对象
    TARSSERVERRESETGRID = -5  # 服务器端灰度状态不一致
    TARSSERVERQUEUETIMEOUT = -6  # 服务器队列超过限制
    TARSASYNCCALLTIMEOUT = -7  # 异步调用超时
    TARSPROXYCONNECTERR = -8  # proxy链接异常
    TARSSERVERUNKNOWNERR = -99  # 服务器端未知异常

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
        # 默认的为3S = ObjectProxy.DEFAULT_TIMEOUT
        return int(self.__timeout() * 1000)

    def tars_ping(self):
        pass

    # def tars_initialize(self):
        # pass

    # def tars_terminate(self):
        # pass

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
        req.iRequestId = 0
        req.sServantName = self.tars_name()
        req.sFuncName = sFuncName
        req.sBuffer = sBuffer
        req.iTimeout = self.tars_timeout()

        reqmsg = ReqMessage()
        reqmsg.type = ReqMessage.SYNC_CALL
        reqmsg.servant = self
        reqmsg.lock = threading.Condition()
        reqmsg.request = req
        reqmsg.begtime = time.time()
        # # test
        reqmsg.isHash = True
        reqmsg.isConHash = True
        reqmsg.hashCode = 123456

        rsp = None
        try:
            rsp = self.__invoke(reqmsg)
        except exception.TarsSyncCallTimeoutException:
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
        req.iRequestId = 0
        req.sServantName = self.tars_name()
        req.sFuncName = sFuncName
        req.sBuffer = sBuffer
        req.iTimeout = self.tars_timeout()

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
        except Exception:
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
        tarsLogger.debug('ServantProxy:invoke, func: %s',
                         reqmsg.request.sFuncName)
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
                raise exception.TarsSyncCallTimeoutException(errmsg)
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
            raise exception.TarsServerDecodeException(
                "server decode exception: errno: %d, msg: %s" % (errno, desc))

        elif errno == ServantProxy.TARSSERVERENCODEERR:
            raise exception.TarsServerEncodeException(
                "server encode exception: errno: %d, msg: %s" % (errno, desc))

        elif errno == ServantProxy.TARSSERVERNOFUNCERR:
            raise exception.TarsServerNoFuncException(
                "server function mismatch exception: errno: %d, msg: %s" % (errno, desc))

        elif errno == ServantProxy.TARSSERVERNOSERVANTERR:
            raise exception.TarsServerNoServantException(
                "server servant mismatch exception: errno: %d, msg: %s" % (errno, desc))

        elif errno == ServantProxy.TARSSERVERRESETGRID:
            raise exception.TarsServerResetGridException(
                "server reset grid exception: errno: %d, msg: %s" % (errno, desc))

        elif errno == ServantProxy.TARSSERVERQUEUETIMEOUT:
            raise exception.TarsServerQueueTimeoutException(
                "server queue timeout exception: errno: %d, msg: %s" % (errno, desc))

        elif errno == ServantProxy.TARSPROXYCONNECTERR:
            raise exception.TarsServerQueueTimeoutException(
                "server connection lost: errno: %d, msg: %s" % (errno, desc))

        else:
            raise exception.TarsServerUnknownException(
                "server unknown exception: errno: %d, msg: %s" % (errno, desc))
