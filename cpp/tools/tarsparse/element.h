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

#ifndef _ELEMENT_H
#define _ELEMENT_H

#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <iterator>
#include <map>
#include <stack>
#include <sstream>
#include "errno.h"
#include "util/tc_autoptr.h"

using namespace std;


void yyerror(char const *msg);

/**
 * 语法元素基类
 */
class GrammarBase : virtual public tars::TC_HandleBase
{
public:
    virtual ~GrammarBase(){};
};

typedef tars::TC_AutoPtr<GrammarBase> GrammarBasePtr;

/**
 * 解析过程中的字符串
 */
class StringGrammar : public GrammarBase
{
public:

    StringGrammar() { }
    string v;
};

typedef tars::TC_AutoPtr<StringGrammar> StringGrammarPtr;

/**
 * 解析过程中的整形数
 */
class IntergerGrammar : public GrammarBase
{
public:

    IntergerGrammar():v(0) { }
    long long v;
};

typedef tars::TC_AutoPtr<IntergerGrammar> IntergerGrammarPtr;

/**
 * 解析过程中的浮点数
 */
class FloatGrammar : public GrammarBase
{
public:

    FloatGrammar():v(0.0f) { }
    double v;
};

typedef tars::TC_AutoPtr<FloatGrammar> FloatGrammarPtr;

/**
 * 解析过程中的bool类型
 */
class BoolGrammar : public GrammarBase
{
public:

    BoolGrammar():v(false) { }
    bool v;
};

typedef tars::TC_AutoPtr<BoolGrammar> BoolGrammarPtr;

/**
 * 解析过程中的常类型
 */
class ConstGrammar : public GrammarBase
{
public:

    ConstGrammar():t(VALUE) { }

    enum
    {
        VALUE,
        STRING,
        BOOL,
        ENUM
    };

    int    t;
    string v;
};

typedef tars::TC_AutoPtr<ConstGrammar> ConstGrammarPtr;

///////////////////////////////////////////////

/**
 * 类型基类
 */
class Type : virtual public GrammarBase
{
public:
    /**
     * 构造函数
     * @param Type
     */
    Type():_size(0),_array(false),_pointer(false){};
    
    //是否简单类型
    virtual bool isSimple() const { return false;}

    //是否数组类型
    virtual bool isArray() const { return _array;}
    virtual void setArray(int size) {_array = true;_size = size;}
    virtual int  getSize() const {return _size;};

    //是否指针类型
    virtual bool isPointer() const { return _pointer;}
    virtual void setPointer(bool b) {_pointer = b;}
public:
    int     _size;
    bool    _array;
    bool    _pointer;
};

typedef tars::TC_AutoPtr<Type> TypePtr;

/**
 * 内建类型
 */
class Builtin : public Type
{
public:

    enum Kind
    {
        KindVoid,
        KindBool,
        KindByte,
        KindShort,
        KindInt,
        KindLong,
        KindFloat,
        KindDouble,
        KindString,
        KindVector,
        KindMap
    };

    /**
     * 构造函数
     * @param kind
     */
    Builtin(Kind kind,bool isUnsigned);

    /**
     * 类型
     *
     * @return Kind
     */
    Kind kind() const;

    /**
     * 是否是简单类型
     *
     * @return bool
     */
    bool isSimple() const;

    /**
     * 是否是unsined类型
     *
     * @return bool
     */
    bool isUnsigned() const {return _isUnsigned;}

    /**
     * 设置是unsined类型
     *
     * @return bool
     */
    void setUnsigned(bool isUnsigned = false) {_isUnsigned = isUnsigned;}

    /**
     * 缺省值
     *
     * @return string
     */
    string def() const;

    /**
     * 字符串标示
     */
    static const char* builtinTable[];

protected:

    Kind _kind;
    bool _isUnsigned;
};

typedef tars::TC_AutoPtr<Builtin> BuiltinPtr;

/**
 * Vector类型
 */
class Vector : public Type
{
public:
    /**
     * 构造函数
     * @param ptr
     */
    Vector(const TypePtr& ptr);

