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

#include <cassert>
#include <sys/file.h>
#include <unistd.h>
#include "util/tc_file_mutex.h"
#include "util/tc_lock.h"

namespace tars
{
    
TC_FileMutex::TC_FileMutex() 
{
    _fd = -1;
}

TC_FileMutex::~TC_FileMutex()
{
    unlock();
}

void TC_FileMutex::init(const std::string& filename)
{
    if (filename.empty())
    {
         throw TC_FileMutex_Exception("[TC_FileMutex::init] filename is empty");
    }

    if(_fd > 0)
    {
        close(_fd);
    }
    _fd = open(filename.c_str(), O_RDWR|O_CREAT, 0660);
    if (_fd < 0) 
    {
        throw TC_FileMutex_Exception("[TC_FileMutex::init] open '" + filename + "' error", errno);
    }
}

int TC_FileMutex::rlock()
{
    assert(_fd > 0);

    return lock(_fd, F_SETLKW, F_RDLCK, 0, 0, 0);
}

int TC_FileMutex::unrlock() 
{
    return unlock();
}

bool TC_FileMutex::tryrlock()
{
    return hasLock(_fd, F_RDLCK, 0, 0, 0);
}

int TC_FileMutex::wlock()
{
    assert(_fd > 0);

    return lock(_fd, F_SETLKW, F_WRLCK, 0, 0, 0);
}

int TC_FileMutex::unwlock() 
{
    return unlock();
}

bool TC_FileMutex::trywlock()
{
    return hasLock(_fd, F_WRLCK, 0, 0, 0);
}

int TC_FileMutex::unlock()
{
    return lock(_fd, F_SETLK, F_UNLCK, 0, 0, 0);
}

int TC_FileMutex::lock(int fd, int cmd, int type, off_t offset, int whence, off_t len)
{
    struct flock lock;
    lock.l_type     = type;
    lock.l_start     = offset;
    lock.l_whence     = whence;
    lock.l_len         = len;

    return fcntl(fd, cmd, &lock);
}

bool TC_FileMutex::hasLock(int fd, int type, off_t offset, int whence, off_t len)
{
    struct flock lock;
    lock.l_type     = type;
    lock.l_start     = offset;
    lock.l_whence     = whence;
    lock.l_len         = len;

    if(fcntl(fd, F_GETLK, &lock)  == -1)
    {
        throw TC_FileMutex_Exception("[TC_FileMutex::hasLock] fcntl error", errno);
    }

    if(lock.l_type == F_UNLCK)
    {
        return false;
    }
    
    return true;
}

}

