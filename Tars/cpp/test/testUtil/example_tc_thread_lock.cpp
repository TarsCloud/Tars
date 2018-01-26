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

#include "util/tc_monitor.h"
#include "util/tc_common.h"
#include <iostream>

using namespace tars;

TC_ThreadLock mutex;

template<typename T>
class TestLock : public T
{
public:
    TestLock()
    {
        i = 0;
    }

    void out()
    {
        typename T::Lock lock(*this);
        ++i;
        cout << pthread_self() << ":" << i << endl;
        sleep(3);
        cout << pthread_self() << ":" << i << endl;
    }

    void testLock(int i)
    {
        long long n = TC_Common::now2us();
        while(i)
        {
            typename T::Lock lock(*this);
            --i;
        }
        cout << (TC_Common::now2us() - n)/1000. << endl;
    }
protected:
    int i;
};

//TestLock<TC_ThreadLock> tl;
TestLock<TC_ThreadRecLock> tl;

void ThreadEntry1(void *)
{
    while(1)
        tl.out();
}

void ThreadEntry2(void *)
{
    while(1)
        tl.out();
}

int main(int argc, char *argv[])
{
    try
    {
        int i = 1000000;
        tl.testLock(i);

        pthread_t itid1;
        pthread_t itid2;

        pthread_create(&itid1, NULL, (void *(*)(void *))&ThreadEntry1, NULL);
        pthread_create(&itid2, NULL, (void *(*)(void *))&ThreadEntry2, NULL);

        pthread_join(itid1, NULL);
        pthread_join(itid2, NULL);
    }
    catch(exception &ex)
    {
        cout << ex.what() << endl;
    }

    return 0;
}


