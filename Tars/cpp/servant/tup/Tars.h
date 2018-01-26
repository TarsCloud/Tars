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

#ifndef __TARS_H__
#define __TARS_H__

#include <netinet/in.h>
#include <iostream>
#include <cassert>
#include <vector>
#include <map>
#include <string>
#include <stdexcept>
#include <stdint.h>
#include <string.h>
#include <limits.h>
#include <stdio.h>



//支持iphone
#ifdef __APPLE__
    #include "TarsType.h"
#elif defined ANDROID  // android
    #include "TarsType.h"
#else
    #include "tup/TarsType.h"
#endif


#ifndef tars_likely
#if defined(__GNUC__) && __GNUC__ >= 4
#define tars_likely(x)            (__builtin_expect(!!(x),1))
#else
#define tars_likely(x)   (x)
#endif
#endif

#ifndef tars_unlikely
#if defined(__GNUC__) && __GNUC__ >= 4
#define tars_unlikely(x)            (__builtin_expect(!!(x),0))
#else
#define tars_unlikely(x)   (x)
#endif
#endif

//数据头类型
#define TarsHeadeChar  0
#define TarsHeadeShort 1
#define TarsHeadeInt32 2
#define TarsHeadeInt64 3
#define TarsHeadeFloat 4
#define TarsHeadeDouble 5
#define TarsHeadeString1 6
#define TarsHeadeString4 7
#define TarsHeadeMap 8
#define TarsHeadeList 9
#define TarsHeadeStructBegin 10
#define TarsHeadeStructEnd 11
#define TarsHeadeZeroTag 12
#define TarsHeadeSimpleList 13


//////////////////////////////////////////////////////////////////
//// 保留接口版本Tars宏定义
//编码相应的宏
#define TarsReserveBuf(os, len) \
do{ \
    if((os)._reverse) \
    { \
        if(tars_unlikely((os)._buf_len < (len))) \
        { \
            size_t len1 = (len)<<1; \
            char * p = new char[(len1)]; \
            memcpy(p, (os)._buf, (os)._len); \
            delete[] (os)._buf; \
            (os)._buf = p; \
            (os)._buf_len = (len1); \
        } \
    } \
}while(0)

#define TarsWriteToHead(os, type, tag) \
do { \
    if (tars_likely((tag) < 15)) \
    { \
        TarsWriteUInt8TTypeBuf( os, (type) + ((tag)<<4) , (os)._len); \
    } \
    else \
    { \
        TarsWriteUInt8TTypeBuf( os, (type) + (240) , (os)._len); \
        TarsWriteUInt8TTypeBuf( os, (tag),  (os)._len);\
    } \
} while(0)

#define TarsWriteCharTypeBuf(os, val, osLen) \
do { \
    TarsReserveBuf(os, (osLen)+sizeof(Char)); \
    (*(Char *)((os)._buf + (osLen))) = (val); \
    (osLen) += sizeof(Char); \
} while(0)

#define TarsWriteInt32TypeBuf(os, val, osLen) \
do { \
    TarsReserveBuf(os, (osLen)+sizeof(Int32)); \
    (*(Int32 *)((os)._buf + (osLen))) = (val); \
    (osLen) += sizeof(Int32); \
} while(0)

#define TarsWriteInt64TypeBuf(os, val, osLen) \
do { \
    TarsReserveBuf(os, (osLen)+sizeof(Int64)); \
    (*(Int64 *)((os)._buf + (osLen))) = (val); \
    (osLen) += sizeof(Int64); \
} while(0)

#define TarsWriteFloatTypeBuf(os, val, osLen) \
do { \
    TarsReserveBuf(os, (osLen)+sizeof(Float)); \
    (*(Float *)((os)._buf + (osLen))) = (val); \
    (osLen) += sizeof(Float); \
} while(0)

#define TarsWriteDoubleTypeBuf(os, val, osLen) \
do { \
    TarsReserveBuf(os, (osLen)+sizeof(Double)); \
    (*(Double *)((os)._buf + (osLen))) = (val); \
    (osLen) += sizeof(Double); \
} while(0)

#define TarsWriteUInt32TTypeBuf(os, val, osLen) \
do { \
    TarsReserveBuf(os, (osLen)+sizeof(uint32_t)); \
    (*(uint32_t *)((os)._buf + (osLen))) = (val); \
    (osLen) += sizeof(uint32_t); \
} while(0)

#define TarsWriteUInt8TTypeBuf(os, val, osLen) \
do { \
    TarsReserveBuf(os, (osLen)+sizeof(uint8_t)); \
    (*(uint8_t *)((os)._buf + (osLen))) = (val); \
    (osLen) += sizeof(uint8_t); \
} while(0)

#define TarsWriteUIntTypeBuf(os, val, osLen) \
do { \
    TarsReserveBuf(os, (osLen)+sizeof(unsigned int)); \
    (*(unsigned int *)((os)._buf + (osLen))) = (val); \
    (osLen) += sizeof(unsigned int); \
} while(0)

#define TarsWriteShortTypeBuf(os, val, osLen) \
do { \
    TarsReserveBuf(os, (osLen)+sizeof(Short)); \
    (*(Short *)((os)._buf + (osLen))) = (val); \
    (osLen) += sizeof(Short); \
} while(0)

#define TarsWriteTypeBuf(os, buf, len) \
do { \
    TarsReserveBuf(os, (os)._len + (len)); \
    memcpy((os)._buf + (os)._len, (const void *)(buf), (len)); \
    (os)._len += (len); \
} while(0)


//解码相应的宏
#define TarsPeekTypeBuf(is, buf, offset, type) \
do { \
    if(tars_unlikely((is)._cur+(offset)+sizeof(type)>(is)._buf_len) )\
    { \
        char s[64]; \
        snprintf(s, sizeof(s), "buffer overflow when peekBuf, over %u.", (uint32_t)((is)._buf_len)); \
        throw TarsDecodeException(s); \
    } \
    (buf) = (*((type *)((is)._buf+(is)._cur+(offset)))); \
} while(0)

#define TarsPeekTypeBufNoTag(is, offset, type) \
do { \
    if(tars_unlikely((is)._cur+(offset)+sizeof(type)>(is)._buf_len) )\
    { \
        char s[64]; \
        snprintf(s, sizeof(s), "buffer overflow when peekBuf, over %u.", (uint32_t)((is)._buf_len)); \
        throw TarsDecodeException(s); \
    } \
} while(0)

#define TarsReadCharTypeBuf(is, buf) \
do { \
    TarsPeekTypeBuf(is, buf, 0, Char); \
    (is)._cur += sizeof(Char); \
} while(0)

#define TarsReadShortTypeBuf(is, buf) \
do { \
    TarsPeekTypeBuf(is, buf, 0, Short); \
    (is)._cur += sizeof(Short); \
} while(0)

#define TarsReadInt32TypeBuf(is, buf) \
do { \
    TarsPeekTypeBuf(is, buf, 0, Int32); \
    (is)._cur += sizeof(Int32); \
} while(0)

#define TarsReadInt64TypeBuf(is, buf) \
do { \
    TarsPeekTypeBuf(is, buf, 0, Int64); \
    (is)._cur += sizeof(Int64); \
} while(0)

