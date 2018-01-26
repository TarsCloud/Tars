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
#ifndef _TARS2ANDROID_H
#define _TARS2ANDROID_H

#include "parse.h"

#include <cassert>
#include <string>

#define TARS_PACKAGE     ".tup.tars"
#define PROXY_PACKAGE   ".proxy"
#define WUP_PACKAGE     ".tup"

/**
 * 根据tars生成java文件
 * 包括结构的编解码以及生成Proxy和Servant
 */
class Tars2Java
{
public:
    Tars2Java();

    /**
     * 设置代码生成的根目录
     * @param dir
     */
    void setBaseDir(const string& dir);

    /**
     * 设置包前缀
     * @param prefix
     */
    void setBasePackage(const string& prefix);

    /**
    * 默认将非byte的vector转list 加上该参数后转数组
    */
    void setForceArray(bool bArray) { _bForceArray = bArray; }

    void setCheckDefault(bool bCheck) { _bCheckDefault = bCheck; }

    /*支持自定义字符集*/
    void setCharset(string charset) { _sCharset = charset; }
    /*支持javabean规范*/
    void setWithJbr(bool bJbr) { _bWithJbr = bJbr;}
    /*去掉属性的read write方法、 cloneable、equals、hashCode、clone、display*/
    void setWithCompact(bool bCompact) { _bWithCompact = bCompact;}

    /*生成枚举类时，采用int方式实现*/
    void setEnumCompact(bool bCompact) { _bEnumCompact = bCompact;}

    /*tars与json互相转换*/
    void setJsonSupport(bool bJson) { _bJson = bJson;}

    void setWithGenerateInterfaceDependencies(bool bWithGenerateInterfaceDependencies)
    {
        _bWithGenerateInterfaceDependencies = bWithGenerateInterfaceDependencies;
    }
    void setWithFilterRomTars(bool bWithFilterRomTars)
    {
        _bWithFilterRomTars = bWithFilterRomTars;
    }

// 增加的接口定义
    string generateConstructor(const TypePtr& vType) const;
    string generateAndroidJavaParams(const vector<ParamDeclPtr>& vParamDecl, bool needParamType, bool needOutParam) const;
    string generateAndroidStub(const InterfacePtr& pPtr, const NamespacePtr& nPtr) const;
    
    /**
     * 生成
     * @param file
     * @param isFramework 是否是框架
     */
    void createFile(const string& file);

    /**
     * 设置TAF库的报名
     */
    void setTafPacket(const std::string& sPacket)
    {
        s_TARS_PACKAGE 		= sPacket + TARS_PACKAGE;
        s_PROXY_PACKAGE		= sPacket + PROXY_PACKAGE;
        s_WUP_PACKAGE		= sPacket + WUP_PACKAGE;
    }

protected:
    /**
     * 根据命名空间获取文件路径
     * @param ns 命名空间
     *
     * @return string
     */
    string getFilePath(const string& ns) const;

    string 	_packagePrefix;
    string 	_baseDir;
    bool  	_bForceArray;
    bool    _bCheckDefault;
    string  _sCharset;
    bool    _bWithJbr;
    bool    _bWithCompact;
    bool    _bEnumCompact;
    bool    _bJson;

    bool	_bWithGenerateInterfaceDependencies;
    bool	_bWithFilterRomTars;

protected:

    /**
     * 生成某类型的解码源码
     * @param pPtr
     *
     * @return string
     */
    string writeTo(const TypeIdPtr& pPtr) const;

    /**
     * 生成某类型的编码源码
     * @param pPtr
     *
     * @return string
     */
    string readFrom(const TypeIdPtr& pPtr) const;

    /**
     * 
     * 这个函数目前是用不到的
     * @param pPtr
     * 
     * @return string
     */
    string display(const TypeIdPtr& pPtr) const;

    //下面是类型描述的源码生成
protected:

    /*
     * 生成某类型的初始化字符串
     * @param pPtr
     *
     * @return string
     */
    string toTypeInit(const TypePtr& pPtr) const;

    /**
     * 生成某类型的对应对象的字符串描述源码
     * @param pPtr
     *
     * @return string
     */
    string toObjStr(const TypePtr& pPtr) const;

    /**
     * 判断是否是对象类型
     */
    bool isObjType(const TypePtr& pPtr) const;

    /**
     * 生成某类型的字符串描述源码
     * @param pPtr
     *
     * @return string
     */
    string tostr(const TypePtr& pPtr) const;

