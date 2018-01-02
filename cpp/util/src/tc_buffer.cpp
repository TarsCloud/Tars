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

#include "util/tc_buffer.h"
#include <iostream>
#include <algorithm>
#include <limits>
#include <cassert>

inline static std::size_t RoundUp2Power(std::size_t size)
{
    if (size == 0)
        return 0;

    std::size_t roundUp = 1;
    while (roundUp < size)
        roundUp *= 2;

    return roundUp;
}

namespace tars
{

const std::size_t TC_Buffer::kMaxBufferSize = std::numeric_limits<std::size_t>::max() / 2;
const std::size_t TC_Buffer::kDefaultSize = 128;

std::size_t TC_Buffer::PushData(const void* data, std::size_t size)
{
    if (!data || size == 0)
        return 0;

    if (ReadableSize() + size >= kMaxBufferSize)
        return 0; // overflow

    AssureSpace(size);
    ::memcpy(&_buffer[_writePos], data, size);
    Produce(size);

    return size;
}

std::size_t TC_Buffer::PopData(void* buf, std::size_t size)
{
    const std::size_t dataSize = ReadableSize();
    if (!buf ||
         size == 0 ||
         dataSize == 0)
        return 0;

    if (size > dataSize)
        size = dataSize; // truncate

    ::memcpy(buf, &_buffer[_readPos], size);
    Consume(size);

    return size;
}

void TC_Buffer::PeekData(void*& buf, std::size_t& size)
{
    buf = ReadAddr();
    size = ReadableSize();
}
    
void TC_Buffer::Consume(std::size_t bytes)
{
    assert (_readPos + bytes <= _writePos);

    _readPos += bytes;
    if (IsEmpty())
        Clear();
}


void TC_Buffer::AssureSpace(std::size_t needsize)
{
    if (WritableSize() >= needsize)
        return;

    const size_t dataSize = ReadableSize();
    const size_t oldCap = _capacity;

    while (WritableSize() + _readPos < needsize)
    {
        if (_capacity < kDefaultSize)
        {
            _capacity = kDefaultSize;
        }
        else if (_capacity <= kMaxBufferSize)
        {
            const size_t newCapcity = RoundUp2Power(_capacity);
            if (_capacity < newCapcity)
                _capacity = newCapcity;
            else
                _capacity = 2 * newCapcity;
        }
        else 
        {
            assert(false);
        }
    }

    if (oldCap < _capacity)
    {
        char* tmp(new char[_capacity]);

        if (dataSize != 0)
            memcpy(&tmp[0], &_buffer[_readPos], dataSize);

        ResetBuffer(tmp);
    }
    else
    {
        assert (_readPos > 0);
        ::memmove(&_buffer[0], &_buffer[_readPos], dataSize);
    }

    _readPos = 0;
    _writePos = dataSize;

    assert (needsize <= WritableSize());
}


void TC_Buffer::Shrink()
{
    if (IsEmpty())
    {
        Clear();
        _capacity = 0;
        ResetBuffer();
        return;
    }

    if (_capacity <= kDefaultSize)
        return;

    std::size_t oldCap = _capacity;
    std::size_t dataSize = ReadableSize();
    if (dataSize * 100 > oldCap * _highWaterPercent)
        return;

    std::size_t newCap = RoundUp2Power(dataSize);

    char* tmp(new char[newCap]);
    memcpy(&tmp[0], &_buffer[_readPos], dataSize);
    ResetBuffer(tmp);
    _capacity = newCap;

    _readPos  = 0;
    _writePos = dataSize;
}

void TC_Buffer::Clear()
{
    _readPos = _writePos = 0; 
}

void TC_Buffer::Swap(TC_Buffer& buf)
{
    std::swap(_readPos, buf._readPos);
    std::swap(_writePos, buf._writePos);
    std::swap(_capacity, buf._capacity);
    std::swap(_buffer, buf._buffer);
}

void TC_Buffer::ResetBuffer(void* ptr)
{
    delete[] _buffer;
    _buffer = reinterpret_cast<char*>(ptr);
}
    
void TC_Buffer::SetHighWaterPercent(size_t percents)
{
    if (percents < 10 || percents >= 100)
        return;

    _highWaterPercent = percents;
}
    
} // end namespace tars

