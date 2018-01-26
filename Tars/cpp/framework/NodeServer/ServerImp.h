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

#ifndef __SERVER_IMP_H_
#define __SERVER_IMP_H_
#include "Node.h"
#include <unistd.h>
#include "ServerFactory.h"
#include "util/tc_common.h"



using namespace tars;
using namespace std;

class ServerImp : public ServerF
{
public:
    /**
     * 构造函数
     */
    ServerImp()
    {
    }


    /**
     * 析构函数
     */
    virtual ~ServerImp()
    {
    }

    /**
     * 初始化
     */
    virtual void initialize()
    {
    };

    /**
     * 退出
     */
    virtual void destroy()
    {
    };

    /**
     * 上报心跳
     */
    virtual int keepAlive( const tars::ServerInfo& serverInfo, tars::TarsCurrentPtr current ) ;
    
    /**
     * 上报tars版本
     */
    virtual int reportVersion( const string &app,const string &serverName,const string &version,tars::TarsCurrentPtr current) ;

private:
};

typedef TC_AutoPtr<ServerImp> ServerImpPtr;

#endif

