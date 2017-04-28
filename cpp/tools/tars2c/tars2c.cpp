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
#include "tars2c.h"
#include "util/tc_md5.h"
#include "util/tc_file.h"
#include "util/tc_common.h"
#include <string>

#define TAB g_parse->getTab()
#define INC_TAB g_parse->incTab()
#define DEL_TAB g_parse->delTab()

//////////////////////////////////////////////////////////////////////////////////
//
string Tars2C::writeTo(const TypeIdPtr &pPtr,const string& namespaceId) const
{
    ostringstream s;
    StructPtr sPtr = StructPtr::dynamicCast(pPtr->getTypePtr());
    VectorPtr vPtr = VectorPtr::dynamicCast(pPtr->getTypePtr());
    MapPtr mPtr = MapPtr::dynamicCast(pPtr->getTypePtr());

    if (EnumPtr::dynamicCast(pPtr->getTypePtr()))
    {
        s << TAB << "ret = TarsOutputStream_writeInt32(os, (Int32)tars_st->" << pPtr->getId() << ", " << pPtr->getTag() << ");" << endl;
    }
    else if (toClassName(VectorPtr::dynamicCast(pPtr->getTypePtr())) == "list<char>" )
    {
        s << TAB << "ret = TarsOutputStream_writeVectorChar(os, tars_st->" << pPtr->getId() << ", " << pPtr->getTag() << ");" << endl;
    }
    else if (sPtr)
    {
        s << TAB << "ret = TarsOutputStream_writeStruct(os, tars_st->" << pPtr->getId() << ", " << pPtr->getTag() << ");" << endl;
    }
    else if (m_bCheckDefault == false || pPtr->isRequire() || (!pPtr->hasDefault() && !mPtr && !vPtr))
    {
        s << TAB << "ret = TarsOutputStream_write" << toFuncName(pPtr->getTypePtr()) << "(os, tars_st->" << pPtr->getId() << ", " << pPtr->getTag() << ");" << endl;
    }
    else
    {
        std::string sCheckCond = "true";

        if (mPtr)
        {
            sCheckCond = "JMapWrapper_size(tars_st->" + pPtr->getId() + ") > 0";
        }

        if (vPtr)
        {
            sCheckCond = "JArray_size(tars_st->" + pPtr->getId() + ") > 0";
        }

        BuiltinPtr bPtr = BuiltinPtr::dynamicCast(pPtr->getTypePtr());
        if (bPtr && bPtr->kind() == Builtin::KindString && pPtr->hasDefault())
        {
            string tmp = tars::TC_Common::replace(pPtr->def(), "\"", "\\\"");
            sCheckCond = "JString_size(tars_st->" + pPtr->getId() + ") != strlen(\"" + tmp + "\") || strncmp(JString_data(tars_st->" + pPtr->getId() + "), \"" + tmp + "\", JString_size(tars_st->" + pPtr->getId() + ")) != 0";
        }
        else if (pPtr->hasDefault())
        {
            sCheckCond = "tars_st->" + pPtr->getId() + " != " + pPtr->def();
        }

        s << TAB << "if (" << sCheckCond << ")" << endl;
        s << TAB << "{" << endl;
        INC_TAB;
        s << TAB << "ret = TarsOutputStream_write" << toFuncName(pPtr->getTypePtr()) << "(os, tars_st->" << pPtr->getId() << ", " << pPtr->getTag() << ");" << endl;
        DEL_TAB;
        s << TAB << "}" << endl;
    }


    s << TAB <<"if (TARS_SUCCESS != ret) return ret;" << endl;
    s <<endl;

    return s.str();
}

