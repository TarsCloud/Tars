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

#include "servant/AppCache.h"
#include "servant/Communicator.h"

namespace tars
{

//////////////////////////////////////////////////////////////////////
// 缓存
void AppCache::setCacheInfo(const string &sFile,int32_t iSynInterval)
{
    try
    {
        TC_LockT<TC_ThreadMutex> lock(*this);

        string sPath = TC_File::extractFilePath(sFile);

        TC_File::makeDirRecursive(sPath);

        _file = sFile;

        _synInterval = iSynInterval;

        if (TC_File::isFileExistEx(_file))
        {
            _fileCache.parseFile(_file);
        }

        //如果是旧版本数据（无版本号)直接清理
        if (_fileCache.get(string(APPCACHE_ROOT_PATH)+"<tarsversion>","") == "")
        {
            TC_Config tFileCache;

            _fileCache = tFileCache;
        }
    }
    catch(exception &e)
    {
        TLOGERROR("[TARS][AppCache setCacheInfo ex:" << e.what() << "]" << endl);
    }
}

string AppCache::get(const string & sName, const string sDomain)
{
    if(_file.empty())
    {
        return "";
    }

    try
    {
        TC_LockT<TC_ThreadMutex> lock(*this);
        string sValue = _fileCache.get(string(APPCACHE_ROOT_PATH) + "/" + sDomain + "<" + sName + ">");
        return sValue;
    }
    catch(exception &e)
    {
        TLOGERROR("[TARS][AppCache get sName:" << sName << ",ex:" << e.what() << "]" << endl);
    }
    return "";
}

map<string, string> AppCache::getDomainMap(const string &path)
{
    map<string, string> m;

    if(_file.empty())
    {
        return m;
    }

    try
    {
        TC_LockT<TC_ThreadMutex> lock(*this);
        m = _fileCache.getDomainMap(string(APPCACHE_ROOT_PATH) + "/" + path);
    }
    catch(exception &e)
    {
        TLOGERROR("[TARS][AppCache getDomainMap path:" << path << ",ex:" << e.what() << "]" << endl);
    }
    return m;
}

int AppCache::set(const string &sName,const string &sValue,const string sDomain)
{
    if(_file.empty())
    {
        return -1;
    }

    try
    {
        TC_LockT<TC_ThreadMutex> lock(*this);

        map<string, string> m;
        m[sName] = sValue;

        TC_Config tConf;
        tConf.insertDomainParam(string(APPCACHE_ROOT_PATH)+"/"+sDomain,m,true);
        if(_lastSynTime == 0)   //第一次写数据 打印tarsversion
        {
            m.clear();
            m["tarsversion"] = ClientConfig::TarsVersion;
            tConf.insertDomainParam(string(APPCACHE_ROOT_PATH),m,true);
        }

        {
            m.clear();
            m["modify"] = TC_Common::now2str("%Y-%m-%d %H:%M:%S");
            tConf.insertDomainParam(string(APPCACHE_ROOT_PATH),m,true);
        }

        _fileCache.joinConfig(tConf,true);

        time_t now = TNOW;
        if(_lastSynTime + _synInterval/1000 > now)
        {
            return 0;
        }
        _lastSynTime = now;

        TC_File::save2file(_file,_fileCache.tostr());

        return 0;
    }
    catch(exception &e)
    {
        TLOGERROR("[TARS][AppCache set name:" << sName << ",value:" << sValue << ",ex:" << e.what() << "]" << endl);
    }

    return -1;
}

//////////////////////////////////////////////////////////////////////
}