#define TarsReadFloatTypeBuf(is, buf) \
do { \
    TarsPeekTypeBuf(is, buf, 0, Float); \
    (is)._cur += sizeof(Float); \
} while(0)

#define TarsReadDoubleTypeBuf(is, buf) \
do { \
    TarsPeekTypeBuf(is, buf, 0, Double); \
    (is)._cur += sizeof(Double); \
} while(0)

#define TarsReadTypeBuf(is, buf, type) \
do { \
    TarsPeekTypeBuf(is, buf, 0, type); \
    (is)._cur += sizeof(type); \
} while(0)

#define TarsReadHeadSkip(is, len) \
do {\
    (is)._cur += (len); \
} while(0)

#define TarsPeekFromHead(is, type, tag, n) \
do { \
    (n) = 1; \
    uint8_t typeTag, tmpTag; \
    TarsPeekTypeBuf(is, typeTag, 0, uint8_t); \
    tmpTag = typeTag >> 4; \
    (type) = (typeTag & 0x0F); \
    if(tars_unlikely(tmpTag == 15)) \
    { \
        TarsPeekTypeBuf(is, tag, 1, uint8_t); \
        (n) += 1; \
    } \
    else \
    { \
        (tag) = tmpTag; \
    } \
} while(0)

#define readFromHead(is, type ,tag) \
do { \
    size_t n = 0; \
    TarsPeekFromHead(is, type, tag, n); \
    TarsReadHeadSkip(is, n); \
} while(0)

#define TarsPeekFromHeadNoTag(is, type, n) \
do { \
    (n) = 1; \
    uint8_t typeTag, tmpTag; \
    TarsPeekTypeBuf(is, typeTag, 0, uint8_t); \
    tmpTag = typeTag >> 4; \
    (type) = (typeTag & 0x0F); \
    if(tars_unlikely(tmpTag == 15)) \
    { \
        TarsPeekTypeBufNoTag(is, 1, uint8_t); \
        (n) += 1; \
    } \
} while(0)

#define readFromHeadNoTag(is, type) \
do { \
    size_t n = 0; \
    TarsPeekFromHeadNoTag(is, type, n); \
    TarsReadHeadSkip(is, n); \
} while(0)

#define TarsPeekBuf(is ,buf, len, offset) \
do {\
    if (tars_unlikely((is)._cur + (offset) + (len) > (is)._buf_len)) \
    { \
        char s[64]; \
        snprintf(s, sizeof(s), "buffer overflow when peekBuf, over %u.", (uint32_t)((is)._buf_len)); \
        throw TarsDecodeException(s); \
    } \
    ::memcpy(buf, (is)._buf + (is)._cur + (offset), (len)); \
} while(0)

#define TarsReadBuf(is, buf, len) \
do { \
    TarsPeekBuf(is, buf, len, 0); \
    (is)._cur += (len); \
} while(0)

#define TarsReadStringBuf(is, str, len) \
do{\
    if (tars_unlikely((is)._cur + (len) > (is)._buf_len)) \
    { \
        char s[64]; \
        snprintf(s, sizeof(s), "buffer overflow when peekBuf, over %u.", (uint32_t)((is)._buf_len)); \
        throw TarsDecodeException(s); \
    } \
    str.assign((is)._buf + (is)._cur, (is)._buf + (is)._cur + (len)); \
    (is)._cur += len; \
} while (0)

#define TarsSkipToTag(flag, tag, retHeadType, retHeadTag) \
do { \
    try \
    { \
        uint8_t nextHeadType, nextHeadTag; \
        while (!ReaderT::hasEnd()) \
        { \
            size_t len = 0; \
            TarsPeekFromHead(*this, nextHeadType, nextHeadTag, len); \
            if (tars_unlikely(nextHeadType == TarsHeadeStructEnd || tag < nextHeadTag)) \
            { \
                break; \
            } \
            if (tag == nextHeadTag) \
            { \
                (retHeadType) = nextHeadType; \
                (retHeadTag) = nextHeadTag; \
                TarsReadHeadSkip(*this, len); \
                (flag) = true; \
                break; \
            } \
            TarsReadHeadSkip(*this, len); \
            skipField(nextHeadType); \
        } \
    } \
    catch (TarsDecodeException& e) \
    { \
    } \
} while(0)

namespace tars
{
//////////////////////////////////////////////////////////////////
    struct TarsStructBase
    {
    protected:
        TarsStructBase() {}

        ~TarsStructBase() {}
    };

    struct TarsProtoException : public std::runtime_error
    {
        TarsProtoException(const std::string& s) : std::runtime_error(s) {}
    };

    struct TarsEncodeException : public TarsProtoException
    {
        TarsEncodeException(const std::string& s) : TarsProtoException(s) {}
    };

    struct TarsDecodeException : public TarsProtoException
    {
        TarsDecodeException(const std::string& s) : TarsProtoException(s) {}
    };

    struct TarsDecodeMismatch : public TarsDecodeException
    {
        TarsDecodeMismatch(const std::string & s) : TarsDecodeException(s) {}
    };

    struct TarsDecodeRequireNotExist : public TarsDecodeException
    {
        TarsDecodeRequireNotExist(const std::string & s) : TarsDecodeException(s) {}
    };

    struct TarsDecodeInvalidValue : public TarsDecodeException
    {
        TarsDecodeInvalidValue(const std::string & s) : TarsDecodeException(s) {}
    };

    struct TarsNotEnoughBuff : public TarsProtoException
    {
        TarsNotEnoughBuff(const std::string & s) : TarsProtoException(s) {}
    };

//////////////////////////////////////////////////////////////////
    namespace
    {
        /// 数据头信息的封装，包括类型和tag
        class DataHead
        {
            uint8_t _type;
            uint8_t _tag;
        public:
            enum
            {
                eChar = 0,
                eShort = 1,
                eInt32 = 2,
                eInt64 = 3,
                eFloat = 4,
                eDouble = 5,
                eString1 = 6,
                eString4 = 7,
                eMap = 8,
                eList = 9,
                eStructBegin = 10,
                eStructEnd = 11,
                eZeroTag = 12,
                eSimpleList = 13,
            };

            struct helper
            {
                uint8_t     type : 4;
                uint8_t     tag  : 4;
            }__attribute__((packed));

        public:
            DataHead() : _type(0), _tag(0) {}
            DataHead(uint8_t type, uint8_t tag) : _type(type), _tag(tag) {}

            uint8_t getTag() const      { return _tag;}
            void setTag(uint8_t t)      { _tag = t;}
            uint8_t getType() const     { return _type;}
            void setType(uint8_t t)     { _type = t;}

            /// 读取数据头信息
            template<typename InputStreamT>
            void readFrom(InputStreamT& is)
            {
                size_t n = peekFrom(is);
                is.skip(n);
            }

            /// 读取头信息，但不前移流的偏移量
            template<typename InputStreamT>
            size_t peekFrom(InputStreamT& is)
            {
                helper h;
                size_t n = sizeof(h);
                is.peekBuf(&h, sizeof(h));
                _type = h.type;
                if (h.tag == 15)
                {
                    is.peekBuf(&_tag, sizeof(_tag), sizeof(h));
                    n += sizeof(_tag);
                }
                else
                {
                    _tag = h.tag;
                }
                return n;
            }

