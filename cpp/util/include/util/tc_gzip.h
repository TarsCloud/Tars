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

#ifndef __TC_GZIP_H
#define __TC_GZIP_H

#include <string>
#include <vector>
#include <zlib.h>
#include <string.h>
#include <cassert>

using namespace std;

namespace tars
{
/////////////////////////////////////////////////
/** 
* @file tc_gzip.h 
* @brief  gzip类, 封装的zlib库
*/

/////////////////////////////////////////////////

/**
* @brief 该类提供标准GZIP压缩和解压算法
*/
class TC_GZip
{
protected:
    struct Output2Vector {
        Output2Vector(vector<char>& buffer) : _buffer(buffer)
        {
            _buffer.clear();
        }
        void operator ()(char *begin, size_t length)
        {
            _buffer.insert(_buffer.end(), begin, begin + length);
        }

        vector<char>& _buffer;
    };

    struct Output2String {
        Output2String(string& buffer) : _buffer(buffer)
        {
            _buffer.clear();
        }
        void operator ()(char *begin, size_t length)
        {
            _buffer.append(begin, length);
        }

        string& _buffer;
    };

public:
    /**
    * @brief  对数据进行压缩
    *  
    * @param src         需要压缩的数据
    * @param length      数据长度
    * @param buffer      输出buffer
    * @return bool       成功失败
    */
    static bool compress(const char *src, size_t length, vector<char>& buffer);

    /**
    * @brief  对数据进行解压
    *  
    * @param src         需要解压的数据
    * @param length      数据长度
    * @param buffer      输出buffer
    * @return bool       成功失败
    */
    static bool uncompress(const char *src, size_t length, vector<char>& buffer)
    {
        Output2Vector output(buffer);

        return uncompress(src, length, output);
    }

    /**
    * @brief  对数据进行解压
    *  
    * @param src         需要解压的数据
    * @param length      数据长度
    * @param buffer      输出buffer
    * @return bool       成功失败
    */
    static bool uncompress(const char *src, size_t length, string& buffer)
    {
        Output2String output(buffer);

        return uncompress(src, length, output);
    }

    /**
    * @brief  对数据进行分片解压, 
    *         每次解压的数据调用Output输出
    *  
    * @param src         需要解压的数据
    * @param length      数据长度
    * @param o           输出buffer的函数对象 
    *                    struct Output
    *                    {
    *                        void operator()(char *begin, size_t
    *                      length);
    *                      }
    * @return bool       成功失败
    */
    template<typename Output>
    static bool uncompress(const char *src, size_t length, Output& o)
    {
//        buffer.clear();

        z_stream strm;

        /* allocate inflate state */
        strm.zalloc   = Z_NULL;
        strm.zfree    = Z_NULL;
        strm.opaque   = Z_NULL;
        strm.avail_in = 0;
        strm.next_in  = Z_NULL;

        int ret = inflateInit2(&strm, 47);

        if (ret != Z_OK)
        {
            return false;
        }

        strm.avail_in = length;
        strm.next_in  = (unsigned char *)src;

        static size_t CHUNK = 1024 * 256;
        unsigned char *out  = new unsigned char[CHUNK];

        /* run inflate() on input until output buffer not full */
        do
        {
            strm.avail_out = CHUNK;
            strm.next_out  = out;

            ret = inflate(&strm, Z_NO_FLUSH);

            assert(ret != Z_STREAM_ERROR); /* state not clobbered */
            switch (ret)
            {
            case Z_NEED_DICT:
                ret = Z_DATA_ERROR;     /* and fall through */
            case Z_DATA_ERROR:
            case Z_MEM_ERROR:
                inflateEnd(&strm);
                delete[] out;
                return false;
            }
            o((char *)out, CHUNK - strm.avail_out);
            //           buffer.insert(buffer.end(), (char *)out, (char *)out + CHUNK - strm.avail_out);
        }
        while (strm.avail_out == 0);

        /* clean up and return */
        inflateEnd(&strm);
        delete[] out;

        return (ret == Z_STREAM_END);
    }
};

}
#endif
