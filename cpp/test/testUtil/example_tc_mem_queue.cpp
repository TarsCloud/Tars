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

#include "util/tc_mem_queue.h"
#include "util/tc_sem_mutex.h"
#include "util/tc_shm.h"
#include "util/tc_thread_pool.h"
#include "util/tc_common.h"
#include <sstream>
#include <iostream>

using namespace tars;

TC_Shm shm;
TC_SemMutex  semLock;
TC_MemQueue  memQueue;

/**
 * 线程调用过程
 * @param s
 * @param i
 */
void writeQueue()
{
    int i = 100000000;
    while(i)
    {
        TC_LockT<TC_SemMutex> l(semLock);
        if(memQueue.push_back(TC_Common::tostr(i)))
        {
            cout << pthread_self() << " | writeQueue OK " << i << ":" << memQueue.elementCount() << endl;
            i--;
        }
        else
        {
            cout << pthread_self() << " | writeQueue FULL " << i << endl;
        }
    }
}

void readQueue()
{
    while(true)
    {
        string s;
        TC_LockT<TC_SemMutex> l(semLock);
        if(memQueue.pop_front(s))
        {
            cout << pthread_self() << " | readQueue OK " << s << endl;
        }
        else
        {
            cout << pthread_self() << " | readQueue EMPTY" << endl;
            sleep(1);
        }
    }
}

int main(int argc, char *argv[])
{
    try
    {
        size_t l = 1024000;
        shm.init(l, 8000);
        semLock.init(8000);

        if(shm.iscreate())
        {
            memQueue.create(shm.getPointer(), l);
        }
        else
        {
            memQueue.connect(shm.getPointer(), l);
        }

        if(argc > 1)
        {
            TC_ThreadPool twpool;
            twpool.init(4);
            twpool.start();

            for(size_t i = 0; i < twpool.getThreadNum(); i++)
            {
                twpool.exec(writeQueue);
            }

            twpool.waitForAllDone();
        }
        else
        {
            TC_ThreadPool trpool;
            trpool.init(4);
            trpool.start();

            for(size_t i = 0; i < trpool.getThreadNum(); i++)
            {
                trpool.exec(readQueue);
            }

            trpool.waitForAllDone();
        }
    }
    catch(exception &ex)
    {
        cout << ex.what() << endl;
    }

    return 0;
}


