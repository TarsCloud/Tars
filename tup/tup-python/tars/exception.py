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

class TarsException(Exception): pass

class TarsTarsDecodeRequireNotExist(TarsException):    pass
class TarsTarsDecodeMismatch(TarsException):           pass
class TarsTarsDecodeInvalidValue(TarsException):       pass
class TarsTarsUnsupportType(TarsException):            pass

class TarsNetConnectException(TarsException):         pass
class TarsNetConnectLostException(TarsException):     pass
class TarsNetSocketException(TarsException):          pass
class TarsProxyDecodeException(TarsException):        pass
class TarsProxyEncodeException(TarsException):        pass
class TarsServerEncodeException(TarsException):       pass
class TarsServerDecodeException(TarsException):       pass
class TarsServerNoFuncException(TarsException):       pass
class TarsServerNoServantException(TarsException):    pass
class TarsServerQueueTimeoutException(TarsException): pass
class TarsServerUnknownException(TarsException):      pass
class TarsSyncCallTimeoutException(TarsException):    pass
class TarsRegistryException(TarsException):           pass
class TarsServerResetGridException(TarsException):    pass