string Tars2C::readFrom(const TypeIdPtr &pPtr, bool bIsRequire,const string& namespaceId) const
{
    ostringstream s;

    bool isPtr = false;
    string type = toFuncName(pPtr->getTypePtr());
    StructPtr sPtr = StructPtr::dynamicCast(pPtr->getTypePtr());
    if (type == "String" || type == "Vector" || type == "Map" || type == "Struct")
    {
        isPtr = true;
    }

    if (EnumPtr::dynamicCast(pPtr->getTypePtr()))
    {
        s << TAB << "ret = TarsInputStream_readInt32(is, & tars_st->" << pPtr->getId();
    }
    else if (toClassName(VectorPtr::dynamicCast(pPtr->getTypePtr())) == "list<char>" )
    {
        s << TAB << "ret = TarsInputStream_readVectorChar(is, tars_st->" << pPtr->getId();
    }
    else if (sPtr)
    {
        s << TAB << "ret = TarsInputStream_readStruct(is, tars_st->" << pPtr->getId();
    }
    else
    {
        s << TAB << "ret = TarsInputStream_read" << toFuncName(pPtr->getTypePtr()) << "(is, "<< (isPtr ? "" : "&") <<"tars_st->" << pPtr->getId();
    }

    s << ", " << pPtr->getTag() << ", " << ((pPtr->isRequire() && bIsRequire)?"true":"false") << ");"<<endl;
    s << TAB<<"if(TARS_SUCCESS != ret) return ret;" << endl;

    return s.str();
}

string Tars2C::toClassName(const TypePtr &pPtr) const
{
    string s;
    BuiltinPtr bPtr = BuiltinPtr::dynamicCast(pPtr);
    if (bPtr)
    {
        switch (bPtr->kind())
        {
            case Builtin::KindBool:     s = "bool";     break;
            case Builtin::KindByte:     s = "char";     break;
            case Builtin::KindShort:    s = "short";    break;
            case Builtin::KindInt:      s = "int32";    break;
            case Builtin::KindLong:     s = "int64";    break;
            case Builtin::KindFloat:    s = "float";    break;
            case Builtin::KindDouble:   s = "double";   break;
            case Builtin::KindString:   s = "string";   break;
            default:                    assert(false);  break;
        }
        
        return s;
    }

    VectorPtr vPtr = VectorPtr::dynamicCast(pPtr);
    if (vPtr) return tostrVector(vPtr);

    MapPtr mPtr = MapPtr::dynamicCast(pPtr);
    if (mPtr) return tostrMap(mPtr);

    StructPtr sPtr = StructPtr::dynamicCast(pPtr);
    if (sPtr) return TC_Common::replace(sPtr->getSid(), "::", ".");

    EnumPtr ePtr = EnumPtr::dynamicCast(pPtr);
    if (ePtr) return tostrEnum(ePtr);

    if (!pPtr) return "void";

    assert(false);
    return "";
}


string Tars2C::toFuncName(const TypePtr &pPtr) const
{
    string s;
    BuiltinPtr bPtr = BuiltinPtr::dynamicCast(pPtr);
    if (bPtr)
    {
        switch (bPtr->kind())
        {
            case Builtin::KindBool:     s = "Bool";     break;
            case Builtin::KindByte:     s = "Char";     break;
            case Builtin::KindShort:    s = bPtr->isUnsigned()?"UInt8" :"Short";    break;
            case Builtin::KindInt:      s = bPtr->isUnsigned()?"UInt16":"Int32";    break;
            case Builtin::KindLong:     s = bPtr->isUnsigned()?"UInt32":"Int64";    break;
            case Builtin::KindFloat:    s = "Float";    break;
            case Builtin::KindDouble:   s = "Double";   break;
            case Builtin::KindString:   s = "String";   break;
            default:                    assert(false);  break;
        }
        
        return s;
    }

    VectorPtr vPtr = VectorPtr::dynamicCast(pPtr);
    if (vPtr) return "Vector";

    MapPtr mPtr = MapPtr::dynamicCast(pPtr);
    if (mPtr) return "Map";

    StructPtr sPtr = StructPtr::dynamicCast(pPtr);
    if (sPtr) return "Struct";

    EnumPtr ePtr = EnumPtr::dynamicCast(pPtr);
    if (ePtr) return tostrEnum(ePtr);

    if (!pPtr) return "void";

    assert(false);
    return "";
}


