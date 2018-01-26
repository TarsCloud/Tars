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

#include "util/tc_mem_chunk.h"
#include "util/tc_sem_mutex.h"
#include "util/tc_shm.h"
#include "util/tc_mmap.h"
#include "util/tc_file.h"
#include "util/tc_option.h"
#include "util/tc_common.h"
#include <sstream>
#include <iostream>

using namespace tars;

TC_Mmap m;
TC_MemMultiChunkAllocator alloc;


void testAllocate()
{
    vector<size_t> b = alloc.getBlockSize();
    cout << TC_Common::tostr(b) << endl;
    cout << alloc.allBlockChunkCount() << endl;

    vector<void*> v;
    size_t n = 77;
    while(true)
    {
        size_t iAllocSize;
        void *p = alloc.allocate(n, iAllocSize);
        cout << p << endl;
        if(!p)
        {
            break;
        }
        v.push_back(p);
        n += 2;
    }

    cout << v.size() << endl;

    for(size_t i = 0; i < v.size(); i++)
    {
        alloc.deallocate(v[i]);
    }
}
int main(int argc, char *argv[])
{
    try
    {
        TC_Option option;
        option.decode(argc, argv);

        if(TC_File::isFileExist("mc.hmap"))
        {
            m.mmap("mc.hmap", TC_File::getFileSize("mc.hmap"));
        }
        else
        {
            m.mmap("mc.hmap", 1204);
        }

        if(m.iscreate())
        {
            alloc.create(m.getPointer(), m.getSize(), 30, 100, 1.2);
        }
        else
        {
            alloc.connect(m.getPointer());
        }

        if(option.hasParam("append"))
        {
            size_t i = m.getSize() * 3;

            m.munmap();
            m.mmap("mc.hmap", i);
            alloc.append(m.getPointer(), i);

            m.munmap();
            m.mmap("mc.hmap", i*3);
            alloc.append(m.getPointer(), i*3);

            m.munmap();
            m.mmap("mc.hmap", i*5);
            alloc.append(m.getPointer(), i*5);
        }
        else if(option.hasParam("alloc"))
        {
            testAllocate();
        }
    }
    catch(exception &ex)
    {
        cout << ex.what() << endl;
    }

    return 0;
}


