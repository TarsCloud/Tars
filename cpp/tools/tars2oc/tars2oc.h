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
#ifndef _TARS2OC_H
#define _TARS2OC_H

#include "parse.h"

#include <cassert>
#include <string>

using namespace tars;

/**
 * 根据tars生成h和m文件
 */
class Tars2OC
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
	 * @param bCheck
	 */
	void setArc(const bool bArc) { m_bARC = bArc; }
	/**
	 * 设置各个成员属性是否加上名字空间前缀
	 */
	void setNeedNS(bool bNeed) { m_bNeedNS = bNeed;}
    /**
	 * 设置用户自定义名字空间
	 */
	void setNS(const string& sNS) {m_sNamespace = sNS;}
    //下面是类型描述的源码生成
protected:
	/**
	 * 获取枚举或者结构体名字前缀字符串
	 * @param sTarsNS  tars文件中定义的名字空间
	 * @param bEnum 是否是枚举类型
	 *
	 * @return 处理后的前缀字符串
	 */
	string getNamePrix(const string& sTarsNS) const;
	
	/**
	 * 获取符合规范的成员名称
	 */
	string getPropertyName(const string& sId) const;

    /**
	 *
	 * 获取自定义setter的名字
	 */
	string getSetterName(const string& sId) const;
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
	 * 生成OBJC 版本2的属性定义
	 * @param pPtr
	 *
	 * @return string
	 */
	string toTarsV2Procstr(const TypeIdPtr &pPtr) const;

	/**
	 * 生成OBJC版本2的附加信息编码
	 * @param pPtr
	 *
	 * @return string
	 */
	string toAddtionalClassName(const TypePtr &pPtr) const;

	/**
	 * 生成结构体属性的synthesize定义
	 * @param vMember
	 *
	 * @return string
	 */
    string writesynthesize(const vector<TypeIdPtr>& vMember) const;

	/**
	 * 生成Initialize函数
	 * @param pPtr
	 *
	 * @return string
	 */
    string writeInitialize(const StructPtr &pPtr) const;

	/**
	 * 生成初始化定义函数init
	 * @param vMember
	 *
	 * @return string
	 */
	string writeInit(const vector<TypeIdPtr>& vMember) const;

	/**
	 * 生成析构定义
	 * @param vMember
	 *
	 * @return string
	 */
	string writedealloc(const vector<TypeIdPtr>& vMember) const;

	/**
	 * 生成结构名称定义
	 * @param pPtr
	 *
	 * @return string
	 */
	string writeTarsType(const StructPtr &pPtr) const;

	/**
	 * 生成类型标示符
	 * @param pPtr
	 *
	 * @return string
	 */
    string toClassName(const TypePtr &pPtr) const;
protected:
    /**
	 * 获取tars中定义的修饰类型，require和optional
	 * @param pPtr
	 *
	 */
	string getReqOptionType(const TypeIdPtr &pPtr) const;
	/**
	 * 判断类型是否属于retain类型，string和对象类型都属于retain
	 * @param pPtr
	 *
	 */
	bool IsRetainType(const TypePtr &pPtr) const;
	/**
	 * 获取结构体中依赖其它类型的标识符名称和类型，目前是结构体和枚举两种类型
	 *
	 * @param pPtr
	 * @param namespaceId
	 *
	 * @return
	 */
	map<string,int> getReferences(const StructPtr &pPtr, const string& namespaceId="") const;

	/**
	 * 获取成员对象依赖的类型标识符名称,
	 * 例如, stHello hello;stHello为类型标识符名称,类型为结构体
	 *
	 * @param pPtr
	 * @param mRefs
	 */
	void toIncludeName(const TypePtr &pPtr,map<string,int>& mRefs) const;

    /**
     * 生成结构的头文件内容
     * @param pPtr
     *
     * @return string
     */
    string generateH(const StructPtr &pPtr, const string& namespaceId) const;

    /**
     * 生成结构的m文件内容
     * @param pPtr
     *
     * @return string
     */
    string generateM(const StructPtr &pPtr, const string& namespaceId) const;

    /**
     * 生成枚举的m文件内容
     * @param pPtr
     *
     * @return string
     */
	string generateM(const EnumPtr &pPtr, const string& namespaceId) const;
    /**
     * 生成容器的头文件源码
     * @param pPtr
     *
     * @return string
     */
    string generateH(const ContainerPtr &pPtr) const;

    /**
     * 生成枚举的头文件源码
     * @param pPtr
     *
     * @return string
     */
    string generateH(const EnumPtr &pPtr, const string& namespaceId) const;

    /**
     * 生成名字空间的头文件和实现源码
     * @param pPtr
     *
     * @return string
     */
    string generate(const NamespacePtr &pPtr) const;

    /**
     * 生成名字空间m文件源码
     * @param pPtr
     *
     * @return string
     */
    string generateM(const NamespacePtr &pPtr) const;

    /**
     * 生成名字空间的头文件源码
     * @param pPtr
     *
     * @return string
     */
    void generate(const ContextPtr &pPtr) const;

    /**
     * 生成名字空间m文件源码
     * @param pPtr
     *
     * @return string
     */
    void generateM(const ContextPtr &pPtr) const;

private:
	enum EM_TYPE
	{
		EM_STRUCT_TYPE = 1,
		EM_ENUM_TYPE = 2
	};
    std::string m_sBaseDir;

    //<modulename,EM_TYPE>, 2:enum,1,stuct;
    mutable map<string,int> m_mIncludes;
	/**
	 * 是否支持arc版本
	 *
	 */
	bool m_bARC;
	/**
	 * 是否支持添加名字空间前缀
	 */
	bool m_bNeedNS;
	/**
	 *用户自定义的名字空间前缀，如果该值为空，默认是取tars文件中定义的值
	 */
	string m_sNamespace;
};

#endif