/*******************************BuiltinPtr********************************/
string Tars2C::tostr(const TypePtr &pPtr) const
{
    BuiltinPtr bPtr = BuiltinPtr::dynamicCast(pPtr);
    if (bPtr) return tostrBuiltin(bPtr);

    VectorPtr vPtr = VectorPtr::dynamicCast(pPtr);
    if (vPtr)
    {
        if (toClassName(vPtr) == "list<char>")
            return "JString * ";
        else
            return "JArray * ";
    }

    MapPtr mPtr = MapPtr::dynamicCast(pPtr);
    if (mPtr) return "JMapWrapper * ";

    StructPtr sPtr = StructPtr::dynamicCast(pPtr);

    if (sPtr) return tostrStruct(sPtr)+ " *";

    EnumPtr ePtr = EnumPtr::dynamicCast(pPtr);
    if (ePtr) return "Int32";

    if (!pPtr) return "void";

    assert(false);
    return "";
}

string Tars2C::tostrBuiltin(const BuiltinPtr &pPtr) const
{
    string s;

    switch (pPtr->kind())
    {
    case Builtin::KindBool:
        s = "Bool";
        break;
    case Builtin::KindByte:
        s = "Char";
        break;
    case Builtin::KindShort: 
        s = (pPtr->isUnsigned()?"UInt8":"Short");
        break;
    case Builtin::KindInt:
        s = (pPtr->isUnsigned()?"UInt16":"Int32");
        break;
    case Builtin::KindLong:
        s =  (pPtr->isUnsigned()?"UInt32":"Int64");
        break;
    case Builtin::KindFloat:
        s = "Float";
        break;
    case Builtin::KindDouble:
        s = "Double";
        break;
    case Builtin::KindString:
        s = "JString * ";
        break;
    default:
        assert(false);
        break;
    }

    return s;
}
/*******************************VectorPtr********************************/
string Tars2C::tostrVector(const VectorPtr &pPtr) const
{
    string s = string("list<") + toClassName(pPtr->getTypePtr());

    if (MapPtr::dynamicCast(pPtr->getTypePtr()) || VectorPtr::dynamicCast(pPtr->getTypePtr()))
    {
        s += " >";
    }
    else
    {
        s += ">";
    }
    return s;
}
/*******************************MapPtr********************************/
string Tars2C::tostrMap(const MapPtr &pPtr) const
{
    string s = string("map<") + toClassName(pPtr->getLeftTypePtr()) + "," + toClassName(pPtr->getRightTypePtr());
    if (MapPtr::dynamicCast(pPtr->getRightTypePtr()) || VectorPtr::dynamicCast(pPtr->getRightTypePtr()))
    {
        s += ">";
    }
    else
    {
        s += ">";
    }
    return s;
}

/*******************************StructPtr********************************/
string Tars2C::tostrStruct(const StructPtr &pPtr) const
{
    return TC_Common::replace(pPtr->getSid(), "::", "_");
}

string Tars2C::MD5(const StructPtr &pPtr) const
{
    string s;
    vector<TypeIdPtr>& member = pPtr->getAllMemberPtr();
    for (size_t j = 0; j < member.size(); j++)
    {
        s += "_" + tostr(member[j]->getTypePtr());
    }

    return "\"" + tars::TC_MD5::md5str(s) + "\"";
}