    /**
     * 获取类型
     *
     * @return TypePtr&
     */
    TypePtr& getTypePtr() {return _ptr;}
protected:
    TypePtr _ptr;
};

typedef tars::TC_AutoPtr<Vector> VectorPtr;

/**
 * Map类型
 */
class Map : public Type
{
public:
    /**
     * 构造函数
     * @param pleft
     * @param pright
     */
    Map(const TypePtr& pleft, const TypePtr& pright);

    /**
     * 获取左类型
     *
     * @return TypePtr&
     */
    TypePtr& getLeftTypePtr() {return _pleft;}

    /**
     * 获取右类型
     *
     * @return TypePtr&
     */
    TypePtr& getRightTypePtr() {return _pright;}

protected:
    TypePtr _pleft;
    TypePtr _pright;
};

typedef tars::TC_AutoPtr<Map> MapPtr;

/**
 * 变量定义
 */
class TypeId : public GrammarBase
{
public:
    /**
     * 构造函数
     * @param ptr
     * @param id
     */
    TypeId(const TypePtr& ptr, const string& id);

    /**
     * 变量名称
     *
     * @return string
     */
    string getId() const { return _id; }

    /**
     * 变量类型
     *
     * @return Type*
     */
    TypePtr& getTypePtr() { return _ptr;}

    /**
     * 是否需要该参数
     *
     * @return bool
     */
    bool isRequire() const { return _bRequire; }

    /**
     * 设置必选字段
     * @param tag
     */
    void setRequire(int tag);

    /**
     * 设置可选字段
     * 只有基本类型才有缺省值
     */
    void setDefault(const string &def);

    /** 
     *  
     * 只有基本类型才有缺省值
     */
    void disableDefault();

    /**
     * 设置可选字段
     * @param tag
     */
    void setOptional(int tag);

    /**
     * tag标识
     */
    int getTag() const { return _tag; }

    /**
     * 缺省值
     *
     * @return string
     */
    string def() const { return _default; }

    /**
     * 是否有缺省值
     *
     * @return bool
     */
    bool hasDefault() const { return _bHasDefault; }

protected:
    TypePtr _ptr;
    string  _id;
    bool    _bRequire;
    int     _tag;
    bool    _bHasDefault;
    string  _default;

public:
    int     _size;
    bool    _array;
};

typedef tars::TC_AutoPtr<TypeId> TypeIdPtr;


////////////////////////////////////////////////////
//
class Namespace;
typedef tars::TC_AutoPtr<Namespace> NamespacePtr;
/**
 * 容器基类, 所有可以包含其他元素的元素都从该基类继承
 */
class Container : virtual public GrammarBase
{
public:
    /**
     * 构造函数
     * @param id
     */
    Container(const string &id) : _id(id)
    {
    }

    /**
     * 生成名字空间
     * @param id
     *
     * @return NamespacePtr
     */
    NamespacePtr createNamespace(const string &id);

    /**
     * 获取ID
     *
     * @return string
     */
    string getId() const { return _id;}

    /**
     * 获取所有的名字空间
     *
     * @return vector<NamespacePtr>&
     */
    vector<NamespacePtr> &getAllNamespacePtr() { return _ns; }
protected:
    string                  _id;
    vector<NamespacePtr>    _ns;
};

typedef tars::TC_AutoPtr<Container> ContainerPtr;

////////////////////////////////////////////////////
//
class Const : public GrammarBase
{
public:
    /**
     * 
     * @param tPtr
     * @param cPtr
     */
    Const(TypeIdPtr &tPtr, ConstGrammarPtr &cPtr) : _tPtr(tPtr), _cPtr(cPtr)
    {
    }

    /**
     * 
     * 
     * @return TypePtr&
     */
    TypeIdPtr &getTypeIdPtr()       { return _tPtr; }

    /**
     * 
     * 
     * @return ConstGrammarPtr
     */
    ConstGrammarPtr getConstGrammarPtr()    { return _cPtr; }

protected:
    TypeIdPtr   _tPtr;
    ConstGrammarPtr _cPtr;
};

