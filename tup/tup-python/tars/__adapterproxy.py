#!/usr/bin/env python
# -*- coding: utf-8 -*-
# filename: __adapterproxymanager.py_compiler

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
@brief: 将rpc部分中的adapterproxymanager抽离出来，实现不同的负载均衡
'''

from enum import Enum
import random
import socket
import select
import os
import time


from __util import (LockGuard, NewLock, ConsistentHashNew)
from __trans import EndPointInfo
from __logger import tarsLogger
import exception
from __trans import TcpTransceiver
from __TimeoutQueue import ReqMessage
from exception import TarsException


# 因为循环import的问题只能放这里，不能放文件开始处
from QueryF import QueryFProxy
from QueryF import QueryFPrxCallback


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
        self.__activeStateInReg = True

    @property
    def activatestateinreg(self):
        return self.__activeStateInReg

    @activatestateinreg.setter
    def activatestateinreg(self, value):
        self.__activeStateInReg = value

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
        # self.__lock = threading.Lock()
        self.__lock = NewLock()
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
    def checkActive(self, forceConnect=False):
        '''
        @brief: 检测连接是否失效
        @param forceConnect: 是否强制发起连接，为true时不对状态进行判断就发起连接
        @type forceConnect: bool
        @return: 连接是否有效
        @rtype: bool
        '''
        tarsLogger.debug('AdapterProxy:checkActive')
        # self.__lock.acquire()
        lock = LockGuard(self.__lock)
        tarsLogger.info('checkActive, %s, forceConnect: %s',
                        self.__trans.getEndPointInfo(), forceConnect)

        if not self.__trans.isConnecting() and not self.__trans.hasConnected():
            self.doReconnect()

        # self.__lock.release()
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


class QueryRegisterCallback(QueryFPrxCallback):
    def __init__(self, adpManager):
        self.__adpManager = adpManager
        super(QueryRegisterCallback, self).__init__()
        # QueryFPrxCallback.__init__(self)

    def callback_findObjectById4All(self, ret, activeEp, inactiveEp):
        eplist = [EndPointInfo(x.host, x.port, x.timeout, x.weight, x.weightType)
                  for x in activeEp if ret == 0 and x.istcp]
        ieplist = [EndPointInfo(x.host, x.port, x.timeout, x.weight, x.weightType)
                   for x in inactiveEp if ret == 0 and x.istcp]
        self.__adpManager.setEndpoints(eplist, ieplist)

    def callback_findObjectById4All_exception(self, ret):
        tarsLogger.error('callback_findObjectById4All_exception ret: %d', ret)


class EndpointWeightType(Enum):
    E_LOOP = 0
    E_STATIC_WEIGHT = 1


class AdapterProxyManager:
    '''
    @brief: 管理Adapter
    '''

    def __init__(self):
        tarsLogger.debug('AdapterProxyManager:__init__')
        self.__comm = None
        self.__object = None
        # __adps的key=str(EndPointInfo) value=[EndPointInfo, AdapterProxy, cnt]
        # cnt是访问次数
        self.__adps = {}
        self.__iadps = {}
        self.__newLock = None
        self.__isDirectProxy = True
        self.__lastFreshTime = 0
        self.__queryRegisterCallback = QueryRegisterCallback(self)
        self.__regAdapterProxyDict = {}
        self.__lastConHashPrxList = []
        self.__consistentHashWeight = None
        self.__weightType = EndpointWeightType.E_LOOP
        self.__update = True
        self.__lastWeightedProxyData = {}

    def initialize(self, comm, objectProxy, eplist):
        '''
        @brief: 初始化
        '''
        tarsLogger.debug('AdapterProxyManager:initialize')
        self.__comm = comm
        self.__object = objectProxy
        self.__newLock = NewLock()

        self.__isDirectProxy = len(eplist) > 0
        if self.__isDirectProxy:
            self.setEndpoints(eplist, {})
        else:
            self.refreshEndpoints()

    def terminate(self):
        '''
        @brief: 释放资源
        '''
        tarsLogger.debug('AdapterProxyManager:terminate')
        # self.__lock.acquire()
        lock = LockGuard(self.__newLock)
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

        interval = self.__comm.getProperty(
            'refresh-endpoint-interval', float) / 1000
        locator = self.__comm.getProperty('locator')

        if '@' not in locator:
            raise exception.TarsRegistryException(
                'locator is not valid: ' + locator)

        now = time.time()
        last = self.__lastFreshTime
        epSize = len(self.__adps)
        if last + interval < now or (epSize <= 0 and last + 2 < now):
            queryFPrx = self.__comm.stringToProxy(QueryFProxy, locator)
            # 首次访问是同步调用，之后访问是异步调用
            if epSize == 0 or last == 0:
                ret, activeEps, inactiveEps = (
                    queryFPrx.findObjectById4All(self.__object.name()))
                # 目前只支持TCP
                eplist = [EndPointInfo(x.host, x.port, x.timeout, x.weight, x.weightType)
                          for x in activeEps if ret == 0 and x.istcp]
                ieplist = [EndPointInfo(x.host, x.port, x.timeout, x.weight, x.weightType)
                           for x in inactiveEps if ret == 0 and x.istcp]
                self.setEndpoints(eplist, ieplist)
            else:
                queryFPrx.async_findObjectById4All(self.__queryRegisterCallback,
                                                   self.__object.name())
            self.__lastFreshTime = now

    def getEndpoints(self):
        '''
        @brief: 获取可用服务列表 如果启用分组,只返回同分组的服务端ip
        @return: 获取节点列表
        @rtype: EndPointInfo列表
        '''
        tarsLogger.debug('AdapterProxyManager:getEndpoints')
        # self.__lock.acquire()
        lock = LockGuard(self.__newLock)
        ret = [x[1][0] for x in self.__adps.items()]
        # self.__lock.release()

        return ret

    def setEndpoints(self, eplist, ieplist):
        '''
        @brief: 设置服务端信息
        @para eplist: 活跃的被调节点列表
        @para ieplist: 不活跃的被调节点列表
        '''
        tarsLogger.debug('AdapterProxyManager:setEndpoints')
        adps = {}
        iadps = {}
        comm = self.__comm
        isNeedNotify = False
        # self.__lock.acquire()
        lock = LockGuard(self.__newLock)
        isStartStatic = True

        for ep in eplist:
            if ep.getWeightType() == 0:
                isStartStatic = False
            epstr = str(ep)
            if epstr in self.__adps:
                adps[epstr] = self.__adps[epstr]
                continue
            isNeedNotify = True
            self.__update = True
            adapter = AdapterProxy()
            adapter.initialize(ep, self.__object,
                               comm.getReactor(), comm.getAsyncProc())
            adapter.activatestateinreg = True
            adps[epstr] = [ep, adapter, 0]
        self.__adps, adps = adps, self.__adps

        for iep in ieplist:
            iepstr = str(iep)
            if iepstr in self.__iadps:
                iadps[iepstr] = self.__iadps[iepstr]
                continue
            isNeedNotify = True
            adapter = AdapterProxy()
            adapter.initialize(iep, self.__object,
                               comm.getReactor(), comm.getAsyncProc())
            adapter.activatestateinreg = False
            iadps[iepstr] = [iep, adapter, 0]
        self.__iadps, iadps = iadps, self.__iadps

        if isStartStatic:
            self.__weightType = EndpointWeightType.E_STATIC_WEIGHT
        else:
            self.__weightType = EndpointWeightType.E_LOOP

        # self.__lock.release()
        if isNeedNotify:
            self.__notifyEndpoints(self.__adps, self.__iadps)
        # 关闭已经失效的连接
        for ep in adps:
            if ep not in self.__adps:
                adps[ep][1].terminate()

    def __notifyEndpoints(self, actives, inactives):
        # self.__lock.acquire()
        lock = LockGuard(self.__newLock)
        self.__regAdapterProxyDict.clear()
        self.__regAdapterProxyDict.update(actives)
        self.__regAdapterProxyDict.update(inactives)
        # self.__lock.release()

    def __getNextValidProxy(self):
        '''
        @brief: 刷新本地缓存列表，如果服务下线了，要求删除本地缓存
        @return:
        @rtype: EndPointInfo列表
        @todo: 优化负载均衡算法
        '''
        tarsLogger.debug('AdapterProxyManager:getNextValidProxy')
        lock = LockGuard(self.__newLock)
        if len(self.__adps) == 0:
            raise TarsException("the activate adapter proxy is empty")

        sortedActivateAdp = sorted(
            self.__adps.items(), key=lambda item: item[1][2])
        # self.refreshEndpoints()
        # self.__lock.acquire()
        sortedActivateAdpSize = len(sortedActivateAdp)

        while sortedActivateAdpSize != 0:
            if sortedActivateAdp[0][1][1].checkActive():
                self.__adps[sortedActivateAdp[0][0]][2] += 1
                # 返回的是 adapterProxy
                return self.__adps[sortedActivateAdp[0][0]][1]
            sortedActivateAdp.pop(0)
            sortedActivateAdpSize -= 1
        # 随机重连一个可用节点
        adpPrx = self.__adps.items()[random.randint(
            0, len(self.__adps))][1][1]
        adpPrx.checkActive()
        return None
        # self.__lock.release()

    def __getHashProxy(self, reqmsg):
        if self.__weightType == EndpointWeightType.E_LOOP:
            if reqmsg.isConHash:
                return self.__getConHashProxyForNormal(reqmsg.hashCode)
            else:
                return self.__getHashProxyForNormal(reqmsg.hashCode)
        else:
            if reqmsg.isConHash:
                return self.__getConHashProxyForWeight(reqmsg.hashCode)
            else:
                return self.__getHashProxyForWeight(reqmsg.hashCode)

    def __getHashProxyForNormal(self, hashCode):
        tarsLogger.debug('AdapterProxyManager:getHashProxyForNormal')
        # self.__lock.acquire()
        lock = LockGuard(self.__newLock)
        regAdapterProxyList = sorted(
            self.__regAdapterProxyDict.items(), key=lambda item: item[0])

        allPrxSize = len(regAdapterProxyList)
        if allPrxSize == 0:
            raise TarsException("the adapter proxy is empty")
        hashNum = hashCode % allPrxSize

        if regAdapterProxyList[hashNum][1][1].activatestateinreg and regAdapterProxyList[hashNum][1][1].checkActive():
            epstr = regAdapterProxyList[hashNum][0]
            self.__regAdapterProxyDict[epstr][2] += 1
            if epstr in self.__adps:
                self.__adps[epstr][2] += 1
            elif epstr in self.__iadps:
                self.__iadps[epstr][2] += 1
            return self.__regAdapterProxyDict[epstr][1]
        else:
            if len(self.__adps) == 0:
                raise TarsException("the activate adapter proxy is empty")
            activeProxyList = self.__adps.items()
            actPrxSize = len(activeProxyList)
            while actPrxSize != 0:
                hashNum = hashCode % actPrxSize
                if activeProxyList[hashNum][1][1].checkActive():
                    self.__adps[activeProxyList[hashNum][0]][2] += 1
                    return self.__adps[activeProxyList[hashNum][0]][1]
                activeProxyList.pop(hashNum)
                actPrxSize -= 1
            # 随机重连一个可用节点
            adpPrx = self.__adps.items()[random.randint(
                0, len(self.__adps))][1][1]
            adpPrx.checkActive()
            return None

    def __getConHashProxyForNormal(self, hashCode):
        tarsLogger.debug('AdapterProxyManager:getConHashProxyForNormal')
        lock = LockGuard(self.__newLock)
        if len(self.__regAdapterProxyDict) == 0:
            raise TarsException("the adapter proxy is empty")
        if self.__consistentHashWeight is None or self.__checkConHashChange(self.__lastConHashPrxList):
            self.__updateConHashProxyWeighted()

        if len(self.__consistentHashWeight.nodes) > 0:
            conHashIndex = self.__consistentHashWeight.getNode(hashCode)
            if conHashIndex in self.__regAdapterProxyDict and self.__regAdapterProxyDict[conHashIndex][1].activatestateinreg and self.__regAdapterProxyDict[conHashIndex][1].checkActive():
                self.__regAdapterProxyDict[conHashIndex][2] += 1
                if conHashIndex in self.__adps:
                    self.__adps[conHashIndex][2] += 1
                elif conHashIndex in self.__iadps:
                    self.__iadps[conHashIndex][2] += 1
                return self.__regAdapterProxyDict[conHashIndex][1]
            else:
                if len(self.__adps) == 0:
                    raise TarsException("the activate adapter proxy is empty")
                activeProxyList = self.__adps.items()
                actPrxSize = len(activeProxyList)
                while actPrxSize != 0:
                    hashNum = hashCode % actPrxSize
                    if activeProxyList[hashNum][1][1].checkActive():
                        self.__adps[activeProxyList[hashNum][0]][2] += 1
                        return self.__adps[activeProxyList[hashNum][0]][1]
                    activeProxyList.pop(hashNum)
                    actPrxSize -= 1
                # 随机重连一个可用节点
                adpPrx = self.__adps.items()[random.randint(
                    0, len(self.__adps))][1][1]
                adpPrx.checkActive()
                return None
            pass
        else:
            return self.__getHashProxyForNormal(hashCode)

    def __getHashProxyForWeight(self, hashCode):
        return None
        pass

    def __getConHashProxyForWeight(self, hashCode):
        return None
        pass

    def __checkConHashChange(self, lastConHashPrxList):
        tarsLogger.debug('AdapterProxyManager:checkConHashChange')
        lock = LockGuard(self.__newLock)
        if len(lastConHashPrxList) != len(self.__regAdapterProxyDict):
            return True
        regAdapterProxyList = sorted(
            self.__regAdapterProxyDict.items(), key=lambda item: item[0])
        regAdapterProxyListSize = len(regAdapterProxyList)
        for index in range(regAdapterProxyListSize):
            if cmp(lastConHashPrxList[index][0], regAdapterProxyList[index][0]) != 0:
                return True
        return False

    def __updateConHashProxyWeighted(self):
        tarsLogger.debug('AdapterProxyManager:updateConHashProxyWeighted')
        lock = LockGuard(self.__newLock)
        if len(self.__regAdapterProxyDict) == 0:
            raise TarsException("the adapter proxy is empty")
        self.__lastConHashPrxList = sorted(
            self.__regAdapterProxyDict.items(), key=lambda item: item[0])
        nodes = []
        for var in self.__lastConHashPrxList:
            nodes.append(var[0])
        if self.__consistentHashWeight is None:
            self.__consistentHashWeight = ConsistentHashNew(nodes)
        else:
            theOldActiveNodes = [
                var for var in nodes if var in self.__consistentHashWeight.nodes]

            theOldInactiveNodes = [
                var for var in self.__consistentHashWeight.nodes if var not in theOldActiveNodes]
            for var in theOldInactiveNodes:
                self.__consistentHashWeight.removeNode(var)

            theNewActiveNodes = [
                var for var in nodes if var not in theOldActiveNodes]
            for var in theNewActiveNodes:
                self.__consistentHashWeight.addNode(var)

            self.__consistentHashWeight.nodes = nodes
        pass

    def __getWeightedProxy(self):
        tarsLogger.debug('AdapterProxyManager:getWeightedProxy')
        lock = LockGuard(self.__newLock)
        if len(self.__adps) == 0:
            raise TarsException("the activate adapter proxy is empty")

        if self.__update is True:
            self.__lastWeightedProxyData.clear()
            weightedProxyData = {}
            minWeight = (self.__adps.items()[0][1][0]).getWeight()
            for item in self.__adps.items():
                weight = (item[1][0].getWeight())
                weightedProxyData[item[0]] = (weight)
                if minWeight > weight:
                    minWeight = weight

            if minWeight <= 0:
                addWeight = -minWeight + 1
                for item in weightedProxyData.items():
                    item[1] += addWeight

            self.__update = False
            self.__lastWeightedProxyData = weightedProxyData

        weightedProxyData = self.__lastWeightedProxyData
        while len(weightedProxyData) > 0:
            total = sum(weightedProxyData.values())
            rand = random.randint(1, total)
            temp = 0
            for item in weightedProxyData.items():
                temp += item[1]
                if rand <= temp:
                    if self.__adps[item[0]][1].checkActive():
                        self.__adps[item[0]][2] += 1
                        return self.__adps[item[0]][1]
                    else:
                        weightedProxyData.pop(item[0])
                        break
        # 没有一个活跃的节点
        # 随机重连一个可用节点
        adpPrx = self.__adps.items()[random.randint(
            0, len(self.__adps))][1][1]
        adpPrx.checkActive()
        return None

    def selectAdapterProxy(self, reqmsg):
        '''
        @brief: 刷新本地缓存列表，如果服务下线了，要求删除本地缓存，通过一定算法返回AdapterProxy
        @param: reqmsg:请求响应报文
        @type reqmsg: ReqMessage
        @return:
        @rtype: EndPointInfo列表
        @todo: 优化负载均衡算法
        '''
        tarsLogger.debug('AdapterProxyManager:selectAdapterProxy')
        self.refreshEndpoints()
        if reqmsg.isHash:
            return self.__getHashProxy(reqmsg)
        else:
            if self.__weightType == EndpointWeightType.E_STATIC_WEIGHT:
                return self.__getWeightedProxy()
            else:
                return self.__getNextValidProxy()