            /// 写入数据头信息
            template<typename OutputStreamT>
            void writeTo(OutputStreamT& os)
            {
                /*
                helper h;
                h.type = _type;
                if(_tag < 15){
                    h.tag = _tag;
                    os.writeBuf(&h, sizeof(h));
                }else{
                    h.tag = 15;
                    os.writeBuf(&h, sizeof(h));
                    os.writeBuf(&_tag, sizeof(_tag));
                }
                */
                writeTo(os, _type, _tag);
            }

            /// 写入数据头信息
            template<typename OutputStreamT>
            static void writeTo(OutputStreamT& os, uint8_t type, uint8_t tag)
            {
                helper h;
                h.type = type;
                if (tag < 15)
                {
                    h.tag = tag;
                    os.writeBuf(&h, sizeof(h));
                }
                else
                {
                    h.tag = 15;
                    os.writeBuf(&h, sizeof(h));
                    os.writeBuf(&tag, sizeof(tag));
                }
            }
        };
    }


//////////////////////////////////////////////////////////////////
/// 缓冲区读取器封装
    class BufferReader
    {
    public:
        const char *        _buf;        ///< 缓冲区
        size_t              _buf_len;    ///< 缓冲区长度
        size_t              _cur;        ///< 当前位置

    public:

        BufferReader() : _buf(NULL),_buf_len(0),_cur(0) {}

        void reset() { _cur = 0;}

        /// 读取缓存
        void readBuf(void * buf, size_t len)
        {
            if(len <= _buf_len && (_cur + len) <= _buf_len)
            {
                peekBuf(buf, len);
                _cur += len;
            }
            else
            {
                char s[64];
                snprintf(s, sizeof(s), "buffer overflow when skip, over %u.", (uint32_t)_buf_len);
                throw TarsDecodeException(s);
            }
        }

        /// 读取缓存，但不改变偏移量
        void peekBuf(void * buf, size_t len, size_t offset = 0)
        {
            if (_cur + offset + len > _buf_len)
            {
                char s[64];
                snprintf(s, sizeof(s), "buffer overflow when peekBuf, over %u.", (uint32_t)_buf_len);
                throw TarsDecodeException(s);
            }
            ::memcpy(buf, _buf + _cur + offset, len);
        }

        /// 读取缓存 for vector<char>
        template <typename Alloc>
        void readBuf(std::vector<Char, Alloc>& v, size_t len)
        {
            if(len <= _buf_len && (_cur + len) <= _buf_len)
            {
                peekBuf(v, len);
                _cur += len;
            }
            else
            {
                char s[64];
                snprintf(s, sizeof(s), "buffer overflow when skip, over %u.", (uint32_t)_buf_len);
                throw TarsDecodeException(s);
            }
        }

        /// 读取缓存，但不改变偏移量 for vector<char>
        template <typename Alloc>
        void peekBuf(std::vector<Char, Alloc>& v, size_t len, size_t offset = 0)
        {
            if (_cur + offset + len > _buf_len)
            {
                char s[64];
                snprintf(s, sizeof(s), "buffer overflow when peekBuf, over %u.", (uint32_t)_buf_len);
                throw TarsDecodeException(s);
            }

            const char* begin = _buf + _cur + offset;
            v.assign(begin, begin + len);
        }

        /// 跳过len个字节
        void skip(size_t len)
        {
            if(len <= _buf_len && (_cur + len) <= _buf_len)
            {
                _cur += len;
            }
            else
            {
                char s[64];
                snprintf(s, sizeof(s), "buffer overflow when skip, over %u.", (uint32_t)_buf_len);
                throw TarsDecodeException(s);
            }
        }

        /// 设置缓存
        void setBuffer(const char * buf, size_t len)
        {
            _buf = buf;
            _buf_len = len;
            _cur = 0;
        }

        /// 设置缓存
        template<typename Alloc>
        void setBuffer(const std::vector<char,Alloc> &buf)
        {
            _buf = &buf[0];
            _buf_len = buf.size();
            _cur = 0;
        }

        /**
         * 判断是否已经到BUF的末尾
         */
        bool hasEnd()
        {
            return _cur >= _buf_len;
        }
        size_t tellp() const
        {
            return _cur;
        }
        const char* base() const
        {
            return _buf;
        }
        size_t size() const
        {
            return _buf_len;
        }
    };

//当tars文件中含有指针型类型的数据用MapBufferReader读取
//在读数据时利用MapBufferReader提前分配的内存 减少运行过程中频繁内存分配
//结构中定义byte指针类型，指针用*来定义，如下：
//byte *m;
//指针类型使用时需要MapBufferReader提前设定预分配内存块setMapBuffer()，
//指针需要内存时通过偏移指向预分配内存块，减少解码过程中的内存申请
    class MapBufferReader : public BufferReader
    {

    public:
        MapBufferReader() : _buf_m(NULL),_buf_len_m(0),_cur_m(0) {}

        void reset() { _cur_m = 0; BufferReader::reset();}

        char* cur()
        {
            if (tars_unlikely(_buf_m == NULL))
            {
                char s[64];
                snprintf(s, sizeof(s), "MapBufferReader's buff not set,_buf = null");
                throw TarsDecodeException(s);
            }
            return _buf_m+_cur_m;
        }

        size_t left(){return _buf_len_m-_cur_m;}

        /// 跳过len个字节
        void mapBufferSkip(size_t len)
        {
            if (tars_unlikely(_cur_m + len > _buf_len_m))
            {
                char s[64];
                snprintf(s, sizeof(s), "MapBufferReader's buffer overflow when peekBuf, over %u.", (uint32_t)_buf_len_m);
                throw TarsDecodeException(s);
            }
            _cur_m += len;
        }

        /// 设置缓存
        void setMapBuffer(char * buf, size_t len)
        {
            _buf_m = buf;
            _buf_len_m = len;
            _cur_m = 0;
        }

        /// 设置缓存
        template<typename Alloc>
        void setMapBuffer(std::vector<char,Alloc> &buf)
        {
            _buf_m = &buf[0];
            _buf_len_m = buf.size();
            _cur_m = 0;
        }
    public:
        char *              _buf_m;        ///< 缓冲区
        size_t              _buf_len_m;    ///< 缓冲区长度
        size_t              _cur_m;        ///< 当前位置
    };

//////////////////////////////////////////////////////////////////
/// 缓冲区写入器封装
    class BufferWriter
    {
    public:
        char *  _buf;
        size_t  _len;
        size_t  _buf_len;
        bool    _reverse;

    public:
        BufferWriter(const BufferWriter & bw)
        {
            _buf = NULL;
            _len = 0;
            _buf_len = 0;
            _reverse = true;

            writeBuf(bw._buf, bw._len);
            _len = bw._len;
            //_buf_len    = bw._buf_len;
        }

        BufferWriter& operator=(const BufferWriter& buf)
        {
            _reverse = true;
            writeBuf(buf._buf,buf._len);
            _len = buf._len;
            //_buf_len = buf._buf_len;
            return *this;
        }

        BufferWriter()
        : _buf(NULL)
        , _len(0)
        , _buf_len(0)
        , _reverse(true)
        {}
        ~BufferWriter()
        {
            delete[] _buf;
        }