/////////////////////////////////////////////////////////////////////
string Tars2C::tostrEnum(const EnumPtr &pPtr) const
{
    return TC_Common::replace(pPtr->getSid(), "::", "_");
}
///////////////////////////////////////////////////////////////////////
string Tars2C::generateH(const StructPtr &pPtr, const string& namespaceId) const
{
    ostringstream s;
    string sStructName = namespaceId + "_" + pPtr->getId();

    s << TAB << "typedef struct " << sStructName << " " << sStructName << ";" << endl;
    s << TAB << "struct " << sStructName << endl;
    s << TAB << "{" << endl;
    //s << TAB << "public:" << endl;

    INC_TAB;

    s << TAB << "char * className;" << endl;
    s << TAB << "Int32 (*writeTo)( const "<<sStructName<<"*, TarsOutputStream *);" << endl;
    s << TAB << "Int32 (*readFrom)( "<<sStructName<<"*, TarsInputStream *);" << endl;

    //定义成员变量
    vector<TypeIdPtr>& member = pPtr->getAllMemberPtr();
    for (size_t j = 0; j < member.size(); j++)
    {
        s << TAB << tostr(member[j]->getTypePtr()) << " " << member[j]->getId() << ";" << endl;
    }


    s << endl;

    DEL_TAB;
    s << TAB << "};" << endl;
    s << endl;

    ////////////////////////////////////////////////////////////
    s << TAB << "Int32 " << sStructName << "_writeTo(const " << sStructName << "* tars_st, TarsOutputStream * os);" << endl; 
    s << TAB << "Int32 " << sStructName << "_readFrom(" << sStructName << "* tars_st, TarsInputStream *is);" << endl;
    s << TAB << "void " <<  sStructName << "_del(" << sStructName << " ** handle);" << endl;
    s << TAB << "Int32 " << sStructName << "_init(" << sStructName << " * handle);" << endl;
    s << TAB << sStructName << " * " << sStructName << "_new(void);" << endl;
    s << TAB <<endl;

    return s.str();
}

