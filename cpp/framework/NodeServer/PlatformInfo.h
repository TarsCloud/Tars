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

#ifndef __PLAT_FORM_INFO_H_
#define __PLAT_FORM_INFO_H_

#include "Node.h"
#include <unistd.h>
#include "util/tc_config.h"
#include <iostream>
#include "servant/Application.h"

using namespace tars;
using namespace std;

extern TC_Config* g_pconf;

class PlatformInfo
{
public:
    /**
     * 获取node的相关信息
     */
    NodeInfo getNodeInfo() const;

    /**
     * 获取node的所在机器的负载信息
     */
    LoadInfo getLoadInfo() const;

    /**
     * 获取node的名称
     */
    string getNodeName() const;

    /**
     * 获取node的数据目录
     */
    string getDataDir() const;

    /**
     * 获取文件下载目录
     */
    string getDownLoadDir() const;
};

#endif