typedef tars::TC_AutoPtr<Const> ConstPtr;

/////////////////////////////////////////////////////////
//
class Enum : virtual public Container, virtual public Type
{
public:
    Enum(const string &id, const string &sid) : Container(id), _sid(sid)
    {
    }

    /**
     * 添加成员变量
     * @param ptid
     */
    void addMember(const TypeIdPtr &ptr);

    /**
     * 获取所有的成员变量
     *
     * @return vector<TypeIdPtr>&
     */
    vector<TypeIdPtr>& getAllMemberPtr() {return _members;}

    /**
     * 获取结构的名称
     *
     * @return string
     */
    string getSid() const { return _sid; }

    /**
     * 是否是简单类型
     * 
     * @return bool
     */
    virtual bool isSimple() const { return true; }

protected:

    /**
     * 每个变量名称
     */
    vector<TypeIdPtr>   _members;

    /**
     * 包含名字的空间的名称
    */
    string          _sid;
};

typedef tars::TC_AutoPtr<Enum> EnumPtr;

////////////////////////////////////////////////////
//
/**
 * 结构
 */
class Struct : virtual public Container, virtual public Type
{
public:
    /**
     * 构造函数
     * @param id
     */
    Struct(const string& id, const string &sid) : Container(id), _sid(sid)
    {
    }

    /**
     * 添加成员变量
     * @param ptid
     */
    void addTypeId(const TypeIdPtr &ptr);

    /**
     * 获取所有的成员变量
     *
     * @return vector<TypeIdPtr>&
     */
    vector<TypeIdPtr>& getAllMemberPtr() {return _members;}

    /**
     * 获取结构的名称
     *
     * @return string
     */
    string getSid() const { return _sid; }

    /**
     * 增加小于memeber
     * @param member
     */
    void addKey(const string &member);

    vector<string> getKey() { return _key; }
protected:
    vector<TypeIdPtr>   _members;
    vector<string>      _key;
    string              _sid;
};

typedef tars::TC_AutoPtr<Struct> StructPtr;

////////////////////////////////////////////////////
//
/**
 * 参数描述
 */
class ParamDecl : public GrammarBase
{
public:
    /**
     * 构造
     * @param typeIdPtr
     * @param v
     */
    ParamDecl(const TypeIdPtr &typeIdPtr, bool v, bool k) 
    : _typeIdPtr(typeIdPtr), _v(v), _k(k)
    {
    }

    /**
     * 变量声明
     *
     * @return TypeIdPtr&
     */
    TypeIdPtr& getTypeIdPtr() { return _typeIdPtr; }

    /**
     * 是否是输出参数
     *
     * @return bool
     */
    bool    isOut() const { return _v; }

    /**
     * 是否是需要路由的字段
     *
     * @return bool
     */
    bool    isRouteKey() const { return _k; }

protected:
    TypeIdPtr   _typeIdPtr;
    bool        _v;
    bool        _k;
};
typedef tars::TC_AutoPtr<ParamDecl> ParamDeclPtr;

///////////////////////////////////////////////////////
//
/**
 * 操作类
 */
class Operation : public Container
{
public:
    /**
     * 构造函数
     * @param id
     * @param typePtr
     */
    Operation(const string &id, const TypePtr &typePtr) : Container(id), _itag(0)
    {
        _retPtr = new TypeId(typePtr, "_ret");
        _retPtr->setRequire(_itag);
    }

    /**
     * 生成一个参数
     * @param typeIdPtr
     * @param v
     * @param k
     *
     * @return ParamDeclPtr
     */
    ParamDeclPtr createParamDecl(const TypeIdPtr &typeIdPtr, bool v, bool k);

    /**
     * 获取返回值类型
     *
     * @return TypePtr&
     */
    TypeIdPtr &getReturnPtr() { return _retPtr; }

    /**
     * 获取所有参数
     *
     * @return vector<ParamDeclPtr>&
     */
    vector<ParamDeclPtr> &getAllParamDeclPtr() { return _ps; }
protected:
    int                     _itag;
    TypeIdPtr               _retPtr;
    vector<ParamDeclPtr>    _ps;
};

