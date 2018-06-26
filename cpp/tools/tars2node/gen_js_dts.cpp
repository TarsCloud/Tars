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

string CodeGenerator::generateDTS(const EnumPtr &pPtr, const string &sNamespace)
{
    ostringstream s;

    INC_TAB;
    s << TAB << "enum " << pPtr->getId() << " {" << endl;

    INC_TAB;
    //成员变量
    int nenum = -1;
    bool bDependent = false;
    vector<TypeIdPtr>& member = pPtr->getAllMemberPtr();
    for (size_t i = 0; i < member.size(); i++)
    {
        bDependent |= isDependent(sNamespace, member[i]->getId());
        if (member[i]->hasDefault())
        {
            nenum = TC_Common::strto<int>(member[i]->def());
        }
        else
        {
            nenum++;
        }
        s << TAB << "\"" << member[i]->getId() << "\" = " << TC_Common::tostr(nenum) << ((i < member.size() - 1) ? "," : "") << endl;
    }
    DEL_TAB;

    s << TAB << "}" << endl;
    DEL_TAB;

    if (!_bMinimalMembers || _bEntry || bDependent || isDependent(sNamespace, pPtr->getId())) {
        return s.str();
    } else {
        return "";
    }
}

string CodeGenerator::generateDTS(const ConstPtr &pPtr, const string &sNamespace, bool &bNeedStream)
{
    if (_bMinimalMembers && !_bEntry && !isDependent(sNamespace, pPtr->getTypeIdPtr()->getId()))
    {
        return "";
    }

    ostringstream s;
    if (_bStringBinaryEncoding && GET_CONST_GRAMMAR_PTR(pPtr)->t == CONST_GRAMMAR(STRING))
    {
        bNeedStream = true;
    }

    INC_TAB;
    s << TAB << "const " <<  pPtr->getTypeIdPtr()->getId() << ":" 
        << getDtsType(pPtr->getTypeIdPtr()->getTypePtr()) << ";" 
        << endl;
    DEL_TAB;
    return s.str();
}

string CodeGenerator::generateDTS(const StructPtr &pPtr, const string &sNamespace)
{
    if (_bMinimalMembers && !_bEntry && !isDependent(sNamespace, pPtr->getId()))
    {
        return "";
    }

    string sStructName = pPtr->getId() + "$OBJ";
    vector<TypeIdPtr> &member = pPtr->getAllMemberPtr();
    ostringstream s;

    INC_TAB;
    s << TAB << "class " << pPtr->getId() << " {" << endl;
    INC_TAB;
    
    for (size_t i = 0; i < member.size(); i++)
    {
        s << TAB << (member[i]->getId()) << (member[i]->isRequire()?": ":"?: ") << getDtsType(member[i]->getTypePtr()) << ";" << endl;
    }
    if (member.size() > 0)
    {
        s << endl;
    }

    s << TAB << "toObject(): " << sStructName << ";" << endl;
    s << TAB << "readFromObject(json: " << sStructName << "): void;" << endl;
    s << TAB << "toBinBuffer(): " << IDL_NAMESPACE_STR << "Stream.BinBuffer;" << endl;
    s << TAB << "static new(): " << pPtr->getId() << ";" << endl;
    s << TAB << "static create(is: " << IDL_NAMESPACE_STR << "Stream." << IDL_TYPE << "InputStream): " << pPtr->getId() << ";" << endl;
    DEL_TAB;
    s << TAB << "}" << endl;

    s << TAB << "interface " << sStructName << " {" << endl;
    INC_TAB;
    
    for (size_t i = 0; i < member.size(); i++)
    {
        const string &sType = getDtsType(member[i]->getTypePtr(), false);
        if (!sType.empty())
        {
            s << TAB << (member[i]->getId()) << (member[i]->isRequire()?": ":"?: ") << sType << ";" << endl;
        }
    }
    DEL_TAB;
    s << TAB << "}" << endl;
    DEL_TAB;
    return s.str();
}

string CodeGenerator::generateDTS(const NamespacePtr &pPtr, bool &bNeedStream)
{
    //结构
    ostringstream sstr;
    vector<StructPtr> ss(pPtr->getAllStructPtr());
    for (size_t last = 0; last != ss.size() && ss.size() != 0;)
    {
        last = ss.size();
        for (vector<StructPtr>::iterator iter=ss.begin(); iter!=ss.end();)
        {
            string str = generateDTS(*iter, pPtr->getId());
            if (!str.empty()) 
            {
                sstr << str << endl;
                iter = ss.erase(iter);
            } 
            else 
            {
                iter++;
            }
        }
    }

    //常量
    ostringstream cstr;
    vector<ConstPtr> &cs = pPtr->getAllConstPtr();
    for (size_t i = 0; i < cs.size(); i++)
    {
        cstr << generateDTS(cs[i], pPtr->getId(), bNeedStream);
    }

    //枚举
    ostringstream estr;
    vector<EnumPtr> &es = pPtr->getAllEnumPtr();
    for (size_t i = 0; i < es.size(); i++)
    {
        estr << generateDTS(es[i], pPtr->getId());
    }

    ostringstream kstr;
    if (!estr.str().empty()) kstr << estr.str() << endl;
    if (!cstr.str().empty()) kstr << cstr.str() << endl;
    if (!sstr.str().empty())
    {
        bNeedStream = true;
        kstr << sstr.str();
    }

    return kstr.str();
}

string CodeGenerator::generateDTS(const NamespacePtr &pPtr, const string &sContent)
{
    ostringstream str;
    if (!sContent.empty())
    {
        str << "export declare namespace " << pPtr->getId() << " {" << endl;
        str << sContent;
        str << "}" << endl << endl;
    }
    return str.str();
}

void CodeGenerator::generateDTS(const ContextPtr &pPtr)
{
    vector<NamespacePtr> namespaces = pPtr->getNamespaces();
    
    //先生成编解码体
    ostringstream estr;
    bool bNeedStream = false;
    for (size_t i = 0; i < namespaces.size(); i++)
    {
        estr << generateDTS(namespaces[i], generateDTS(namespaces[i], bNeedStream));
    }
    if (estr.str().empty())
    {
        return;
    }

    //再生成导入模块
    ostringstream ostr;
    if (bNeedStream)
    {
        ostr << "import * as " << IDL_NAMESPACE_STR << "Stream from \"" << _sStreamPath << "\";" << endl;
    }

    for (map<string, ImportFile>::iterator it = _mapFiles.begin(); it != _mapFiles.end(); it++)
    {
        if (it->second.sModule.empty()) continue;
        
        if (estr.str().find(it->second.sModule + ".") == string::npos) continue;

        ostr << "import * as " << it->second.sModule << " from \"" << TC_File::excludeFileExt(it->second.sFile) << "\";" << endl;
    }

    //生成文件内容    
    ostringstream sstr;
    sstr << printHeaderRemark("Structure");
    sstr << ostr.str() << endl;
    sstr << estr.str() << endl;

    string sFileName = TC_File::excludeFileExt(_sToPath + TC_File::extractFileName(pPtr->getFileName())) + IDL_TYPE + ".d.ts";

    TC_File::makeDirRecursive(_sToPath, 0755);
    makeUTF8File(sFileName, sstr.str());
}