///////////////////////////////////////////////////////////////////////
string Tars2C::generateC(const StructPtr &pPtr, const string& namespaceId) const
{
    ostringstream s;
    string sStructName = namespaceId + "_" + pPtr->getId();

    //定义成员变量
    vector<TypeIdPtr>& member = pPtr->getAllMemberPtr();

    ////////////////////////////////////////////////////////////
    s << TAB << "Int32 " << sStructName << "_writeTo(const " << sStructName << "* tars_st, TarsOutputStream * os)" << endl; 
    s << TAB << "{" << endl;
    INC_TAB;
    s << TAB << "Int32 ret=0;" << endl;

    for (size_t j = 0; j < member.size(); j++)
    {
        s << writeTo(member[j],namespaceId);
    }

    s << endl;
    s << TAB << "return TARS_SUCCESS;" << endl;
    DEL_TAB;
    s << TAB << "}" << endl;

    ///////////////////////////////////////////////////////////
    s << endl;
    s << TAB << "Int32 " << sStructName << "_readFrom(" << sStructName << "* tars_st, TarsInputStream *is)" << endl;;
    s << TAB << "{" << endl;
    INC_TAB;
    s << TAB << "Int32 ret=0;" << endl;

    for (size_t j = 0; j < member.size(); j++)
    {
        s << readFrom(member[j],true,namespaceId);
    }

    s << endl;
    s << TAB << "return TARS_SUCCESS;" << endl;
    DEL_TAB;
    s << TAB << "}" << endl;

    s << endl;
    s << TAB << "void " << sStructName << "_del(" << sStructName << " ** handle)" << endl;
    s << TAB << "{" << endl;
    INC_TAB;
    s << TAB << sStructName << " ** this = (" << sStructName << "**)handle;" << endl;

    for (size_t j = 0; j < member.size(); j++)
    {
        string type = toFuncName(member[j]->getTypePtr());
        if (type == "String")
            s << TAB << "if((*this)->" << member[j]->getId() << ") JString_del(&(*this)->" << member[j]->getId() << ");" << endl;
        else if (type == "Vector")
            if (toClassName(member[j]->getTypePtr()) == "list<char>")
                s << TAB << "if((*this)->" << member[j]->getId() << ") JString_del(&(*this)->" << member[j]->getId() << ");" << endl;
            else
                s << TAB << "if((*this)->" << member[j]->getId() << ") JArray_del(&(*this)->" << member[j]->getId() << ");" << endl;
        else if (type == "Map")
            s << TAB << "if((*this)->" << member[j]->getId() << ") JMapWrapper_del(&(*this)->" << member[j]->getId() << ");" << endl;
        else if (type == "Struct")
        {
            StructPtr sPtr = StructPtr::dynamicCast(member[j]->getTypePtr());
            if (sPtr)
                s << TAB << "if((*this)->" << member[j]->getId() << ") "<< namespaceId + "_" + sPtr->getId()<<"_del(&(*this)->" << member[j]->getId() << ");" << endl;
        }

    }
    s << TAB << "if((*this)->className) TarsFree((*this)->className);" << endl;
    s << TAB << "TarsFree(*this);" << endl;
    s << TAB << "*this = NULL;" << endl;

    DEL_TAB;
    s << TAB << "}" << endl;
    s << endl;

    s << TAB << "Int32 " << sStructName << "_init(" << sStructName << " * handle)" << endl;
    s << TAB << "{" << endl;
    INC_TAB;

    s << TAB << sStructName << " * this = (" << sStructName << "*) handle;" << endl;
    s << endl;

    s << TAB << "char * tarsClassName = \"" << namespaceId << "." << pPtr->getId() << "\";" << endl;
    s << endl;

    s << TAB << "this->className = TarsMalloc(strlen(tarsClassName)+1);" << endl;
    s << TAB << "this->writeTo = "<< sStructName << "_writeTo;" <<endl;
    s << TAB << "this->readFrom = "<< sStructName << "_readFrom;" << endl;
    s << endl;

    string sCleanCondition = "!this->className ";
    for (size_t j = 0; j < member.size(); j++)
    {
        BuiltinPtr bPtr  = BuiltinPtr::dynamicCast(member[j]->getTypePtr());
        //string值要转义
        if (bPtr && bPtr->kind() == Builtin::KindString)
        {
            s << TAB << "this->" << member[j]->getId() << " = JString_new();" << endl;
            sCleanCondition += " || !this->" + member[j]->getId();
            continue;
        }

        VectorPtr vPtr = VectorPtr::dynamicCast(member[j]->getTypePtr());
        if (vPtr)
        {
            if (toClassName(vPtr) == "list<char>")
            {
                s << TAB << "this->" << member[j]->getId() << " = JString_new();" << endl;
                sCleanCondition += " || !this->" + member[j]->getId();
            }
            else
            {
                s << TAB << "this->" << member[j]->getId() << " = JArray_new(\"" 
                << toClassName(vPtr->getTypePtr()) << "\");" << endl;
                sCleanCondition += " || !this->" + member[j]->getId();
            }
            continue;
        }

        MapPtr mPtr = MapPtr::dynamicCast(member[j]->getTypePtr());
        if (mPtr)
        {
            s << TAB << "this->" << member[j]->getId() << " = JMapWrapper_new(\""
            << toClassName(mPtr->getLeftTypePtr()) << "\", \"" << toClassName(mPtr->getRightTypePtr()) << "\");" << endl;
            sCleanCondition += " || !this->" + member[j]->getId();
            continue;
        }

        StructPtr sPtr = StructPtr::dynamicCast(member[j]->getTypePtr());
        if (sPtr)
        {
            s << TAB << "this->" << member[j]->getId() << " = "<< namespaceId + "_" + sPtr->getId()+"_new();" << endl;
            sCleanCondition += " || !this->" + member[j]->getId();
            continue;
        }

        if (tostr(member[j]->getTypePtr()) == "Int64")
        {
            s << "#ifdef __MTK_64"<<endl;
            s << TAB << "this->" << member[j]->getId() << ".high = " << member[j]->def() << ";" << endl;
            s << TAB << "this->" << member[j]->getId() << ".low = " << member[j]->def() << ";" << endl;
            s << "#else"<<endl;
            s << TAB << "this->" << member[j]->getId() << " = " << member[j]->def() << ";" << endl;
            s << "#endif"<<endl;
            continue;
        }
        if (member[j]->hasDefault())
        {
            s << TAB << "this->" << member[j]->getId() << " = " << member[j]->def() << ";" << endl;
        }
    }
    s << endl;

    s << TAB << "if(" << sCleanCondition << ")" << endl;
    s << TAB << "{"<<endl;
    INC_TAB;
    s << TAB <<  sStructName << "_del(&this);"<<endl;
    s << TAB <<  "return TARS_MALLOC_ERROR;"<<endl;
    DEL_TAB;
    s << TAB <<"}" << endl;

    s << TAB << "memcpy(this->className, tarsClassName, strlen(tarsClassName)+1);" << endl;
    s << endl;

    //string default
    for (size_t j = 0; j < member.size(); j++)
    {
        BuiltinPtr bPtr  = BuiltinPtr::dynamicCast(member[j]->getTypePtr());
        //string值要转义
        if (bPtr && bPtr->kind() == Builtin::KindString)
        {
            if (member[j]->hasDefault())
            {
                string tmp = tars::TC_Common::replace(member[j]->def(), "\"", "\\\"");
                s << TAB << "JString_assign(this->" << member[j]->getId() << ", \"" 
                << tmp << "\", strlen(\"" << tmp << "\"));" << endl;
            }
            continue;
        }
    }
    s << endl;

    s << TAB << "return TARS_SUCCESS;" << endl;
    DEL_TAB;
    s << TAB << "}" << endl;
    s << endl;


    s << TAB << sStructName << " * " << sStructName << "_new()" << endl;
    s << TAB << "{" << endl;
    INC_TAB;
    s << TAB << "Int32 ret=0;" << endl;
    s << TAB << sStructName << " *this = (" << sStructName << " *)TarsMalloc(sizeof("<< sStructName << "));" << endl;
    s << TAB << "if(!this) return NULL;" << endl;
    s << TAB << "ret = " << sStructName << "_init(this);" << endl;
    s << TAB << "if(TARS_SUCCESS != ret) { return NULL; }" << endl;
    s << TAB << "return this;" << endl;
    DEL_TAB;
    s << TAB << "}" << endl;

    return s.str();
}


