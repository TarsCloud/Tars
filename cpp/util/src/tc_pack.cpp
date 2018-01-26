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

#include "util/tc_pack.h"
#include <iostream>
#include <string.h>

namespace tars
{

TC_PackIn::TC_PackInInner& TC_PackIn::TC_PackInInner::operator << (bool t)
{
    _buffer.append(sizeof(bool), (char)t);
    return *this;
}

TC_PackIn::TC_PackInInner& TC_PackIn::TC_PackInInner::operator << (char t)
{
    _buffer.append(sizeof(char), (char)t);
    return *this;
}

TC_PackIn::TC_PackInInner& TC_PackIn::TC_PackInInner::operator << (unsigned char t)
{
    (*this) << (char)t;
    return *this;
}

TC_PackIn::TC_PackInInner& TC_PackIn::TC_PackInInner::operator << (short t)
{
    t = htons(t);

    _buffer.append((const char *)&t, sizeof(t));
    return *this;
}

TC_PackIn::TC_PackInInner& TC_PackIn::TC_PackInInner::operator << (unsigned short t)
{
    (*this) << (short)t;
    return *this;
}

TC_PackIn::TC_PackInInner& TC_PackIn::TC_PackInInner::operator << (int t)
{
    t = htonl(t);

    _buffer.append((const char *)&t, sizeof(int));
    return *this;
}

TC_PackIn::TC_PackInInner& TC_PackIn::TC_PackInInner::operator << (unsigned int t)
{
    (*this) << (int)t;
    return *this;
}

TC_PackIn::TC_PackInInner& TC_PackIn::TC_PackInInner::operator << (long t)
{
    if(sizeof(long) == 8)
    {
        #if __BYTE_ORDER == __LITTLE_ENDIAN
        t = __bswap_64(t);
        #endif
    }
    else
    {
        t = htonl(t);
    }

    _buffer.append((const char *)&t, sizeof(long));
    return *this;
}

TC_PackIn::TC_PackInInner& TC_PackIn::TC_PackInInner::operator << (unsigned long t)
{
    if(sizeof(unsigned long) == 8)
    {
        #if __BYTE_ORDER == __LITTLE_ENDIAN
        t = __bswap_64(t);
        #endif
    }
    else
    {
        t = htonl(t);
    }

    _buffer.append((const char *)&t, sizeof(unsigned long));
    return *this;
}

TC_PackIn::TC_PackInInner& TC_PackIn::TC_PackInInner::operator << (long long t)
{
    #if __BYTE_ORDER == __LITTLE_ENDIAN
    t = __bswap_64(t);
    #endif

    _buffer.append((const char *)&t, sizeof(long long));
    return *this;
}

TC_PackIn::TC_PackInInner& TC_PackIn::TC_PackInInner::operator << (unsigned long long t)
{
    #if __BYTE_ORDER == __LITTLE_ENDIAN
    t = __bswap_64(t);
    #endif

    _buffer.append((const char *)&t, sizeof(unsigned long long));

    return *this;
}

TC_PackIn::TC_PackInInner& TC_PackIn::TC_PackInInner::operator << (float t)
{
    _buffer.append((const char *)&t, sizeof(float));
    return *this;
}

TC_PackIn::TC_PackInInner& TC_PackIn::TC_PackInInner::operator << (double t)
{
    _buffer.append((const char *)&t, sizeof(double));
    return *this;
}

TC_PackIn::TC_PackInInner& TC_PackIn::TC_PackInInner::operator << (const char *sBuffer)
{
    _buffer.append(sBuffer, strlen(sBuffer) + 1);
    return *this;
}

TC_PackIn::TC_PackInInner& TC_PackIn::TC_PackInInner::operator << (const string& sBuffer)
{
    uint32_t len = (uint32_t)sBuffer.length();
    if(len < 255)
    {
        unsigned char c = (unsigned char)len;
        (*this) << c;
    }
    else
    {
        unsigned char c = 255;
        (*this) << c;
        (*this) << len;
    }
    _buffer.append(sBuffer);
    return *this;
}

TC_PackIn::TC_PackInInner& TC_PackIn::TC_PackInInner::operator << (const TC_PackIn& pi)
{
    _buffer.append(pi.topacket());
    return (*this);
}

/************************************************************************/

bool TC_PackOut::isEnd()
{
    if(_pos >= _length)
    {
        return true;
    }
    return false;
}

TC_PackOut& TC_PackOut::operator >> (bool &t)
{
    size_t len = sizeof(bool);
    if(_pos + len > _length)
    {
        throw TC_PackOut_Exception("TC_PackOut read bool error.");
    }

    memcpy(&t, _pbuffer + _pos, len);
    _pos += len;

    return *this;
}

TC_PackOut& TC_PackOut::operator >> (char &t)
{
    size_t len = sizeof(char);
    if(_pos + len > _length)
    {
        throw TC_PackOut_Exception("TC_PackOut read char error.");
    }

    memcpy(&t, _pbuffer + _pos, len);
    _pos += len;

    return *this;
}

TC_PackOut& TC_PackOut::operator >> (unsigned char &t)
{
    size_t len = sizeof(unsigned char);
    if(_pos + len > _length)
    {
        throw TC_PackOut_Exception("TC_PackOut read unsigned char error.");
    }

    memcpy(&t, _pbuffer + _pos, len);
    _pos += len;

    return *this;
}

TC_PackOut& TC_PackOut::operator >> (short &t)
{
    size_t len = sizeof(short);
    if(_pos + len > _length)
    {
        throw TC_PackOut_Exception("TC_PackOut read short error.");
    }

    memcpy(&t, _pbuffer + _pos, len);

    t = ntohs(t);

    _pos += len;

    return *this;
}

TC_PackOut& TC_PackOut::operator >> (unsigned short &t)
{
    size_t len = sizeof(unsigned short);
    if(_pos + len > _length)
    {
        throw TC_PackOut_Exception("TC_PackOut read unsigned short error.");
    }

    memcpy(&t, _pbuffer + _pos, len);

    t = ntohs(t);

    _pos += len;

    return *this;
}

TC_PackOut& TC_PackOut::operator >> (int &t)
{
    size_t len = sizeof(int);
    if(_pos + len > _length)
    {
        throw TC_PackOut_Exception("TC_PackOut read int error.");
    }

    memcpy(&t, _pbuffer + _pos, len);

    t = ntohl(t);

    _pos += len;

    return *this;
}

TC_PackOut& TC_PackOut::operator >> (unsigned int &t)
{
    size_t len = sizeof(unsigned int);
    if(_pos + len > _length)
    {
        throw TC_PackOut_Exception("TC_PackOut read unsigned int error.");
    }

    memcpy(&t, _pbuffer + _pos, len);

    t = ntohl(t);

    _pos += len;

    return *this;
}

TC_PackOut& TC_PackOut::operator >> (long &t)
{
    size_t len = sizeof(long);
    if(_pos + len > _length)
    {
        throw TC_PackOut_Exception("TC_PackOut read long error.");
    }

    memcpy(&t, _pbuffer + _pos, len);

    if(sizeof(unsigned long) == 8)
    {
        #if __BYTE_ORDER == __LITTLE_ENDIAN
        t = __bswap_64(t);
        #endif
    }
    else
    {
        t = ntohl(t);
    }

    _pos += len;

    return *this;
}

TC_PackOut& TC_PackOut::operator >> (unsigned long &t)
{
    size_t len = sizeof(unsigned long);
    if(_pos + len > _length)
    {
        throw TC_PackOut_Exception("TC_PackOut read long error.");
    }

    memcpy(&t, _pbuffer + _pos, len);

    if(sizeof(unsigned long) == 8)
    {
        #if __BYTE_ORDER == __LITTLE_ENDIAN
        t = __bswap_64(t);
        #endif
    }
    else
    {
        t = ntohl(t);
    }

    _pos += len;

    return *this;
}

TC_PackOut& TC_PackOut::operator >> (long long &t)
{
    size_t len = sizeof(long long);
    if(_pos + len > _length)
    {
        throw TC_PackOut_Exception("TC_PackOut read long long error.");
    }

    memcpy(&t, _pbuffer + _pos, len);

    #if __BYTE_ORDER == __LITTLE_ENDIAN
    t = __bswap_64(t);
    #endif

    _pos += len;

    return *this;
}

TC_PackOut& TC_PackOut::operator >> (unsigned long long &t)
{
    size_t len = sizeof(unsigned long long);
    if(_pos + len > _length)
    {
        throw TC_PackOut_Exception("TC_PackOut read long long error.");
    }

    memcpy(&t, _pbuffer + _pos, len);

    #if __BYTE_ORDER == __LITTLE_ENDIAN
    t = __bswap_64(t);
    #endif

    _pos += len;

    return *this;
}

TC_PackOut& TC_PackOut::operator >> (float &f)
{
    if(_pos + sizeof(float) > _length)
    {
        throw TC_PackOut_Exception("TC_PackOut read float error.");
    }

    memcpy(&f, _pbuffer + _pos, sizeof(float));
    _pos += sizeof(float);

    return *this;
}

TC_PackOut& TC_PackOut::operator >> (double &f)
{
    if(_pos + sizeof(double) > _length)
    {
        throw TC_PackOut_Exception("TC_PackOut read double error.");
    }

    memcpy(&f, _pbuffer + _pos, sizeof(double));
    _pos += sizeof(double);

    return *this;
}

TC_PackOut& TC_PackOut::operator >> (char *sBuffer)
{
    strcpy(sBuffer, _pbuffer + _pos);
    _pos += strlen(sBuffer) + 1;

    if(_pos > _length)
    {
        throw TC_PackOut_Exception("TC_PackOut read zero string error.");
    }
    return *this;
}

TC_PackOut& TC_PackOut::operator >> (string& sBuffer)
{
    uint32_t len = 0;
    unsigned char c;
    (*this) >> c;
    if(c == 255)
    {
        (*this) >> len;
    }
    else
    {
        len = c;
    }

    if(_pos + len > _length)
    {
        throw TC_PackOut_Exception("TC_PackOut read string error.");
    }

    sBuffer.assign((const char*)(_pbuffer + _pos), len);

    _pos += len;

    return *this;
}

}

