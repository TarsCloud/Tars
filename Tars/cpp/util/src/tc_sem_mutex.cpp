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

#include <errno.h>
#include <string.h>
#include "util/tc_sem_mutex.h"

namespace tars
{

TC_SemMutex::TC_SemMutex():_semID(-1),_semKey(-1)
{

}

TC_SemMutex::TC_SemMutex(key_t iKey)
{
    init(iKey);
}

void TC_SemMutex::init(key_t iKey)
{
    #if defined(__GNU_LIBRARY__) && !defined(_SEM_SEMUN_UNDEFINED)
    /* union semun is defined by including <sys/sem.h> */
    #else
    /* according to X/OPEN we have to define it ourselves */
    union semun
    {
         int val;                  /* value for SETVAL */
         struct semid_ds *buf;     /* buffer for IPC_STAT, IPC_SET */
         unsigned short *array;    /* array for GETALL, SETALL */
                                   /* Linux specific part: */
         struct seminfo *__buf;    /* buffer for IPC_INFO */
    };
    #endif

    int  iSemID;
    union semun arg;
    u_short array[2] = { 0, 0 };

    //生成信号量集, 包含两个信号量
    if ( (iSemID = semget( iKey, 2, IPC_CREAT | IPC_EXCL | 0666)) != -1 )
    {
        arg.array = &array[0];

        //将所有信号量的值设置为0
        if ( semctl( iSemID, 0, SETALL, arg ) == -1 )
        {
            throw TC_SemMutex_Exception("[TC_SemMutex::init] semctl error:" + string(strerror(errno)));
        }
    }
    else
    {
        //信号量已经存在
        if ( errno != EEXIST )
        {
            throw TC_SemMutex_Exception("[TC_SemMutex::init] sem has exist error:" + string(strerror(errno)));
        }

        //连接信号量
        if ( (iSemID = semget( iKey, 2, 0666 )) == -1 )
        {
            throw TC_SemMutex_Exception("[TC_SemMutex::init] connect sem error:" + string(strerror(errno)));
        }
    }

    _semKey = iKey;
    _semID  = iSemID;
}

int TC_SemMutex::rlock() const
{
    //进入共享锁, 第二个信号量的值表示当前使用信号量的进程个数
    //等待第一个信号量变为0(排他锁没有使用)
    //占用第二个信号量(第二个信号量值+1, 表示被共享锁使用)
    struct sembuf sops[2] = { {0, 0, SEM_UNDO}, {1, 1, SEM_UNDO} };
    size_t nsops = 2;
    int ret = -1;

    do
    {
        ret=semop(_semID,&sops[0],nsops);

    } while ((ret == -1) &&(errno==EINTR));

    return ret;

    //return semop( _semID, &sops[0], nsops);
}

int TC_SemMutex::unrlock( ) const
{
    //解除共享锁, 有进程使用过第二个信号量
    //等到第二个信号量可以使用(第二个信号量的值>=1)
    struct sembuf sops[1] = { {1, -1, SEM_UNDO} };
    size_t nsops = 1;

    int ret = -1;

    do
    {
        ret=semop(_semID,&sops[0],nsops);

    } while ((ret == -1) &&(errno==EINTR));

    return ret;

    //return semop( _semID, &sops[0], nsops);
}

bool TC_SemMutex::tryrlock() const
{
    struct sembuf sops[2] = { {0, 0, SEM_UNDO|IPC_NOWAIT}, {1, 1, SEM_UNDO|IPC_NOWAIT}};
    size_t nsops = 2;

    int iRet = semop( _semID, &sops[0], nsops );
    if(iRet == -1)
    {
        if(errno == EAGAIN)
        {
            //无法获得锁
            return false;
        }
        else
        {
            throw TC_SemMutex_Exception("[TC_SemMutex::tryrlock] semop error : " + string(strerror(errno)));
        }
    }
    return true;
}

int TC_SemMutex::wlock() const
{
    //进入排他锁, 第一个信号量和第二个信号都没有被使用过(即, 两个锁都没有被使用)
    //等待第一个信号量变为0
    //等待第二个信号量变为0
    //释放第一个信号量(第一个信号量+1, 表示有一个进程使用第一个信号量)
    struct sembuf sops[3] = { {0, 0, SEM_UNDO}, {1, 0, SEM_UNDO}, {0, 1, SEM_UNDO} };
    size_t nsops = 3;

    int ret = -1;

    do
    {
        ret=semop(_semID,&sops[0],nsops);

    } while ((ret == -1) &&(errno==EINTR));

    return ret;
    //return semop( _semID, &sops[0], nsops);
}

int TC_SemMutex::unwlock() const
{
    //解除排他锁, 有进程使用过第一个信号量
    //等待第一个信号量(信号量值>=1)
    struct sembuf sops[1] = { {0, -1, SEM_UNDO} };
    size_t nsops = 1;

    int ret = -1;

    do
    {
        ret=semop(_semID,&sops[0],nsops);

    } while ((ret == -1) &&(errno==EINTR));

    return ret;

    //return semop( _semID, &sops[0], nsops);

}

bool TC_SemMutex::trywlock() const
{
    struct sembuf sops[3] = { {0, 0, SEM_UNDO|IPC_NOWAIT}, {1, 0, SEM_UNDO|IPC_NOWAIT}, {0, 1, SEM_UNDO|IPC_NOWAIT} };
    size_t nsops = 3;

    int iRet = semop( _semID, &sops[0], nsops );
    if(iRet == -1)
    {
        if(errno == EAGAIN)
        {
            //无法获得锁
            return false;
        }
        else
        {
            throw TC_SemMutex_Exception("[TC_SemMutex::trywlock] semop error : " + string(strerror(errno)));
        }
    }

    return true;
}

}

