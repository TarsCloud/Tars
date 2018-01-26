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

#ifndef __TARS_NOTIFY_OBSERVER_H_
#define __TARS_NOTIFY_OBSERVER_H_

#include "servant/Global.h"
#include "servant/TarsCurrent.h"
#include "util/tc_singleton.h"

namespace tars
{

class BaseNotify;

//////////////////////////////////////////////////////////////
/**
 * 全局接收管理命令，并通知到已注册的对象
 */
class NotifyObserver : public TC_Singleton<NotifyObserver>, public TC_ThreadRecMutex
{
public:
    /**
     * 构造函数
     */
    NotifyObserver();

    /**
     * 析构函数
     */
    ~NotifyObserver();

public:

    /**
     * 接收管理命令
     * @param command
     * @param current
     * @return string
     */
    string notify(const string& command, TarsCurrentPtr current);

public:
    /**
     * 注册前置命令处理器，在Normal之前处理
     * @param command
     * @param obj
     */
    void registerPrefix(const string& command, BaseNotify* obj);

    /**
     * 注销前置处理器
     * @param command
     * @param obj
     */
    void unregisterPrefix(const string& command, BaseNotify* obj);

    /**
     * 注册普通的命令处理器，多个直接顺序不保证
     * @param command
     * @param obj
     */
    void registerNotify(const string& command, BaseNotify* obj);

    /**
     * 注销普通的消息处理器
     * @param command
     * @param obj
     */
    void unregisterNotify(const string& command, BaseNotify* obj);

    /**
     * 展示已注册的命令处理器，多个顺序不保证
     */
    string viewRegisterCommand();

protected:
    /**
     * 注册对象
     * @param command
     * @param obj
     * @param target
     */
    void registerObject(const string& command, BaseNotify* obj, map<string, set<BaseNotify*> >& target);

    /**
     * 取消对象注册
     * @param command
     * @param obj
     * @param target
     */
    void unregisterObject(const string& command, BaseNotify* obj, map<string, set<BaseNotify*> >& target);

protected:

    /**
     * 前置处理器
     */
    map<string, set<BaseNotify*> > _prefix;

    /**
     * 普通处理器
     */
    map<string, set<BaseNotify*> > _notifys;
};

/////////////////////////////////////////////////////////////////
}

#endif
