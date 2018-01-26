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

#include "util/tc_option.h"
#include "util/tc_common.h"
#include <iterator>
#include <iostream>
#include <sys/time.h>

using namespace tars;


int main(int argc, char *argv[])
{
    try
    {
        TC_Option op;
        op.decode(argc, argv);

        map<string, string> mp = op.getMulti();

        cout << "map:" << endl;
        cout << TC_Common::tostr(mp) << endl;

        vector<string> d = op.getSingle();
        cout << "vector:" << endl;
        cout << TC_Common::tostr(d) << endl;

        cout << op.getValue("abc") << endl;
    }
    catch(exception &ex)
    {
        cout << ex.what() << endl;
    }

    return 0;
}


