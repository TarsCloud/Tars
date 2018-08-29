#!/usr/bin/env python
# -*- coding: utf-8 -*-
# filename: __trans.py

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
@brief: 网络相关模块
'''

import socket
import select
import errno
import threading

from __logger import tarsLogger
from __TimeoutQueue import ReqMessage


class EndPointInfo:
    '''
    @brief: 保存每个连接端口的信息
    '''
    SOCK_TCP = 'TCP'
    SOCK_UDP = 'UDP'

    def __init__(self,
                 ip='',
                 port=0,
                 timeout=-1,
                 weight=0,
                 weightType=0,
                 connType=SOCK_TCP):
        self.__ip = ip
        self.__port = port
        self.__timeout = timeout
        self.__connType = connType
        self.__weightType = weightType
        self.__weight = weight

    def getIp(self):
        return self.__ip

    def getPort(self):
        return self.__port

    def getConnType(self):
        '''
        @return: 传输层连接类型
        @rtype: EndPointInfo.SOCK_TCP 或 EndPointInfo.SOCK_UDP
        '''
        return self.__connType

    def getWeightType(self):
        return self.__weightType

    def getWeight(self):
        return self.__weight

    def __str__(self):
        return '%s %s:%s %d:%d' % (self.__connType, self.__ip, self.__port, self.__weightType, self.__weight)


class Transceiver:
    '''
    @brief: 网络传输基类，提供网络send/recv接口
    '''
    CONNECTED = 0
    CONNECTING = 1
    UNCONNECTED = 2

    def __init__(self, endPointInfo):
        tarsLogger.debug('Transceiver:__init__, %s', endPointInfo)
        self.__epi = endPointInfo
        self.__sock = None
        self.__connStatus = Transceiver.UNCONNECTED
        self.__connFailed = False
        # 这两个变量要给子类用，不能用name mangling隐藏
        self._sendBuff = ''
        self._recvBuf = ''

    def __del__(self):
        tarsLogger.debug('Transceiver:__del__')
        self.close()

    def getSock(self):
        '''
        @return: socket对象
        @rtype: socket.socket
        '''
        return self.__sock

    def getFd(self):
        '''
        @brief: 获取socket的文件描述符
        @return: 如果self.__sock没有建立返回-1
        @rtype: int
        '''
        if self.__sock:
            return self.__sock.fileno()
        else:
            return -1

    def getEndPointInfo(self):
        '''
        @return: 端口信息
        @rtype: EndPointInfo
        '''
        return self.__epi

    def isValid(self):
        '''
        @return: 是否创建了socket
        @rtype: bool
        '''
        return self.__sock is not None

    def hasConnected(self):
        '''
        @return: 是否连接上了
        @rtype: bool
        '''
        return self.isValid() and self.__connStatus == Transceiver.CONNECTED

    def isConnFailed(self):
        '''
        @return: 是否连接失败
        @rtype: bool
        '''
        return self.__connFailed

    def isConnecting(self):
        '''
        @return: 是否正在连接
        @rtype: bool
        '''
        return self.isValid() and self.__connStatus == Transceiver.CONNECTING

    def setConnFailed(self):
        '''
        @brief: 设置为连接失败
        @return: None
        @rtype: None
        '''
        self.__connFailed = True
        self.__connStatus = Transceiver.UNCONNECTED

    def setConnected(self):
        '''
        @brief: 设置为连接完
        @return: None
        @rtype: None
        '''
        self.__connFailed = False
        self.__connStatus = Transceiver.CONNECTED

    def close(self):
        '''
        @brief: 关闭连接
        @return: None
        @rtype: None
        @note: 多次调用不会有问题
        '''
        tarsLogger.debug('Transceiver:close')
        if not self.isValid():
            return
        self.__sock.close()
        self.__sock = None
        self.__connStatus = Transceiver.UNCONNECTED
        self.__connFailed = False
        self._sendBuff = ''
        self._recvBuf = ''
        tarsLogger.info('trans close : %s' % self.__epi)

    def writeToSendBuf(self, msg):
        '''
        @brief: 把数据添加到send buffer里
        @param msg: 发送的数据
        @type msg: str
        @return: None
        @rtype: None
        @note: 没有加锁，多线程调用会有race conditions
        '''
        self._sendBuff += msg

    def recv(self, bufsize, flag=0):
        raise NotImplementedError()

    def send(self, buf, flag=0):
        raise NotImplementedError()

    def doResponse(self):
        raise NotImplementedError()

    def doRequest(self):
        '''
        @brief: 将请求数据发送出去
        @return: 发送的字节数
        @rtype: int
        '''
        tarsLogger.debug('Transceiver:doRequest')
        if not self.isValid():
            return -1

        nbytes = 0
        buf = buffer(self._sendBuff)
        while True:
            if not buf:
                break
            ret = self.send(buf[nbytes:])
            if ret > 0:
                nbytes += ret
            else:
                break

        # 发送前面的字节后将后面的字节拷贝上来
        self._sendBuff = buf[nbytes:]
        return nbytes

    def reInit(self):
        '''
        @brief: 初始化socket，并连接服务器
        @return: 成功返回0，失败返回-1
        @rtype: int
        '''
        tarsLogger.debug('Transceiver:reInit')
        assert(self.isValid() is False)
        if self.__epi.getConnType() != EndPointInfo.SOCK_TCP:
            return -1
        try:
            self.__sock = socket.socket()
            self.__sock.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
            self.__sock.setblocking(0)
            self.__sock.connect((self.__epi.getIp(), self.__epi.getPort()))
            self.__connStatus = Transceiver.CONNECTED
        except socket.error, msg:
            if msg.errno == errno.EINPROGRESS:
                self.__connStatus = Transceiver.CONNECTING
            else:
                tarsLogger.info('reInit, %s, faild!, %s',
                                self.__epi, msg)
                self.__sock = None
                return -1
        tarsLogger.info('reInit, connect: %s, fd: %d',
                        self.__epi, self.getFd())
        return 0


class TcpTransceiver(Transceiver):
    '''
    @brief: TCP传输实现
    '''

    def send(self, buf, flag=0):
        '''
        @brief: 实现tcp的发送
        @param buf: 发送的数据
        @type buf: str
        @param flag: 发送标志
        @param flag: int
        @return: 发送字节数
        @rtype: int
        '''
        tarsLogger.debug('TcpTransceiver:send')
        if not self.isValid():
            return -1

        nbytes = 0
        try:
            nbytes = self.getSock().send(buf, flag)
            tarsLogger.info('tcp send, fd: %d, %s, len: %d',
                            self.getFd(), self.getEndPointInfo(), nbytes)
        except socket.error, msg:
            if msg.errno != errno.EAGAIN:
                tarsLogger.error('tcp send, fd: %d, %s, fail!, %s, close',
                                 self.getFd(), self.getEndPointInfo(), msg)
                self.close()
                return 0
        return nbytes

    def recv(self, bufsize, flag=0):
        '''
        @brief: 实现tcp的recv
        @param bufsize: 接收大小
        @type bufsize: int
        @param flag: 接收标志
        @param flag: int
        @return: 接收的内容，接收出错返回None
        @rtype: str
        '''
        tarsLogger.debug('TcpTransceiver:recv')
        assert(self.isValid())

        buf = ''
        try:
            buf = self.getSock().recv(bufsize, flag)
            if len(buf) == 0:
                tarsLogger.info('tcp recv, fd: %d, %s, recv 0 bytes, close',
                                self.getFd(), self.getEndPointInfo())
                self.close()
                return None
        except socket.error, msg:
            if msg.errno != errno.EAGAIN:
                tarsLogger.info('tcp recv, fd: %d, %s, faild!, %s, close',
                                self.getFd(), self.getEndPointInfo(), msg)
                self.close()
                return None

        tarsLogger.info('tcp recv, fd: %d, %s, nbytes: %d',
                        self.getFd(), self.getEndPointInfo(), len(buf))
        return buf

    def doResponse(self):
        '''
        @brief: 处理接收的数据
        @return: 返回响应报文的列表，如果出错返回None
        @rtype: list: ResponsePacket
        '''
        tarsLogger.debug('TcpTransceiver:doResponse')
        if not self.isValid():
            return None

        bufs = [self._recvBuf]
        while True:
            buf = self.recv(8292)
            if not buf:
                break
            bufs.append(buf)
        self._recvBuf = ''.join(bufs)
        tarsLogger.info('tcp doResponse, fd: %d, recvbuf: %d',
                        self.getFd(), len(self._recvBuf))

        if not self._recvBuf:
            return None

        rsplist = None
        try:
            rsplist, bufsize = ReqMessage.unpackRspList(self._recvBuf)
            self._recvBuf = self._recvBuf[bufsize:]
        except Exception, msg:
            tarsLogger.error(
                'tcp doResponse, fd: %d, %s, tcp recv unpack error: %s',
                self.getFd(), self.getEndPointInfo(), msg)
            self.close()

        return rsplist


class FDReactor(threading.Thread):
    '''
    @brief: 监听FD事件并解发注册的handle
    '''

    def __init__(self):
        tarsLogger.debug('FDReactor:__init__')
        # threading.Thread.__init__(self)
        super(FDReactor, self).__init__()
        self.__terminate = False
        self.__ep = None
        self.__shutdown = None
        # {fd : adapterproxy}
        self.__adapterTab = {}

    def __del__(self):
        tarsLogger.debug('FDReactor:__del__')
        self.__ep.close()
        self.__shutdown.close()
        self.__ep = None
        self.__shutdown = None

    def initialize(self):
        '''
        @brief: 初始化，使用FDReactor前必须调用
        @return: None
        @rtype: None
        '''
        tarsLogger.debug('FDReactor:initialize')
        self.__ep = select.epoll()
        self.__shutdown = socket.socket()
        self.__ep.register(self.__shutdown.fileno(),
                           select.EPOLLET | select.EPOLLIN)
        tarsLogger.debug('FDReactor init, shutdown fd : %d',
                         self.__shutdown.fileno())

    def terminate(self):
        '''
        @brief: 结束FDReactor的线程
        @return: None
        @rtype: None
        '''
        tarsLogger.debug('FDReactor:terminate')
        self.__terminate = True
        self.__ep.modify(self.__shutdown.fileno(), select.EPOLLOUT)
        self.__adapterTab = {}

    def handle(self, adapter, events):
        '''
        @brief: 处理epoll事件
        @param adapter: 事件对应的adapter
        @type adapter: AdapterProxy
        @param events: epoll事件
        @param events: int
        @return: None
        @rtype: None
        '''
        tarsLogger.debug('FDReactor:handle events : %d', events)
        assert(adapter)

        try:
            if events == 0:
                return

            if events & (select.EPOLLERR | select.EPOLLHUP):
                tarsLogger.debug('FDReactor::handle EPOLLERR or EPOLLHUP: %s',
                                 adapter.trans().getEndPointInfo())
                adapter.trans().close()
                return

            if adapter.shouldCloseTrans():
                tarsLogger.debug('FDReactor::handle should close trans: %s',
                                 adapter.trans().getEndPointInfo())
                adapter.setCloseTrans(False)
                adapter.trans().close()
                return

            if adapter.trans().isConnecting():
                if not adapter.finishConnect():
                    return

            if events & select.EPOLLIN:
                self.handleInput(adapter)

            if events & select.EPOLLOUT:
                self.handleOutput(adapter)

        except Exception, msg:
            tarsLogger.error('FDReactor handle exception: %s', msg)

    def handleExcept(self):
        pass

    def handleInput(self, adapter):
        '''
        @brief: 处理接收事件
        @param adapter: 事件对应的adapter
        @type adapter: AdapterProxy
        @return: None
        @rtype: None
        '''

        tarsLogger.debug('FDReactor:handleInput')
        if not adapter.trans().isValid():
            return

        rsplist = adapter.trans().doResponse()
        if not rsplist:
            return
        for rsp in rsplist:
            adapter.finished(rsp)

    def handleOutput(self, adapter):
        '''
        @brief: 处理发送事件
        @param adapter: 事件对应的adapter
        @type adapter: AdapterProxy
        @return: None
        @rtype: None
        '''
        tarsLogger.debug('FDReactor:handleOutput')
        if not adapter.trans().isValid():
            return
        while adapter.trans().doRequest() >= 0 and adapter.sendRequest():
            pass

    def notify(self, adapter):
        '''
        @brief: 更新adapter对应的fd的epoll状态
        @return: None
        @rtype: None
        @note: FDReactor使用的epoll是EPOLLET模式，同一事件只通知一次
               希望某一事件再次通知需调用此函数
        '''
        tarsLogger.debug('FDReactor:notify')
        fd = adapter.trans().getFd()
        if fd != -1:
            self.__ep.modify(fd,
                             select.EPOLLET | select.EPOLLOUT | select.EPOLLIN)

    def registerAdapter(self, adapter, events):
        '''
        @brief: 注册adapter
        @param adapter: 收发事件处理类
        @type adapter: AdapterProxy
        @param events: 注册事件
        @type events: int
        @return: None
        @rtype: None
        '''
        tarsLogger.debug('FDReactor:registerAdapter events : %d', events)
        events |= select.EPOLLET
        try:
            self.__ep.unregister(adapter.trans().getFd())
        except:
            pass
        self.__ep.register(adapter.trans().getFd(), events)
        self.__adapterTab[adapter.trans().getFd()] = adapter

    def unregisterAdapter(self, adapter):
        '''
        @brief: 注销adapter
        @param adapter: 收发事件处理类
        @type adapter: AdapterProxy
        @return: None
        @rtype: None
        '''
        tarsLogger.debug('FDReactor:registerAdapter')
        self.__ep.unregister(adapter.trans().getFd())
        self.__adapterTab.pop(adapter.trans().getFd(), None)

    def run(self):
        '''
        @brief: 线程启动函数，循环监听网络事件
        '''
        tarsLogger.debug('FDReactor:run')

        while not self.__terminate:
            try:
                eplist = self.__ep.poll(1)
                if eplist:
                    tarsLogger.debug('FDReactor run get eplist : %s, terminate : %s', str(
                        eplist), self.__terminate)
                if self.__terminate:
                    tarsLogger.debug('FDReactor terminate')
                    break
                for fd, events in eplist:
                    adapter = self.__adapterTab.get(fd, None)
                    if not adapter:
                        continue
                    self.handle(adapter, events)
            except Exception, msg:
                tarsLogger.error('FDReactor run exception: %s', msg)

        tarsLogger.debug('FDReactor:run finished')


if __name__ == '__main__':
    print 'hello world'
    epi = EndPointInfo('127.0.0.1', 1313)
    print epi
    trans = TcpTransceiver(epi)
    print trans.getSock()
    print trans.getFd()
    print trans.reInit()
    print trans.isConnecting()
    print trans.hasConnected()
    buf = 'hello world'
    print trans.send(buf)
    buf = trans.recv(1024)
    print buf
    trans.close()