        void reserve(size_t len)
        {
            if (tars_unlikely(_buf_len < len))
            {
                len <<= 1;
                if(len<128)
                    len=128;
                char * p = new char[len];
                memcpy(p, _buf, _len);
                delete[] _buf;
                _buf = p;
                _buf_len = len;
            }
        }
        void reset() { _len = 0;}
        void writeBuf(const void * buf, size_t len)
        {
            TarsReserveBuf(*this, _len + len);
            memcpy(_buf + _len, buf, len);
            _len += len;
        }
        //const std::vector<char> &getByteBuffer() const    { return _buf; }
        std::vector<char> getByteBuffer() const             { return std::vector<char>(_buf, _buf + _len);}
        const char * getBuffer() const                      { return _buf;}//{ return &_buf[0]; }
        size_t getLength() const                            { return _len;}    //{ return _buf.size(); }
        //void swap(std::vector<char>& v)                   { _buf.swap(v); }
        void swap(std::vector<char>& v)
        {
            v.assign(_buf, _buf + _len);
        }
        void swap(BufferWriter& buf)
        {
            std::swap(_buf, buf._buf);
            std::swap(_buf_len, buf._buf_len);
            std::swap(_len, buf._len);
            std::swap(_reverse, buf._reverse);
        }
    };

///////////////////////////////////////////////////////////////////////////////////////////////////
/// 预先设定缓存的封装器
    class BufferWriterBuff
    {
    public:
        char *  _buf;
        size_t  _len;
        size_t  _buf_len;
        bool    _reverse;
    private:
        BufferWriterBuff(const BufferWriterBuff&);
    public:

        BufferWriterBuff& operator=(const BufferWriterBuff& buf)
        {
            _reverse = false;
            writeBuf(buf._buf, buf._len);
            _len = buf._len;
            _buf_len = buf._buf_len;
            return *this;
        }

        BufferWriterBuff()
        : _buf(NULL)
        , _len(0)
        , _buf_len(0)
        , _reverse(false)
        {}
        ~BufferWriterBuff()
        {

        }

        void setBuffer(char * buffer, size_t size_buff)
        {
            _buf = buffer;
            _len = 0;
            _buf_len = size_buff;
            _reverse = false;
        }

        /*
        void reserve(size_t len)
        {
            if(_buf_len < len)
            {

            }
        }
        */
        void reset() { _len = 0;}

        void writeBuf(const void * buf, size_t len)
        {
            if (tars_unlikely(_buf_len < _len + len))
            {
                throw TarsNotEnoughBuff("not enough buffer");
            }

            memcpy(_buf + _len, buf, len);
            _len += len;
        }

        std::vector<char> getByteBuffer() const      { return std::vector<char>(_buf, _buf + _len);}
        const char * getBuffer() const               { return _buf;}
        size_t getLength() const                     { return _len;}
        void swap(std::vector<char>& v)
        {
            v.assign(_buf, _buf + _len);
        }
        void swap(BufferWriterBuff& buf)
        {
            std::swap(_buf, buf._buf);
            std::swap(_buf_len, buf._buf_len);
            std::swap(_len, buf._len);
            std::swap(_reverse, buf._reverse);
        }
    };

//////////////////////////////////////////////////////////////////
    template<typename ReaderT = BufferReader>
    class TarsInputStream : public ReaderT
    {
    public:
        
        /// 跳到指定标签的元素前
        bool skipToTag(uint8_t tag)
        {
            try
            {
                uint8_t headType = 0, headTag = 0;
                while (!ReaderT::hasEnd())
                {
                    size_t len = 0;
                    TarsPeekFromHead(*this, headType, headTag, len);
                    if (tag <= headTag || headType == TarsHeadeStructEnd)
                        return headType == TarsHeadeStructEnd?false:(tag == headTag);
                    TarsReadHeadSkip(*this, len);
                    skipField(headType);
                }
            }
            catch (TarsDecodeException& e)
            {
            }
            return false;
        }

        /// 跳到当前结构的结束
        void skipToStructEnd()
        {
            uint8_t headType = 0;
            do
            {
                readFromHeadNoTag(*this, headType);
                skipField(headType);
            }while (headType != TarsHeadeStructEnd);
        }
        
        /// 跳过一个字段
        void skipField()
        {
            uint8_t headType = 0;
            readFromHeadNoTag(*this, headType);
            skipField(headType);
        }

        /// 跳过一个字段，不包含头信息
        void skipField(uint8_t type)
        {
            switch (type)
            {
            case TarsHeadeChar:
                TarsReadHeadSkip(*this, sizeof(Char));
                break;
            case TarsHeadeShort:
                TarsReadHeadSkip(*this, sizeof(Short));
                break;
            case TarsHeadeInt32:
                TarsReadHeadSkip(*this, sizeof(Int32));
                break;
            case TarsHeadeInt64:
                TarsReadHeadSkip(*this, sizeof(Int64));
                break;
            case TarsHeadeFloat:
                TarsReadHeadSkip(*this, sizeof(Float));
                break;
            case TarsHeadeDouble:
                TarsReadHeadSkip(*this, sizeof(Double));
                break;
            case TarsHeadeString1:
                {
                    size_t len = 0;
                    TarsReadTypeBuf(*this, len, uint8_t);
                    TarsReadHeadSkip(*this, len);
                }
                break;
            case TarsHeadeString4:
                {
                    size_t len = 0;
                    TarsReadTypeBuf(*this, len, uint32_t);
                    len = ntohl(len);
                    TarsReadHeadSkip(*this, len);
                }
                break;
            case TarsHeadeMap:
                {
                    Int32 size = 0;
                    read(size, 0);
                    for (Int32 i = 0; i < size * 2; ++i)
                        skipField();
                }
                break;
            case TarsHeadeList:
                {
                    Int32 size = 0;
                    read(size, 0);
                    for (Int32 i = 0; i < size; ++i)
                        skipField();
                }
                break;
            case TarsHeadeSimpleList:
                {
                    uint8_t headType = 0, headTag = 0;
                    readFromHead(*this, headType, headTag);
                    if (tars_unlikely(headType != TarsHeadeChar))
                    {
                        char s[64];
                        snprintf(s, sizeof(s), "skipField with invalid type, type value: %d, %d, %d.", type, headType, headTag);
                        throw TarsDecodeMismatch(s);
                    }
                    Int32 size = 0;
                    read(size, 0);
                    TarsReadHeadSkip(*this, size);
                }
                break;
            case TarsHeadeStructBegin:
                skipToStructEnd();
                break;
            case TarsHeadeStructEnd:
            case TarsHeadeZeroTag:
                break;
            default:
                {
                    char s[64];
                    snprintf(s, sizeof(s), "skipField with invalid type, type value:%d.", type);
                    throw TarsDecodeMismatch(s);
                }
            }
        }

        /// 读取一个指定类型的数据（基本类型）
        template<typename T>
        inline T readByType()
        {
            T n;
            this->readBuf(&n, sizeof(n));
            return n;
        }
        void readUnknown(std::string & sUnkown, uint8_t tag)
        {

            size_t start = ReaderT::tellp();
            size_t last  = start;
            try
            {
                uint8_t lasttag = tag;
                DataHead h;
                while (!ReaderT::hasEnd())
                {
                    size_t len = h.peekFrom(*this);
                    if ( h.getTag() <=lasttag)
                    {
                        break;
                    }
                    lasttag = h.getTag();
                    this->skip(len);
                    skipField(h.getType());
                    last = ReaderT::tellp(); //记录下最后一次正常到达的位置
                }
            }
            catch (...) //
            {
                throw;
            }
            std::string s(ReaderT::base() +start, last - start);
            sUnkown = s;
            return ;

        }
        friend class XmlProxyCallback;

