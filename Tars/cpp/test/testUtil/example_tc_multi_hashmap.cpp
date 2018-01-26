/**
 * Tencent is pleased to support the open source community by making Tars available.
 *
 * Copyright (C) 2016 THL A29 Limited, a Tencent company. All rights reserved.
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

#include "util/tc_multi_hashmap.h"
#include "util/tc_shm.h"
#include "util/tc_option.h"
#include <stdlib.h>
#include <iostream>
#include "util/tc_common.h"
#include <sys/time.h>

using namespace tars;

////////////////////////////////////////////////////////////////////
//

TC_Shm                g_shm;
TC_Multi_HashMap    g_hmap;
bool                g_bSilent = false;        // 是否为安静模式，不输出日志，可以取得更高的速度

// 普通的功能测试
struct FunctionTest
{
    static void initMem()
    {
#if __WORDSIZE == 64
        size_t iMemSize    = 1024*20;
#else
        size_t iMemSize    = 1024*10;
#endif
        g_shm.init(iMemSize, 0x12345);
        cout << "init mem, shm key=0x" << hex << 0x12345 << dec << endl;
    }
    
    static void initMap()
    {
        if (g_shm.iscreate())
        {
            size_t iMinSize = 2, iMaxSize = 8;
            g_hmap.initDataBlockSize(iMinSize, iMaxSize, 2.0);
            g_hmap.create(g_shm.getPointer(), g_shm.size());
            g_hmap.setEraseCount(3);
        }
        else
        {
            g_hmap.connect(g_shm.getPointer(), g_shm.size());
        }
    }
};

// 大数据量性能测试
struct PerformanceTest
{
    static void initMem()
    {
        size_t iMemSize    = 1073741824;        // 1G
        g_shm.init(iMemSize, 0x23456);
        cout << "init mem, shm key=0x" << hex << 0x23456 << dec << endl;
    }
    
    static void initMap()
    {
        if (g_shm.iscreate())
        {
            size_t iMinSize = 5, iMaxSize = 20;
            g_hmap.initDataBlockSize(iMinSize, iMaxSize, 2.0);
            g_hmap.create(g_shm.getPointer(), g_shm.size());
            g_hmap.setEraseCount(10);
        }
        else
        {
            g_hmap.connect(g_shm.getPointer(), g_shm.size());
        }
    }
};

void set(const string &mk, const string &uk, const string &v, uint8_t version, bool dirty, bool full, bool head)
{
    vector<TC_Multi_HashMap::Value> vRecords;

    TC_Multi_HashMap::DATATYPE eType = (full ? TC_Multi_HashMap::FULL_DATA : TC_Multi_HashMap::PART_DATA);

    timeval tv_1, tv_2;
    gettimeofday(&tv_1, NULL);
    
    int ret = TC_Multi_HashMap::RT_OK;
    if(uk.empty())
    {
        // 设置主key的only key
        ret = g_hmap.set(mk, vRecords);
    }
    else
    {
        if(v.empty())
        {
            // 设置主键的only key
            ret = g_hmap.set(mk, uk, eType, head, vRecords);
        }
        else
        {
            ret = g_hmap.set(mk, uk, v, version, dirty, eType, head, vRecords);
        }
    }
    
    gettimeofday(&tv_2, NULL);
    
    long lTime = 0;
    if(tv_2.tv_usec < tv_1.tv_usec)
    {
        lTime = (tv_2.tv_sec -1 - tv_1.tv_sec) * 1000000;
    }
    else
    {
        lTime = (tv_2.tv_sec - tv_1.tv_sec) * 1000000 + (tv_2.tv_usec - tv_1.tv_usec);
    }
    
    //assert(ret == 0);
    cout << "set:" << mk << "|" << uk << "|" << v << "|" << ret << "|" << lTime << endl;
    for(size_t i = 0; i < vRecords.size(); i++)
    {
        cout << "set erase:" << vRecords[i]._mkey << "|" << vRecords[i]._data._key 
            << "|" << vRecords[i]._data._value << endl;
    }
}

void get(const string &mk, const string &uk)
{
    TC_Multi_HashMap::Value v;
    if(!uk.empty())
    {
        // 根据主键查询数据
        int ret = g_hmap.get(mk, uk, v);
        cout << "get:" << mk << "|" << uk << "|" << v._data._value << "|" 
            << (int)v._data._iVersion << "|" << v._data._dirty << "|" << ret << endl;
    }
    else
    {
        // 查主key下的所有数据
        size_t count = g_hmap.count(mk);
        cout << "count of " << mk << ": " << count << endl;
        TC_Multi_HashMap::lock_iterator it = g_hmap.find(mk);
        while(it != g_hmap.end())
        {
            TC_Multi_HashMap::Value v;
            int ret = it->get(v);
            cout << v._mkey << "|" << v._data._key << "|" << v._data._value 
                << "|" << (int)v._data._iVersion << "|" << v._data._dirty << "|" << ret << endl;
            it ++;
        }
    }
}

void del(const string &mk, const string &uk)
{
    vector<TC_Multi_HashMap::Value> vtDel;
    int ret = TC_Multi_HashMap::RT_OK;
    if(uk.empty())
    {
        ret = g_hmap.del(mk, vtDel);
    }
    else
    {
        TC_Multi_HashMap::Value v;
        ret = g_hmap.del(mk, uk, v);
        vtDel.push_back(v);
    }
    cout << "del:" << mk << "|" << uk << "|" << ret << endl;
    for(size_t i = 0; i < vtDel.size(); i ++)
    {
        cout << "del:" << vtDel[i]._mkey << "|" << vtDel[i]._data._key << "|" << vtDel[i]._data._value << endl;
    }
}

void sync()
{
    TC_Multi_HashMap::Value data;
    size_t n = 0;
    g_hmap.setSyncTime(30);
    g_hmap.sync();
    while(true)
    {
        int ret = g_hmap.sync(time(NULL), data);
        if (ret == TC_Multi_HashMap::RT_OK)
        {
            break;
        }
        else if(ret == TC_Multi_HashMap::RT_NEED_SYNC)
        {
            ++n;
            cout << "sync:" << data._mkey << "|" << data._data._key << "|" 
                << data._data._value << "|" << data._data._synct << endl;
        }
    }
    cout << n << " records synced" << endl;
}

void erase(int ratio)
{
    vector<TC_Multi_HashMap::Value> vtErased;
    int ret = g_hmap.erase(ratio, vtErased, true);
    cout << "erase:" << ratio << "|" << vtErased.size() << "|" << ret << endl;
    for(size_t i = 0; i < vtErased.size(); i++)
    {
        cout << "erase:" << vtErased[i]._mkey << "|" << vtErased[i]._data._key 
            << "|" << vtErased[i]._data._value << endl;
    }
}

void listh()
{
    const TC_Multi_HashMap::tagMapHead &head = g_hmap.getMapHead();
    cout << "list hash count: " << head._iElementCount << endl;
    TC_Multi_HashMap::hash_iterator it = g_hmap.hashBegin();
    while(it != g_hmap.hashEnd())
    {
        vector<TC_Multi_HashMap::Value> vv;
        it->get(vv);
        ++it;
        
        for(size_t i = 0; i < vv.size(); i ++)
        {
            cout << vv[i]._mkey << "|" << vv[i]._data._key << "|" << vv[i]._data._value 
                << "|" << (int)vv[i]._data._iVersion << "|" << vv[i]._data._dirty << "|" << vv[i]._data._synct << endl;
        }
    }
}

void list(int type, bool asc = true)
{
    TC_Multi_HashMap::lock_iterator it;
    switch(type)
    {
    case 0:
        // 按block链遍历
        if(asc)
        {
            // 升序
            it = g_hmap.begin();
        }
        else
        {
            // 降序
            it = g_hmap.rbegin();
        }
        break;
    case 1:
        // 按get时间链遍历
        if(asc)
        {
            // 升序
            it = g_hmap.beginGetTime();
        }
        else
        {
            // 降序
            it = g_hmap.rbeginGetTime();
        }
        break;
    case 2:
        // 按set时间链遍历
        if(asc)
        {
            // 升序
            it = g_hmap.beginSetTime();
        }
        else
        {
            // 降序
            it = g_hmap.rbeginSetTime();
        }
        break;
    case 3:
        // 按脏数据链遍历
        it = g_hmap.beginDirty();
        break;
    default:
        return;
    }

    while(it != g_hmap.end())
    {
        TC_Multi_HashMap::Value v;
        it->get(v);
        ++it;
        
        cout << v._mkey << "|" << v._data._key << "|" << v._data._value 
            << "|" << (int)v._data._iVersion << "|" << v._data._dirty << "|" << v._data._synct << endl;
    }
}

void batchset(size_t iCount, size_t iAvgRec)
{
    srand(time(NULL));
    string mk, uk, v;
    vector<TC_Multi_HashMap::Value> vRecords;

    timeval tv_1, tv_2;
    gettimeofday(&tv_1, NULL);

    for(size_t i = 1; i <= iCount / iAvgRec; i ++)
    {
        mk = TC_Common::tostr(i);
        size_t iRec = rand() % iAvgRec + 1;
        for(size_t j = 1; j <= iRec; j ++)
        {
            uk = TC_Common::tostr(j);
            v = mk + uk;

            timeval tv_1, tv_2;
            if(!g_bSilent)
            {
                gettimeofday(&tv_1, NULL);
            }

            vRecords.clear();
            int ret = g_hmap.set(mk, uk, v, 0, true, TC_Multi_HashMap::FULL_DATA, true, vRecords);

            if(!g_bSilent)
            {
                gettimeofday(&tv_2, NULL);
                long lTime = 0;
                if(tv_2.tv_usec < tv_1.tv_usec)
                {
                    lTime = (tv_2.tv_sec -1 - tv_1.tv_sec) * 1000000;
                }
                else
                {
                    lTime = (tv_2.tv_sec - tv_1.tv_sec) * 1000000 + (tv_2.tv_usec - tv_1.tv_usec);
                }
                
                //assert(ret == 0);
                cout << "set:" << mk << "|" << uk << "|" << v << "|" << ret << "|" << lTime << endl;
                for(size_t i = 0; i < vRecords.size(); i++)
                {
                    cout << "set erase:" << vRecords[i]._mkey << "|" << vRecords[i]._data._key 
                        << "|" << vRecords[i]._data._value << endl;
                }
            }
        }
    }

    gettimeofday(&tv_2, NULL);
    long lTime = 0;
    if(tv_2.tv_usec < tv_1.tv_usec)
    {
        lTime = (tv_2.tv_sec -1 - tv_1.tv_sec) * 1000000;
    }
    else
    {
        lTime = (tv_2.tv_sec - tv_1.tv_sec) * 1000000 + (tv_2.tv_usec - tv_1.tv_usec);
    }
    cout << "batchset:" << iCount << "|" << iAvgRec << "|" << lTime << endl;
}

void batchset()
{
    vector<TC_Multi_HashMap::Value> vtSet, vtErased;

    TC_Multi_HashMap::Value v;

    v._mkey = "1", v._data._key = "1", v._data._value = "11";
    vtSet.push_back(v);

    v._mkey = "1", v._data._key = "2", v._data._value = "12";
    vtSet.push_back(v);

    v._mkey = "2", v._data._key = "1", v._data._value = "21";
    vtSet.push_back(v);

    v._mkey = "3", v._data._key = "1", v._data._value = "31";
    vtSet.push_back(v);

    v._mkey = "4", v._data._key = "1", v._data._value = "41";
    vtSet.push_back(v);

    int ret = g_hmap.set(vtSet, TC_Multi_HashMap::FULL_DATA, true, true, vtErased);

    for(size_t i = 0; i < vtSet.size(); i ++)
    {
        cout << "set:" << vtSet[i]._mkey << "|" << vtSet[i]._data._key << "|" << vtSet[i]._data._value << "|" << ret << endl;
    }
    for(size_t i = 0; i < vtErased.size(); i++)
    {
        cout << "set erase:" << vtErased[i]._mkey << "|" << vtErased[i]._data._key 
            << "|" << vtErased[i]._data._value << endl;
    }
}

void batchget(size_t iCount, size_t iAvgRec)
{
    srand(time(NULL));
    string mk, uk;
    TC_Multi_HashMap::Value v;

    timeval tv_1, tv_2;
    gettimeofday(&tv_1, NULL);
    
    for(size_t i = 1; i <= iCount / iAvgRec; i ++)
    {
        mk = TC_Common::tostr(i);
        size_t iRec = rand() % iAvgRec + 1;
        for(size_t j = 1; j <= iRec; j ++)
        {
            uk = TC_Common::tostr(j);
            
            timeval tv_1, tv_2;
            if(!g_bSilent)
            {
                gettimeofday(&tv_1, NULL);
            }
            
            int ret = g_hmap.get(mk, uk, v);
            
            if(!g_bSilent)
            {
                gettimeofday(&tv_2, NULL);
                long lTime = 0;
                if(tv_2.tv_usec < tv_1.tv_usec)
                {
                    lTime = (tv_2.tv_sec -1 - tv_1.tv_sec) * 1000000;
                }
                else
                {
                    lTime = (tv_2.tv_sec - tv_1.tv_sec) * 1000000 + (tv_2.tv_usec - tv_1.tv_usec);
                }
                
                //assert(ret == 0);
                cout << "get:" << mk << "|" << uk << "|" << v._data._value << "|" 
                    << (int)v._data._iVersion << "|" << v._data._dirty << "|" << ret << "|" << lTime << endl;
            }
        }
    }
    
    gettimeofday(&tv_2, NULL);
    long lTime = 0;
    if(tv_2.tv_usec < tv_1.tv_usec)
    {
        lTime = (tv_2.tv_sec -1 - tv_1.tv_sec) * 1000000;
    }
    else
    {
        lTime = (tv_2.tv_sec - tv_1.tv_sec) * 1000000 + (tv_2.tv_usec - tv_1.tv_usec);
    }
    cout << "batchset:" << iCount << "|" << iAvgRec << "|" << lTime << endl;
}


// 内存大小测试，测试一些结构在不同的机器上所占用字节数
void testMemSize()
{
    cout << "sizeof(int)=" << sizeof(int) << endl;
    cout << "sizeof(long)=" << sizeof(long) << endl;
    cout << "sizeof(size_t)=" << sizeof(size_t) << endl;
    cout << "sizeof(time_t)=" << sizeof(time_t) << endl;
    cout << "sizeof(tagBlockHead)=" << sizeof(TC_Multi_HashMap::Block::tagBlockHead) << endl;
    cout << "sizeof(tagMainKeyHead)=" << sizeof(TC_Multi_HashMap::MainKey::tagMainKeyHead) << endl;
    cout << "sizeof(tagChunkHead)=" << sizeof(TC_Multi_HashMap::Block::tagChunkHead) << endl;
    cout << "sizeof(tagHashItem)=" << sizeof(TC_Multi_HashMap::tagHashItem) << endl;
    cout << "sizeof(tagMainKeyHashItem)=" << sizeof(TC_Multi_HashMap::tagMainKeyHashItem) << endl;
}

void usage(char *argv)
{
    cout << argv << " --help\tshow this help message" << endl;
    cout << "\t --mem\tview memory size of some structs" << endl;
    cout << "\t --f\tdo the functionality test(default option)" << endl;
    cout << "\t --p\tdo the performance test" << endl;
    cout << "\t --s\tsilent mode in performance test, no output" << endl;
    cout << "\t --release\trelease the allocated share memory" << endl;
    cout << "\t --view\tview statistic information of this map" << endl;
    cout << "\t --set [--mkey=] [--ukey=] [--value=] [--version] [--clean] [--full] [--tail]" << endl;
    cout << "\t --get [--mkey=] [--ukey=]" << endl;
    cout << "\t --del [--mkey=] [--ukey=]" << endl;
    cout << "\t --sync" << endl;
    cout << "\t --erase [--ratio]" << endl;
    cout << "\t --isdirty [--mkey=] [--ukey=]" << endl;
    cout << "\t --listh\tlist data by hash index" << endl;
    cout << "\t --listb [--desc]\tlist data by block chain" << endl;
    cout << "\t --listg [--desc]\tlist data by get time chain" << endl;
    cout << "\t --lists [--desc]\tlist data by set time chain" << endl;
    cout << "\t --listd\tlist data by dirty data chain" << endl;
    cout << "\t --set-batch [--count] [--avgrec]\tdo batch set operation, will set total count records with avgrec records per main key" << endl;
    cout << "\t --get-batch [--count] [--avgrec]\tdo batch get operation" << endl;
    cout << "\t --chkmk [--mkey]\tcheck if main key exists" << endl;
    cout << "\t --setstate [--mkey] [--ukey] [--dirty] [--clean] [--part] [--full]\tcheck if main key exists" << endl;
    cout << "\t --chkbad [--hash] [--repair]" << endl;
}

int main(int argc, char *argv[])
{
    try
    {
        TC_Option option;
        option.decode(argc, argv);

        map<string, string> m = option.getMulti();

        if(option.hasParam("help") || option.getMulti().empty())
        {
            usage(argv[0]);
            return 0;
        }

        if(option.hasParam("mem"))
        {
            testMemSize();
            return 0;
        }

        g_bSilent = option.hasParam("s");

        // 随机设置和获取时也当作性能测试
        bool bPerfTest = option.hasParam("p");
        if(bPerfTest)
        {
            PerformanceTest::initMem();
        }
        else
        {
            FunctionTest::initMem();
        }

        if(option.hasParam("release"))
        {
            g_shm.del();
            return 0;
        }

        if(bPerfTest)
        {
            PerformanceTest::initMap();
        }
        else
        {
            FunctionTest::initMap();
        }

        if(option.hasParam("view"))
        {
            cout << g_hmap.desc() << endl;
        }
        else if(option.hasParam("set"))
        {
            string mk = option.getValue("mkey");
            string uk = option.getValue("ukey");
            string v = option.getValue("value");
            uint8_t version = option.getValue("version").empty() ? 0 : atoi(option.getValue("version").c_str());
            bool dirty = option.getValue("clean").empty() ? true : false;
            bool full = option.getValue("full").empty() ? false : true;
            bool head = option.getValue("tail").empty() ? true : false;

            if(mk.empty())
            {
                usage(argv[0]);
                return 0;
            }
            
            set(mk, uk, v, version, dirty, full, head);
        }
        else if(option.hasParam("get"))
        {
            string mk;
            string uk;
            mk = option.getValue("mkey");
            uk = option.getValue("ukey");

            if(mk.empty())
            {
                usage(argv[0]);
                return 0;
            }
            
            get(mk, uk);
        }
        else if(option.hasParam("del"))
        {
            string mk;
            string uk;
            mk = option.getValue("mkey");
            uk = option.getValue("ukey");
            
            if(mk.empty())
            {
                usage(argv[0]);
                return 0;
            }

            del(mk, uk);
        }
        else if(option.hasParam("sync"))
        {
            sync();
        }
        else if(option.hasParam("erase"))
        {
            string ratio = option.getValue("ratio");
            if(ratio.empty())
            {
                usage(argv[0]);
                return 0;
            }

            erase(TC_Common::strto<int>(ratio));
        }
        else if(option.hasParam("isdirty"))
        {
            string mk;
            string uk;
            mk = option.getValue("mkey");
            uk = option.getValue("ukey");
            if(mk.empty())
            {
                usage(argv[0]);
                return 0;
            }
            int ret = TC_Multi_HashMap::RT_OK;
            if(uk.empty())
            {
                ret = g_hmap.checkDirty(mk);
            }
            else
            {
                ret = g_hmap.checkDirty(mk, uk);
            }
            cout << "isdirty:" << mk << "|" << uk << "|" << (ret == TC_Multi_HashMap::RT_DIRTY_DATA) << endl;
        }
        else if(option.hasParam("listh"))
        {
            listh();
        }
        else if(option.hasParam("listb"))
        {
            if(option.hasParam("desc"))
            {
                list(0, false);
            }
            else
            {
                list(0, true);
            }
        }
        else if(option.hasParam("listg"))
        {
            if(option.hasParam("desc"))
            {
                list(1, false);
            }
            else
            {
                list(1, true);
            }
        }
        else if(option.hasParam("lists"))
        {
            if(option.hasParam("desc"))
            {
                list(2, false);
            }
            else
            {
                list(2, true);
            }
        }
        else if(option.hasParam("listd"))
        {
            list(3);
        }
        else if(option.hasParam("set-batch"))
        {
            string sCount = option.getValue("count");
            string sAvgRec = option.getValue("avgrec");
            if(!sCount.empty())
            {
                batchset(TC_Common::strto<size_t>(sCount), sAvgRec.empty() ? 2 : TC_Common::strto<size_t>(sAvgRec));
            }
            else
            {
                // 不带参数的批量设置
                batchset();
            }
        }
        else if(option.hasParam("get-batch"))
        {
            string sCount = option.getValue("count");
            string sAvgRec = option.getValue("avgrec");
            if(sCount.empty())
            {
                usage(argv[0]);
                return 0;
            }

            batchget(TC_Common::strto<size_t>(sCount), sAvgRec.empty() ? 2 : TC_Common::strto<size_t>(sAvgRec));
        }
        else if(option.hasParam("chkmk"))
        {
            string mk = option.getValue("mkey");
            if(mk.empty())
            {
                usage(argv[0]);
                return 0;
            }

            int ret = g_hmap.checkMainKey(mk);
            if(ret == TC_Multi_HashMap::RT_OK)
            {
                cout << "main key " << mk << " exists" << endl;
            }
            else if(ret == TC_Multi_HashMap::RT_ONLY_KEY)
            {
                cout << "main key " << mk << " exists, but only key" << endl;
            }
            else if(ret == TC_Multi_HashMap::RT_PART_DATA)
            {
                cout << "main key " << mk << " exists, and possible part data" << endl;
            }
            else if(ret == TC_Multi_HashMap::RT_NO_DATA)
            {
                cout << "main key " << mk << " not exists" << endl;
            }
            else
            {
                cout << "chkmk " << mk << " failed: " << ret << endl;
            }
        }
        else if(option.hasParam("setstate"))
        {
            string mk = option.getValue("mkey");
            string uk = option.getValue("ukey");
            bool bDirty = option.hasParam("dirty");
            bool bClean = option.hasParam("clean");
            bool bFull = option.hasParam("full");
            bool bPart = option.hasParam("part");
            if(mk.empty())
            {
                usage(argv[0]);
                return 0;
            }
            if(!uk.empty())
            {
                if(bDirty)
                {
                    int ret = g_hmap.setDirty(mk, uk);
                    cout << "setdirty:" << mk << "|" << uk << "|" << ret << endl;
                }
                else if(bClean)
                {
                    int ret = g_hmap.setClean(mk, uk);
                    cout << "setclean:" << mk << "|" << uk << "|" << ret << endl;
                }
                else
                {
                    usage(argv[0]);
                    return 0;
                }
            }
            else
            {
                if(bFull)
                {
                    int ret = g_hmap.setFullData(mk, true);
                    cout << "set full data:" << mk << "|" << ret << endl;
                }
                else if(bPart)
                {
                    int ret = g_hmap.setFullData(mk, false);
                    cout << "set part data:" << mk << "|" << ret << endl;
                }
                else
                {
                    usage(argv[0]);
                    return 0;
                }
            }
        }
        else if(option.hasParam("chkbad"))
        {
            string sHash = option.getValue("hash");
            if(sHash.empty())
            {
                usage(argv[0]);
                return 0;
            }
            uint32_t iHash = TC_Common::strto<uint32_t>(sHash);
            size_t iCount = g_hmap.checkBadBlock(iHash, option.hasParam("repair"));
            cout << iCount << " bad block under hash " << iHash << endl;
        }
        else
        {
            usage(argv[0]);
        }
    }
    catch(exception &e)
    {
        cout << "error! " << e.what() << endl;
    }

    return 0;
}

