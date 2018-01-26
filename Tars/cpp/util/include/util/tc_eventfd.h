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

#ifndef __TC_EVENT_FD_H_
#define __TC_EVENT_FD_H_

#include <stdint.h>
#include <unistd.h>
#include <fcntl.h>
#include <syscall.h>

#ifdef SYS_eventfd
// since glibc 2.9
#include <sys/eventfd.h>
#else

#if defined __x86_64
#define SYS_eventfd 284
#elif defined __i386
#define SYS_eventfd 323
#else
#error unknown architecture
#endif

#ifndef O_CLOEXEC
#define O_CLOEXEC       02000000        /* set close_on_exec */
#endif

#define EFD_SEMAPHORE (1 << 0)
#define EFD_CLOEXEC   O_CLOEXEC
#define EFD_NONBLOCK  O_NONBLOCK

inline int eventfd(unsigned int initval, int flags)
{
    return syscall(SYS_eventfd, initval, flags);
}

typedef uint64_t eventfd_t;

inline int eventfd_read(int fd, eventfd_t *value)
{
    return read(fd, value, sizeof(*value)) == sizeof(*value) ?  0 : -1;
}

inline int eventfd_write(int fd, eventfd_t value)
{
    return write(fd, &value, sizeof(value)) == sizeof(value) ?  0 : -1;
}
#endif

#endif