/*******************************ContainerPtr********************************/
string Tars2C::generateH(const ContainerPtr &pPtr) const
{
    ostringstream s;
    for (size_t i = 0; i < pPtr->getAllNamespacePtr().size(); i++)
    {
        s << generateH(pPtr->getAllNamespacePtr()[i]) << endl;
        s << endl;
    }
    return s.str();
}

/******************************EnumPtr***************************************/

string Tars2C::generateH(const EnumPtr &pPtr, const string& namespaceId) const
{
    ostringstream s;
    s << TAB << "enum " << namespaceId << "_" << pPtr->getId() << endl;
    s << TAB << "{" << endl;
    INC_TAB;
    vector<TypeIdPtr>& member = pPtr->getAllMemberPtr();
    for (size_t i = 0; i < member.size(); i++)
    {
        s << TAB << member[i]->getId();
        if (member[i]->hasDefault())
        {
            s << " = " << member[i]->def();
        }
        s << (i==member.size()-1 ? "" : ",") << endl;
    }
    DEL_TAB;
    s << TAB << "};" << endl;

    return s.str();
}

/******************************ConstPtr***************************************/
string Tars2C::generateH(const ConstPtr &pPtr) const
{
    ostringstream s;

    if (pPtr->getConstGrammarPtr()->t == ConstGrammar::STRING)
    {
        string tmp = tars::TC_Common::replace(pPtr->getConstGrammarPtr()->v, "\"", "\\\"");
        s << TAB << "const " << tostr(pPtr->getTypeIdPtr()->getTypePtr()) << " " << pPtr->getTypeIdPtr()->getId() << " = \"" << tmp << "\";"<< endl;
    }
    else
    {
        s << TAB << "const " << tostr(pPtr->getTypeIdPtr()->getTypePtr()) << " " << pPtr->getTypeIdPtr()->getId() << " = " << pPtr->getConstGrammarPtr()->v 
        << ((tostr(pPtr->getTypeIdPtr()->getTypePtr()) == "tars::Int64") ? "LL;" : ";" )<< endl;
    }

    return s.str();
}
/******************************NamespacePtr***************************************/

