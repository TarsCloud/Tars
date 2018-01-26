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

#ifndef __TC_EX_H
#define __TC_EX_H

#include <stdexcept>
using namespace std;

namespace tars
{
/////////////////////////////////////////////////
/** 
* @file  tc_ex.h 
* @brief 异常类 
*/           
/////////////////////////////////////////////////

/**
* @brief 异常类.
*/
class TC_Exception : public exception
{
public:
    /**
     * @brief 构造函数，提供了一个可以传入errno的构造函数， 
     *  
     *        异常抛出时直接获取的错误信息
     *  
     * @param buffer 异常的告警信息 
     */
    explicit TC_Exception(const string &buffer);

    /**
     * @brief 构造函数,提供了一个可以传入errno的构造函数， 
     *  
     *        异常抛出时直接获取的错误信息
     *  
     * @param buffer 异常的告警信息 
     * @param err    错误码, 可用strerror获取错误信息
     */
    TC_Exception(const string &buffer, int err);

    /**
     * @brief 析够数函
     */
    virtual ~TC_Exception() throw();

    /**
     * @brief 错误信息.
     *
     * @return const char*
     */
    virtual const char* what() const throw();

    /**
     * @brief 获取错误码
     * 
     * @return 成功获取返回0
     */
    int getErrCode() { return _code; }

private:
    void getBacktrace();

private:
    /**
     * 异常的相关信息
     */
    string  _buffer;

    /**
     * 错误码
     */
    int     _code;

};

}
#endif