        void read(Bool& b, uint8_t tag, bool isRequire = true)
        {
            Char c = b;
            read(c, tag, isRequire);
            b = c ? true : false;
        }

        void read(Char& c, uint8_t tag, bool isRequire = true)
        {
            uint8_t headType = 0, headTag = 0;
            bool skipFlag = false;
            TarsSkipToTag(skipFlag, tag, headType, headTag);
            if (tars_likely(skipFlag))
            {
                switch (headType)
                {
                case TarsHeadeZeroTag:
                    c = 0;
                    break;
                case TarsHeadeChar:
                    TarsReadTypeBuf(*this, c, Char);
                    break;
                default:
                    {
                        char s[64];
                        snprintf(s, sizeof(s), "read 'Char' type mismatch, tag: %d, get type: %d.", tag, headType);
                        throw TarsDecodeMismatch(s);
                    }

                }
            }
            else if (tars_unlikely(isRequire))
            {
                char s[64];
                snprintf(s, sizeof(s), "require field not exist, tag: %d, headTag: %d.", tag, headTag);
                throw TarsDecodeRequireNotExist(s);
            }
        }

        void read(UInt8& n, uint8_t tag, bool isRequire = true)
        {
            Short i = (Short)n;
            read(i,tag,isRequire);
            n = (UInt8)i;
        }

        void read(Short& n, uint8_t tag, bool isRequire = true)
        {
            uint8_t headType = 0, headTag = 0;
            bool skipFlag = false;
            TarsSkipToTag(skipFlag, tag, headType, headTag);
            if (tars_likely(skipFlag))
            {
                switch (headType)
                {
                case TarsHeadeZeroTag:
                    n = 0;
                    break;
                case TarsHeadeChar:
                    TarsReadTypeBuf(*this, n, Char);
                    break;
                case TarsHeadeShort:
                    TarsReadTypeBuf(*this, n, Short);
                    n = ntohs(n);
                    break;
                default:
                    {
                        char s[64];
                        snprintf(s, sizeof(s), "read 'Short' type mismatch, tag: %d, get type: %d.", tag, headType);
                        throw TarsDecodeMismatch(s);
                    }
                }
            }
            else if (tars_unlikely(isRequire))
            {
                char s[64];
                snprintf(s, sizeof(s), "require field not exist, tag: %d, headTag: %d", tag, headTag);
                throw TarsDecodeRequireNotExist(s);
            }
        }

        void read(UInt16& n, uint8_t tag, bool isRequire = true)
        {
            Int32 i = (Int32)n;
            read(i,tag,isRequire);
            n = (UInt16)i;
        }

        void read(Int32& n, uint8_t tag, bool isRequire = true)
        {
            uint8_t headType = 1, headTag = 1;
            bool skipFlag = false;
            TarsSkipToTag(skipFlag, tag, headType, headTag);
            if (tars_likely(skipFlag)) 
            {            
                switch (headType)
                {
                case TarsHeadeZeroTag:
                    n = 0;
                    break;
                case TarsHeadeChar:
                    TarsReadTypeBuf(*this, n, Char);
                    break;
                case TarsHeadeShort:
                    TarsReadTypeBuf(*this, n, Short);
                    n = (Short)ntohs(n);
                    break;
                case TarsHeadeInt32:
                    TarsReadTypeBuf(*this, n, Int32);
                    n = ntohl(n);
                    break;
                default:
                {
                           char s[64];
                           snprintf(s, sizeof(s), "read 'Int32' type mismatch, tag: %d, get type: %d.", tag, headType);
                           throw TarsDecodeMismatch(s);
                }
                }
            }
            else if (tars_unlikely(isRequire))
            {
                char s[64];
                snprintf(s, sizeof(s), "require field not exist, tag: %d headType: %d, headTag: %d", tag, headType, headTag);
                throw TarsDecodeRequireNotExist(s);
            }
        }

        void read(UInt32& n, uint8_t tag, bool isRequire = true)
        {
            Int64 i = (Int64)n;
            read(i,tag,isRequire);
            n = (UInt32)i;
        }

        void read(Int64& n, uint8_t tag, bool isRequire = true)
        {
            uint8_t headType = 0, headTag = 0;
            bool skipFlag = false;
            TarsSkipToTag(skipFlag, tag, headType, headTag);
            if (tars_likely(skipFlag))
            {
                switch(headType)
                {
                case TarsHeadeZeroTag:
                    n = 0;
                    break;
                case TarsHeadeChar:
                    TarsReadTypeBuf(*this, n, Char); 
                    break;
                case TarsHeadeShort:
                    TarsReadTypeBuf(*this, n, Short); 
                    n = (Short) ntohs(n);
                    break;
                case TarsHeadeInt32:
                    TarsReadTypeBuf(*this, n, Int32);
                    n = (Int32) ntohl(n);
                    break;
                case TarsHeadeInt64:
                    TarsReadTypeBuf(*this, n, Int64);
                    n = tars_ntohll(n);
                    break;
                default:
                    {
                        char s[64];
                        snprintf(s, sizeof(s), "read 'Int64' type mismatch, tag: %d, get type: %d.", tag, headType); 
                        throw TarsDecodeMismatch(s);
                    }

                }
            }
            else if (tars_unlikely(isRequire))
            {
                char s[64];
                snprintf(s, sizeof(s), "require field not exist, tag: %d, headTag: %d", tag, headTag);
                throw TarsDecodeRequireNotExist(s);
            }
        }

        void read(Float& n, uint8_t tag, bool isRequire = true)
        {
            uint8_t headType = 0, headTag = 0;
            bool skipFlag = false;
            TarsSkipToTag(skipFlag, tag, headType, headTag);
            if (tars_likely(skipFlag))
            {
                switch(headType)
                {
                case TarsHeadeZeroTag:
                    n = 0;
                    break;
                case TarsHeadeFloat:
                    TarsReadTypeBuf(*this, n, float);
                    n = tars_ntohf(n);
                    break;
                default:
                    {
                        char s[64];
                        snprintf(s, sizeof(s), "read 'Float' type mismatch, tag: %d, get type: %d.", tag, headType);
                        throw TarsDecodeMismatch(s);
                    }
                }
            }
            else if (tars_unlikely(isRequire))
            {
                char s[64];
                snprintf(s, sizeof(s), "require field not exist, tag: %d, headTag: %d", tag, headTag);
                throw TarsDecodeRequireNotExist(s);
            }
        }

