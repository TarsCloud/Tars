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

string CodeGenerator::generateDTSServer(const NamespacePtr &nPtr, const InterfacePtr &pPtr)
{
    vector<OperationPtr> &vOperation = pPtr->getAllOperationPtr();
    ostringstream str;

    //class
    INC_TAB;
    str << TAB << "class " << pPtr->getId() << "Imp {" << endl;
    INC_TAB;
    str << TAB << "initialize(): Promise<any> | void;" << endl;
    str << TAB << "protected onDispatch(current: " << IDL_NAMESPACE_STR << "Rpc." << IDL_TYPE << "Current, funcName: string, binBuffer: " << IDL_NAMESPACE_STR << "Stream.BinBuffer): number" << endl;
    for (size_t i = 0; i < vOperation.size(); i++)
    {
        OperationPtr &oPtr = vOperation[i];
        str << TAB << oPtr->getId() << "(current: " << pPtr->getId() << "$" << oPtr->getId() << "$CUR";

        vector<ParamDeclPtr> &vParamDecl = oPtr->getAllParamDeclPtr();
        for (size_t j = 0; j < vParamDecl.size(); j++) 
        {
            str << ", " << vParamDecl[j]->getTypeIdPtr()->getId() << ": " << getDtsType(vParamDecl[j]->getTypeIdPtr()->getTypePtr());
        }
        str << "): void;" << endl;
    }
    DEL_TAB;
    str << TAB << "}" << endl;

    //interface
    for (size_t i = 0; i < vOperation.size(); i++)
    {
        OperationPtr &oPtr = vOperation[i];

        str << TAB << "interface " << pPtr->getId() << "$" << oPtr->getId() << "$CUR extends " << IDL_NAMESPACE_STR << "Rpc." << IDL_TYPE << "Current {" <<endl;
        INC_TAB;
        str << TAB;
        if (oPtr->getReturnPtr()->getTypePtr())
        {
            str << "sendResponse(ret: " << getDtsType(oPtr->getReturnPtr()->getTypePtr()); 

            vector<ParamDeclPtr> &vParamDecl = oPtr->getAllParamDeclPtr();
            for (size_t j = 0; j < vParamDecl.size(); j++)
            {
                if(!vParamDecl[j]->isOut()) {
                    continue;
                }
                str << ", " << vParamDecl[j]->getTypeIdPtr()->getId() << ": " << getDtsType(vParamDecl[j]->getTypeIdPtr()->getTypePtr()) ;
            }
            str << "): void;" << endl;
        }
        else
        {
            str << "sendResponse(): void;" << endl;
        }

        DEL_TAB;
        str << TAB << "}" << endl;
    }
    DEL_TAB;

    return str.str();
}

string CodeGenerator::generateDTSServer(const NamespacePtr &pPtr, bool &bNeedStream, bool &bNeedRpc)
{
    vector<InterfacePtr> &is = pPtr->getAllInterfacePtr();
    ostringstream str;

    for (size_t i = 0; i < is.size(); i++)
    {
        str << generateDTSServer(pPtr, is[i]) << endl;
    }

    if (is.size() != 0)
    {
        bNeedRpc = true;
        bNeedStream = true;
    }

    return str.str();
}

void CodeGenerator::generateDTSServer(const ContextPtr &pPtr)
{
    vector<NamespacePtr> namespaces = pPtr->getNamespaces();

    //生成编解码 + 服务类
    ostringstream estr;
    bool bNeedStream = false;
    bool bNeedRpc = false;
    for(size_t i = 0; i < namespaces.size(); i++)
    {
        ostringstream kstr;

        kstr << generateDTS(namespaces[i], bNeedStream);
        kstr << generateDTSServer(namespaces[i], bNeedStream, bNeedRpc);
        
        estr << generateDTS(namespaces[i], kstr.str());
    }
    if(estr.str().empty())
    {
        return;
    }

    //再生成导入模块
    ostringstream ostr;
    if (bNeedStream)
    {
        ostr << "import * as " << IDL_NAMESPACE_STR << "Stream from \"" << _sStreamPath << "\";" << endl;
    }
    if (bNeedRpc)
    {
        ostr << "import * as " << IDL_NAMESPACE_STR << "Rpc from \"" << _sRpcPath << "\";" << endl;
    }
    for (map<string, ImportFile>::iterator it = _mapFiles.begin(); it != _mapFiles.end(); it++)
    {
        if (it->second.sModule.empty()) continue;
        
        if (estr.str().find(it->second.sModule + ".") == string::npos) continue;

        ostr << "import * as " << it->second.sModule << " from \"" << TC_File::excludeFileExt(it->second.sFile) << "\";" << endl;
    }

    ostringstream str;
    str << printHeaderRemark("Server");
    str << ostr.str() << endl;
    str << estr.str() << endl;

    string sFileName = TC_File::excludeFileExt(_sToPath + TC_File::extractFileName(pPtr->getFileName())) + ".d.ts";

    TC_File::makeDirRecursive(_sToPath, 0755);
    makeUTF8File(sFileName, str.str());
}