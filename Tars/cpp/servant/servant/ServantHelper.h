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

#ifndef __TARS_SERVANT_HELPER_H
#define __TARS_SERVANT_HELPER_H

#include <iostream>
#include <map>
#include <vector>
#include "util/tc_autoptr.h"
#include "util/tc_singleton.h"
#include "servant/Servant.h"

namespace tars
{
//////////////////////////////////////////////////////////////////////////////
/**
 * Servant
 */
class ServantHelperCreation : public TC_HandleBase
{
public:
    virtual ServantPtr create(const string &s) = 0;
};
typedef TC_AutoPtr<ServantHelperCreation> ServantHelperCreationPtr;

//////////////////////////////////////////////////////////////////////////////
/**
 * Servant
 */
template<class T>
struct ServantCreation : public ServantHelperCreation
{
    ServantPtr create(const string &s) { T *p = new T; p->setName(s); return p; }
};

//////////////////////////////////////////////////////////////////////////////
//
/**
 * Servant管理
 */
class ServantHelperManager : public TC_Singleton<ServantHelperManager>
{
public:
    /**
     * 构造函数
     */
    ServantHelperManager()
    : _isDyeing(false)
    {
    }

    /**
     * 添加Servant
     * @param T
     * @param id
     */
    template<typename T>
    void addServant(const string &id,bool check = false)
    {
        if(check && _servant_adapter.end() == _servant_adapter.find(id))
        {
            cerr<<"[TARS]ServantHelperManager::addServant "<< id <<" not find adapter.(maybe not conf in the web)"<<endl;
            throw runtime_error("[TARS]ServantHelperManager::addServant " + id + " not find adapter.(maybe not conf in the web)");
        }
        _servant_creator[id] = new ServantCreation<T>();
    }

    /**
     * 生成Servant
     * @param id
     *
     * @return ServantPtr
     */
    ServantPtr create(const string &sAdapter);

    /**
     * 添加Adapter的Servant
     * @param sAdapter
     * @param sServant
     */
    void setAdapterServant(const string &sAdapter, const string &sServant);

    /**
     * 根据adapter名称获取Servant名称
     * @param sAdapter
     * @return string
     */
    string getAdapterServant(const string &sAdapter)
    {
        map<string, string>::iterator it = _adapter_servant.find(sAdapter);
        if(it != _adapter_servant.end())
        {
            return it->second;
        }
        return "(NO TARS PROTOCOL)";
    }

    /**
     * 根据servant名称获取adapter名称
     * @param sServant
     * @return string
     */
    string getServantAdapter(const string& sServant)
    {
        map<string, string>::iterator it = _servant_adapter.find(sServant);
        if(it != _servant_adapter.end())
        {
            return it->second;
        }
        return "";
    }

    /**
     * 获取Adapter/Servant对应表
     * @return map<string, string>
     */
    map<string, string> getAdapterServant() {return _adapter_servant;}

    /**
     * 设置染色信息
     * @param params: notify的输入参数
     * @param sDyeingKey:      用户号码
     * @param sDyeingServant:  对象名称
     * @param sDyeingInterface:接口名称
     * @return string: 设置结果
     */
    bool setDyeing(const string & sDyeingKey, const string & sDyeingServant, const string & sDyeingInterface);

    /**
     * 是否是染色的请求
     * @param sKey:      用户号码
     * @param sServant:  对象名称
     * @param sInterface:接口名称
     * @return string: 设置结果
     */
    bool isDyeingReq(const string & sKey, const string & sServant, const string & sInterface);

    /**
     * 是否是已经被染色
     */
    bool isDyeing() {return _isDyeing;}


protected:

    /**
     * Servant生成类
     */
    map<string, ServantHelperCreationPtr>   _servant_creator;

    /**
     * Adapter包含的Servant(Adapter名称:servant名称)
     */
    map<string, string>                     _adapter_servant;

    /**
     * Adapter包含的Servant(Servant名称:Adapter名称)
     */
    map<string, string>                     _servant_adapter;

protected:

    /**
     * 锁
     */
    TC_ThreadMutex _mutex;

    /**
     * 是否染色
     */
    bool           _isDyeing;

    /**
     * 染色用户号码
     */
    string         _dyeingKey;

    /**
     * 染色的servant对象
     */
    string         _dyeingServant;

    /**
     * 染色的接口
     */
    string         _dyeingInterface;

};

}

#endif

