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

#include "util/tc_thread.h"
#include <unistd.h>
#include <iostream>
#include <vector>

using namespace std;
using namespace tars;

class MyThread : public TC_Thread, public TC_ThreadLock
{
public:
    MyThread()
    {
        bTerminate = false;
    }
    /**
     * 结束线程
     */
    void terminate()
    {
        bTerminate = true;

        {
            TC_ThreadLock::Lock sync(*this);
            notifyAll();
        }
    }

    void doSomething()
    {
        cout << "doSomething" << endl;
    }
    /**
     * 运行
     */
protected:
    virtual void run() 
    {
        while(!bTerminate)
        {
             //TODO: your business
            doSomething();

            {
                TC_ThreadLock::Lock sync(*this);
                timedWait(1000);
            }
        }
    }

protected:
    bool bTerminate;
};

int main(int argc, char *argv[])
{
    try
    {
        MyThread mt;
        mt.start();

        sleep(5);

        mt.terminate();
        mt.getThreadControl().join();
    }
    catch(exception &ex)
    {
        cout << ex.what() << endl;
    }

    return 0;
}


