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

#ifndef __TC_TIMEOUT_QUEUE_MAP_H
#define __TC_TIMEOUT_QUEUE_MAP_H

#include <iostream>
#include <cassert>
#include "util/tc_timeprovider.h"

using namespace std;

namespace tars
{
/////////////////////////////////////////////////
/**
 * @file tc_timeout_queue_map.h
 * @brief 超时队列.
 *
 */
/////////////////////////////////////////////////

//超时时间精确单位是100ms
#define MAX_TIME_OUT    60000

template<class T>
class TC_TimeoutQueueMap
{
public:

    struct PtrInfo;
    struct NodeInfo;
    struct SendInfo;

    struct NodeInfo
    {
        NodeInfo()
        :id(0)
        ,free(true)
        ,noSendNext(0)
        ,noSendPrev(0)
        ,noSendPrevEnd(false)
        ,timeoutNext(0)
        ,timeoutPrev(0)
        ,timeoutPrevEnd(false)
        ,freeNext(0)
        {
        }
        //request id
        uint16_t    id;

        //是否空闲
        bool        free;

        bool        hasSend;

        //未发送链表下一个id
        uint16_t    noSendNext; 
        //未发送链表前一个id
        uint16_t    noSendPrev;
        //如果为false noSendPrev的值是requestid
        //如果为true noSendPrev的值是
        bool        noSendPrevEnd;

        //超时链表的下一个id
        uint16_t    timeoutNext;
        //超时链表的前一个id
        uint16_t    timeoutPrev;
        //如果为false timeoutPrev的值是requestid
        //如果为true timeoutPrev的值是timeout数组的id
        bool        timeoutPrevEnd;

        //空闲链表的下一个id
        uint16_t    freeNext;

        T           ptr;
    };

    /**
     * @brief 超时队列，缺省5s超时.
     *
     * @param timeout 超时设定时间
     * @param size
     */
    TC_TimeoutQueueMap()
    :_noSendHead(0)
    ,_noSendTail(0)
    ,_noSendSize(0)
    ,_freeHead(1)
    ,_timeoutPtr(0)
    ,_timeoutPtrTime(0)
    {
        _dataSize = 51200;
        _data = NULL;
        _data = new NodeInfo[_dataSize];
        assert(NULL != _data);

        for(uint16_t i = 1; i < _dataSize; ++i)
        {
            _data[i].id = i;
            _data[i].free = true;
            _data[i].noSendNext = 0; 
            _data[i].noSendPrev = 0;
            _data[i].timeoutNext = 0;
            _data[i].timeoutPrev = 0;
            _data[i].freeNext = i+1;
        }
        //最后free链表的next要改成0
        _data[_dataSize-1].freeNext = 0;
        _freeHead = 1;
        _freeTail = _dataSize-1;

        _timeoutSize = MAX_TIME_OUT >> 5;
        _timeoutHead = NULL;
        _timeoutHead = new uint16_t[_timeoutSize];

        for(uint16_t i = 0; i < _timeoutSize; ++i)
        {
            _timeoutHead[i] = 0;
        }

        _timeoutPtr = 0;
        _timeoutPtrTime = 0;
    }

    /**
     * @brief  产生该队列的下一个ID
     */
    uint32_t generateId();

    /**
     * 要发送的链表是否为空
     */
    bool sendListEmpty()
    {
        return (0 == _noSendHead);
    }

    /**
     * 获取要发送的数据
     */
    bool getSend(T & t);

    /**
     * 把已经发送的数据从list里面删除
     */
    void popSend(bool del = false);

    /**
     *获取要发送list的size
     */
    size_t getSendListSize()
    {
        return _noSendSize;
    }

    /**
     * @brief 获取指定id的数据.
     *
     * @param id 指定的数据的id
     * @param T 指定id的数据
     * @return bool get的结果
     */
    bool get(uint16_t uniqId, T & t,bool bErase = true);

    /**
     * @brief 删除.
     *
     * @param uniqId 要删除的数据的id
     * @param T     被删除的数据
     * @return bool 删除结果
     */
    bool erase(uint16_t uniqId, T & t);

    /**
     * @brief 设置消息到队列尾端.
     *
     * @param ptr        要插入到队列尾端的消息
     * @param uniqId     序列号
     * @param timeout    超时时间
     * @return true  成功 false 失败
     */
    uint16_t push(T& ptr, uint16_t id, int64_t timeout,bool hasSend = true);

