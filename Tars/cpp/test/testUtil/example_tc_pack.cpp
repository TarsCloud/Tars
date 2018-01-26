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

#include "util/tc_pack.h"
#include <iostream>
#include <cassert>

using namespace tars;

int main(int argc, char *argv[])
{
    try
    {
        bool b = true;
        char c = 'a';
        short si = 3;
        int   ii = 4;
        char cn[] = "abc";
        string sn = "def";

        TC_PackIn pi;
        pi << b << c << si << ii << cn << sn;

        string s = pi.topacket();

        TC_PackOut po(s.c_str(), s.length());
        po >> b;
        assert(b == true);
        cout << "bool OK" << endl;

        po >> c;
        assert(c == 'a');
        cout << "char OK" << endl;

        po >> si;
        assert(si == 3);
        cout << "short OK" << endl;

        po >> ii;
        assert(ii == 4);
        cout << "int OK" << endl;

        po >> cn;
        assert(cn == string("abc"));
        cout << "char[] OK" << endl;

        po >> sn;
        assert(sn == "def");
        cout << "string OK" << endl;

        {
            pi.clear();

            pi << b << c;
            pi.insert(1) << cn;

            s = pi.topacket();
            po.init(s.c_str(), s.length());
            po >> b;
            assert(b == true);
            cout << "bool OK" << endl;

            po >> cn;
            assert(cn == string("abc"));
            cout << "char[] OK" << endl;

            po >> c;
            assert(c == 'a');
            cout << "char OK" << endl;
        }

        {
            pi.clear();
            pi << b << c;
            pi.replace(1) << 'b';

            s = pi.topacket();
            po.init(s.c_str(), s.length());
            po >> b;
            assert(b == true);
            cout << "bool OK" << endl;

            po >> c;
            assert(c == 'b');
            cout << "char OK" << endl;
        }
    }
    catch(exception &ex)
    {
        cout << ex.what() << endl;
    }

    return 0;
}


