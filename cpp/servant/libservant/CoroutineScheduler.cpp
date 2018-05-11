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

#include <fcntl.h>
#include <signal.h>
#include <sys/mman.h>
#include <sys/resource.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>
#include <algorithm>
#include <cmath>
#include <cstring>
#include <stdexcept>
#include <assert.h>

#include "servant/CoroutineScheduler.h"
#include "servant/TarsLogger.h"
#include "servant/ServantHandle.h"

namespace tars 
{
////////////////////////////////////////////////////////
std::size_t pagesize()
{
    static std::size_t size = ::sysconf( _SC_PAGESIZE);
    return size;
}

rlimit stacksize_limit_()
{
    rlimit limit;

    const int result = ::getrlimit( RLIMIT_STACK, & limit);
    assert( 0 == result);

    return limit;
}

rlimit stacksize_limit()
{
    static rlimit limit = stacksize_limit_();
    return limit;
}

std::size_t page_count( std::size_t stacksize)
{
    return static_cast< std::size_t >( std::ceil(static_cast< float >(stacksize) / pagesize() ) );
}

bool standard_stack_allocator::is_stack_unbound()
{ 
    return RLIM_INFINITY == stacksize_limit().rlim_max; 
}

std::size_t standard_stack_allocator::default_stacksize()
{
    std::size_t size = 8 * minimum_stacksize();
    if ( is_stack_unbound() ) return size;

    assert( maximum_stacksize() >= minimum_stacksize() );
    return maximum_stacksize() == size ? size : (std::min)( size, maximum_stacksize() );
}

std::size_t standard_stack_allocator::minimum_stacksize()
{ 
    return 8 * 1024 + sizeof(fcontext_t) + 15; 
}

std::size_t standard_stack_allocator::maximum_stacksize()
{
    assert( ! is_stack_unbound() );
    return static_cast< std::size_t >( stacksize_limit().rlim_max);
}

int standard_stack_allocator::allocate( stack_context & ctx, std::size_t size)
{
    assert( minimum_stacksize() <= size);
    assert( is_stack_unbound() || ( maximum_stacksize() >= size) );

    const std::size_t pages( page_count( size) + 1);
    const std::size_t size_( pages * pagesize() );
    assert( 0 < size && 0 < size_);

    void * limit = ::mmap( 0, size_, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANON, -1, 0);

    if (limit == (void *) -1)
    {
        TLOGERROR("[TARS][[standard_stack_allocator::allocate memory failed]" << endl);
        return -1;
    }

    std::memset( limit, '\0', size_);

    const int result( ::mprotect( limit, pagesize(), PROT_NONE) );
    assert( 0 == result);

    ctx.size = size_;
    ctx.sp = static_cast< char * >( limit) + ctx.size;

    return 0;
}

void standard_stack_allocator::deallocate( stack_context & ctx)
{
    assert( ctx.sp);
    assert( minimum_stacksize() <= ctx.size);
    assert( is_stack_unbound() || ( maximum_stacksize() >= ctx.size) );

    void * limit = static_cast< char * >( ctx.sp) - ctx.size;

    ::munmap( limit, ctx.size);
}

////////////////////////////////////////////////////////
CoroutineInfo::CoroutineInfo()
: _prev(NULL)
, _next(NULL)
, _main(true)
, _scheduler(NULL)
, _uid(0)
, _eStatus(CORO_FREE)
, _ctx_to(NULL)
{
}

CoroutineInfo::CoroutineInfo(CoroutineScheduler* scheduler)
: _prev(NULL)
, _next(NULL)
, _main(false)
, _scheduler(scheduler)
, _uid(0)
, _eStatus(CORO_FREE)
, _ctx_to(NULL)
{
}

CoroutineInfo::CoroutineInfo(CoroutineScheduler* scheduler, uint32_t iUid, stack_context stack_ctx)
: _prev(NULL)
, _next(NULL)
, _main(false)
, _scheduler(scheduler)
, _uid(iUid)
, _eStatus(CORO_FREE)
, _stack_ctx(stack_ctx)
, _ctx_to(NULL)
{
}

CoroutineInfo::~CoroutineInfo()
{
}

void CoroutineInfo::registerFunc(const std::function<void ()>& callback)
{
    _callback = callback;

    _init_func.coroFunc = CoroutineInfo::corotineProc;

    _init_func.args = this;

    _ctx_to = make_fcontext(_stack_ctx.sp, _stack_ctx.size, CoroutineInfo::corotineEntry);

    jump_fcontext(&_ctx_from, _ctx_to, (intptr_t)this, false);
}

void CoroutineInfo::setStackContext(stack_context stack_ctx)
{
    _stack_ctx = stack_ctx;
}

void CoroutineInfo::corotineEntry(intptr_t q)
{
    CoroutineInfo *coro = (CoroutineInfo*)q;

    Func    func = coro->_init_func.coroFunc;
    void*    args = coro->_init_func.args;

    jump_fcontext(coro->_ctx_to, &(coro->_ctx_from), 0, false);

    try
    {
        func(args);
    }
    catch(std::exception &ex)
    {
        TLOGERROR("[TARS][CoroutineInfo::corotineEntry exception:" << ex.what() << endl);
    }
    catch(...)
    {
        TLOGERROR("[TARS][CoroutineInfo::corotineEntry unknown exception." << endl);
    }
}

void CoroutineInfo::corotineProc(void * args)
{
    CoroutineInfo *coro = (CoroutineInfo*)args;

    try
    {
        std::function<void ()> cb = coro->_callback;

        cb();
    }
    catch(std::exception &ex)
    {
        TLOGERROR("[TARS][CoroutineInfo::corotineProc exception:" << ex.what() << endl);
    }
    catch(...)
    {
        TLOGERROR("[TARS][CoroutineInfo::corotineProc unknown exception." << endl);
    }

    CoroutineScheduler* scheduler =  coro->getScheduler();
    scheduler->decUsedSize();
    scheduler->moveToFreeList(coro);

    scheduler->switchCoro(coro, &(scheduler->getMainCoroutine()));
    TLOGERROR("[TARS][CoroutineInfo::corotineProc no come." << endl);
}

//////////////////////////////////////////////////////////////
CoroutineScheduler::CoroutineScheduler()
: _terminal(false)
, _poolSize(1000)
, _stackSize(128*1024)
, _currentSize(0)
, _usedSize(0)
, _uniqId(0)
, _handle(NULL)
, _currentCoro(NULL)
, _all_coro(NULL)
{
    _all_coro = new CoroutineInfo*[_poolSize+1];
    for(size_t i = 0; i <= _poolSize; ++i)
    {
        _all_coro[i] = NULL;
    }

    CoroutineInfo::CoroutineHeadInit(&_active);
    CoroutineInfo::CoroutineHeadInit(&_avail);
    CoroutineInfo::CoroutineHeadInit(&_inactive);
    CoroutineInfo::CoroutineHeadInit(&_timeout);
    CoroutineInfo::CoroutineHeadInit(&_free);
}

CoroutineScheduler::~CoroutineScheduler()
{}

void CoroutineScheduler::init(uint32_t iPoolSize, size_t iStackSize)
{
    if(iPoolSize <= 0)
    {
        TLOGERROR("[TARS][[CoroutineScheduler::init iPoolSize <= 0." << endl);
        return ;
    }

    _terminal = false;
    _poolSize = iPoolSize;
    _stackSize = iStackSize;

    if(_poolSize <= 100)
    {
        _currentSize = _poolSize;
    }
    else
    {
        _currentSize = 100;
    }

    if(_all_coro != NULL)
    {
        delete [] _all_coro;
        _all_coro = new CoroutineInfo*[_poolSize+1];
        for(size_t i = 0; i <= _poolSize; ++i)
        {
            _all_coro[i] = NULL;
        }
    }

    _usedSize = 0;
    _uniqId = 0;

    int iSucc = 0;
    for(size_t i = 0; i < _currentSize; ++i)
    {
        CoroutineInfo *coro = new CoroutineInfo(this);

        stack_context s_ctx;
        int ret = _alloc.allocate(s_ctx, iStackSize);
        if(ret != 0)
        {
            TLOGERROR("[TARS][CoroutineScheduler::init iPoolSize:" << iPoolSize << "|iStackSize:" << iStackSize << "|i:" << i << endl);

            delete coro;
            coro = NULL;
            break;
        }

        coro->setStackContext(s_ctx);

        uint32_t iId = generateId();
        coro->setUid(iId);
        coro->setStatus(CORO_FREE);

        //_free.push_front(coro);

        //_all.insert(make_pair(coro->getUid(), coro));

        _all_coro[iId] = coro;

        CoroutineInfo::CoroutineAddTail(coro, &_free);

        ++iSucc;
    }

    _currentSize = iSucc;

    _mainCoro.setUid(0);
    _mainCoro.setStatus(CORO_FREE);

    _currentCoro = &_mainCoro;

    TLOGDEBUG("[TARS][CoroutineScheduler::init iPoolSize:" << _poolSize << "|iCurrentSize:" << _currentSize << "|iStackSize:" << _stackSize << endl);
}

int    CoroutineScheduler::increaseCoroPoolSize()
{
    int iInc = ((_poolSize - _currentSize) > 100) ? 100 : (_poolSize - _currentSize);
    if(iInc <= 0)
    {
        TLOGERROR("[TARS][CoroutineScheduler::increaseCoroPoolSize full iPoolSize:" << _poolSize << "|iCurrentSize:" << _currentSize << endl);
        return -1;
    }

    int iSucc = 0;
    for(int i = 0; i < iInc; ++i)
    {
        CoroutineInfo *coro = new CoroutineInfo(this);
        uint32_t iId = generateId();
        coro->setUid(iId);
        coro->setStatus(CORO_FREE);

        stack_context s_ctx;
        int ret = _alloc.allocate(s_ctx, _stackSize);
        if(ret != 0)
        {
            TLOGERROR("[TARS][CoroutineScheduler::increaseCoroPoolSize iPoolSize:" << _poolSize << "|iStackSize:" << _stackSize << "|i:" << i << endl);

            delete coro;
            coro = NULL;
            break;
        }

        coro->setStackContext(s_ctx);

        _all_coro[iId] = coro;

        CoroutineInfo::CoroutineAddTail(coro, &_free);

        ++iSucc;
    }

    if(iSucc == 0)
    {
        TLOGERROR("[TARS][CoroutineScheduler::increaseCoroPoolSize cannot create iInc:" << iInc << "|iPoolSize:" << _poolSize << endl);
        return -1;
    }

    _currentSize += iSucc;

    return 0;
}

uint32_t CoroutineScheduler::createCoroutine(const std::function<void ()> &callback)
{
    if(_usedSize >= _currentSize || CoroutineInfo::CoroutineHeadEmpty(&_free))
    {
        int iRet = increaseCoroPoolSize();

        if(iRet != 0)
            return 0;
    }

    CoroutineInfo *coro = _free._next;
    assert(coro != NULL);

    CoroutineInfo::CoroutineDel(coro);

    _usedSize++;

    coro->setStatus(CORO_AVAIL);

    CoroutineInfo::CoroutineAddTail(coro, &_avail);

    coro->registerFunc(callback);

    return coro->getUid();
}

void CoroutineScheduler::run()
{
    while(!_terminal)
    {
        if(CoroutineInfo::CoroutineHeadEmpty(&_avail) && CoroutineInfo::CoroutineHeadEmpty(&_active))
        {
            TC_ThreadLock::Lock lock(_monitor);

            if(_activeCoroQueue.size() <= 0)
            {
                _monitor.timedWait(1000);
            }
        }

        wakeupbytimeout();

        wakeupbyself();

        wakeup();

        if(!CoroutineInfo::CoroutineHeadEmpty(&_active))
        {
            int iLoop = 100;
            while(iLoop > 0 && !CoroutineInfo::CoroutineHeadEmpty(&_active))
            {
                CoroutineInfo *coro = _active._next;

                assert(coro != NULL);

                switchCoro(&_mainCoro, coro);

                --iLoop;
            }

        }

        if(!CoroutineInfo::CoroutineHeadEmpty(&_avail))
        {
            CoroutineInfo *coro = _avail._next;

            assert(coro != NULL);

            switchCoro(&_mainCoro, coro);

        }

        if(_usedSize == 0)
            break;
    }

    destroy();
}

void CoroutineScheduler::tars_run()
{
    if(!_terminal)
    {
        wakeupbytimeout();

        wakeupbyself();

        wakeup();

        if(!CoroutineInfo::CoroutineHeadEmpty(&_active))
        {
            int iLoop = 100;
            while(iLoop > 0 && !CoroutineInfo::CoroutineHeadEmpty(&_active))
            {
                CoroutineInfo *coro = _active._next;

                assert(coro != NULL);

                switchCoro(&_mainCoro, coro);

                --iLoop;
            }

        }

        if(!CoroutineInfo::CoroutineHeadEmpty(&_avail))
        {
            int iLoop = 100;
            while(iLoop > 0 && !CoroutineInfo::CoroutineHeadEmpty(&_avail))
            {
                CoroutineInfo *coro = _avail._next;

                assert(coro != NULL);

                switchCoro(&_mainCoro, coro);

                --iLoop;
            }

        }
    }
}

void CoroutineScheduler::yield(bool bFlag)
{
    if(bFlag)
    {
        putbyself(_currentCoro->getUid());
    }

    moveToInactive(_currentCoro);
    switchCoro(_currentCoro, &_mainCoro);
}

void CoroutineScheduler::sleep(int iSleepTime)
{
    int64_t iNow = TNOWMS;
    int64_t iTimeout = iNow + (iSleepTime >= 0 ? iSleepTime : -iSleepTime);

    _timeoutCoroId.insert(make_pair(iTimeout, _currentCoro->getUid()));

    moveToTimeout(_currentCoro);
    switchCoro(_currentCoro, &_mainCoro);
}

void CoroutineScheduler::putbyself(uint32_t iCoroId)
{
    if(!_terminal)
    {
        _needActiveCoroId.push_back(iCoroId);
    }
}

void CoroutineScheduler::wakeupbyself()
{
    if(!_terminal)
    {
        if(_needActiveCoroId.size() > 0)
        {
            list<uint32_t>::iterator it = _needActiveCoroId.begin();
            while(it != _needActiveCoroId.end())
            {
                CoroutineInfo *coro = _all_coro[*it];

                assert(coro != NULL);

                moveToAvail(coro);

                ++it;
            }
            _needActiveCoroId.clear();
        }
    }
}

void CoroutineScheduler::put(uint32_t iCoroId)
{
    if(!_terminal)
    {
        _activeCoroQueue.push_back(iCoroId);

        if(_handle)
        {
            _handle->notifyFilter();
        }
        else
        {
            TC_ThreadLock::Lock lock(_monitor);

            _monitor.notifyAll();
        }
    }
}

void CoroutineScheduler::wakeup()
{
    if(!_terminal)
    {
        if(_activeCoroQueue.size() > 0)
        {

            TC_ThreadQueue<uint32_t, deque<uint32_t> >::queue_type coroIds;
            _activeCoroQueue.swap(coroIds);

            TC_ThreadQueue<uint32_t, deque<uint32_t> >::queue_type::iterator it = coroIds.begin();

            TC_ThreadQueue<uint32_t, deque<uint32_t> >::queue_type::iterator itEnd = coroIds.end();

            while(it != itEnd)
            {
                CoroutineInfo *coro = _all_coro[*it];

                assert(coro != NULL);

                moveToActive(coro);

                ++it;
            }

        }
    }
}

void CoroutineScheduler::wakeupbytimeout()
{
    if(!_terminal)
    {
        if(_timeoutCoroId.size() > 0)
        {
            int64_t iNow = TNOWMS;
            while(true)
            {
                multimap<int64_t, uint32_t>::iterator it = _timeoutCoroId.begin();

                if(it == _timeoutCoroId.end() || it->first > iNow)
                    break;

                CoroutineInfo *coro = _all_coro[it->second];

                assert(coro != NULL);

                moveToActive(coro);

                _timeoutCoroId.erase(it);
            }

        }
    }
}

void CoroutineScheduler::terminate()
{
    _terminal = true;

    if(_handle)
    {
        _handle->notifyFilter();
    }
    else
    {
        TC_ThreadLock::Lock lock(_monitor);

        _monitor.notifyAll();
    }
}

uint32_t CoroutineScheduler::generateId()
{
    while (++_uniqId < 1);

    assert(_uniqId <= _poolSize);

    return _uniqId;
}

void CoroutineScheduler::switchCoro(CoroutineInfo *from, CoroutineInfo *to)
{
    _currentCoro = to;

    jump_fcontext(from->getCtx(), to->getCtx(), 0, false);
}

void CoroutineScheduler::moveToActive(CoroutineInfo *coro, bool bFlag)
{
    if(coro->getStatus() == CORO_INACTIVE)
    {
        CoroutineInfo::CoroutineDel(coro);
        coro->setStatus(CORO_ACTIVE);
        CoroutineInfo::CoroutineAddTail(coro, &_active);
    }
    else if(coro->getStatus() == CORO_TIMEOUT)
    {
        CoroutineInfo::CoroutineDel(coro);
        coro->setStatus(CORO_ACTIVE);
        CoroutineInfo::CoroutineAddTail(coro, &_active);
    }
    else
    {
        TLOGERROR("[TARS][CoroutineScheduler::moveToActive ERROR|iCoroId:" << coro->getUid() << "|tyep:" << coro->getStatus() << endl);
    }
}

void CoroutineScheduler::moveToAvail(CoroutineInfo *coro)
{
    if(coro->getStatus() == CORO_INACTIVE)
    {
        CoroutineInfo::CoroutineDel(coro);
        coro->setStatus(CORO_AVAIL);
        CoroutineInfo::CoroutineAddTail(coro, &_avail);
    }
    else
    {
        TLOGERROR("[TARS][CoroutineScheduler::moveToAvail ERROR:|iCoroId:" << coro->getUid() << "|tyep:" << coro->getStatus() << endl);
    }
}

void CoroutineScheduler::moveToInactive(CoroutineInfo *coro)
{
    if(coro->getStatus() == CORO_ACTIVE)
    {
        CoroutineInfo::CoroutineDel(coro);
        coro->setStatus(CORO_INACTIVE);
        CoroutineInfo::CoroutineAddTail(coro, &_inactive);
    }
    else if(coro->getStatus() == CORO_AVAIL)
    {
        CoroutineInfo::CoroutineDel(coro);
        coro->setStatus(CORO_INACTIVE);
        CoroutineInfo::CoroutineAddTail(coro, &_inactive);
    }
    else
    {
        TLOGERROR("[TARS][CoroutineScheduler::moveToInactive ERROR|iCoroId:" << coro->getUid() << "|tyep:" << coro->getStatus() << endl);
    }
}

void CoroutineScheduler::moveToTimeout(CoroutineInfo *coro)
{
    if(coro->getStatus() == CORO_ACTIVE)
    {
        CoroutineInfo::CoroutineDel(coro);
        coro->setStatus(CORO_TIMEOUT);
        CoroutineInfo::CoroutineAddTail(coro, &_timeout);
    }
    else if(coro->getStatus() == CORO_AVAIL)
    {
        CoroutineInfo::CoroutineDel(coro);
        coro->setStatus(CORO_TIMEOUT);
        CoroutineInfo::CoroutineAddTail(coro, &_timeout);
    }
    else
    {
        TLOGERROR("[TARS][CoroutineScheduler::moveToTimeout ERROR|iCoroId:" << coro->getUid() << "|tyep:" << coro->getStatus() << endl);
    }
}

void CoroutineScheduler::moveToFreeList(CoroutineInfo *coro)
{
    if(coro->getStatus() == CORO_ACTIVE)
    {
        CoroutineInfo::CoroutineDel(coro);
        coro->setStatus(CORO_FREE);
        CoroutineInfo::CoroutineAddTail(coro, &_free);
    }
    else if(coro->getStatus() == CORO_AVAIL)
    {
        CoroutineInfo::CoroutineDel(coro);
        coro->setStatus(CORO_FREE);
        CoroutineInfo::CoroutineAddTail(coro, &_free);
    }
    else if(coro->getStatus() == CORO_INACTIVE)
    {
        CoroutineInfo::CoroutineDel(coro);
        coro->setStatus(CORO_FREE);
        CoroutineInfo::CoroutineAddTail(coro, &_free);
    }
    else if(coro->getStatus() == CORO_TIMEOUT)
    {
        CoroutineInfo::CoroutineDel(coro);
        coro->setStatus(CORO_FREE);
        CoroutineInfo::CoroutineAddTail(coro, &_free);
    }
    else
    {
        TLOGERROR("[TARS][CoroutineScheduler::moveToFreeList ERROR: already free|iCoroId:" << coro->getUid() << "|tyep:" << coro->getStatus() << endl);
    }
}

void CoroutineScheduler::destroy()
{
    if(_all_coro)
    {
        for(size_t i = 1; i <= _poolSize; i++)
        {
            if(_all_coro[i])
            {
                _alloc.deallocate(_all_coro[i]->getStackContext());
            }
        }
        delete [] _all_coro;
    }
}
/////////////////////////////////////////////////////////
Coroutine::Coroutine()
: _coroSched(NULL)
, _num(1)
, _maxNum(128)
, _stackSize(128*1024)
{
}

Coroutine::~Coroutine()
{
    if(isAlive())
    {
        terminate();

        getThreadControl().join();
    }
}

void Coroutine::setCoroInfo(uint32_t iNum, uint32_t iMaxNum, size_t iStackSize)
{
    _maxNum = (iMaxNum > 0 ? iMaxNum : 1);
    _num = (iNum > 0 ? (iNum <= _maxNum ? iNum : _maxNum) : 1);
    _stackSize = (iStackSize >= pagesize() ? iStackSize : pagesize());
}

void Coroutine::run()
{
    initialize();

    handleCoro();

    destroy();
}

void Coroutine::terminate()
{
    if(_coroSched)
    {
        _coroSched->terminate();
    }
}

void Coroutine::handleCoro()
{
    _coroSched = new CoroutineScheduler();

    _coroSched->init(_maxNum, _stackSize);

    ServantProxyThreadData * pSptd = ServantProxyThreadData::getData();

    assert(pSptd != NULL);

    pSptd->_sched = _coroSched;

    for(uint32_t i = 0; i < _num; ++i)
    {
        _coroSched->createCoroutine(std::bind(&Coroutine::coroEntry, this));
    }

    _coroSched->run();

    delete _coroSched;
    _coroSched = NULL;
}

void Coroutine::coroEntry(Coroutine *pCoro)
{
    try
    {
        pCoro->handle();
    }
    catch(exception &ex)
    {
        TLOGERROR("[TARS][[Coroutine::coroEntry exception:" << ex.what() << "]" << endl);
    }
    catch(...)
    {
        TLOGERROR("[TARS][[Coroutine::coroEntry unknown exception]" << endl);
    }
}

uint32_t Coroutine::createCoroutine(const std::function<void ()> &coroFunc)
{
    if(_coroSched)
    {
        return _coroSched->createCoroutine(coroFunc);
    }
    else
    {
        TLOGERROR("[TARS][[Coroutine::createCoroutine coro sched no init]" << endl);
    }
    return -1;
}

void Coroutine::yield()
{
    if(_coroSched)
    {
        _coroSched->yield();
    }
    else
    {
        throw CoroutineException("[TARS][[Coroutine::yield coro sched no init]");
    }
}

void Coroutine::Sleep(int iSleepTime)
{
    if(_coroSched)
    {
        _coroSched->sleep(iSleepTime);
    }
    else
    {
        throw CoroutineException("[TARS][[Coroutine::yield coro sched no init]");
    }
}

}
