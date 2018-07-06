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

string CodeGenerator::generateDTSProxy(const InterfacePtr &pPtr)
{
    vector<OperationPtr> &vOperation = pPtr->getAllOperationPtr();
    ostringstream str;

    //class
    INC_TAB;
    str << TAB << "class " << pPtr->getId() << "Proxy {" << endl;
    INC_TAB;
    str << TAB << "setTimeout(timeout: number): void;" << endl;
    str << TAB << "getTimeout(): number;" << endl;
    for (size_t i = 0; i < vOperation.size(); i++)
    {
        OperationPtr &oPtr = vOperation[i];

        str << TAB << oPtr->getId() << "(";

        vector<ParamDeclPtr> &vParamDecl = oPtr->getAllParamDeclPtr();
        for (size_t j = 0; j < vParamDecl.size(); j++) 
        {
            if(vParamDecl[j]->isOut()) 
            {
                continue;
            }
            str << vParamDecl[j]->getTypeIdPtr()->getId() << ": " << getDtsType(vParamDecl[j]->getTypeIdPtr()->getTypePtr());
            str << ", ";
        }
        str << "property?: " << IDL_NAMESPACE_STR << "Rpc.InvokeProperty): Promise<" << pPtr->getId() << "$" << oPtr->getId() << "$DE>;" << endl;
    }
    DEL_TAB;
    str << TAB << "}" << endl;

    //interface
    for (size_t i = 0; i < vOperation.size(); i++)
    {
        OperationPtr &oPtr = vOperation[i];

        str << TAB << "interface " << pPtr->getId() << "$" << oPtr->getId() << "$DE {" << endl;
        INC_TAB;
        str << TAB << "request: object;" << endl;
        str << TAB << "response: {" << endl;
        INC_TAB;
        str << TAB << "costtime: number;" << endl;
        if (oPtr->getReturnPtr()->getTypePtr())
        {
            str << TAB << "return: " << getDtsType(oPtr->getReturnPtr()->getTypePtr()) << ";" << endl;
        }
        else
        {
            str << TAB << "return: void;" << endl;
        }

        vector<ParamDeclPtr> &vParamDecl = oPtr->getAllParamDeclPtr();
        bool hasArgs = false;
        for (size_t j = 0; j < vParamDecl.size(); j++)
        {
            if(vParamDecl[j]->isOut()) {
                hasArgs = true;
                break;
            }
        }

        if(hasArgs)
        {
            str << TAB << "arguments: {" << endl;
            INC_TAB;
            for (size_t j = 0; j < vParamDecl.size(); j++)
            {
                if(!vParamDecl[j]->isOut()) {
                    continue;
                }
                str << TAB << vParamDecl[j]->getTypeIdPtr()->getId() << ": " << getDtsType(vParamDecl[j]->getTypeIdPtr()->getTypePtr()) << ";" << endl;
            }
            DEL_TAB;
            str << TAB << "}" << endl;
        }

        DEL_TAB;
        str << TAB << "}" << endl;
        DEL_TAB;
        str << TAB << "}" << endl;
    }
    DEL_TAB;

    return str.str();
}

string CodeGenerator::generateDTSProxy(const NamespacePtr &nPtr, bool &bNeedStream, bool &bNeedRpc)
{
    ostringstream str;
    vector<InterfacePtr> &is = nPtr->getAllInterfacePtr();
    for (size_t i = 0; i < is.size(); i++)
    {
        str << generateDTSProxy(is[i]) << endl;
    }
    if (is.size() != 0)
    {
        bNeedStream = true;
        bNeedRpc = true;
    }
    return str.str();
}

void CodeGenerator::generateDTSProxy(const ContextPtr &cPtr)
{
    vector<NamespacePtr> namespaces = cPtr->getNamespaces();

    //先生成编解码 + 代理类
    ostringstream estr;
    bool bNeedStream = false;
    bool bNeedRpc = false;
    for(size_t i = 0; i < namespaces.size(); i++)
    {
        ostringstream kstr;

        kstr << generateDTS(namespaces[i], bNeedStream);
        kstr << generateDTSProxy(namespaces[i], bNeedStream, bNeedRpc);

        estr << generateDTS(namespaces[i], kstr.str());
    }
    if (estr.str().empty())
    {
        return;
    }

    //再生成导入模块
    ostringstream ostr;
    for (map<string, ImportFile>::iterator it = _mapFiles.begin(); it != _mapFiles.end(); it++)
    {
        if (it->second.sModule.empty()) continue;
        
        if (estr.str().find(it->second.sModule + ".") == string::npos) continue;

        ostr << "import * as " << it->second.sModule << " from \"" << TC_File::excludeFileExt(it->second.sFile) << "\";" << endl;
    }

    //生成文件内容
    ostringstream sstr;
    sstr << printHeaderRemark("Client");
    if (bNeedStream)
    {
        sstr << "import * as " << IDL_NAMESPACE_STR << "Stream from \"" << _sStreamPath << "\";" << endl;
    }
    if (bNeedRpc)
    {
        sstr << "import * as " << IDL_NAMESPACE_STR << "Rpc from \"" << _sRpcPath << "\";" << endl;
    }

    sstr << ostr.str() << endl;
    sstr << estr.str() << endl;

    string sFileName = TC_File::excludeFileExt(_sToPath + TC_File::extractFileName(cPtr->getFileName())) + "Proxy.d.ts";
    TC_File::makeDirRecursive(_sToPath, 0755);
    makeUTF8File(sFileName, sstr.str());
}