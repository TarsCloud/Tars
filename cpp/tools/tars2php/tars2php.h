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
#ifndef _TARS2PHP_H
#define _TARS2PHP_H

#include "parse.h"

#include <cassert>
#include <string>

using namespace tars;

/**
 * 根据tars生成c++文件
 * 包括结构的编解码以及生成Proxy和Servant
 */
class Tars2Php
{
public:
	Tars2Php();

    /**
     * 生成
     * @param file
     */
    void createFile(const string &file, const vector<string> &vsCoder);


    /**
    * 设置生成文件的目录
    *
    */
    void setBaseDir(const std::string & sPath) { m_sBaseDir = sPath; }

	/**
	 * 设置是否需要打包默认值
	 */
	void setCheckDefault(bool bCheck) { m_bCheckDefault = bCheck; }

    //下面是编解码的源码生成
protected:

    /**
     * 生成某类型的解码源码
     * @param pPtr
     *
     * @return string
     */
    string writeTo(const TypeIdPtr &pPtr) const;

    /**
     * 生成某类型的编码源码
     * @param pPtr
     *
     * @return string
     */
    string readFrom(const TypeIdPtr &pPtr, bool bIsRequire = true) const;


    //下面是类型描述的源码生成
protected:
    /**
     * 生成某类型的字符串描述源码
     * @param pPtr
     *
     * @return string
     */
    string tostr(const TypePtr &pPtr) const;

    /**
     * 生成内建类型的字符串源码
     * @param pPtr
     *
     * @return string
     */
    string tostrBuiltin(const BuiltinPtr &pPtr) const;
    /**
     * 生成vector的字符串描述
     * @param pPtr
     *
     * @return string
     */
    string tostrVector(const VectorPtr &pPtr) const;

    /**
     * 生成map的字符串描述
     * @param pPtr
     *
     * @return string
     */
    string tostrMap(const MapPtr &pPtr) const;

    /**
     * 生成某种结构的符串描述
     * @param pPtr
     *
     * @return string
     */
    string tostrStruct(const StructPtr &pPtr) const;

    /**
     * 生成某种枚举的符串描述
     * @param pPtr
     *
     * @return string
     */
    string tostrEnum(const EnumPtr &pPtr) const;

    /**
     * 获取定长数组的坐标
     * @param pPtr
     *
     * @return string
     */

    string toStrSuffix(const TypeIdPtr &pPtr) const;

    /**
     * 获取定长数组的坐标
     * @param pPtr
     *
     * @return int
     */
    int getSuffix(const TypeIdPtr &pPtr) const;

    /**
     * 生成类型变量的解码源码
     * @param pPtr
     *
     * @return string
     */
    string decode(const TypeIdPtr &pPtr) const;

    /**
     * 生成类型变量的编码源码
     * @param pPtr
     *
     * @return string
     */
    string encode(const TypeIdPtr &pPtr) const;

    //以下是h和cpp文件的具体生成
protected:
    /**
     * 生成结构的头文件内容
     * @param pPtr
     *
     * @return string
     */
    string generatePHP(const StructPtr &pPtr, const string& namespaceId) const;


    /**
     * 生成名字空间的头文件源码
     * @param pPtr
     *
     * @return string
     */
    string generatePHP(const NamespacePtr &pPtr) const;


    /**
     * 生成名字空间的头文件源码
     * @param pPtr
     *
     * @return string
     */
    void generatePHP(const ContextPtr &pPtr) const;

    void generatePHP_Pdu(const ContextPtr &pPtr) const;

private:
    std::string m_sBaseDir;

	bool m_bCheckDefault;
};

#endif


