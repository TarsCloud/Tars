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

#include "code_generator.h"

string CodeGenerator::toFunctionName(const TypeIdPtr& pPtr, const string& sAction)
{
    BuiltinPtr bPtr = BuiltinPtr::dynamicCast(pPtr->getTypePtr());
    if (bPtr)
    {
        switch (bPtr->kind())
        {
            case Builtin::KindBool   : return sAction + "Boolean";
            case Builtin::KindString : return sAction + "String";
            case Builtin::KindByte   : return sAction + "Int8";
            case Builtin::KindShort  : return sAction + (bPtr->isUnsigned() ? "UInt8"  : "Int16");
            case Builtin::KindInt    : return sAction + (bPtr->isUnsigned() ? "UInt16" : "Int32");
            case Builtin::KindLong   : return sAction + (bPtr->isUnsigned() ? "UInt32" : "Int64");
            case Builtin::KindFloat  : return sAction + "Float";
            case Builtin::KindDouble : return sAction + "Double";
            default                  : assert(false);
        }
    }

    VectorPtr vPtr = VectorPtr::dynamicCast(pPtr->getTypePtr());
    if (vPtr)
    {
        BuiltinPtr vbPtr = BuiltinPtr::dynamicCast(vPtr->getTypePtr());
        if (vbPtr && vbPtr->kind() == Builtin::KindByte)
        {
            return sAction + "Bytes";
        }

        return sAction + "List";
    }

    StructPtr sPtr = StructPtr::dynamicCast(pPtr->getTypePtr());
    if (sPtr)
    {
        return sAction + "Struct";
    }

    EnumPtr ePtr = EnumPtr::dynamicCast(pPtr->getTypePtr());
    if (ePtr)
    {
        return sAction + "Int32";
    }

    MapPtr mPtr = MapPtr::dynamicCast(pPtr->getTypePtr());
    if (mPtr)
    {
        return sAction + "Map";
    }

    return "";
}

bool CodeGenerator::isRawOrString(const TypePtr& pPtr) const
{
    BuiltinPtr bPtr = BuiltinPtr::dynamicCast(pPtr);
    if (bPtr)
    {
        if (_bUseStringRepresent && bPtr->kind() == Builtin::KindLong)
        {
            return true;
        }

        if (_bStringBinaryEncoding && bPtr->kind() == Builtin::KindString)
        {
            return true;
        }
    }

    return false;
}

string CodeGenerator::getDataType(const TypePtr & pPtr)
{
    BuiltinPtr bPtr = BuiltinPtr::dynamicCast(pPtr);
    if (bPtr)
    {
        switch (bPtr->kind())
        {
            case Builtin::KindBool   : return IDL_NAMESPACE_STR + "Stream.Boolean";
            case Builtin::KindString : return IDL_NAMESPACE_STR + "Stream.String";
            case Builtin::KindByte   : return IDL_NAMESPACE_STR + "Stream.Int8";
            case Builtin::KindShort  : return IDL_NAMESPACE_STR + "Stream.Int16";
            case Builtin::KindInt    : return IDL_NAMESPACE_STR + "Stream.Int32";
            case Builtin::KindLong   : return IDL_NAMESPACE_STR + "Stream.Int64";
            case Builtin::KindFloat  : return IDL_NAMESPACE_STR + "Stream.Float";
            case Builtin::KindDouble : return IDL_NAMESPACE_STR + "Stream.Double";
            default                  : assert(false);
        }
    }

    VectorPtr vPtr = VectorPtr::dynamicCast(pPtr);
    if (vPtr)
    {
        BuiltinPtr vbPtr = BuiltinPtr::dynamicCast(vPtr->getTypePtr());
        if (vbPtr && vbPtr->kind() == Builtin::KindByte)
        {
            return IDL_NAMESPACE_STR + "Stream.BinBuffer";
        }

        return IDL_NAMESPACE_STR + "Stream.List(" + getDataType(vPtr->getTypePtr()) + (isRawOrString(vPtr->getTypePtr()) ? ", 1" : "") + ")";
    }

    StructPtr sPtr = StructPtr::dynamicCast(pPtr);
    if (sPtr)
    {
        vector<string> vecNames = TC_Common::sepstr<string>(sPtr->getSid(), "::");
        assert(vecNames.size() == 2);

        return findName(vecNames[0], vecNames[1]);
    }

    MapPtr mPtr = MapPtr::dynamicCast(pPtr);
    if (mPtr)
    {
        bool bLeft = isRawOrString(mPtr->getLeftTypePtr());
        bool bRight = isRawOrString(mPtr->getRightTypePtr());

        if (!bRight && !bLeft) {
            return IDL_NAMESPACE_STR + "Stream.Map(" + getDataType(mPtr->getLeftTypePtr()) + ", " + getDataType(mPtr->getRightTypePtr()) + ")";
        } else if (bRight && bLeft) {
            return IDL_NAMESPACE_STR + "Stream.Map(" + getDataType(mPtr->getLeftTypePtr()) + ", " + getDataType(mPtr->getRightTypePtr()) + ", 1, 1)";
        } else if (bRight) {
            return IDL_NAMESPACE_STR + "Stream.Map(" + getDataType(mPtr->getLeftTypePtr()) + ", " + getDataType(mPtr->getRightTypePtr()) + ", 0, 1)";
        } else if (bLeft) {
            return IDL_NAMESPACE_STR + "Stream.Map(" + getDataType(mPtr->getLeftTypePtr()) + ", " + getDataType(mPtr->getRightTypePtr()) + ", 1)";
        } else {
            assert(false);
        }
    }

    EnumPtr ePtr = EnumPtr::dynamicCast(pPtr);
    if (ePtr)
    {
        vector<string> vecNames = TC_Common::sepstr<string>(ePtr->getSid(), "::");
        assert(vecNames.size() == 2);

        return findName(vecNames[0], vecNames[1]);
    }

    assert(false);
    return "";
}

