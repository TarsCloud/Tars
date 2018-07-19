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

#ifndef __TC_HTTP2CLIENTMANAGER_H
#define __TC_HTTP2CLIENTMANAGER_H

#if TARS_HTTP2

#include <map>

#include "util/tc_singleton.h"

namespace tars
{

/////////////////////////////////////////////////
/** 
 *@file   tc_http2clientmgr.h
 *@brief  http2客户端session集合
 */
/////////////////////////////////////////////////

class TC_NgHttp2;

class Http2ClientSessionManager : public TC_Singleton<Http2ClientSessionManager>
{
public:
    Http2ClientSessionManager();
   ~Http2ClientSessionManager();

    TC_NgHttp2* getSession(int id, bool isServer = false);

    bool delSession(int id);

private:
    typedef std::map<int, TC_NgHttp2*> SESSION_MAP;
    SESSION_MAP _sessions;
};

} // end namespace tars

#endif // end #if TARS_SSL

#endif

