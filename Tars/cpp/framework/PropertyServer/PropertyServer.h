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

#ifndef __PROPERTY_SERVER_H_
#define __PROPERTY_SERVER_H_

#include "servant/Application.h"
#include "PropertyReapThread.h"

using namespace tars;

class PropertyServer : public Application
{
    
public:

    PropertyServer()
    : _reapThread(NULL)
    , _hashmap(NULL)
    {}

    void   getTimeInfo(time_t &tTime,string &sDate,string &sFlag);

    bool   cmdSetRandOrder(const string& command, const string& params, string& result);

    string getRandOrder(void);

    string getClonePath(void);

    int    getInserInterv(void);

    map<int,vector<pair<int64_t, int> > >& getBuffer(){return _buffer;}

    bool getSelectBuffer(int iIndex, int64_t iInterval);

    int  getSelectBufferFromFlag(const string& sFlag);

    int  getSelectBufferIndex() { return _selectBuffer; }

    void setSelectBufferIndex(int iIndex) { _selectBuffer = iIndex; }

    PropertyHashMap * getHashMapBuff(int iIndex, int iBuffer) { return &(_hashmap[iIndex][iBuffer]); }

    int getBuffNum() { return _buffNum; }    
        
protected:
    /**
     * 初始化, 只会进程调用一次
     */
    virtual void initialize();

    /**
     * 析够, 每个进程都会调用一次
     */
    virtual void destroyApp();

private:

    void initHashMap();
private:
    
    PropertyReapThread* _reapThread;
    
    // 随机入库开关
    string  _randOrder;

    //数据换存目录
    string  _clonePath;

    //数据库插入间隔,单位分钟
    int     _insertInterval;

    //双buffer机制
    map<int,vector<pair<int64_t, int> > > _buffer;

    int     _selectBuffer;

    PropertyHashMap **_hashmap;

    int     _buffNum;

};

extern PropertyServer g_app;
extern TC_Config* g_pconf;

#endif