typedef tars::TC_AutoPtr<Operation> OperationPtr;
///////////////////////////////////////////////////////
//
/**
 * 接口描述
 */
class Interface : public Container
{
public:
    /**
     * 构造
     * @param id
     */
    Interface(const string &id) : Container(id)
    {
    }

    /**
     * 生成一个操作
     * @param id
     * @param typePtr
     *
     * @return OperationPtr
     */
    OperationPtr createOperation(const string &id, const TypePtr &typePtr);

    /**
     * 获取所有操作
     *
     * @return vector<OperationPtr>&
     */
    vector<OperationPtr> &getAllOperationPtr() { return _ops; }
protected:
    vector<OperationPtr>    _ops;
};

typedef tars::TC_AutoPtr<Interface> InterfacePtr;

/////////////////////////////////////////////////////////
//
/**
 * 名字空间
 */
class Namespace : public Container
{
public:
    /**
     * 构造函数
     * @param id
     */
    Namespace(const string &id) : Container(id)
    {
    }

    /**
     * 生成接口
     * @param id
     *
     * @return InterfacePtr
     */
    InterfacePtr createInterface(const string &id);

    /**
     * 生成结构
     * @param id
     *
     * @return StructPtr
     */
    StructPtr createStruct(const string& id);

    /**
     * 生成枚举类型
     * @param id
     * 
     * @return EnumPtr
     */
    EnumPtr createEnum(const string &id);

    /**
     * 
     * @param pPtr
     * @param cPtr
     * 
     * @return ConstPtr
     */
    ConstPtr createConst(TypeIdPtr &pPtr, ConstGrammarPtr &cPtr);

    /**
     * 是否有接口
     *
     * @return bool
     */
    bool hasInterface() const { return !_is.empty(); }

    /**
     * 获取所有的接口
     *
     * @return vector<InterfacePtr>&
     */
    vector<InterfacePtr> &getAllInterfacePtr() { return _is; }

    /**
     * 获取所有的结构
     *
     * @return vector<StructPtr>&
     */
    vector<StructPtr> &getAllStructPtr() { return _ss; }

    /**
     * 生成枚举类型
     * 
     * @return vector<EnumPtr>&
     */
    vector<EnumPtr> &getAllEnumPtr() { return _es; }

    /**
     * 常量类型
     * 
     * @return vector<ConstPtr>&
     */
    vector<ConstPtr> &getAllConstPtr()     { return _cs; }

protected:
    vector<InterfacePtr>    _is;
    vector<StructPtr>       _ss;
    vector<EnumPtr>         _es;
    vector<ConstPtr>        _cs;
};

/////////////////////////////////////////////////////////////////
/**
 * 上下文
 */
class Context : public tars::TC_HandleBase
{
public:
    /**
     * 构造函数
     */
    Context(const string &file) : _currline(1), _filename(file)
    {
    }

    /**
     * 下一行
     */
    void nextLine() { _currline++; }

    /**
     * 目前的行
     * 
     * @return size_t
     */
    size_t getCurrLine() { return _currline; }

    /**
     * 当前文件名
     * 
     * @return string
     */
    string getFileName() { return _filename; }

    /**
     * 添加include的文件
     * @param incl
     */
    void addInclude(const string &incl);

    /**
     * 添加属于这个文件的名字空间
     * @param c
     */
    void addNamespacePtr(const NamespacePtr &c)
    {
        _namespaces.push_back(c);
    }

    /**
     * 获取includes的文件
     * 
     * @return vector<string>
     */
    vector<string> getIncludes() { return _includes; }

    /**
     * 获取名字空间
     * 
     * @return vector<NamespacePtr>
     */
    vector<NamespacePtr> getNamespaces() { return _namespaces; }

protected:
    size_t                  _currline;
    string                  _filename;
    vector<string>          _includes;
    vector<NamespacePtr>    _namespaces;
};

typedef tars::TC_AutoPtr<Context> ContextPtr;

#endif


