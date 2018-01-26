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

#include "element.h"
#include <iostream>
#include <cassert>
#include "parse.h"
#include "util/tc_file.h"
#include "util/tc_common.h"

/*************************************************************************************************/
const char* Builtin::builtinTable[] =
    {
        "void",
        "bool",
        "byte",
        "short",
        "int",
        "long",
        "float",
        "double",
        "string",
        "vector",
        "map"
    };

Builtin::Builtin(Kind kind,bool isUnsigned = false) : _kind(kind),_isUnsigned(isUnsigned)
{
}

Builtin::Kind Builtin::kind() const
{
    return _kind;
}

bool Builtin::isSimple() const
{
    switch(_kind)
    {
    case Builtin::KindBool:
    case Builtin::KindByte:
    case Builtin::KindShort:
    case Builtin::KindInt:
    case Builtin::KindLong:
    case Builtin::KindFloat:
    case Builtin::KindDouble:
        return true;
    default:
        return false;
    }

    return true;
}

string Builtin::def() const
{
    switch(_kind)
    {
    case Builtin::KindBool:
        return "true";
    case Builtin::KindByte:
    case Builtin::KindShort:
    case Builtin::KindInt:
    case Builtin::KindLong:
    case Builtin::KindFloat:
    case Builtin::KindDouble:
        return "0";
    case Builtin::KindString:
        return "";
    default:
        assert(false);
    }

    return "";
}
/*************************************************************************************************/
Vector::Vector(const TypePtr& ptr) : _ptr(ptr)
{

}

/*************************************************************************************************/
Map::Map(const TypePtr& pleft, const TypePtr& pright): _pleft(pleft), _pright(pright)
{

}


/*************************************************************************************************/

TypeId::TypeId(const TypePtr& ptr, const string&id)
: _ptr(ptr)
, _id(id)
, _bRequire(true)
, _tag(0)
, _bHasDefault(false)
, _size(0)
, _array(false)

{
    BuiltinPtr bPtr = BuiltinPtr::dynamicCast(_ptr);
    if(bPtr)
    {
        _bHasDefault    = true;
        _default        = bPtr->def();
    }
}

void TypeId::setRequire(int tag)
{
    _bRequire   = true;
    _tag        = tag;
}

void TypeId::setDefault(const string &def)
{
    BuiltinPtr bPtr = BuiltinPtr::dynamicCast(_ptr);
	EnumPtr    ePtr = EnumPtr::dynamicCast(_ptr);
    assert(bPtr || ePtr);


    _bHasDefault    = true;
    _default        = def;
}

void TypeId::disableDefault()
{
    BuiltinPtr bPtr = BuiltinPtr::dynamicCast(_ptr);
    assert(bPtr);
    _bHasDefault    = false;
}

void TypeId::setOptional(int tag)
{
    _bRequire   = false;
    _tag        = tag;
}

/*************************************************************************************************/
NamespacePtr Container::createNamespace(const string &id)
{
    NamespacePtr np = NamespacePtr::dynamicCast(this);
    if(np)
    {
        g_parse->error("namespace can't be nested!");
    }

    _ns.push_back(new Namespace(id));

    g_parse->currentContextPtr()->addNamespacePtr(_ns.back());
    return _ns.back();
}

/*************************************************************************************************/
ParamDeclPtr Operation::createParamDecl(const TypeIdPtr &typeIdPtr, bool v, bool k)
{
    _itag++;
    if(_itag > 255)
    {
        g_parse->error("operation '" + _id + "' can't have more than 255 paramters!");
    }

    if(typeIdPtr->getId() == getId())
    {
        g_parse->error("paramters name '" + _id + "' conflicts with operation name!");
    }

    typeIdPtr->setRequire(_itag);
    _ps.push_back(new ParamDecl(typeIdPtr, v, k));
    return _ps.back();
}