        void read(Double& n, uint8_t tag, bool isRequire = true)
        {
            uint8_t headType = 0, headTag = 0;
            bool skipFlag = false;
            TarsSkipToTag(skipFlag, tag, headType, headTag);
            if (tars_likely(skipFlag))
            {
                switch(headType)
                {
                case TarsHeadeZeroTag:
                    n = 0;
                    break;
                case TarsHeadeFloat:
                    TarsReadTypeBuf(*this, n, float);
                    n = tars_ntohf(n);
                    break;
                case TarsHeadeDouble:
                    TarsReadTypeBuf(*this, n, double);
                    n = tars_ntohd(n);
                    break;
                default:
                    {
                        char s[64];
                        snprintf(s, sizeof(s), "read 'Double' type mismatch, tag: %d, get type: %d.", tag, headType); 
                        throw TarsDecodeMismatch(s);
                    }
                }
            }
            else if (tars_unlikely(isRequire))
            {
                char s[64];
                snprintf(s, sizeof(s), "require field not exist, tag: %d, headTag: %d", tag, headTag);
                throw TarsDecodeRequireNotExist(s);
            }
        }

        /*void read(std::string& s, uint8_t tag, bool isRequire = true)
        {
            uint8_t headType = 0, headTag = 0;
            bool skipFlag = false;
            TarsSkipToTag(skipFlag, tag, headType, headTag);
            if (tars_likely(skipFlag))
            {
                switch(headType)
                {
                case TarsHeadeString1:
                    {
                        size_t len = 0;
                        TarsReadTypeBuf(*this, len, uint8_t); 
                        char ss[256];
                        //s.resize(len);
                        //this->readBuf((void *)s.c_str(), len);
                        TarsReadStringBuf(*this, s, len);
                        //TarsReadBuf(*this, s, len);
                        //s.assign(ss, ss + len);
                    }
                    break;
                case TarsHeadeString4:
                    {
                        uint32_t len = 0;
                        TarsReadTypeBuf(*this, len, uint32_t);
                        len = ntohl(len);
                        if (tars_unlikely(len > TARS_MAX_STRING_LENGTH))
                        {
                            char s[128];
                            snprintf(s, sizeof(s), "invalid string size, tag: %d, size: %d", tag, len);
                            throw TarsDecodeInvalidValue(s);
                        }
                        //char *ss = new char[len];
                        //s.resize(len);
                        //this->readBuf((void *)s.c_str(), len);
                        
                        char *ss = new char[len];
                        try
                        {
                            TarsReadBuf(*this, ss, len);
                            s.assign(ss, ss + len);
                        }
                        catch (...)
                        {
                            delete[] ss;
                            throw;
                        }
                        delete[] ss;
                        TarsReadStringBuf(*this, s, len);
                    }
                    break;
                default:
                {
                           char s[64];
                           snprintf(s, sizeof(s), "read 'string' type mismatch, tag: %d, get type: %d.", tag, headType);
                           throw TarsDecodeMismatch(s);
                }
                }
            }
            else if (tars_unlikely(isRequire))
            {
                char s[64];
                snprintf(s, sizeof(s), "require field not exist, tag: %d", tag);
                throw TarsDecodeRequireNotExist(s);
            }
        }*/

        void read(std::string& s, uint8_t tag, bool isRequire = true)
        {
            uint8_t headType = 0, headTag = 0;
            bool skipFlag = false;
            TarsSkipToTag(skipFlag, tag, headType, headTag);
            if (tars_likely(skipFlag))
            {
                uint32_t strLength = 0;
                switch (headType)
                {
                    case TarsHeadeString1:
                    {
                        TarsReadTypeBuf(*this, strLength, uint8_t);
                    }
                    break;
                    case TarsHeadeString4:
                    {
                        TarsReadTypeBuf(*this, strLength, uint32_t);
                        strLength = ntohl(strLength);
                        if (tars_unlikely(strLength > TARS_MAX_STRING_LENGTH))
                        {
                            char s[128];
                            snprintf(s, sizeof(s), "invalid string size, tag: %d, size: %d", tag, strLength);
                            throw TarsDecodeInvalidValue(s);
                        }
                    }
                    break;
                    default:
                    {
                        char s[64];
                        snprintf(s, sizeof(s), "read 'string' type mismatch, tag: %d, get type: %d, tag: %d.", tag, headType, headTag);
                        throw TarsDecodeMismatch(s);
                    }
                }
                TarsReadStringBuf(*this, s, strLength);
            }
            else if (tars_unlikely(isRequire))
            {
                char s[64];
                snprintf(s, sizeof(s), "require field not exist, tag: %d", tag);
                throw TarsDecodeRequireNotExist(s);
            }
        }

        void read(char *buf, const UInt32 bufLen, UInt32 & readLen, uint8_t tag, bool isRequire = true)
        {
            uint8_t headType = 0, headTag = 0;
            bool skipFlag = false;
            TarsSkipToTag(skipFlag, tag, headType, headTag);
            if (tars_likely(skipFlag))
            {
                switch(headType)
                {
                case TarsHeadeSimpleList:
                    {
                        uint8_t hheadType, hheadTag;
                        readFromHead(*this, hheadType, hheadTag);
                        if (tars_unlikely(hheadType != TarsHeadeChar))
                        {
                            char s[128];
                            snprintf(s, sizeof(s), "type mismatch, tag: %d, type: %d, %d, %d", tag, headType, hheadType, hheadTag);
                            throw TarsDecodeMismatch(s);
                        }
                        UInt32 size = 0;
                        read(size, 0);
                        if (tars_unlikely(size > bufLen))
                        {
                            char s[128];
                            snprintf(s, sizeof(s), "invalid size, tag: %d, type: %d, %d, size: %d", tag, headType, hheadType, size); 
                            throw TarsDecodeInvalidValue(s);
                        }
                        //TarsReadTypeBuf(*this, size, UInt32);
                        this->readBuf(buf, size);
                        readLen = size;
                    }
                    break;

                default:
                    {
                        char s[128];
                        snprintf(s, sizeof(s), "type mismatch, tag: %d, type: %d", tag, headType); 
                        throw TarsDecodeMismatch(s);
                    }
                }
            }
            else if (tars_unlikely(isRequire))
            {
                char s[128];
                snprintf(s, sizeof(s), "require field not exist, tag: %d, headTag: %d", tag, headTag);
                throw TarsDecodeRequireNotExist(s);
            }
        }


        template<typename K, typename V, typename Cmp, typename Alloc>
        void read(std::map<K, V, Cmp, Alloc>& m, uint8_t tag, bool isRequire = true)
        {
            uint8_t headType = 0, headTag = 0;
            bool skipFlag = false;
            TarsSkipToTag(skipFlag, tag, headType, headTag);
            if (tars_likely(skipFlag))
            {
                switch(headType)
                {
                case TarsHeadeMap:
                    {
                        UInt32 size = 0;
                        read(size, 0);
                        if (tars_unlikely(size > this->size()))
                        {
                            char s[128];
                            snprintf(s, sizeof(s), "invalid map, tag: %d, size: %d", tag, size);
                            throw TarsDecodeInvalidValue(s);
                        }
                        m.clear();

                        for (UInt32 i = 0; i < size; ++i)
                        {
                            std::pair<K, V> pr;
                            read(pr.first, 0);
                            read(pr.second, 1);
                            m.insert(pr);
                        }
                    }
                    break;
                default:
                    {
                        char s[64];
                        snprintf(s, sizeof(s), "read 'map' type mismatch, tag: %d, get type: %d.", tag, headType); 
                        throw TarsDecodeMismatch(s);
                    }
                }
            }
            else if (tars_unlikely(isRequire))
            {
                char s[64];
                snprintf(s, sizeof(s), "require field not exist, tag: %d, headTag: %d", tag, headTag);
                throw TarsDecodeRequireNotExist(s);
            }
        }