string CodeGenerator::getDtsType(const TypePtr &pPtr, const bool bStream)
{
    BuiltinPtr bPtr = BuiltinPtr::dynamicCast(pPtr);
    if (bPtr)
    {
        switch (bPtr->kind())
        {
            case Builtin::KindBool   : return "boolean";
            case Builtin::KindString : return _bStringBinaryEncoding ? (bStream ? (IDL_NAMESPACE_STR + "Stream.BinBuffer") : "Buffer") : "string";
            case Builtin::KindByte   : return "number";
            case Builtin::KindShort  : return "number";
            case Builtin::KindInt    : return "number";
            case Builtin::KindLong   : return _bUseStringRepresent ? "string" : "number";
            case Builtin::KindFloat  : return "number";
            case Builtin::KindDouble : return "number";
            default                  : assert(false);
        }
    }

    VectorPtr vPtr = VectorPtr::dynamicCast(pPtr);
    if (vPtr)
    {
        BuiltinPtr vbPtr = BuiltinPtr::dynamicCast(vPtr->getTypePtr());
        if (vbPtr && vbPtr->kind() == Builtin::KindByte)
        {
            return bStream ? (IDL_NAMESPACE_STR + "Stream.BinBuffer") : "Buffer";
        }
        return (bStream ? (IDL_NAMESPACE_STR + "Stream.List") : "Array") + string("<") + getDtsType(vPtr->getTypePtr(), bStream) + string(">");
    }

    StructPtr sPtr = StructPtr::dynamicCast(pPtr);
    if (sPtr)
    {
        vector<string> vecNames = TC_Common::sepstr<string>(sPtr->getSid(), "::");
        assert(vecNames.size() == 2);

        return findName(vecNames[0], vecNames[1]) + (bStream ? "" : "$OBJ");
    }

    MapPtr mPtr = MapPtr::dynamicCast(pPtr);
    if (mPtr)
    {
        if (bStream)
        {
            return IDL_NAMESPACE_STR + "Stream.Map<" + getDtsType(mPtr->getLeftTypePtr(), bStream) + ", " + getDtsType(mPtr->getRightTypePtr(), bStream) + ">";
        }
        else
        {
            const string& sLeftType = getDtsType(mPtr->getLeftTypePtr(), bStream);
            if (sLeftType == "number" || sLeftType == "string")
            {
                return "{[key: " + getDtsType(mPtr->getLeftTypePtr(), bStream) + "]: " + getDtsType(mPtr->getRightTypePtr(), bStream) + "}";
            }
            else if (isSimple(mPtr->getLeftTypePtr()) && sLeftType != "Buffer")
            {
                return "object";
            }
            else 
            {
                return "";
            }
        }
    }

    EnumPtr ePtr = EnumPtr::dynamicCast(pPtr);
    if (ePtr)
    {
        vector<string> vecNames = TC_Common::sepstr<string>(ePtr->getSid(), "::");
        assert(vecNames.size() == 2);

        return findName(vecNames[0], vecNames[1]);
    }

    assert(false);
    return "";
}

