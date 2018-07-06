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

#include "util/tc_epoller.h"
#include <unistd.h>

namespace tars
{

TC_Epoller::TC_Epoller(bool bEt)
{
    _iEpollfd   = -1;
    _pevs       = NULL;
    _et         = bEt;
    _max_connections = 1024;
}

TC_Epoller::~TC_Epoller()
{
    if(_pevs != NULL)
    {
        delete[] _pevs;
        _pevs = NULL;
    }

    if(_iEpollfd > 0)
    {
        close(_iEpollfd);
    }
}

void TC_Epoller::ctrl(int fd, long long data, __uint32_t events, int op)
{
    struct epoll_event ev;
    ev.data.u64 = data;
    if(_et)
    {
        ev.events   = events | EPOLLET;
    }
    else
    {
        ev.events   = events;
    }

    epoll_ctl(_iEpollfd, op, fd, &ev);
}

void TC_Epoller::create(int max_connections)
{
    _max_connections = max_connections;

    _iEpollfd = epoll_create(_max_connections + 1);

    delete[] _pevs;

    _pevs = new epoll_event[_max_connections + 1];
}

void TC_Epoller::add(int fd, long long data, __uint32_t event)
{
    ctrl(fd, data, event, EPOLL_CTL_ADD);
}

void TC_Epoller::mod(int fd, long long data, __uint32_t event)
{
    ctrl(fd, data, event, EPOLL_CTL_MOD);
}

void TC_Epoller::del(int fd, long long data, __uint32_t event)
{
    ctrl(fd, data, event, EPOLL_CTL_DEL);
}

int TC_Epoller::wait(int millsecond)
{
    return epoll_wait(_iEpollfd, _pevs, _max_connections + 1, millsecond);
}

}