        template<typename Alloc>
        void read(std::vector<Char, Alloc>& v, uint8_t tag, bool isRequire = true)
        {
            uint8_t headType = 0, headTag = 0;
            bool skipFlag = false;
            TarsSkipToTag(skipFlag, tag, headType, headTag);
            if (tars_likely(skipFlag))
            {
                switch(headType)
                {
                case TarsHeadeSimpleList:
                    {
                        uint8_t hheadType, hheadTag;
                        readFromHead(*this, hheadType, hheadTag);
                        if (tars_unlikely(hheadType != TarsHeadeChar))
                        {
                            char s[128];
                            snprintf(s, sizeof(s), "type mismatch, tag: %d, type: %d, %d, %d", tag, headType, hheadType, hheadTag);
                            throw TarsDecodeMismatch(s);
                        }
                        UInt32 size = 0;
                        read(size, 0);
                        if (tars_unlikely(size > this->size()))
                        {
                            char s[128];
                            snprintf(s, sizeof(s), "invalid size, tag: %d, type: %d, %d, size: %d", tag, headType, hheadType, size);
                            throw TarsDecodeInvalidValue(s);
                        }
                        
                        this->readBuf(v, size);
                    }
                    break;
                case TarsHeadeList:
                    {
                        UInt32 size = 0;
                        read(size, 0);
                        if (tars_unlikely(size > this->size()))
                        {
                            char s[128];
                            snprintf(s, sizeof(s), "invalid size, tag: %d, type: %d, size: %d", tag, headType, size);
                            throw TarsDecodeInvalidValue(s);
                        }
                        v.reserve(size);
                        v.resize(size);
                        for (UInt32 i = 0; i < size; ++i)
                            read(v[i], 0);
                    }
                    break;
                default:
                    {
                        char s[128];
                        snprintf(s, sizeof(s), "type mismatch, tag: %d, type: %d", tag, headType);
                        throw TarsDecodeMismatch(s);
                    }
                }
            }
            else if (tars_unlikely(isRequire))
            {
                char s[128];
                snprintf(s, sizeof(s), "require field not exist, tag: %d, headTag: %d", tag, headTag);
                throw TarsDecodeRequireNotExist(s);
            }
        }

        template<typename T, typename Alloc>
        void read(std::vector<T, Alloc>& v, uint8_t tag, bool isRequire = true)
        {
            uint8_t headType = 0, headTag = 0;
            bool skipFlag = false;
            TarsSkipToTag(skipFlag, tag, headType, headTag);
            if (tars_likely(skipFlag))
            {
                switch(headType)
                {
                case TarsHeadeList:
                    {
                        UInt32 size = 0;
                        read(size, 0);
                        if (tars_unlikely(size > this->size()))
                        {
                            char s[128];
                            snprintf(s, sizeof(s), "invalid size, tag: %d, type: %d, size: %d", tag, headType, size); 
                            throw TarsDecodeInvalidValue(s);
                        }
                        v.reserve(size);
                        v.resize(size);
                        for (UInt32 i = 0; i < size; ++i)
                            read(v[i], 0);
                    }
                    break;
                default:
                    {
                        char s[64];
                        snprintf(s, sizeof(s), "read 'vector' type mismatch, tag: %d, get type: %d.", tag, headType); 
                        throw TarsDecodeMismatch(s);
                    }
                }
            }
            else if (tars_unlikely(isRequire))
            {
                char s[64];
                snprintf(s, sizeof(s), "require field not exist, tag: %d, headTag: %d", tag, headTag);
                throw TarsDecodeRequireNotExist(s);
            }
        }

        /// 读取结构数组
        template<typename T>
        void read(T* v, const UInt32 len, UInt32 & readLen, uint8_t tag, bool isRequire = true)
        {
            uint8_t headType = 0, headTag = 0;
            bool skipFlag = false;
            TarsSkipToTag(skipFlag, tag, headType, headTag);
            if (tars_likely(skipFlag))
            {
                switch(headType)
                {
                case TarsHeadeList:
                    {
                        UInt32 size = 0;
                        read(size, 0);
                        if (tars_unlikely(size > this->size()))
                        {
                            char s[128];
                            snprintf(s, sizeof(s), "invalid size, tag: %d, type: %d, size: %d", tag, headType, size);
                            throw TarsDecodeInvalidValue(s);
                        }
                        for (UInt32 i = 0; i < size; ++i)
                            read(v[i], 0);
                        readLen = size;
                    }
                    break;
                default:
                    {
                        char s[64];
                        snprintf(s, sizeof(s), "read 'vector struct' type mismatch, tag: %d, get type: %d.", tag, headType); 
                        throw TarsDecodeMismatch(s);
                    }
                }
            }
            else if (tars_unlikely(isRequire))
            {
                char s[64];
                snprintf(s, sizeof(s), "require field not exist, tag: %d, headTag: %d", tag, headTag);
                throw TarsDecodeRequireNotExist(s);
            }
        }

        template<typename T>
        void read(T& v, uint8_t tag, bool isRequire = true, typename detail::disable_if<detail::is_convertible<T*, TarsStructBase*>, void ***>::type dummy = 0)
        {
            Int32 n = 0;
            read(n, tag, isRequire);
            v = (T) n;
        }

        /// 读取结构
        template<typename T>
        void read(T& v, uint8_t tag, bool isRequire = true, typename detail::enable_if<detail::is_convertible<T*, TarsStructBase*>, void ***>::type dummy = 0)
        {
            uint8_t headType = 0, headTag = 0;
            bool skipFlag = false;
            TarsSkipToTag(skipFlag, tag, headType, headTag);
            if (tars_likely(skipFlag))
            {
                if (tars_unlikely(headType != TarsHeadeStructBegin))
                {
                    char s[64];
                    snprintf(s, sizeof(s), "read 'struct' type mismatch, tag: %d, get type: %d.", tag, headType);
                    throw TarsDecodeMismatch(s);
                }
                v.readFrom(*this);
                skipToStructEnd();
            }
            else if (tars_unlikely(isRequire))
            {
                char s[64];
                snprintf(s, sizeof(s), "require field not exist, tag: %d, headTag: %d", tag, headTag);
                throw TarsDecodeRequireNotExist(s);
            }
        }
    };

//////////////////////////////////////////////////////////////////
    template<typename WriterT = BufferWriter>
    class TarsOutputStream : public WriterT
    {
    public:
        void writeUnknown(const std::string& s)
        {
            this->writeBuf(s.data(), s.size());
        }
        void writeUnknownV2(const std::string& s)
        {
            DataHead::writeTo(*this, DataHead::eStructBegin, 0);
            this->writeBuf(s.data(), s.size());
            DataHead::writeTo(*this, DataHead::eStructEnd, 0);
        }
        void write(Bool b, uint8_t tag)
        {
            write((Char) b, tag);
        }

        void write(Char n, uint8_t tag)
        {
            /*
            DataHead h(DataHead::eChar, tag);
            if(n == 0){
                h.setType(DataHead::eZeroTag);
                h.writeTo(*this);
            }else{
                h.writeTo(*this);
                this->writeBuf(&n, sizeof(n));
            }
            */
            if (tars_unlikely(n == 0))
            {
                TarsWriteToHead(*this, TarsHeadeZeroTag, tag);
            }
            else
            {
                TarsWriteToHead(*this, TarsHeadeChar, tag);
                TarsWriteCharTypeBuf(*this, n, (*this)._len);  
            }
        }

