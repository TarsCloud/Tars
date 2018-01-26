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

#include "util/tc_file.h"
#include "util/tc_gzip.h"
#include "util/tc_common.h"

using namespace tars;

int main(int argc, char *argv[])
{
    try
    {
        vector<char> file;
        TC_File::load2str("./example_tc_gzip.cpp", file);

        {
            vector<char> v;
            tars::TC_GZip::compress(&file[0], file.size(), v);

            cout << "compress ok, size:" << v.size() << endl;

            vector<char> s1;
            tars::TC_GZip::uncompress(&v[0], v.size(), s1);

            assert(file == s1);
        }
    }
    catch(exception &ex)
    {
        cout << ex.what() << endl;
    }

    return 0;
}


