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

#ifndef __PATCH_SERVER_H_
#define __PATCH_SERVER_H_

#include "servant/Application.h"
#include "Patch.h"

using namespace tars;

class PatchServer : public Application
{
protected:
    /**
     * 初始化, 只会进程调用一次
     */
    virtual void initialize();

    /**
     * 析够, 进程退出时会调用一次
     */
    virtual void destroyApp();

public:
    int getExpireTime() const;

private:
    //文件的内存缓存的过期时间
    int _expireTime;
};

extern TC_Config *g_conf;
extern PatchServer g_app;

#endif
