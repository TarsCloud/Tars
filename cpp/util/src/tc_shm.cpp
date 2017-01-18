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

#include "util/tc_shm.h"
#include <cassert>
#include <errno.h>

namespace tars
{

TC_Shm::TC_Shm(size_t iShmSize, key_t iKey, bool bOwner)
{
    init(iShmSize, iKey, bOwner);
}

TC_Shm::~TC_Shm()
{
    if(_bOwner)
    {
        detach();
    }
}

void TC_Shm::init(size_t iShmSize, key_t iKey, bool bOwner)
{
    assert(_pshm == NULL);

    _bOwner     = bOwner;

    //注意_bCreate的赋值位置:保证多线程用一个对象的时候也不会有问题
    //试图创建
    if ((_shemID = shmget(iKey, iShmSize, IPC_CREAT | IPC_EXCL | 0666)) < 0)
    {
        _bCreate = false;
        //有可能是已经存在同样的key_shm,则试图连接
        if ((_shemID = shmget(iKey, iShmSize, 0666)) < 0)
        {
            throw TC_Shm_Exception("[TC_Shm::init()] shmget error", errno);
        }
    }
    else
    {
        _bCreate    = true;
    }

    //try to access shm
    if ((_pshm = shmat(_shemID, NULL, 0)) == (char *) -1)
    {
        throw TC_Shm_Exception("[TC_Shm::init()] shmat error", errno);
    }

    _shmSize = iShmSize;
    _shmKey = iKey;
}

int TC_Shm::detach()
{
    int iRetCode = 0;
    if(_pshm != NULL)
    {
        iRetCode = shmdt(_pshm);

        _pshm = NULL;
    }

    return iRetCode;
}

int TC_Shm::del()
{
    int iRetCode = 0;
    if(_pshm != NULL)
    {
        iRetCode = shmctl(_shemID, IPC_RMID, 0);

        _pshm = NULL;
    }

    return iRetCode;
}

}
