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
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>

namespace tars
{

#ifdef __linux__

void TC_Epoller::Notify::init() {
}

void TC_Epoller::Notify::uninit() {
}

TC_Epoller::Notify::Notify(): _ep(NULL), _data(0L) {
	//
}

TC_Epoller::Notify::~Notify() {
	_soc.close();
}

void TC_Epoller::Notify::bind(TC_Epoller& ep, long long data) {
	_ep = &ep;
	_data = data;
	_soc.createSocket();
	_ep->add(_soc.getfd(), data, EPOLLIN);
}

void TC_Epoller::Notify::unbind() {
	_ep->del(_soc.getfd(), _data, EPOLLIN);
}

void TC_Epoller::Notify::signal() {
	_ep->mod(_soc.getfd(), _data, EPOLLOUT);
}

void TC_Epoller::Notify::wait() {
}

#else //

#ifndef O_NOATIME
#define O_NOATIME     01000000 /* Do not set atime.  */
#endif

void TC_Epoller::Notify::init() {
	uninit();

	int filedes[2];

	pipe(filedes);

	fcntl(filedes[0], F_SETFL, O_NOATIME);

	_in = fdopen(filedes[0], "rb");
	assert(_in);
	_out = fdopen(filedes[1], "wb");
	assert(_out);
	setvbuf(_in, NULL, _IONBF, 0);
	setvbuf(_out, NULL, _IONBF, 0);
}

void TC_Epoller::Notify::uninit() {
	if (_in) fclose(_in);
	if (_out) fclose(_out);
}

TC_Epoller::Notify::Notify(): _in(NULL), _out(NULL), _ep(NULL), _data(0L) {
	//
}

TC_Epoller::Notify::~Notify() {
	uninit();
}

void TC_Epoller::Notify::bind(TC_Epoller& ep, long long data) {
	_ep = &ep;
	_data = data;
}

void TC_Epoller::Notify::signal() {
	init();

	_ep->add(fileno(_in), _data, EPOLLIN);
	fclose(_out);
	_out = NULL;
}

void TC_Epoller::Notify::unbind() {
	//
}

void TC_Epoller::Notify::wait() {
	if (_ep) {
		_ep->del(fileno(_in), (long long)0, EPOLLIN);
	}

	uninit();
}

#endif //!

//=============================================================================

#ifdef __APPLE__

TC_Epoller::TC_Epoller(bool bEt)
{
    _iKqueuefd   = -1;
    _pevs       = NULL;
    _max_connections = 1024;
}

TC_Epoller::~TC_Epoller()
{
    if(_pevs != NULL)
    {
        delete[] _pevs;
        _pevs = NULL;
    }

    if(_iKqueuefd > 0)
    {
        close(_iKqueuefd);
    }
}

void TC_Epoller::ctrl(int fd, long long data, __uint32_t events, int op)
{
    struct kevent ev;
    int r = 0;

    if (op == EPOLL_CTL_MOD) {
        if (events & EPOLLIN) {
            EV_SET(&ev, fd, EVFILT_READ, EV_ADD | EV_CLEAR, 0, 0, (void*)data);
            r |= kevent(_iKqueuefd, &ev, 1, NULL, 0, NULL);
        }
        else {
            EV_SET(&ev, fd, EVFILT_READ, EV_DELETE | EV_CLEAR, 0, 0, NULL);
            kevent(_iKqueuefd, &ev, 1, NULL, 0, NULL);
        }
        if (events & EPOLLOUT) {
            EV_SET(&ev, fd, EVFILT_WRITE, EV_ADD | EV_CLEAR, 0, 0, (void*)data);
            r |= kevent(_iKqueuefd, &ev, 1, NULL, 0, NULL);
        }
        else {
            EV_SET(&ev, fd, EVFILT_WRITE, EV_DELETE | EV_CLEAR, 0, 0, NULL);
            kevent(_iKqueuefd, &ev, 1, NULL, 0, NULL);
        }
    }
    else if (op == EV_DELETE) {
    	if (events & EPOLLIN) {
			EV_SET(&ev, fd, EVFILT_READ, EV_DELETE | EV_CLEAR, 0, 0, NULL);
			kevent(_iKqueuefd, &ev, 1, NULL, 0, NULL);
		}
		if (events & EPOLLOUT) {
			EV_SET(&ev, fd, EVFILT_WRITE, EV_DELETE | EV_CLEAR, 0, 0, NULL);
			kevent(_iKqueuefd, &ev, 1, NULL, 0, NULL);
		}
    }
    else if (op == EV_ADD) {
    	if (events & EPOLLIN) {
			EV_SET(&ev, fd, EVFILT_READ, EV_ADD | EV_CLEAR, 0, 0, (void*)data);
			r |= kevent(_iKqueuefd, &ev, 1, NULL, 0, NULL);
		}
		if (events & EPOLLOUT) {
			EV_SET(&ev, fd, EVFILT_WRITE, EV_ADD | EV_CLEAR, 0, 0, (void*)data);
			r |= kevent(_iKqueuefd, &ev, 1, NULL, 0, NULL);
		}
    }
    assert(r == 0);
}

void TC_Epoller::create(int max_connections)
{ 
    _iKqueuefd = kqueue();

    if(_pevs != NULL)
    {
        delete[] _pevs;
    }

    _pevs = new struct kevent[_max_connections + 1];
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
    if (millsecond == 0) {
        struct timespec zerotv = {0, 0};
        return kevent(_iKqueuefd, NULL, 0, _pevs, _max_connections + 1, &zerotv);
    }
    else if (millsecond > 0) {
        struct timespec timeout;
        timeout.tv_sec = millsecond / 1000;
        timeout.tv_nsec = (millsecond % 1000) * 1000000;
        return kevent(_iKqueuefd, NULL, 0, _pevs, _max_connections + 1, &timeout);
    } else {
        return kevent(_iKqueuefd, NULL, 0, _pevs, _max_connections + 1, NULL);
    }
    return 0;
}

#else //linux

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

    if(_pevs != NULL)
    {
        delete[] _pevs;
    }

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

#endif

}
