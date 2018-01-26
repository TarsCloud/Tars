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
#ifndef _TARS2C_H
#define _TARS2C_H

#include "parse.h"

#include <cassert>
#include <string>

using namespace tars;

/**
 * 根据tars生成c文件
 * 包括结构的编解码以及生成Proxy和Servant
 */
class Tars2C
{
public:

    /**
     * 生成
     * @param file
     */
    void createFile(const string &file);


    /**
    * 设置生成文件的目录
    *
    */
    void setBaseDir(const std::string & sPath) { m_sBaseDir = sPath; }


	/**
	 * 设置是否检查默认值
	 * 
	 * @author kevintian (2010-10-8)
	 * 
	 * @param bCheck 
	 */
	void setCheckDefault(const bool bCheck) { m_bCheckDefault = bCheck; }

    //下面是编解码的源码生成
protected:

    /**
     * 生成某类型的解码源码
     * @param pPtr
     *
     * @return string
     */
    string writeTo(const TypeIdPtr &pPtr,const string& namespaceId = "") const;

    /**
     * 生成某类型的编码源码
     * @param pPtr
     *
     * @return string
     */
    string readFrom(const TypeIdPtr &pPtr, bool bIsRequire = true,const string& namespaceId="") const;

    /**
     *
     * @param pPtr
     *
     * @return string
     */
    string display(const TypeIdPtr &pPtr) const;

    /**
     *
     * @param pPtr
     *
     * @return string
     */
    string displaySimple(const TypeIdPtr &pPtr, bool bSep) const;

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
     * 生成接口名称
     * @param pPtr
     *
     * @return string
     */

    string toFuncName(const TypePtr &pPtr) const;

    string toClassName(const TypePtr &pPtr) const;
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

    //以下是h和c文件的具体生成
protected:
    /**
     * 结构的md5
     * @param pPtr
     *
     * @return string
     */
    string MD5(const StructPtr &pPtr) const;

    /**
     * 生成结构的头文件内容
     * @param pPtr
     *
     * @return string
     */
    string generateH(const StructPtr &pPtr, const string& namespaceId) const;

    /**
     * 生成结构的cpp文件内容
     * @param pPtr
     *
     * @return string
     */
    string generateC(const StructPtr &pPtr, const string& namespaceId) const;

    /**
     * 生成容器的头文件源码
     * @param pPtr
     *
     * @return string
     */
    string generateH(const ContainerPtr &pPtr) const;

    /**
     * 生成容器的c源码
     * @param pPtr
     *
     * @return string
     */
    string generateCpp(const ContainerPtr &pPtr) const;

    /**
     * 生成参数声明的头文件内容
     * @param pPtr
     *
     * @return string
     */
    string generateH(const ParamDeclPtr &pPtr) const;

    /**
     *
     * @param pPtr
     *
     * @return string
     */
    string generateOutH(const ParamDeclPtr &pPtr) const;

    /**
     *
     * @param pPtr
     * @param cn
     *
     * @return string
     */
    //string generateAsyncResponseCpp(const OperationPtr &pPtr, const string &cn) const;

    /**
     * 生成参数声明的cpp文件内容
     * @param pPtr
     *
     * @return string
     */
    string generateCpp(const ParamDeclPtr &pPtr) const;

    /**
     * 生成作操作的proxy的cpp文件内容
     * @param pPtr
     * @param cn
     *
     * @return string
     */
    string generateCpp(const OperationPtr &pPtr, const string &cn) const;

    /**
     * 生成操作的servant的头文件源码
     * @param pPtr
     * @param bVirtual
     *
     * @return string
     */
    string generateH(const OperationPtr &pPtr, bool bVirtual, const string& interfaceId) const;

    /**
     * 生成接口的头文件源码
     * @param pPtr
     *
     * @return string
     */
    string generateH(const InterfacePtr &pPtr) const;

    /**
     * 生的接口的cpp文件的源码
     * @param pPtr
     *
     * @return string
     */
    string generateCpp(const InterfacePtr &pPtr, const NamespacePtr &nPtr) const;

    /**
     * 生成枚举的头文件源码
     * @param pPtr
     *
     * @return string
     */
    string generateH(const EnumPtr &pPtr, const string& namespaceId) const;

    /**
     * 生成常量头文件源码
     * @param pPtr
     *
     * @return string
     */
    string generateH(const ConstPtr &pPtr) const;

    /**
     * 生成名字空间的头文件源码
     * @param pPtr
     *
     * @return string
     */
    string generateH(const NamespacePtr &pPtr) const;

    /**
     * 生成名字空间cpp文件源码
     * @param pPtr
     *
     * @return string
     */
    string generateC(const NamespacePtr &pPtr) const;

    /**
     * 生成名字空间的头文件源码
     * @param pPtr
     *
     * @return string
     */
    void generateH(const ContextPtr &pPtr) const;

    /**
     * 生成名字空间cpp文件源码
     * @param pPtr
     *
     * @return string
     */
    void generateC(const ContextPtr &pPtr) const;

    /**
     *  
     * 根据名字查找结构 
     * @param pPtr 
     * @param id 
     * 
     * @return StructPtr 
     */
    StructPtr findStruct(const ContextPtr &pPtr,const string &id);

    std::string m_sBaseDir;


	/**
	 * 是否检查默认值
	 * 
	 * @author kevintian (2010-10-8)
	 */
	bool m_bCheckDefault;
};

#endif