/*************************************************************************************************/
OperationPtr Interface::createOperation(const string &id, const TypePtr &typePtr)
{
    for(size_t i = 0; i < _ops.size(); i++)
    {
        if(_ops[i]->getId() == id)
        {
            g_parse->error("operation '" + id + "' exists in interface '" + _id + "'");
            return NULL;
        }
    }

    _ops.push_back(new Operation(id, typePtr));
    return _ops.back();
}

/*************************************************************************************************/

InterfacePtr Namespace::createInterface(const string &id)
{
    for(size_t i = 0; i < _is.size(); i++)
    {
        if(_is[i]->getId() == id)
        {
            g_parse->error("interface '" + id + "' exists in namespace '" + _id + "'");
            return NULL;
        }
    }

    if(id == _id)
    {
        g_parse->error("interface '" + id + "' must not be same with namespace '" + _id + "'");
        return NULL;
    }

    _is.push_back(new Interface(id));
    return _is.back();
}

StructPtr Namespace::createStruct(const string& id)
{
    g_parse->checkConflict(_id + "::" + id);

    _ss.push_back(new Struct(id, _id + "::" + id));
    g_parse->addStructPtr(_ss.back());

    return _ss.back();
}

EnumPtr Namespace::createEnum(const string &id)
{
    g_parse->checkConflict(_id + "::" + id);

    _es.push_back(new Enum(id, _id + "::" + id));
    g_parse->addEnumPtr(_es.back());

    return _es.back();
}

ConstPtr Namespace::createConst(TypeIdPtr &pPtr, ConstGrammarPtr &cPtr)
{
    for(size_t i = 0; i < _cs.size(); i++)
    {
        if(_cs[i]->getTypeIdPtr()->getId() == pPtr->getId())
        {
            g_parse->error("const type '" + pPtr->getId() + "' exists in namespace '" + _id + "'");
            return NULL;
        }
    }

    g_parse->checkConstValue(pPtr, cPtr->t);
    _cs.push_back(new Const(pPtr, cPtr));

    return _cs.back();
}

/*************************************************************************************************/

void Struct::addTypeId(const TypeIdPtr &typeIdPtr)
{
    StructPtr sp = StructPtr::dynamicCast(typeIdPtr->getTypePtr());
    if(sp)
    {
        if(sp->getSid() == getSid())
        {
            g_parse->error("struct can't take self as member data");
        }
    }

    for(size_t i = 0; i < _members.size(); i++)
    {
        if(_members[i]->getId() == typeIdPtr->getId())
        {
            g_parse->error("data member '" + typeIdPtr->getId() + "' duplicate definition");
        }
        if(_members[i]->getTag() == typeIdPtr->getTag())
        {
            g_parse->error("data member '" + typeIdPtr->getId() + "' has equal tag with '" + _members[i]->getId() + "'");
        }

        if(_members[i]->getTag() > typeIdPtr->getTag())
        {
            _members.insert(_members.begin() + i, typeIdPtr);
            return;
        }
    }

    _members.push_back(typeIdPtr);
}

void Struct::addKey(const string &member)
{
    size_t i;
    for(i = 0; i < _members.size(); i++)
    {
        if(_members[i]->getId() == member)
        {
            break;
        }
    }

    if(i == _members.size())
    {
        g_parse->error("key member '" + member + "' is not struct member");
    }

    _key.push_back(member);
}
/***********************************************************************************/

void Enum::addMember(const TypeIdPtr &typeIdPtr)
{
    for(size_t i = 0; i < _members.size(); i++)
    {
        if(_members[i]->getId() == typeIdPtr->getId())
        {
            g_parse->error("enum member '" + typeIdPtr->getId() + "' duplicate definition");
        }
    }
    _members.push_back(typeIdPtr);
}

/***********************************************************************************/

void Context::addInclude(const string &incl)
{
    if(incl == _filename)
    {
        g_parse->error("can't include self");
    }
    _includes.push_back(tars::TC_File::excludeFileExt(incl) + ".h");
}


