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

#ifndef  _TC_LOOP_QUEUE_H_
#define  _TC_LOOP_QUEUE_H_

#include <vector>
#include <stdlib.h>
#include <string.h>

using namespace std;

namespace tars
{
/////////////////////////////////////////////////
/**
 * @file tc_loop_queue.h 
 * @brief 循环队列,大小固定 . 
 *  
 */
/////////////////////////////////////////////////

template<typename T, int Size=5>
class TC_LoopQueue
{
public:
    typedef vector<T> queue_type;

    TC_LoopQueue(uint32_t iSize=Size)
    {
        //做个保护 最多不能超过 1000000
        assert(iSize<1000000);
        _iBegin = 0;
        _iEnd = 0;
        _iCapacitySub = iSize;
        _iCapacity = iSize + 1;
        _p=(T*)malloc(_iCapacity*sizeof(T));
        //_p= new T[_iCapacity];
    }
    ~TC_LoopQueue()
    {
        free(_p);
        //delete _p;
    }

    bool push_back(const T &t,bool & bEmpty,uint32_t & iBegin,uint32_t & iEnd)
    {
        bEmpty = false;
        //uint32_t iEnd = _iEnd;
        iEnd = _iEnd;
        iBegin = _iBegin;
        if((iEnd > _iBegin && iEnd - _iBegin < 2) ||
                ( _iBegin > iEnd && _iBegin - iEnd > (_iCapacity-2) ) )
        {
            return false;
        }
        else
        { 
            memcpy(_p+_iBegin,&t,sizeof(T));
            //*(_p+_iBegin) = t;

            if(_iEnd == _iBegin)
                bEmpty = true;

            if(_iBegin == _iCapacitySub)
                _iBegin = 0;
            else
                _iBegin++;

            if(!bEmpty && 1 == size())
                bEmpty = true;

            return true;
        }
    }

    bool push_back(const T &t,bool & bEmpty)
    {
        bEmpty = false;
        uint32_t iEnd = _iEnd;
        if((iEnd > _iBegin && iEnd - _iBegin < 2) ||
                ( _iBegin > iEnd && _iBegin - iEnd > (_iCapacity-2) ) )
        {
            return false;
        }
        else
        { 
            memcpy(_p+_iBegin,&t,sizeof(T));
            //*(_p+_iBegin) = t;

            if(_iEnd == _iBegin)
                bEmpty = true;

            if(_iBegin == _iCapacitySub)
                _iBegin = 0;
            else
                _iBegin++;

            if(!bEmpty && 1 == size())
                bEmpty = true;
#if 0
            if(1 == size())
                bEmpty = true;
#endif

            return true;
        }
    }

    bool push_back(const T &t)
    {
        bool bEmpty;
        return push_back(t,bEmpty);
    }

    bool push_back(const queue_type &vt)
    {
        uint32_t iEnd=_iEnd;
        if(vt.size()>(_iCapacity-1) ||
                (iEnd>_iBegin && (iEnd-_iBegin)<(vt.size()+1)) ||
                ( _iBegin>iEnd && (_iBegin-iEnd)>(_iCapacity-vt.size()-1) ) )
        {
            return false;
        }
        else
        { 
            for(uint32_t i=0;i<vt.size();i++)
            {
                memcpy(_p+_iBegin,&vt[i],sizeof(T));
                //*(_p+_iBegin) = vt[i];
                if(_iBegin == _iCapacitySub)
                    _iBegin = 0;
                else
                    _iBegin++;
            }
            return true;
        }
    }

    bool pop_front(T &t)
    {
        if(_iEnd==_iBegin)
        {
            return false;
        }
        memcpy(&t,_p+_iEnd,sizeof(T));
        //t = *(_p+_iEnd);

        if(_iEnd == _iCapacitySub)
            _iEnd = 0;
        else
            _iEnd++;
        return true;
    }

    bool pop_front()
    {
        if(_iEnd==_iBegin)
        {
            return false;
        }
        if(_iEnd == _iCapacitySub)
            _iEnd = 0;
        else
            _iEnd++;
        return true;
    }

    bool get_front(T &t)
    {
        if(_iEnd==_iBegin)
        {
            return false;
        }
        memcpy(&t,_p+_iEnd,sizeof(T));
        //t = *(_p+_iEnd);
        return true;
    }

    bool empty()
    {
        if(_iEnd == _iBegin)
        {
            return true;
        }
        return false;
    }

    uint32_t size()
    {
        uint32_t iBegin=_iBegin;
        uint32_t iEnd=_iEnd;
        if(iBegin<iEnd)
            return iBegin+_iCapacity-iEnd;
        return iBegin-iEnd;
    }

    uint32_t getCapacity()
    {
        return _iCapacity;
    }

private:
    T * _p;
    uint32_t _iCapacity;
    uint32_t _iCapacitySub;
    uint32_t _iBegin;
    uint32_t _iEnd;
};

}

#endif

