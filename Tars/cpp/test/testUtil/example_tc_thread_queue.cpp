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
#include "util/tc_thread_queue.h"
#include "util/tc_autoptr.h"
#include "util/tc_thread.h"
#include "util/tc_logger.h"
#include <iostream>

using namespace tars;

TC_ThreadQueue<string> _queue;
TC_RollLogger _logger;

struct A : public TC_HandleBase
{
};

typedef TC_AutoPtr<A> APtr;

class WriteThread : public TC_Thread, public TC_HandleBase
{
    /**
     * 运行
     */
protected:
    virtual void run() 
    {
        while(true)
        {
            timeval t1;
            gettimeofday(&t1, NULL);

            _queue.push_back("abc");
            usleep(1000);

            timeval t2;
            gettimeofday(&t2, NULL);

            _logger.debug() << "push_back:" << t2.tv_usec - t1.tv_usec << endl;
                usleep(1000);
                cout << "write" << endl;
        }
    }
};

typedef TC_AutoPtr<WriteThread> WriteThreadPtr;

class ReadThread : public TC_Thread, public TC_HandleBase
{
    /**
     * 运行
     */
protected:
    virtual void run() 
    {
        while(true)
        {
            string t;
            timeval t1;
            gettimeofday(&t1, NULL);
            if(_queue.pop_front(t, 60000))
            {
                timeval t2;
                gettimeofday(&t2, NULL);
                _logger.debug() << "pop_front:" << t2.tv_usec - t1.tv_usec << endl;
//                cout << pthread_self() << ":" << t << endl;
                usleep(20 * 1000);
            }

        }
    }
};

typedef TC_AutoPtr<ReadThread> ReadThreadPtr;

int main(int argc, char *argv[])
{
    try
    {
        WriteThreadPtr wt = new WriteThread();
        wt->start();

        vector<ReadThreadPtr> rts;
        for(size_t i = 0; i < 3;i++)
        {
            rts.push_back(new ReadThread());
            rts.back()->start();
        }

        wt->getThreadControl().join();
    }
    catch(exception &ex)
    {
        cout << ex.what() << endl;
    }

    return 0;
}

                           
