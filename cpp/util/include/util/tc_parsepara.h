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

#ifndef __TC_PARSEPARA_H
#define __TC_PARSEPARA_H

#include <map>
#include <string>

using namespace std;

namespace tars
{
/////////////////////////////////////////////////
/** 
* @file tc_parsepara.h 
* @brief  name=value格式解析类(非标准的http协议) 
*   
*/            
/////////////////////////////////////////////////


/** 
* @brief  提供name=value&name1=value1形式的解析函数.
* 
* 可以和map互相转换,不是标准的cgi参数的解析
* 
* （标准cgi参数解析会把空格和+互转）,非线程安全；
* @param 型如name=value&name=value字符串
*/
class TC_Parsepara
{
public:

    TC_Parsepara(){};

    /**
    * @brief  构造函数
    * @param 型如name=value&name=value字符串
    */
    TC_Parsepara(const string &sParam);

    /**
    * @brief  copy contructor
    * @param : name=value&name=value字符串
    */
    TC_Parsepara(const map<string, string> &mpParam);

    /**
    * @brief  copy contructor
    */
    TC_Parsepara(const TC_Parsepara &para);

    /**
    * =
    */
    TC_Parsepara &operator=(const TC_Parsepara &para);

    /**
    * ==
    */
    bool operator==(const TC_Parsepara &para);

    /**
    *+
    */
    const TC_Parsepara operator+(const TC_Parsepara &para);

    /**
    * +=
    */
    TC_Parsepara& operator+=(const TC_Parsepara &para);

    /**
    * @brief  decontructor
    */
    ~TC_Parsepara();

    /**
    * @brief  遍历函数
    * @param 参数名称
    * @param 参数值
    */
    typedef int (*TC_ParseparaTraverseFunc)(string, string, void *);

    /**
    *@brief  重载[], 获取参数值
    *@return string &参数值
    */
    string &operator[](const string &sName);

    /**
    * @brief  读入字符串,并解析
    * @param sParam:字符串函数
    * @return 无
    */
    void load(const string &sParam);

    /**
    * @brief  读入map,并解析
    * @param mpParam:字符串函数
    * @return void
    */
    void load(const map<string, string> &mpParam);

    /**
    * @brief  转成字符串
    * @return string
    */
    string tostr() const;

    /**
    * @brief  获取参数值
    * @param sName 参数名称
    * @return string
    */
    string getValue(const string &sName) const;

    /**
    * @brief  设置参数值
    * @param sName 参数名称
    * @param sValue 参数值
    * @return void
    */
    void setValue(const string &sName, const string &sValue);

    /**
    * @brief  清除当前参数值对
    * return void
    */
    void clear();

    /**
    * @brief  引用方式获取参数map
    * @return map<string,string>& 返回参数map
    */
    map<string,string> &toMap();

    /**
    * @brief  引用方式获取参数map
    * @return map<string,string>& 返回参数map
    */
    const map<string,string> &toMap() const;

    /**
    * @brief  遍历每个参数值对
    * @param func: 函数
    * @param pParam: 参数,传入func中
    * @return void
    */
    void traverse(TC_ParseparaTraverseFunc func, void *pParam);

    /**
    * @brief  对字符串解码,%XX转成字符,类似于http协议的编码
    * @param sParam 参数
    * @return string,解码后的字符串
    */
    static string decodestr(const string &sParam);

    /**
    * @brief  对字符串编码,特殊字符转成%XX, 
    *         类似于http协议的编码(少了对空格=>+的特殊处理)
    * @param sParam 参数
    * @return string, 编码后的字符串
    */
    static string encodestr(const string &sParam);

protected:

    /**
    * @brief  字符串转换成map
    * @param sParam 参数
    * @param mpParam map
    * @return void
    */
    void decodeMap(const string &sParam, map<string, string> &mpParam) const;

    /**
    * @brief  map转换成字符串
    * @param mpParam map
    * @return string, 转换后的字符串
    */
    string encodeMap(const map<string, string> &mpParam) const;

    /**
    * @brief  "%xx" 转换为字符
    * @param sWhat: %xx字符串后面的两个字符
    * @return char 返回字符
    */
    static char x2c(const string &sWhat);

protected:

    map<string, string> _param;
};

}
#endif /*_TC_PARSEPARA_H*/