bool CodeGenerator::isSimple(const TypePtr & pPtr) const
{
    BuiltinPtr bPtr = BuiltinPtr::dynamicCast(pPtr);
    if (bPtr)
    {
        return true;
    }

    EnumPtr ePtr = EnumPtr::dynamicCast(pPtr);
    if (ePtr)
    {
        return true;
    }

    return false;
}

bool CodeGenerator::isBinBuffer(const TypePtr & pPtr) const
{
    VectorPtr vPtr = VectorPtr::dynamicCast(pPtr);
    if (vPtr)
    {
        BuiltinPtr vbPtr = BuiltinPtr::dynamicCast(vPtr->getTypePtr());
        if (vbPtr && vbPtr->kind() == Builtin::KindByte)
        {
            return true;
        }

    }

    return false;
}

string CodeGenerator::getDefault(const TypeIdPtr & pPtr, const string &sDefault, const string& sNamespace)
{
    return getDefault(pPtr, sDefault, sNamespace, true);
}

string CodeGenerator::getDefault(const TypeIdPtr & pPtr, const string &sDefault, const string& sNamespace, const bool bGlobal)
{
    BuiltinPtr bPtr = BuiltinPtr::dynamicCast(pPtr->getTypePtr());
    if (bPtr)
    {
        switch (bPtr->kind())
        {
            case Builtin::KindBool    : 
                return sDefault.empty() ? "false" : sDefault;
            case Builtin::KindString  : 
            {
                if (_bStringBinaryEncoding)
                {
                    return IDL_NAMESPACE_STR + "Stream.BinBuffer.from(\"" + TC_Common::replace(sDefault, "\"", "\\\"") +  "\").toNodeBufferUnSafe()";
                }
                else
                {
                    return "\"" + TC_Common::replace(sDefault, "\"", "\\\"") +  "\"";
                }
            }
            case Builtin::KindByte    : // [[fallthrough]]
            case Builtin::KindShort   : // [[fallthrough]]
            case Builtin::KindInt     : // [[fallthrough]]
            case Builtin::KindLong    :
            {
                string sTemp = TC_Common::trim(sDefault);
                if (sTemp.empty())
                {
                    sTemp = "0";
                }
                else
                {
                    if (TC_Common::tostr(TC_Common::strto<long>(sTemp)) != sTemp)
                    {
                        //有可能是枚举值，在枚举值中查找
                        vector<string> vecNames = TC_Common::sepstr<string>(sDefault, "::");
                        if (vecNames.size() == 2)
                        {
                            sTemp = findName(vecNames[0], vecNames[1]);
                        }
                        else 
                        {
                            sTemp = findName(sNamespace, sDefault);
                        }
                    }
                }

                if (sTemp.empty())
                {
                    sTemp = "0";
                }

                if (_bUseStringRepresent)
                {
                    if (bPtr->kind() == Builtin::KindLong)
                    {
                        sTemp = "\"" + sTemp + "\"";
                    }
                }

                return sTemp;
            }
            case Builtin::KindFloat    : // [[fallthrough]]
            case Builtin::KindDouble   : 
                return sDefault.empty()?"0.0":sDefault;
            default                    :
                assert(false);
        }
    }

    EnumPtr ePtr = EnumPtr::dynamicCast(pPtr->getTypePtr());
    if (ePtr)
    {
        if (sDefault.empty())
        {
            vector<TypeIdPtr>& eMember = ePtr->getAllMemberPtr();
            if (eMember.size() > 0)
            {
                vector<string> vecNames = TC_Common::sepstr<string>(ePtr->getSid(), "::");

                string sModule = findName(vecNames[0], eMember[0]->getId());

                return sModule;
            }
        }

        if (!TC_Common::isdigit(sDefault))
        {
            string s1 = sNamespace;
            string s2 = sDefault;

            string::size_type index = sDefault.find("::");
            if (index != string::npos)
            {
                s1 = sDefault.substr(0, index);
                s2 = sDefault.substr(index + 2);
            }

            string sModule = findName(s1, s2);

            assert(!sModule.empty());

            return sModule;
        }

        return sDefault;
    }

    if (bGlobal)
    {
        return "new " + getDataType(pPtr->getTypePtr());
    }

    return sDefault;
}