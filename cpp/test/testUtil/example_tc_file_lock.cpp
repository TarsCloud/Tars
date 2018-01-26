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

#include "util/tc_file_mutex.h"
#include <unistd.h>
#include <iostream>

using namespace tars;

void ThreadEntry1(TC_FileMutex *fl)
{
    int i = 1;
    while(i == 0)
    {
        fl->rlock();
        cout << i << ":" << pthread_self() << " : 1234567890abcdefghijklmnopqrstuvwxyz" << endl;
        sleep(1);
        cout << i << ":" << pthread_self() << " : 1234567890abcdefghijklmnopqrstuvwxyz" << endl;
//        fl->unlock();
        sleep(1);
        i++;
    }
}

void ThreadEntry2(TC_FileMutex *fl)
{
    int i = 1;
    while(i == 0)
    {
        fl->wlock();
        cout << i << ":" << pthread_self() << " : 1234567890abcdefghijklmnopqrstuvwxyz" << endl;
        sleep(1);
        cout << i << ":" << pthread_self() << " : 1234567890abcdefghijklmnopqrstuvwxyz" << endl;
//        fl->unlock();
        sleep(1);
        i++;
    }
}


int main(int argc, char *argv[])
{
    try
    {
        TC_FileMutex sl;
        sl.init("./test.lock");

        pthread_t itid1;
        pthread_t itid2;

        pthread_create(&itid1, NULL, (void *(*)(void *))&ThreadEntry1, (void*)&sl);
        pthread_create(&itid2, NULL, (void *(*)(void *))&ThreadEntry2, (void*)&sl);

        pthread_join(itid1, NULL);
        pthread_join(itid2, NULL);
    }
    catch(exception &ex)
    {
        cout << ex.what() << endl;
    }

    return 0;
}


