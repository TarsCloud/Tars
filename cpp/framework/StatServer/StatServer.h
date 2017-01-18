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

#ifndef __STAT_SERVER_H_
#define __STAT_SERVER_H_

#include "servant/Application.h"
#include "servant/StatF.h"
#include "StatHashMap.h"
#include "ReapSSDThread.h"

using namespace tars;

class StatServer : public Application
{
protected:
    /**
     * 初始化, 只会进程调用一次
     */
    virtual void initialize();

    /**
     * 析够, 每个进程都会调用一次
     */
    virtual void destroyApp();

public:

    void getTimeInfo(time_t &tTime,string &sDate,string &sFlag);

    bool cmdSetRandOrder(const string& command, const string& params, string& result);

    //获取主调虚拟ip映射
    map<string, string>& getVirtualMasterIp(void);

    string getRandOrder(void);

    string getClonePath(void);

    int getInserInterv(void);

    map<int,vector<int64_t> >& getBuffer(){ return _vBuffer; }

    bool getSelectBuffer(int iIndex, int64_t iInterval);

    int getSelectBufferFromFlag(const string& sFlag);

    int getSelectBufferIndex() { return _iSelectBuffer; }

    void setSelectBufferIndex(int iIndex) { _iSelectBuffer = iIndex; }

    StatHashMap * getHashMapBuff(int iIndex, int iBuffer) { return &(_hashmap[iIndex][iBuffer]); }

    int getBuffNum() { return _iBuffNum; }

private:
    void initHashMap();

private:

    ReapSSDThread* _pReapSSDThread;

    //主调虚拟ip配置
    map<string, string> _mVirtualMasterIp;

    // 随机入库开关
    string  _sRandOrder;

    //数据换存目录
    string _sClonePath;

    //数据库插入间隔,单位分钟
    int _iInsertInterval;

    //双buffer机制
    map<int,vector<int64_t> > _vBuffer;

    int _iSelectBuffer;

    StatHashMap **_hashmap;

    int _iBuffNum;
};

extern TC_Config* g_pconf;
extern StatServer g_app;

#endif
