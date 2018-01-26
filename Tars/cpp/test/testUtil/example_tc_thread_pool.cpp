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

#include "util/tc_thread_pool.h"
#include "util/tc_common.h"

#include <iostream>
#include <vector>

using namespace std;
using namespace tars;

TC_ThreadPool tpool;
TC_ThreadLock l;

/**
 * 线程私有数据
 */
class MyThreadData : public TC_ThreadPool::ThreadData
{
public:
    virtual ~MyThreadData()
    {
        cout << pthread_self() << endl;
    }

public:
    pthread_t _idata;
};

/**
 * 线程初始化
 */
void threadInitialize()
{
    MyThreadData *p = TC_ThreadPool::ThreadData::makeThreadData<MyThreadData>();
    p->_idata = pthread_self();
    TC_ThreadPool::setThreadData(p);

    cout << p->_idata << endl;
}

/**
 * 线程调用过程
 * @param s
 * @param i
 */
void TestFunction3(const string &s, int i)
{
    MyThreadData *p = (MyThreadData*)TC_ThreadPool::getThreadData();
    assert(pthread_self() == p->_idata);

//    cout << pthread_self() << " | TestFunction3('" << s << "', " << i << ")" << endl;
}

/**
 * 运行线程池
 */
void testThreadPool()
{
    //4个线程
    tpool.init(4);

    TC_Functor<void> init(threadInitialize);
    TC_Functor<void>::wrapper_type iwt(init);
    //启动线程, 指定初始化对象,也可以没有初始化对象:tpool.start();
    tpool.start(iwt);

    string s("a");
    int i = 1000000;

    //调用i次
    TC_Functor<void, TL::TLMaker<const string&, int>::Result> cmd(TestFunction3);
    while(i)
    {
        TC_Functor<void, TL::TLMaker<const string&, int>::Result>::wrapper_type fw(cmd, s, i);
        tpool.exec(fw);
        --i;
    } 

    //等待线程结束
    cout << "waitForAllDone..." << endl;
    bool b = tpool.waitForAllDone(1000);
    cout << "waitForAllDone..." << b << ":" << tpool.getJobNum() << endl;

    //停止线程,析够的时候也会自动停止线程
    //线程结束时,会自动释放私有数据
    tpool.stop();
}

void test(int i, string &s)
{
    cout << i << ":" << s << endl;
    s = TC_Common::tostr(i + 10);
}

/**
 * 运行线程池
 */
void testThreadPool1()
{
    //4个线程
    tpool.init(1);

    //启动线程, 指定初始化对象,也可以没有初始化对象:tpool.start();
    tpool.start();

    typedef void (*TpMem)(int, string&);
    TC_Functor<void, TL::TLMaker<int, string&>::Result> cmd(static_cast<TpMem>(&test));

    string bid;
       for(int i=0; i<10; i++)
       {
        cout << bid << endl;

           bid = TC_Common::tostr(i);

           cout << "index = " << i << ",bid = " << bid << endl;

           TC_Functor<void, TL::TLMaker<int, string&>::Result>::wrapper_type fwrapper(cmd, i, bid);
           tpool.exec(fwrapper);

//        sleep(1);
       }

    //等待线程结束
    cout << "waitForAllDone..." << endl;
    bool b = tpool.waitForAllDone(1000);
    cout << "waitForAllDone..." << b << ":" << tpool.getJobNum() << endl;

    //停止线程,析够的时候也会自动停止线程
    //线程结束时,会自动释放私有数据
    tpool.stop();
}
 
int main(int argc, char *argv[])
{
    try
    {
        testThreadPool1(); 
    }
    catch(exception &ex)
    {
        cout << ex.what() << endl;
    }

    return 0;
}


