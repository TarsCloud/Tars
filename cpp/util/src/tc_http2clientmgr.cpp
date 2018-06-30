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

#if TARS_HTTP2

#include <map>
#include <string>

#include "util/tc_nghttp2.h"
#include "util/tc_http2clientmgr.h"

namespace tars
{

Http2ClientSessionManager::Http2ClientSessionManager()
{
}

Http2ClientSessionManager::~Http2ClientSessionManager()
{
}

TC_NgHttp2* Http2ClientSessionManager::getSession(int id, bool isServer)
{
    SESSION_MAP::iterator it(_sessions.find(id));
    if (it == _sessions.end())
    {
        TC_NgHttp2* http2 = new TC_NgHttp2(isServer);
        it = _sessions.insert(std::make_pair(id, http2)).first;
    }

    return it->second;
}

bool Http2ClientSessionManager::delSession(int id)
{
    SESSION_MAP::iterator it(_sessions.find(id));
    if (it == _sessions.end())
        return false;

    nghttp2_session_del(it->second->session());
    _sessions.erase(it);
    return true;
}

} // end namespace tars

#endif // end #if TARS_SSL