    /**
     * 生成内建类型的字符串源码
     * @param pPtr
     *
     * @return string
     */
    string tostrBuiltin(const BuiltinPtr& pPtr) const;
    /**
     * 生成vector的字符串描述
     * @param pPtr
     *
     * @return string
     */
    string tostrVector(const VectorPtr& pPtr) const;

    /**
     * 生成map的字符串描述
     * @param pPtr
     *
     * @return string
     */
    string tostrMap(const MapPtr& pPtr, bool bNew = false) const;

    /**
     * 生成某种结构的符串描述
     * @param pPtr
     *
     * @return string
     */
    string tostrStruct(const StructPtr& pPtr) const;

    /**
     * 生成某种枚举的符串描述
     * @param pPtr
     *
     * @return string
     */
    string tostrEnum(const EnumPtr& pPtr) const;

    /**
     * 生成类型变量的解码源码
     * @param pPtr
     *
     * @return string
     */
    string decode(const TypeIdPtr& pPtr) const;

    /**
     * 生成类型变量的编码源码
     * @param pPtr
     *
     * @return string
     */
    string encode(const TypeIdPtr& pPtr) const;

    //以下是h和java文件的具体生成
protected:
    /**
     * 结构的md5
     * @param pPtr
     *
     * @return string
     */
    string MD5(const StructPtr& pPtr) const;

    /**
     * 生成结构的Holder类，用于引用传递
     * @param pPtr
     * @param nPtr
     *
     * @return string
     */
    string generateHolder(const StructPtr& pPtr, const NamespacePtr& nPtr) const;

    /**
     * 放置默认元素用于识别map/list类型
     * @param pPtr
     * @param sElemName 元素名称
     *
     * @return string
     */
    string generateDefautElem(const TypePtr& pPtr, const string& sElemName) const;

    /**
     * 生成结构的java文件内容
     * @param pPtr
     *
     * @return string
     */
    string generateJava(const StructPtr& pPtr, const NamespacePtr& nPtr) const;

    /**
     * 生成容器的java源码
     * @param pPtr
     *
     * @return string
     */
    string generateJava(const ContainerPtr& pPtr) const;

    /**
     * 生成参数声明的java文件内容
     * @param pPtr
     *
     * @return string
     */
    string generateJava(const ParamDeclPtr& pPtr) const;

    /**
     * 生成作操作的proxy的java文件内容
     * @param pPtr
     * @param cn
     *
     * @return string
     */
    string generateJava(const OperationPtr& pPtr, const string& cn) const;

    /**
     * 生成操作java文件函数调用分发的源码
     * @param pPtr
     * @param cn
     *
     * @return string
     */
    string generateDispatchJava(const OperationPtr& pPtr, const string& cn) const;

    /**
     * 生成接口的java文件的源码
     * @param pPtr
     * @param nPtr
     *
     * @return string
     */
    string generateJava(const InterfacePtr& pPtr, const NamespacePtr& nPtr) const;

    /**
     * 生成Proxy接口的java文件的源码
     * @param pPtr
     * @param nPtr
     *
     * @return string
     */
    string generatePrx(const InterfacePtr& pPtr, const NamespacePtr& nPtr) const;

    /**
     * 生成Proxy帮助类的java文件的源码
     * @param pPtr
     * @param nPtr
     *
     * @return string
     */
    string generatePrxHelper(const InterfacePtr& pPtr, const NamespacePtr& nPtr) const;

    /**
     * 生成Proxy回调类的java文件的源码
     * @param pPtr
     * @param nPtr
     *
     * @return string
     */
    string generatePrxCallback(const InterfacePtr& pPtr, const NamespacePtr& nPtr) const;

    /**
     * 生成枚举的头文件源码
     * @param pPtr
     *
     * @return string
     */
    string generateJava(const EnumPtr& pPtr, const NamespacePtr& nPtr) const;

    /**
     * 生成常量java源码
     * @param pPtr
     * 
     * @return string
     */
    void generateJava(const ConstPtr& pPtr, const NamespacePtr& nPtr) const;

    /**
     * 生成名字空间java文件源码
     * @param pPtr
     *
     * @return string
     */
    void generateJava(const NamespacePtr& pPtr) const;

    /**
     * 生成每个tars文件的java文件源码
     * @param pPtr
     *
     * @return string
     */
    void generateJava(const ContextPtr& pPtr) const;

private:
    std::string s_TARS_PACKAGE;
    std::string s_PROXY_PACKAGE;
    std::string s_HOLDER_PACKAGE;
    std::string s_SERVER_PACKAGE;
    std::string s_WUP_PACKAGE;
};

#endif