    /**
     * @brief 超时删除数据
     */
    bool timeout(T & t);

    /**
     * @brief 队列中的数据.
     *
     * @return size_t
     */
    size_t size() const { return _dataSize; }
private:
    void delFromTimeout(uint16_t id);
    void delFromData(uint16_t id);
    void delFromNoSend(uint16_t id);

protected:
    uint32_t                        _uniqId;

    uint16_t    _dataSize;

    //未发送列表的head
    uint16_t    _noSendHead; 
    uint16_t    _noSendTail;

    uint16_t    _noSendSize;

    uint16_t *  _timeoutHead;
    uint16_t    _timeoutSize;

    uint16_t    _freeHead;
    uint16_t    _freeTail;

    uint16_t    _timeoutPtr;
    int64_t     _timeoutPtrTime;

    NodeInfo *  _data;
};

template<typename T> bool TC_TimeoutQueueMap<T>::getSend(T & t)
{
    //链表为空返回失败
    if(0 == _noSendHead)
    {
        return false;
    }

    assert(!_data[_noSendHead].hasSend);
    assert(!_data[_noSendHead].free);
    t = _data[_noSendHead].ptr;
    return true;
}


template<typename T> void TC_TimeoutQueueMap<T>::popSend(bool del)
{
    assert(0 != _noSendHead);
    assert(!_data[_noSendHead].hasSend);
    assert(!_data[_noSendHead].free);

    uint16_t popId = _noSendHead;

    //置成已经发送
    _data[popId].hasSend = true;

    //从nosend 里面删除
    delFromNoSend(popId);

    if(del)
    {
        delFromTimeout(popId);
        delFromData(popId);
    }
}

template<typename T> bool TC_TimeoutQueueMap<T>::get(uint16_t id, T & t, bool bErase)
{
    assert(id < _dataSize);
    if(id >= _dataSize)
    {
        return false;
    }

    if(_data[id].free)
    {
        return false;
    }

    t = _data[id].ptr;

    if(bErase)
    {
        delFromTimeout(id);
        delFromData(id);
    }

    return true;
}

template<typename T> uint32_t TC_TimeoutQueueMap<T>::generateId()
{
    if(0 == _freeHead)
    {
        return 0;
    }
    assert(0 != _freeTail);

    //从free里面找一个
    uint16_t id = _freeHead;
    NodeInfo & node = _data[_freeHead];
    assert(node.free);
    node.free = false;

    //修改free链表
    _freeHead = node.freeNext;
    if(0 == _freeHead)
    {
        _freeTail = 0;
    }
    //cerr<<"generateId:"<<id<<endl;
    return id;
}

template<typename T> uint16_t TC_TimeoutQueueMap<T>::push(T& ptr, uint16_t id, int64_t timeout, bool hasSend)
{
    //cerr<<"push:"<<id<<endl;
    assert(id < _dataSize);
    if(id >= _dataSize)
    {
        return false;
    }

    if(_data[id].free)
    {
        return false;
    }

    //时间链表 初始化
    int64_t timeoutSelf = ( timeout >> 6);
    if(0 == _timeoutPtrTime)
    {
        assert(0 == _timeoutPtr);
        _timeoutPtrTime = (TNOWMS >> 6);
    }

    if(timeoutSelf <= _timeoutPtrTime)
    {
        assert(false);
        return 0;
    }

    //检查超时时间是否合法
    if((timeoutSelf - _timeoutPtrTime) >= (int64_t)_timeoutSize)
    {
        assert(false);
        return 0;
    }

    //从free里面找一个
#if 0
    uint16_t id = _freeHead;
    NodeInfo & node = _data[_freeHead];
    assert(node.free);
    node.ptr = ptr;
    node.hasSend = hasSend;
    node.free = false;

    //修改free链表
    _freeHead = node.freeNext;
    if(0 == _freeHead)
    {
        _freeTail = 0;
    }
#endif
    NodeInfo & node = _data[id];
    node.ptr = ptr;
    node.hasSend = hasSend;

    //加入到时间链表里面
    uint16_t timeoutPtr;
    timeoutPtr = (uint16_t)(timeoutSelf - _timeoutPtrTime) + _timeoutPtr;
    timeoutPtr = timeoutPtr % _timeoutSize;

    if(0 == _timeoutHead[timeoutPtr])
    {
        node.timeoutNext = 0;
    }
    else
    {
        uint16_t nextId = _timeoutHead[timeoutPtr];
        assert(_data[nextId].timeoutPrevEnd);
        _data[nextId].timeoutPrev = id;
        _data[nextId].timeoutPrevEnd = false;

        _data[id].timeoutNext = nextId;
    }
    node.timeoutPrevEnd = true;
    node.timeoutPrev = timeoutPtr;
    _timeoutHead[timeoutPtr] = id;
    
    //没有发送放到list队列里面
    if(!hasSend)
    {
        _noSendSize ++;
        if(0 == _noSendTail)
        {
            assert(0 == _noSendHead);
            _noSendHead = id;
            _data[id].noSendPrev = 0;
        }
        else
        {
            assert(0 != _noSendHead);
            _data[_noSendTail].noSendNext = id;
            _data[id].noSendPrev = _noSendTail;
        }
        _data[id].noSendNext = 0;
        _noSendTail = id;
    }

    return id;
}

template<typename T> bool TC_TimeoutQueueMap<T>::timeout(T & t)
{
    if(0 == _timeoutPtrTime)
    {
        return false;
    }

    int64_t nowSelf = TNOWMS >> 6;

    while(true)
    {
        if(_timeoutPtrTime >= nowSelf)
        {
            return false;
        }
        if(0 == _timeoutHead[_timeoutPtr])
        {
            _timeoutPtr++;
            _timeoutPtrTime ++;
            if(_timeoutSize == _timeoutPtr)
            {
                _timeoutPtr = 0;
            }
        }
        else
        {
            uint16_t id = _timeoutHead[_timeoutPtr];
            NodeInfo & node = _data[id];
            t = node.ptr;
            if(!node.hasSend)
            {
                delFromNoSend(id);
            }
            delFromTimeout(id);
            delFromData(id);
            return true;
        }
    }
    return true;
}

template<typename T> bool TC_TimeoutQueueMap<T>::erase(uint16_t id, T & t)
{
    //cerr<<"line:"<<__LINE__<<" erase:"<<id<<endl;
    assert(id < _dataSize);
    if(id >= _dataSize)
    {
        return false;
    }

    if(_data[id].free)
    {
        return false;
    }

    t = _data[id].ptr;
    if(!_data[id].hasSend)
    {
        delFromNoSend(id);
    }
    delFromTimeout(id);
    delFromData(id);

    return true;
}

template<typename T> void TC_TimeoutQueueMap<T>::delFromTimeout(uint16_t delId)
{
    //从超时里面删除
    if(_data[delId].timeoutPrevEnd)
    {
        //cerr<<"line:"<<__LINE__<<endl;
        uint16_t nextId;
        uint16_t timeoutId;
        timeoutId = _data[delId].timeoutPrev;
        nextId = _data[delId].timeoutNext;

        _timeoutHead[timeoutId] = nextId;
        if(0 != nextId)
        {
            _data[nextId].timeoutPrevEnd = true;
            _data[nextId].timeoutPrev = timeoutId;
        }
    }
    else
    {
        //cerr<<"line:"<<__LINE__<<endl;
        uint16_t prevId,nextId;
        prevId = _data[delId].timeoutPrev;
        nextId = _data[delId].timeoutNext;

        assert(0 != prevId);
        _data[prevId].timeoutNext = nextId;
        if(0 != nextId)
        {
            _data[nextId].timeoutPrev = prevId;
        }
    }
}

template<typename T> void TC_TimeoutQueueMap<T>::delFromData(uint16_t id)
{
    //放了链表最后
    _data[id].freeNext = 0;
    if(0 == _freeTail)
    {
        assert(0 == _freeHead);
        _freeHead = id;
    }
    else
    {
        assert(0 != _freeHead);
        _data[_freeTail].freeNext = id;
    }
    _freeTail = id;
    _data[id].free = true;
}

template<typename T> void TC_TimeoutQueueMap<T>::delFromNoSend(uint16_t id)
{
    uint16_t prevId,nextId;
    prevId = _data[id].noSendPrev;
    nextId = _data[id].noSendNext;

    assert(_noSendSize>0);
    _noSendSize --;

    if(0 == prevId)
    {
        _noSendHead = nextId;
    }
    else
    {
        _data[prevId].noSendNext = nextId;
    }

    if(0 == nextId)
    {
        _noSendTail = 0;
    }
    else
    {
        _data[nextId].noSendPrev = prevId;
    }
}
/////////////////////////////////////////////////////////////////
}
#endif
