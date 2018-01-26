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

#ifndef __TARS_NETWORK_UTIL_H_
#define __TARS_NETWORK_UTIL_H_

#include <unistd.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <sys/select.h>
#include <sys/types.h>
#include <sys/ioctl.h>
#include <net/if.h>
#include <net/if_arp.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <string>
#include <vector>

namespace tars
{

struct NetworkUtil
{
    static const int INVALID_SOCKET = -1;
    static const int SOCKET_ERROR = -1;

    static int createSocket(bool, bool isLocal = false);

    static void closeSocketNoThrow(int);

    static void setBlock(int, bool);

    static void setTcpNoDelay(int);

    static void setKeepAlive(int);

    static void doBind(int, struct sockaddr_in&);

    static bool doConnect(int, const struct sockaddr_in&);

    static void getAddress(const std::string&, int, struct sockaddr_in&);

    static std::string errorToString(int);
};

}
#endif