        void write(UInt8 n, uint8_t tag)
        {
            write((Short) n, tag);
        }

        void write(Short n, uint8_t tag)
        {
            //if(n >= CHAR_MIN && n <= CHAR_MAX){
            if (n >= (-128) && n <= 127)
            {
                write((Char) n, tag);
            }
            else
            {
                /*
                DataHead h(DataHead::eShort, tag);
                h.writeTo(*this);
                n = htons(n);
                this->writeBuf(&n, sizeof(n));
                */
                TarsWriteToHead(*this, TarsHeadeShort, tag);
                n = htons(n);
                TarsWriteShortTypeBuf(*this, n, (*this)._len); 
            }
        }

        void write(UInt16 n, uint8_t tag)
        {
            write((Int32) n, tag);
        }

        void write(Int32 n, uint8_t tag)
        {
            //if(n >= SHRT_MIN && n <= SHRT_MAX){
            if (n >= (-32768) && n <= 32767)
            {
                write((Short) n, tag);
            }
            else
            {
                //DataHead h(DataHead::eInt32, tag);
                //h.writeTo(*this);
                TarsWriteToHead(*this, TarsHeadeInt32, tag);
                n = htonl(n);
                TarsWriteInt32TypeBuf(*this, n, (*this)._len);  
            }
        }

        void write(UInt32 n, uint8_t tag)
        {
            write((Int64) n, tag);
        }

        void write(Int64 n, uint8_t tag)
        {
            //if(n >= INT_MIN && n <= INT_MAX){
            if (n >= (-2147483647-1) && n <= 2147483647)
            {
                write((Int32) n, tag);
            }
            else
            {
                //DataHead h(DataHead::eInt64, tag);
                //h.writeTo(*this);
                TarsWriteToHead(*this, TarsHeadeInt64, tag);
                n = tars_htonll(n);
                TarsWriteInt64TypeBuf(*this, n, (*this)._len); 
            }
        }

        void write(Float n, uint8_t tag)
        {
            //DataHead h(DataHead::eFloat, tag);
            //h.writeTo(*this);
            TarsWriteToHead(*this, TarsHeadeFloat, tag);
            n = tars_htonf(n);
            TarsWriteFloatTypeBuf(*this, n, (*this)._len);  
        }

        void write(Double n, uint8_t tag)
        {
            //DataHead h(DataHead::eDouble, tag);
            //h.writeTo(*this);
            TarsWriteToHead(*this, TarsHeadeDouble, tag);
            n = tars_htond(n);
            TarsWriteDoubleTypeBuf(*this, n, (*this)._len); 
        }

        void write(const std::string& s, uint8_t tag)
        {
            if (tars_unlikely(s.size() > 255))
            {
                if (tars_unlikely(s.size() > TARS_MAX_STRING_LENGTH))
                {
                    char ss[128];
                    snprintf(ss, sizeof(ss), "invalid string size, tag: %d, size: %u", tag, (uint32_t)s.size());
                    throw TarsDecodeInvalidValue(ss);
                }
                TarsWriteToHead(*this, TarsHeadeString4, tag);
                uint32_t n = htonl(s.size());
                TarsWriteUInt32TTypeBuf(*this, n, (*this)._len); 
                //this->writeBuf(s.data(), s.size());
                TarsWriteTypeBuf(*this, s.data(), s.size());
            }
            else
            {
                TarsWriteToHead(*this, TarsHeadeString1, tag);
                uint8_t n = s.size();
                TarsWriteUInt8TTypeBuf(*this, n, (*this)._len); 
                //this->writeBuf(s.data(), s.size());
                TarsWriteTypeBuf(*this, s.data(), s.size());
            }
        }

        void write(const char *buf, const UInt32 len, uint8_t tag)
        {
            TarsWriteToHead(*this, TarsHeadeSimpleList, tag);
            TarsWriteToHead(*this, TarsHeadeChar, 0);
            write(len, 0);
            //this->writeBuf(buf, len);
            TarsWriteTypeBuf(*this, buf, len);
        }

        template<typename K, typename V, typename Cmp, typename Alloc>
        void write(const std::map<K, V, Cmp, Alloc>& m, uint8_t tag)
        {
            //DataHead h(DataHead::eMap, tag);
            //h.writeTo(*this);
            TarsWriteToHead(*this, TarsHeadeMap, tag);
            Int32 n = m.size();
            write(n, 0);
            typedef typename std::map<K, V, Cmp, Alloc>::const_iterator IT;
            for (IT i = m.begin(); i != m.end(); ++i)
            {
                write(i->first, 0);
                write(i->second, 1);
            }
        }

        template<typename T, typename Alloc>
        void write(const std::vector<T, Alloc>& v, uint8_t tag)
        {
            //DataHead h(DataHead::eList, tag);
            //h.writeTo(*this);
            TarsWriteToHead(*this, TarsHeadeList, tag);
            Int32 n = v.size();
            write(n, 0);
            typedef typename std::vector<T, Alloc>::const_iterator IT;
            for (IT i = v.begin(); i != v.end(); ++i)
                write(*i, 0);
        }

        template<typename T>
        void write(const T *v, const UInt32 len, uint8_t tag)
        {
            TarsWriteToHead(*this, TarsHeadeList, tag);
            write(len, 0);
            for (Int32 i = 0; i < (Int32)len; ++i)
            {
                write(v[i], 0);
            }
        }

        template<typename Alloc>
        void write(const std::vector<Char, Alloc>& v, uint8_t tag)
        {
            //DataHead h(DataHead::eSimpleList, tag);
            //h.writeTo(*this);
            //DataHead hh(DataHead::eChar, 0);
            //hh.writeTo(*this);
            TarsWriteToHead(*this, TarsHeadeSimpleList, tag);
            TarsWriteToHead(*this, TarsHeadeChar, 0);
            Int32 n = v.size();
            write(n, 0);
            //writeBuf(&v[0], v.size());
            TarsWriteTypeBuf(*this, &v[0], v.size());
        }

        template<typename T>
        void write(const T& v, uint8_t tag, typename detail::disable_if<detail::is_convertible<T*, TarsStructBase*>, void ***>::type dummy = 0)
        {
            write((Int32) v, tag);
        }

        template<typename T>
        void write(const T& v, uint8_t tag, typename detail::enable_if<detail::is_convertible<T*, TarsStructBase*>, void ***>::type dummy = 0)
        {
            //DataHead h(DataHead::eStructBegin, tag);
            //h.writeTo(*this);
            TarsWriteToHead(*this, TarsHeadeStructBegin, tag);
            v.writeTo(*this);
            TarsWriteToHead(*this, TarsHeadeStructEnd, 0);
            /*
            h.setType(DataHead::eStructEnd);
            h.setTag(0);
            h.writeTo(*this);
            */
        }
    };
////////////////////////////////////////////////////////////////////////////////////////////////////
}

//支持iphone
#ifdef __APPLE__
    #include "TarsDisplayer.h"
#else
    #include "tup/TarsDisplayer.h"
#endif

#endif
