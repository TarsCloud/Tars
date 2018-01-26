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

#ifndef __TARS_SERVANT_PROXY_FACTORY_H_
#define __TARS_SERVANT_PROXY_FACTORY_H_

#include "servant/Global.h"
#include "servant/Communicator.h"
#include "servant/ServantProxy.h"

namespace tars
{
/////////////////////////////////////////////////////////////////////////////////////////
/**
 * 创建ServantProxy对象，每个object在进程空间只有一个ServantProxy实例
 */
class ServantProxyFactory : public TC_HandleBase, public TC_ThreadRecMutex
{
public:
    /**
     * 构造函数
     * @param comm
     */
    ServantProxyFactory(Communicator* comm);

    /**
     * 析构
     */
    ~ServantProxyFactory();

    /**
     * 获取ServantProxy对象
     * @param name
     * @param setName 指定set调用的setid
     * @return ServantPrx
     */
    ServantPrx::element_type* getServantProxy(const string& name,const string& setName="");

private:
    /**
     * 通信器
     */
    Communicator* _comm;

    /**
     * 已创建的对象
     */
    map<string, ServantPrx> _servantProxy;
};
//////////////////////////////////////////////////////
}
#endif
