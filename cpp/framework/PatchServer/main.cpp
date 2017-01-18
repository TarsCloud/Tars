/**
 * Tencent is pleased to support the open source community by making Tars available.
 *
 * Copyright (C) 2016THL A29 Limited, a Tencent company. All rights reserved.
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

#include "PatchServer.h"
#include <iostream>

using namespace std;

TC_Config *g_conf;
PatchServer g_app;
/**
 * 说明
 * 1 需要配置一个路径
 * 2 如果子目录是link目录, 则不进入link目录遍历
 * 3 如果文件是link, 则读取link的文件
 * 4 如果同步的文件是link目录, 则进入link目录遍历, 下面的规则如1
 * @param argc
 * @param argv
 *
 * @return int
 */
int main(int argc, char *argv[])
{
    try
    {
        g_conf = &g_app.getConfig();
        g_app.main(argc, argv);
        g_app.waitForShutdown();
    }
    catch(exception &ex)
    {
        cout << ex.what() << endl;
    }

    return 0;
}


