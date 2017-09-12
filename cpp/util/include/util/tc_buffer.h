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

#ifndef __TC_BUFFER_H
#define __TC_BUFFER_H

#include <cstring>

namespace tars
{

/////////////////////////////////////////////////
/** 
 *@file   tc_buffer.h
 *@brief  字节流缓冲区封装类. 
 * 
 */
/////////////////////////////////////////////////

/** 
 *@brief  字节流缓冲区封装类. 
 */
class TC_Buffer
{
public:
   /**
	* @brief 构造函数. 
	*  
    */
    TC_Buffer() :
        _readPos(0),
        _writePos(0),
        _capacity(0),
        _buffer(NULL),
        _highWaterPercent(50)
    {
    }

   /**
    * @brief 析构函数. 
    */
    ~TC_Buffer()
    {
        delete[] _buffer;
    }

private:
   /**
    * @brief 禁止复制
    */
    TC_Buffer(const TC_Buffer& );
    void operator=(const TC_Buffer& );

public:
	/**
	 * @brief 将数据放入缓冲
	 *  
	 * @param data  要写入的数据起始地址
	 * @param size  要写入的数据字节数
	 * @return      写入的字节数
	 */
    std::size_t PushData(const void* data, std::size_t size);

	/**
	 * @brief 调整缓冲区写游标
	 *  
	 * @param bytes 要调整的字节数
	 */
    void Produce(std::size_t bytes) { _writePos += bytes; }

	/**
	 * @brief 从缓冲取出数据,深拷贝,调整读游标
	 *  
	 * @param buf   接收数据的起始地址
	 * @param size  buf的字节数
	 * @return      实际写入的字节数,可能小于size
	 */
    std::size_t PopData(void* buf, std::size_t size);

	/**
	 * @brief 从缓冲取出数据,浅拷贝,不调整读游标
	 *  
	 * @param buf   接收数据的指针
	 * @param size  接收数据的大小
	 */
    void PeekData(void*& buf, std::size_t& size);

	/**
	 * @brief 调整缓冲区读游标
	 *  
	 * @param bytes 要调整的字节数
	 */
    void Consume(std::size_t bytes);

	/**
	 * @brief 缓冲区可读数据起始地址
	 *  
	 * @return 缓冲区可读数据起始地址
	 */
    char* ReadAddr()  {  return &_buffer[_readPos];  }

	/**
	 * @brief 缓冲区可写起始地址
	 *  
	 * @return 缓冲区可写起始地址
	 */
    char* WriteAddr() {  return &_buffer[_writePos]; }

	/**
	 * @brief 缓冲区是否有数据
	 *  
	 * @return True:缓冲区没有数据
	 */
    bool IsEmpty() const { return ReadableSize() == 0; }

	/**
	 * @brief 缓冲区数据大小
	 *  
	 * @return 缓冲区数据字节数
	 */
    std::size_t ReadableSize() const {  return _writePos - _readPos;  }

	/**
	 * @brief 缓冲区可写空间大小
	 *  
	 * @return 缓冲区可写空间字节数
	 */
    std::size_t WritableSize() const {  return _capacity - _writePos;  }

	/**
	 * @brief 缓冲区占用内存大小
	 *  
	 * @return 缓冲区占用内存字节数
	 */
    std::size_t Capacity() const {  return _capacity;  }

	/**
	 * @brief 当内存占用在数据大小两倍以上，尝试释放内存(并不是清除数据)
	 *  
	 */
    void Shrink();

	/**
	 * @brief 清除数据,并不释放内存
	 *  
	 */
    void Clear();

	/**
	 * @brief 交换两个缓冲
	 *  
	 * @param buf 要交换的缓冲
	 */
    void Swap(TC_Buffer& buf);

	/**
	 * @brief 确保缓冲有足够大小容纳size字节的数据写入
	 *  
	 * @param size 将要写入的数据字节数
	 */
    void AssureSpace(std::size_t size);

	/**
	 * @brief 设置负载率,大于此则Shrink什么都不做
	 *  
	 * @param percents 负载率[10,100)
	 */
    void SetHighWaterPercent(size_t percents);
    /**
     * 缓冲区大小上限
     */
    static const std::size_t kMaxBufferSize;
   
    /**
     * 缓冲区默认大小
     */
    static const std::size_t kDefaultSize;

private:
    /**
     * 读游标
     */
    std::size_t _readPos;

    /**
     * 写游标 
     */
    std::size_t _writePos;

    /**
     * 缓冲所占用的内存字节数 
     */
    std::size_t _capacity;

    /**
     * 缓冲区
     */
    char* _buffer;

    /**
     * 高水位百分比，影响Shrink()
     */
    size_t _highWaterPercent;

	/**
	 * @brief 重设缓冲区指针
	 *  
	 * @param ptr 将要设置的指针
	 */
    void ResetBuffer(void* ptr = NULL);
};

} // end namespace tars

#endif

