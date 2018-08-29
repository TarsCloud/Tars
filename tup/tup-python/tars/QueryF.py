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

# from tars.core import tarscore;
# from tars.core import ServantProxy;
# from tars.core import ServantProxyCallback;
# from com.qq.register.EndpointF import *;
from core import tarscore
from __servantproxy import ServantProxy
from __async import ServantProxyCallback
from EndpointF import EndpointF

import time

# proxy for client


class QueryFProxy(ServantProxy):
    def findObjectById(self, id, context=ServantProxy.mapcls_context()):
        oos = tarscore.TarsOutputStream()
        oos.write(tarscore.string, 1, id)

        rsp = self.tars_invoke(ServantProxy.TARSNORMAL,
                               "findObjectById", oos.getBuffer(), context, None)

        ios = tarscore.TarsInputStream(rsp.sBuffer)
        ret = ios.read(tarscore.vctclass(EndpointF), 0, True)

        return (ret)

    def async_findObjectById(self, callback, id, context=ServantProxy.mapcls_context()):
        oos = tarscore.TarsOutputStream()
        oos.write(tarscore.string, 1, id)

        self.tars_invoke_async(
            ServantProxy.TARSNORMAL, "findObjectById", oos.getBuffer(), context, None, callback)

    def findObjectById4Any(self, id, context=ServantProxy.mapcls_context()):
        oos = tarscore.TarsOutputStream()
        oos.write(tarscore.string, 1, id)

        rsp = self.tars_invoke(
            ServantProxy.TARSNORMAL, "findObjectById4Any", oos.getBuffer(), context, None)

        ios = tarscore.TarsInputStream(rsp.sBuffer)
        ret = ios.read(tarscore.int32, 0, True)
        activeEp = ios.read(tarscore.vctclass(EndpointF), 2, True)
        inactiveEp = ios.read(tarscore.vctclass(EndpointF), 3, True)

        return (ret, activeEp, inactiveEp)

    def async_findObjectById4Any(self, callback, id, context=ServantProxy.mapcls_context()):
        oos = tarscore.TarsOutputStream()
        oos.write(tarscore.string, 1, id)

        self.tars_invoke_async(
            ServantProxy.TARSNORMAL, "findObjectById4Any", oos.getBuffer(), context, None, callback)

    def findObjectById4All(self, id, context=ServantProxy.mapcls_context()):
        oos = tarscore.TarsOutputStream()
        oos.write(tarscore.string, 1, id)

        rsp = self.tars_invoke(
            ServantProxy.TARSNORMAL, "findObjectById4All", oos.getBuffer(), context, None)

        ios = tarscore.TarsInputStream(rsp.sBuffer)
        ret = ios.read(tarscore.int32, 0, True)
        activeEp = ios.read(tarscore.vctclass(EndpointF), 2, True)
        inactiveEp = ios.read(tarscore.vctclass(EndpointF), 3, True)

        return (ret, activeEp, inactiveEp)

    def async_findObjectById4All(self, callback, id, context=ServantProxy.mapcls_context()):
        oos = tarscore.TarsOutputStream()
        oos.write(tarscore.string, 1, id)

        self.tars_invoke_async(
            ServantProxy.TARSNORMAL, "findObjectById4All", oos.getBuffer(), context, None, callback)

    def findObjectByIdInSameGroup(self, id, context=ServantProxy.mapcls_context()):
        oos = tarscore.TarsOutputStream()
        oos.write(tarscore.string, 1, id)
        rsp = self.tars_invoke(
            ServantProxy.TARSNORMAL, "findObjectByIdInSameGroup", oos.getBuffer(), context, None)

        startDecodeTime = time.time()
        ios = tarscore.TarsInputStream(rsp.sBuffer)
        ret = ios.read(tarscore.int32, 0, True)
        activeEp = ios.read(tarscore.vctclass(EndpointF), 2, True)
        inactiveEp = ios.read(tarscore.vctclass(EndpointF), 3, True)
        endDecodeTime = time.time()
        return (ret, activeEp, inactiveEp, (endDecodeTime - startDecodeTime))

    def async_findObjectByIdInSameGroup(self, callback, id, context=ServantProxy.mapcls_context()):
        oos = tarscore.TarsOutputStream()
        oos.write(tarscore.string, 1, id)

        self.tars_invoke_async(
            ServantProxy.TARSNORMAL, "findObjectByIdInSameGroup", oos.getBuffer(), context, None, callback)

    def findObjectByIdInSameStation(self, id, sStation, context=ServantProxy.mapcls_context()):
        oos = tarscore.TarsOutputStream()
        oos.write(tarscore.string, 1, id)
        oos.write(tarscore.string, 2, sStation)

        rsp = self.tars_invoke(
            ServantProxy.TARSNORMAL, "findObjectByIdInSameStation", oos.getBuffer(), context, None)

        ios = tarscore.TarsInputStream(rsp.sBuffer)
        ret = ios.read(tarscore.int32, 0, True)
        activeEp = ios.read(tarscore.vctclass(EndpointF), 3, True)
        inactiveEp = ios.read(tarscore.vctclass(EndpointF), 4, True)

        return (ret, activeEp, inactiveEp)

    def async_findObjectByIdInSameStation(self, callback, id, sStation, context=ServantProxy.mapcls_context()):
        oos = tarscore.TarsOutputStream()
        oos.write(tarscore.string, 1, id)
        oos.write(tarscore.string, 2, sStation)

        self.tars_invoke_async(
            ServantProxy.TARSNORMAL, "findObjectByIdInSameStation", oos.getBuffer(), context, None, callback)

    def findObjectByIdInSameSet(self, id, setId, context=ServantProxy.mapcls_context()):
        oos = tarscore.TarsOutputStream()
        oos.write(tarscore.string, 1, id)
        oos.write(tarscore.string, 2, setId)

        rsp = self.tars_invoke(
            ServantProxy.TARSNORMAL, "findObjectByIdInSameSet", oos.getBuffer(), context, None)

        ios = tarscore.TarsInputStream(rsp.sBuffer)
        ret = ios.read(tarscore.int32, 0, True)
        activeEp = ios.read(tarscore.vctclass(EndpointF), 3, True)
        inactiveEp = ios.read(tarscore.vctclass(EndpointF), 4, True)

        return (ret, activeEp, inactiveEp)

    def async_findObjectByIdInSameSet(self, callback, id, setId, context=ServantProxy.mapcls_context()):
        oos = tarscore.TarsOutputStream()
        oos.write(tarscore.string, 1, id)
        oos.write(tarscore.string, 2, setId)

        self.tars_invoke_async(
            ServantProxy.TARSNORMAL, "findObjectByIdInSameSet", oos.getBuffer(), context, None, callback)


