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

#ifndef __LOAD_DB_THREAD_H_
#define __LOAD_DB_THREAD_H_

#include "util/tc_thread.h"
#include "util/tc_monitor.h"
#include "util/tc_mysql.h"
#include "servant/TarsLogger.h"
//#include "NotifyServer.h"

using namespace std;
using namespace tars;

/**
 * 双buff缓存
 */
template <typename T>
class DataWrapper
{
public:
    DataWrapper()
    :_swap(true)
    {
    }

    ~DataWrapper()
    {
    }

    T &getWriter()
    {
        if (_swap)
        {
            return _a;
        }
        else
        {
            return _b;
        }
    }

    T &getReader()
    {
        if (_swap)
        {
            return _b;
        }
        else
        {
            return _a;
        }
    }

    void swap()
    {
        _swap = !_swap;
    }

private:
    bool _swap;
    T    _a;
    T    _b;
};

/**
 * 从db加载set相关信息的线程类
 */
class LoadDbThread : public TC_Thread, public tars::TC_ThreadLock
{
public:
    typedef DataWrapper< map<string, string> > Data;

    /**
     * 构造函数
     */
    LoadDbThread();

    /**
     * 析构函数
     */
    ~LoadDbThread();

    /**
     * 初始化
     */
    void init();
    
    /**
     * 运行
     */
    void run();

    /**
     * 停止
     */
    void terminate();

    /**
     * 获取set信息
     */
    string getSetName(const string &sAppName)
    {
        map<string, string> &mData          = _data.getReader();
        map<string, string>::iterator it    = mData.find(sAppName);

        if (it != mData.end())
        {
            return it->second;
        }

        return "";

    }

private:

    /**
     * 加载db数据
     */
    void loadData();

private:
    //加载db数据的时间间隔
    size_t   _interval;

    //停止
    bool     _terminate;

    //访问数据库
    TC_Mysql _mysql;
    
    //双buff缓存
    Data     _data;
};


#endif
