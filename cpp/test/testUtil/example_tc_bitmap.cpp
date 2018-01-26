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

#include "util/tc_shm.h"
#include "util/tc_bitmap.h"
#include "util/tc_option.h"
#include "util/tc_common.h"

#include <iterator>
#include <iostream>

using namespace tars;

TC_Shm      g_shm;
TC_BitMap   g_bmap;

void initMap()
{
    unsigned iBit       = 3;
    size_t iMemSize     = TC_BitMap::calcMemSize(100, iBit);
    g_shm.init(iMemSize, 9999);

    if (g_shm.iscreate())
    {
        g_bmap.create(g_shm.getPointer(), g_shm.size(), iBit);
    }
    else
    {
        g_bmap.connect(g_shm.getPointer(), g_shm.size(), iBit);
    }

    cout << "init mem:" << iMemSize << endl;
}

void usage(char *argv[])
{
    cout << argv[0] << " --get=uin --bit" << endl;
    cout << argv[0] << " --set=uin --bit" << endl;
    cout << argv[0] << " --clear=uin --bit" << endl;
    cout << argv[0] << " --clear4all --bit" << endl;
    cout << argv[0] << " --dump=file" << endl;
    cout << argv[0] << " --load=file" << endl;
    cout << argv[0] << " --help" << endl;
}

int main(int argc, char *argv[])
{
    try
    {
        TC_Option op;
        op.decode(argc, argv);
 
        initMap();

        if(op.hasParam("get"))
        {
            size_t uin = TC_Common::strto<size_t>(op.getValue("get"));
            unsigned bit = TC_Common::strto<unsigned>(op.getValue("bit"));
            cout << "get:" << uin << "=" << g_bmap.get(uin, bit) << endl;
        }
        else if(op.hasParam("set"))
        {
            size_t uin = TC_Common::strto<size_t>(op.getValue("set"));
            unsigned bit = TC_Common::strto<unsigned>(op.getValue("bit"));
            cout << "set:" << uin << "=" << g_bmap.set(uin, bit) << endl;
        }
        else if(op.hasParam("clear"))
        {
            size_t uin = TC_Common::strto<size_t>(op.getValue("clear"));
            unsigned bit = TC_Common::strto<unsigned>(op.getValue("bit"));
            cout << "clear:" << uin << "=" << g_bmap.clear(uin, bit) << endl;
        }
        else if(op.hasParam("clear4all"))
        {
            int bit = TC_Common::strto<int>(op.getValue("bit"));
            cout << "clear4all:" << bit << "=" << g_bmap.clear4all((unsigned)bit) << endl;
        }
        else if(op.hasParam("dump"))
        { 
            string file = op.getValue("dump");
            int ret = g_bmap.dump2file(file);
            if(ret == 0)
                cout << "dump2file:" << file << " ok" << endl;
            else
                cout << "dump2file:" << file << " err:" << ret << endl;
        } 
        else if(op.hasParam("load"))
        {
            string file = op.getValue("load");
            int ret = g_bmap.load5file(file);
            if(ret == 0)
                cout << "load5file:" << file << " ok" << endl;
            else
                cout << "load5file:" << file << " err:" << ret << endl;
        }
        else
        {
            usage(argv);
        }
    }
    catch(exception &ex)
    {
        cout << ex.what() << endl;
    }

    return 0;
}