# ========================================================
# callback of async proxy for client
# ========================================================
class QueryFPrxCallback(ServantProxyCallback):
    def __init__(self):
        ServantProxyCallback.__init__(self)
        self.callback_map = {
            'findObjectById': self.__invoke_findObjectById,
            'findObjectById4Any': self.__invoke_findObjectById4Any,
            'findObjectById4All': self.__invoke_findObjectById4All,
            'findObjectByIdInSameGroup': self.__invoke_findObjectByIdInSameGroup,
            'findObjectByIdInSameStation': self.__invoke_findObjectByIdInSameStation,
            'findObjectByIdInSameSet': self.__invoke_findObjectByIdInSameSet
        }

    def callback_findObjectById(self, ret):
        raise NotImplementedError()

    def callback_findObjectById_exception(self, ret):
        raise NotImplementedError()

    def callback_findObjectById4Any(self, ret, activeEp, inactiveEp):
        raise NotImplementedError()

    def callback_findObjectById4Any_exception(self, ret):
        raise NotImplementedError()

    def callback_findObjectById4All(self, ret, activeEp, inactiveEp):
        raise NotImplementedError()

    def callback_findObjectById4All_exception(self, ret):
        raise NotImplementedError()

    def callback_findObjectByIdInSameGroup(self, ret, activeEp, inactiveEp):
        raise NotImplementedError()

    def callback_findObjectByIdInSameGroup_exception(self, ret):
        raise NotImplementedError()

    def callback_findObjectByIdInSameStation(self, ret, activeEp, inactiveEp):
        raise NotImplementedError()

    def callback_findObjectByIdInSameStation_exception(self, ret):
        raise NotImplementedError()

    def callback_findObjectByIdInSameSet(self, ret, activeEp, inactiveEp):
        raise NotImplementedError()

    def callback_findObjectByIdInSameSet_exception(self, ret):
        raise NotImplementedError()

    def __invoke_findObjectById(self, reqmsg):
        rsp = reqmsg.response
        if rsp.iRet != ServantProxy.TARSSERVERSUCCESS:
            self.callback_findObjectById_exception(rsp.iRet)
            return rsp.iRet
        ios = tarscore.TarsInputStream(rsp.sBuffer)
        ret = ios.read(tarscore.vctclass(EndpointF), 0, True)
        self.callback_findObjectById(ret)

    def __invoke_findObjectById4Any(self, reqmsg):
        rsp = reqmsg.response
        if rsp.iRet != ServantProxy.TARSSERVERSUCCESS:
            self.callback_findObjectById4Any_exception(rsp.iRet)
            return rsp.iRet
        ios = tarscore.TarsInputStream(rsp.sBuffer)
        ret = ios.read(tarscore.int32, 0, True)
        activeEp = ios.read(tarscore.vctclass(EndpointF), 2, True)
        inactiveEp = ios.read(tarscore.vctclass(EndpointF), 3, True)
        self.callback_findObjectById4Any(ret, activeEp, inactiveEp)

    def __invoke_findObjectById4All(self, reqmsg):
        rsp = reqmsg.response
        if rsp.iRet != ServantProxy.TARSSERVERSUCCESS:
            self.callback_findObjectById4All_exception(rsp.iRet)
            return rsp.iRet
        ios = tarscore.TarsInputStream(rsp.sBuffer)
        ret = ios.read(tarscore.int32, 0, True)
        activeEp = ios.read(tarscore.vctclass(EndpointF), 2, True)
        inactiveEp = ios.read(tarscore.vctclass(EndpointF), 3, True)
        self.callback_findObjectById4All(ret, activeEp, inactiveEp)

    def __invoke_findObjectByIdInSameGroup(self, reqmsg):
        rsp = reqmsg.response
        if rsp.iRet != ServantProxy.TARSSERVERSUCCESS:
            self.callback_findObjectByIdInSameGroup_exception(rsp.iRet)
            return rsp.iRet
        ios = tarscore.TarsInputStream(rsp.sBuffer)
        ret = ios.read(tarscore.int32, 0, True)
        activeEp = ios.read(tarscore.vctclass(EndpointF), 2, True)
        inactiveEp = ios.read(tarscore.vctclass(EndpointF), 3, True)
        self.callback_findObjectByIdInSameGroup(ret, activeEp, inactiveEp)

    def __invoke_findObjectByIdInSameStation(self, reqmsg):
        rsp = reqmsg.response
        if rsp.iRet != ServantProxy.TARSSERVERSUCCESS:
            self.callback_findObjectByIdInSameStation_exception(rsp.iRet)
            return rsp.iRet
        ios = tarscore.TarsInputStream(rsp.sBuffer)
        ret = ios.read(tarscore.int32, 0, True)
        activeEp = ios.read(tarscore.vctclass(EndpointF), 3, True)
        inactiveEp = ios.read(tarscore.vctclass(EndpointF), 4, True)
        self.callback_findObjectByIdInSameStation(ret, activeEp, inactiveEp)

    def __invoke_findObjectByIdInSameSet(self, reqmsg):
        rsp = reqmsg.response
        if rsp.iRet != ServantProxy.TARSSERVERSUCCESS:
            self.callback_findObjectByIdInSameSet_exception(rsp.iRet)
            return rsp.iRet
        ios = tarscore.TarsInputStream(rsp.sBuffer)
        ret = ios.read(tarscore.int32, 0, True)
        activeEp = ios.read(tarscore.vctclass(EndpointF), 3, True)
        inactiveEp = ios.read(tarscore.vctclass(EndpointF), 4, True)
        self.callback_findObjectByIdInSameSet(ret, activeEp, inactiveEp)

    def onDispatch(self, reqmsg):
        self.callback_map[reqmsg.request.sFuncName](reqmsg)
