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

#include "util/tc_hashmap.h"
#include "util/tc_common.h"
#include "util/tc_shm.h"
#include "util/tc_option.h"
#include <stdlib.h>
#include <iostream>

using namespace tars;

////////////////////////////////////////////////////////////////////
//

TC_Shm      g_shm;
TC_HashMap  g_hmap;

vector<pair<string, string> > prepare()
{
    cout << "start pareparing data" << endl;

    size_t count = 10000;
    vector<pair<string, string> > v;

    vector<string> vs;
    vs.push_back("a");
    vs.push_back("ab");
    vs.push_back("abc");
    vs.push_back("abcd");
    vs.push_back("abcde");
    vs.push_back("abcdef");
    vs.push_back("abcdefg");
    vs.push_back("abcdefgh");
    vs.push_back("abcdefghi");
    vs.push_back("abcdefghij");
    vs.push_back("abcdefghijk");
    vs.push_back("abcdefghijkl");
    vs.push_back("abcdefghijklm");

    srand(time(NULL));

    for(size_t i = 0; i < count; i++)
    {
        pair<string, string> data;

        ostringstream s;
        s << i << "_" << rand();
        data.first    = s.str() + "_" + vs[rand() % vs.size()];
        data.second   = s.str() + "_" + vs[rand() % vs.size()];

        v.push_back(data);
    }

    cout << "pareparing data OK:" << v.size() << endl;
    return v;
}

void initMem()
{
    size_t iMemSize    = 1024*10;
    g_shm.init(iMemSize, 8888);
}

void initMap()
{
    if (g_shm.iscreate())
    {
        size_t iAvgSize    = 10;
        g_hmap.initDataBlockSize(iAvgSize, iAvgSize, 1.0);
        g_hmap.create(g_shm.getPointer(), g_shm.size());
        g_hmap.setEraseCount(3);
    }
    else
    {
        g_hmap.connect(g_shm.getPointer(), g_shm.size());
    }

    cout << "init mem" << endl;
}

void testSendRand()
{
    vector<pair<string, string> > v = prepare();
    vector<TC_HashMap::BlockData> vRecords;

    for(int i = 0; i < 10000; i++)
    {
        size_t index = rand() % v.size();

        ostringstream s;
        s << i ;

        v[index].first  = s.str();
        v[index].second = s.str();

        cout << "set:" << v[index].first << ":" << v[index].second << ":" << g_hmap.size() << ":" << g_hmap.dirtyCount() << endl;

        int ret = g_hmap.set(v[index].first, v[index].second, true, vRecords);
        assert(ret == 0);

    }

    cout << g_hmap.size() << "|" << g_hmap.dirtyCount() << endl;
}

void testGetRand()
{
    vector<pair<string, string> > v = prepare();

    for(int i = 0; i < 200000; i++)
    {
        size_t index = rand() % v.size();
        time_t iSyncTime;
        int ret = g_hmap.get(v[index].first, v[index].second, iSyncTime);
        assert(ret == 0);
        cout << "get:" << v[index].first << ":" << v[index].second << ":" << g_hmap.size() << ":" << g_hmap.dirtyCount() << endl;
    }

    cout << g_hmap.size() << "|" << g_hmap.dirtyCount() << endl;
}

void usage(char *argv)
{
    cout << argv << " --help" << endl;
    cout << "\t --release" << endl;
    cout << "\t --view" << endl;
    cout << "\t --sync [--count=]" << endl;
    cout << "\t --set-rand" << endl;
    cout << "\t --set-tars [--key=] [--value=]" << endl;
    cout << "\t --del [--key=]" << endl;
    cout << "\t --get [--key=]" << endl;
    cout << "\t --get-rand" << endl;
    cout << "\t --erase [--key=]" << endl;
    cout << "\t --isdirty [--key=]" << endl;
    cout << "\t --find [--key=]" << endl;
    cout << "\t --listh [--asc]" << endl;
    cout << "\t --listt [--asc]" << endl;
    cout << "\t --listg [--asc]" << endl;
    cout << "\t --listd" << endl;
}

