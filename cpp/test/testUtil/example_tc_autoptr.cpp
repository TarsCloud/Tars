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

#include "util/tc_autoptr.h"
#include "util/tc_thread_pool.h"
#include <vector>
#include <cassert>
#include <iostream>

using namespace std;
using namespace tars;

void func(int &i)
{
}

class TestPointer : public TC_HandleBase
{
public:
    void func(int &i)
    {
        int n = 10000;
        while(n)
        {
            i++;
            n--;
        }
    }
};

void test()
{
    int i = 0;

    typedef TC_AutoPtr<TestPointer> TestPointerPtr;
    vector<TestPointerPtr> vtp;
    for(size_t j = 0; j < 10; j++)
    {
        vtp.push_back(new TestPointer());
    }

    cout << i << endl;
    for(size_t j = 0; j < 10; j++)
    {
        vtp[j]->func(i);
    }

    cout << i << endl;
    vector<TestPointerPtr> vtp1 = vtp;
    for(size_t j = 0; j < 10; j++)
    {
        vtp[j]->func(i);
    }

    cout << i << endl;
}

TC_Atomic a;

void testAdd()
{
    int i = 10000000;
    while(i--)
    {
        a.inc();
    }
}

void testDel()
{
    int i = 10000000;
    while(i--)
    {
        a.dec();
    }
}

int main(int argc, char *argv[])
{
    try
    {
        cout << a.get() << endl;

        TC_ThreadPool tpoolA;
        tpoolA.init(10);
        tpoolA.start();

        TC_ThreadPool tpoolB;
        tpoolB.init(10);
        tpoolB.start();

        {
            for(size_t i = 0; i < tpoolA.getThreadNum(); i++)
            {
                tpoolA.exec(testAdd);
            }
        }

        {
            for(size_t i = 0; i < tpoolB.getThreadNum(); i++)
            {
                tpoolB.exec(testDel);
            }
        }

        tpoolA.waitForAllDone();
        tpoolB.waitForAllDone();

        cout << a.get() << endl;
    }
    catch(exception &ex)
    {
        cout << ex.what() << endl;
    }

    return 0;
}


