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

#include "util/tc_singleton.h"
#include "util/tc_timeprovider.h"
#include <unistd.h>
#include <iostream>

using namespace std;
using namespace tars;

class B : public TC_Singleton<B, CreateStatic,  PhoneixLifetime>
{
public:
    B(){cout << "B" << endl;}
    ~B(){cout << "~B" << endl;}

    void test(){cout << "test B" << endl;}             
};

class A : public TC_Singleton<A, CreateStatic,  PhoneixLifetime>
{
public:
    A(){cout << "A" << endl;}
    ~A()
    {
        cout << "~A" << endl;
        B::getInstance()->test();
    }

    void test(){cout << "test A" << endl;}             
};



int main(int argc, char *argv[])
{
    try
    {
//        A::getInstance()->test();
//        B::getInstance()->test();

        cout << TC_TimeProvider::getInstance()->getNow() << endl;
        sleep(1);
    }
    catch(exception &ex)
    {
        cout << ex.what() << endl;
    }

    return 0;
}

 
