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

#ifndef __TARS_OBJECT_PROXY_FACTORY_H_
#define __TARS_OBJECT_PROXY_FACTORY_H_

#include "servant/Communicator.h"
#include "servant/ObjectProxy.h"
#include <vector>

namespace tars
{
/////////////////////////////////////////////////////////////////////////////////////////
/**
 * 获取ObjectProxy对象
 * 每个objectname在每个客户端网络线程中有唯一一个objectproxy
 *
 */
class ObjectProxyFactory : public TC_HandleBase, public TC_ThreadRecMutex
{
public:
    /**
     * 构造函数
     * @param pCommunicatorEpoll
     */
    ObjectProxyFactory(CommunicatorEpoll * pCommunicatorEpoll);

    /**
     * 获取对象代理
     * @param sObjectProxyName
     * @param setName 指定set调用的setid
     *
     * @return ObjectPrx
     */
    ObjectProxy * getObjectProxy(const string& sObjectProxyName,const string& setName = "");

    /**
     * 析构函数
     */
    ~ObjectProxyFactory();

    /**
     * 所有对象代理加载locator信息
     */
    int loadObjectLocator();

    /**
     * 获取所有对象的个数，为了不加锁不用map
     */
    inline size_t getObjNum()
    {
        return _objNum;
    }

    /**
     * 根据序号 获取所有obj对象，为了不加锁不用map
     */
    inline ObjectProxy * getObjectProxy(size_t iNum)
    {
        assert(iNum < _objNum);
        return _vObjectProxys[iNum];
    }

private:
    /**
     * 客户端网络线程
     */
    CommunicatorEpoll *         _communicatorEpoll;

    /**
     * 保存已创建的objectproxy
     */
    map<string,ObjectProxy*>    _objectProxys;

    /**
     * 保存已经创建的obj 取的时候可以不用加锁
     */
    vector<ObjectProxy *>       _vObjectProxys;

    /*
     *保存已经创建obj的数量
     */
    size_t                      _objNum;
};

/////////////////////////////////////////////////////////////////////////////////////////

}
#endif
