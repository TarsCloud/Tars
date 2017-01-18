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

#ifndef __TC_FIFO_H
#define __TC_FIFO_H
#include <string>


namespace tars
{

/////////////////////////////////////////////////
/** 
 *@file   tc_fifo.h
 *@brief  FIFO封装类. 
 */
/////////////////////////////////////////////////

/** 
 *@brief  管道操作类. 
 */
class TC_Fifo
{
public:

   /**
    * @brief 管道操作的枚举类型.  
    * 定义了对管道的操作类型，EM_WRITE：写管道，EM_READ ：读管道
    */
    enum ENUM_RW_SET
    {
        EM_WRITE = 1,
        EM_READ  = 2
    };

public:
   /**
    * @brief 构造函数. 
    *  
    * @param bOwener : 是否拥有管道，默认为ture
    */
    TC_Fifo(bool bOwener = true);

   /**
    * @brief 析构函数. 
    */
    ~TC_Fifo();

public:
    /**
     * @brief 打开FIFO. 
     *  
     * @param sPath 要打开的FIFO文件的路径 
     * @param enRW  管道操作类型
     * @param mode  该FIFO文件的权限 ，默认为可读可写
     * @return      0-成功,-1-失败 
     */
    int open(const std::string & sPath, ENUM_RW_SET enRW, mode_t mode = 0777);

    /**
     * @brief 关闭fifo 
     */
    void close();

    /**
     * @brief 获取FIFO的文件描述符. 
     * 
     * @return  FIFO的文件描述符
     */
    int fd() const { return _fd; }

    /**
     * @brief 读数据, 当读取成功时，返回实际读取的字节数，如果返回的值是0，代表已经读到文件的结束；小于0表示出现了错误
     * @param buffer     读取的内容
     * @param max_size   读取数据的大小 
     * @return           读到的数据长度 ,小于0则表示失败
     */
    int read(char * szBuff, const size_t sizeMax);

    /**
     * @brief 向管道写数据. 
     * 
     * @param szBuff       要写入的数据
     * @param sizeBuffLen  数据的大小
     * @return             大于0：表示写了部分或者全部数据 
     *                        小于0：表示出现错误
     */
    int write(const char * szBuff, const size_t sizeBuffLen);

private:
   /**
    * FIFO文件的路径
    */
    std::string        _sPathName;

   /**
    * 是否拥有FIFO
    */
    bool            _bOwner;

   /**
    * FIFO的文件的操作类型
    */
    ENUM_RW_SET        _enRW;

   /**
    * FIFO的文件描述符
    */
    int             _fd;
};

}
#endif

