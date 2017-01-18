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

#ifndef __APP_CACHE_H_
#define __APP_CACHE_H_

#include <string>
#include <memory>
#include <map>
#include <vector>
#include "util/tc_singleton.h"
#include "util/tc_config.h"
#include "util/tc_timeprovider.h"
#include "util/tc_file.h"
#include "util/tc_thread_mutex.h"
#include "servant/TarsLogger.h"

#define APPCACHE_ROOT_PATH "/cache"

using namespace std;

namespace tars
{
//////////////////////////////////////////////////////////////////////
/**
 * 缓存
 */
class AppCache : public TC_Singleton<AppCache>, public TC_ThreadMutex
{    
public:
    AppCache()
    : _lastSynTime(0)
    , _synInterval(1000)
    {

    }

    ~AppCache()
    {

    }
     /**
     * 设置本地信息
     * @param sFilePath, cache文件路径
     * @param iSynInterval(ms), 同步间隔时间
     */
    void setCacheInfo(const string &sFile,int32_t iSynInterval = 1000);

    /**
     * 从cache文件获取服务列表
     * @param sObjName 
     * @param sDomain 
     * @return string
     */
    string get(const string & sName,const string sDomain = ""/*=APPCACHE_ROOT_PATH*/);

    /**
     * 获取域下面的参数值对 
     * 不存则返回空map 
     * @param path: 域名称, 域标识, 例如: /Main/Domain
     * 
     * @return map<string,string>
     */
    map<string, string> getDomainMap(const string &path);
    
     /**
     * 设置数据到cache文件
     * @param sName 
     * @param sValue
     * @param sDomain
     * @return int
     */
    int set(const string &sName,const string &sValue,const string sDomain = ""/*=APPCACHE_ROOT_PATH*/);

private:
    /*
     * 缓存文件
     */
    string      _file;

    /*
     * 缓存文件的内存cache
     */
    TC_Config   _fileCache;

    /*
     * 上次同步文件的时间
     */
    time_t      _lastSynTime;

    /*
     * 同步的时间间隔
     */
    int32_t     _synInterval;
};
//////////////////////////////////////////////////////////////////////
}
#endif
