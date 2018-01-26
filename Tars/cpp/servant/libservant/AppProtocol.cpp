/**
 * Tencent is pleased to support the open source community by making Tars available.
 *
 * Copyright (C) 2016THL A29 Limited, a Tencent company. All rights reserved.
 *
 * Licensed under the BSD 3-Clause License (the "License"); you may not use this file except 
 * in compliance with the License. You may obtain a copy of the License at
 *
 * https://opensource.org/licenses/BSD-3-Clause
 *
 * Unless required by applicable law or agreed to in writing, software distributed 
 * under the License is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR 
 * CONDITIONS OF ANY KIND, either express or implied. See the License for the 
 * specific language governing permissions and limitations under the License.
 */

#include "util/tc_epoll_server.h"
#include "servant/AppProtocol.h"
#include "tup/Tars.h"
#include <iostream>

namespace tars
{

//TARSServer的协议解析器
int AppProtocol::parseAdmin(string &in, string &out)
{
    return parse(in, out);
}

void ProxyProtocol::tarsRequest(const RequestPacket& request, string& buff)
{
    TarsOutputStream<BufferWriter> os;

    request.writeTo(os);

    tars::Int32 iHeaderLen = htonl(sizeof(tars::Int32) + os.getLength());

    buff.clear();

    buff.reserve(sizeof(tars::Int32) + os.getLength());

    buff.append((const char*)&iHeaderLen, sizeof(tars::Int32));

    buff.append(os.getBuffer(), os.getLength());
}
////////////////////////////////////////////////////////////////////////////////////
}

