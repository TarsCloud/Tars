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

#ifndef __TARS_BASE_NOTIFY_H_
#define __TARS_BASE_NOTIFY_H_

#include "servant/Global.h"
#include "util/tc_thread_queue.h"
#include "util/tc_thread_mutex.h"
#include <functional>

namespace tars
{
//////////////////////////////////////////////////////////////////////
/**
 * 需要接收到管理命令的对象从该类派生
 */
class BaseNotify : public TC_ThreadRecMutex, public TC_HandleBase
{
public:
    /**
     * 构造函数
     */
    BaseNotify();

    /**
     * 析构函数
     */
    virtual ~BaseNotify();

    /**
     * 接收管理命令
     * @param command
     * @param params
     * @param current
     * @param result
     * @return bool
     */
    bool notify(const string& command, const string& params, TarsCurrentPtr current, string& result);

    /**
     * 处理命令的函数类型
     * bool: true(继续往后通知其他object),false(通知中止)
     * result: 处理结果描述
     */
    using TAdminFunc = std::function<bool (const string&, const string&, string& )>;

    /**
     * 添加Servant管理命令和对应的处理方法
     * @param command
     * @param func
     */
    void addAdminCommandNormal(const string& command, TAdminFunc func);

    /**
     * 添加管理命令和对应的处理方法
     * 提前于Servant对象执行收到的命令
     * @param command
     * @param func
     */
    void addAdminCommandPrefix(const string& command, TAdminFunc func);

protected:
    /**
     * 命令处理方法
     */
    map<string, TAdminFunc> _procFunctors;
};
/////////////////////////////////////////////////////////////////////
}
#endif