int main(int argc, char *argv[])
{
    {
        TC_Option option;
        option.decode(argc, argv);

        map<string, string> m = option.getMulti();

        if(option.hasParam("help") || option.getMulti().empty())
        {
            usage(argv[0]);
            return 0;
        }

        initMem();

        if(option.hasParam("release"))
        {
            g_shm.del();
            return 0;
        }

        initMap();

        /*
        fork();
        fork();
        fork();
        */

        if(option.hasParam("set-rand"))
        {
            testSendRand();
        }
        else if(option.hasParam("set-tars"))
        {
            vector<TC_HashMap::BlockData> vRecords;
            string k;
            string v;
            k = option.getValue("key");
            v = option.getValue("value");
            int ret = g_hmap.set(k, v, true, vRecords);
            assert(ret == 0);
            cout << "set-tars:" << k << ":" << v << " ret=" << ret << endl;
            for(size_t i = 0; i < vRecords.size(); i++)
            {
                cout << "set-tars erase:" << vRecords[i]._key << ":" << vRecords[i]._value << endl;
            }
        }
        else if(option.hasParam("get-rand"))
        {
            testGetRand();
        }
        else if(option.hasParam("del"))
        {
            TC_HashMap::BlockData stRecord;
            string k;
            k = option.getValue("key");
            int ret = g_hmap.del(k, stRecord);
            cout << "del:" << k << " ret=" << ret << endl;
            cout << "del:" << stRecord._key << endl;
        }
        else if(option.hasParam("get"))
        {
            string k;
            string v;
            k = option.getValue("key");
            time_t iSyncTime;
            int ret = g_hmap.get(k, v, iSyncTime);
            cout << "get:" << k << ":" << v << " ret=" << ret << endl;
        }
        else if(option.hasParam("erase"))
        {
            int ratio = TC_Common::strto<int>(option.getValue("ratio"));
            TC_HashMap::BlockData data;
            int ret = TC_HashMap::RT_OK;
            do
            {
                ret = g_hmap.erase(ratio, data, true);
                cout << "erase: " << data._key << "|" << data._value << "|ret:" << ret << endl;
            }while(/*ret == TC_HashMap::RT_ERASE_OK*/false);
        }
        else if(option.hasParam("set-dirty"))
        {
            string k = option.getValue("key");
            int ret = g_hmap.setDirty(k);
            cout << "set-dirty: " << k << "|" << ret << endl;
        }
        else if(option.hasParam("set-clean"))
        {
            string k = option.getValue("key");
            int ret = g_hmap.setClean(k);
            cout << "set-clean: " << k << "|" << ret << endl;
        }
        else if(option.hasParam("isdirty"))
        {
            string k;
            k = option.getValue("key");
            int ret = g_hmap.checkDirty(k);
            cout << "isdirty:" << k << " ret=" << ret << endl;
        }
        else if(option.hasParam("find"))
        {
            string k;
            k = option.getValue("key");
            TC_HashMap::lock_iterator it = g_hmap.find(k);
            if(it != g_hmap.end())
            {
                string v;
                int ret = it->get(k, v);
                if(ret != TC_HashMap::RT_OK)
                {
                    cout << "get error: ret=" << ret << endl;
                }
                else
                {
                    cout << "find ok:" << k << " value=" << v << endl;
                }
            }
            else
            {
                cout << "find none:" << k << endl;
            }
        }
        else if(option.hasParam("sync"))
        {
            TC_HashMap::BlockData data;
//            size_t n = atoi(option.getValue("count").c_str());
            size_t n = 0;
            while(true)
            {
                int ret = g_hmap.sync(time(NULL), data);
                if (ret == TC_HashMap::RT_OK)
                {
                    break;
                }
                else
                {
                    ++n;
                    cout << "sync:" << data._key << endl;
                }
            }
            cout << g_hmap.desc() << endl;
        }
        else if(option.hasParam("view"))
        {
            cout << g_hmap.desc() << endl;
        }
        else if(option.hasParam("listh"))
        {
            TC_HashMap::lock_iterator it;
            if(option.hasParam("asc"))
            {
                it = g_hmap.begin();
            }
            else
            {
                it = g_hmap.rbegin();
            }

            while(it != g_hmap.end())
            {
                string k;
                string v;

                it->get(k, v);
                ++it;

//                usleep(100000);
                cout << k << ":" << v << endl;
            }
        }
        else if(option.hasParam("listd"))
        {
            TC_HashMap::lock_iterator it = g_hmap.beginDirty();

            while(it != g_hmap.end())
            {
                string k;
                string v;

                it->get(k, v);
                it++;

//                usleep(100000);
                cout << k << ":" << v << endl;
            }
        }
        else if(option.hasParam("listt"))
        {
            TC_HashMap::lock_iterator it;

            if(option.hasParam("asc"))
            {
                it = g_hmap.beginSetTime();
            }
            else
            {
                it = g_hmap.rbeginSetTime();
            }

            while(it != g_hmap.end())
            {
                string k;
                string v;

                it->get(k, v);
                ++it;

//                usleep(100000);
                cout << k << ":" << v << endl;
            }
        }
        else if(option.hasParam("listg"))
        {
            TC_HashMap::lock_iterator it;
            
            if(option.hasParam("asc"))
            {
                it = g_hmap.beginGetTime();
            }
            else
            {
                it = g_hmap.rbeginGetTime();
            }
            
            while(it != g_hmap.end())
            {
                string k;
                string v;
                
                it->get(k, v);
                ++it;
                
                cout << k << ":" << v << endl;
            }
        }

        else
        {
            usage(argv[0]);
        }
    }

    return 0;
}