string Tars2C::generateH(const NamespacePtr &pPtr) const
{
    ostringstream s;
    vector<StructPtr>        &ss    = pPtr->getAllStructPtr();
    vector<EnumPtr>            &es    = pPtr->getAllEnumPtr();

    s << endl;

    for (size_t i = 0; i < es.size(); i++)
    {
        s << generateH(es[i], pPtr->getId()) << endl;
    }

    for (size_t i = 0; i < ss.size(); i++)
    {
        s << generateH(ss[i], pPtr->getId()) << endl;
    }

    s << endl;

    return s.str();
}


string Tars2C::generateC(const NamespacePtr &pPtr) const
{
    ostringstream s;
    vector<StructPtr>        &ss    = pPtr->getAllStructPtr();
    s << endl;
    for (size_t i = 0; i < ss.size(); i++)
    {
        s << generateC(ss[i], pPtr->getId()) << endl;
    }
    s << endl;
    return s.str();
}


/******************************Tars2C***************************************/

void Tars2C::generateH(const ContextPtr &pPtr) const
{
    string n        = tars::TC_File::excludeFileExt(tars::TC_File::extractFileName(pPtr->getFileName()));

    string fileH    = m_sBaseDir + "/" + n + ".h";

    string define   = tars::TC_Common::upper("__" + n + "_h_");

    ostringstream s;

    s << g_parse->printHeaderRemark();

    s << "#ifndef " << define << endl;
    s << "#define " << define << endl;
    s << endl;
    s << "#include \"Tars_c.h\"" << endl;

    vector<string> include = pPtr->getIncludes();
    for (size_t i = 0; i < include.size(); i++)
    {
        s << "#include \"" << g_parse->getHeader() << tars::TC_File::extractFileName(include[i]) << "\"" << endl;
    }

    vector<NamespacePtr> namespaces = pPtr->getNamespaces();

    s << endl;

    for (size_t i = 0; i < namespaces.size(); i++)
    {
        s << generateH(namespaces[i]) << endl;
    }

    s << endl;
    s << "#endif" << endl;

    tars::TC_File::makeDirRecursive(m_sBaseDir, 0755);
    tars::TC_File::save2file(fileH, s.str());
}

void Tars2C::generateC(const ContextPtr &pPtr) const
{
    string n        = tars::TC_File::excludeFileExt(tars::TC_File::extractFileName(pPtr->getFileName()));
    string fileC    = m_sBaseDir + "/" + n + ".c";

    string define   = tars::TC_Common::upper("__" + n + "_h_");

    ostringstream s;

    s << g_parse->printHeaderRemark();

    vector<NamespacePtr> namespaces = pPtr->getNamespaces();

    s << "#include \"" << g_parse->getHeader() << n << ".h\"" << endl;
    s << endl;

    for (size_t i = 0; i < namespaces.size(); i++)
    {
        s << generateC(namespaces[i]) << endl;
    }

    s << endl;

    tars::TC_File::makeDirRecursive(m_sBaseDir, 0755);
    tars::TC_File::save2file(fileC, s.str());

}


void Tars2C::createFile(const string &file)
{
    std::vector<ContextPtr> contexts = g_parse->getContexts();
    for (size_t i = 0; i < contexts.size(); i++)
    {
        if (file == contexts[i]->getFileName())
        {
            generateH(contexts[i]);
            generateC(contexts[i]);
        }
    }
}

StructPtr Tars2C::findStruct(const ContextPtr &pPtr,const string &id)
{
    string sid = id;

    //在当前namespace中查找
    vector<NamespacePtr> namespaces = pPtr->getNamespaces();
    for (size_t i = 0; i < namespaces.size(); i++)
    {
        NamespacePtr np = namespaces[i];
        vector<StructPtr> structs = np->getAllStructPtr();

        for (size_t i = 0; i < structs.size(); i++)
        {
            if (structs[i]->getSid() == sid)
            {
                return structs[i];
            }
        }
    }

    return NULL;
}


