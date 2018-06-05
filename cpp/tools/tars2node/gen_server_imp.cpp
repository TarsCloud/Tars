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

string CodeGenerator::generateJSServerImp(const NamespacePtr &nPtr, const InterfacePtr &pPtr, const OperationPtr &oPtr)
{
    ostringstream str;
    str << nPtr->getId() << "." << pPtr->getId() << "Imp.prototype." << oPtr->getId() << " = function (current";

    vector<ParamDeclPtr> & vParamDecl = oPtr->getAllParamDeclPtr();
    for (size_t i = 0; i < vParamDecl.size(); i++)
    {
        str << ", " << vParamDecl[i]->getTypeIdPtr()->getId();
    }

    str << ") {" << endl;
    INC_TAB;
    str << TAB << "//TODO::" << endl;
    DEL_TAB;
    str << "};" << endl << endl;

    return str.str();
}

string CodeGenerator::generateJSServerImp(const NamespacePtr &nPtr, const InterfacePtr &pPtr)
{
    ostringstream str;


    vector<OperationPtr> & vOperation = pPtr->getAllOperationPtr();
    for (size_t i = 0; i < vOperation.size(); i++)
    {
        str << generateJSServerImp(nPtr, pPtr, vOperation[i]);
    }

    return str.str();
}

string CodeGenerator::generateJSServerImp(const ContextPtr &cPtr, const NamespacePtr &nPtr)
{
    ostringstream str;

    vector<InterfacePtr> & is = nPtr->getAllInterfacePtr();
    for (size_t i = 0; i < is.size(); i++)
    {
        str << generateJSServerImp(nPtr, is[i]) << endl;
    }

    return str.str();
}

void CodeGenerator::generateJSServerImp(const ContextPtr &cPtr)
{
    string sFileName = TC_File::excludeFileExt(_sToPath + TC_File::extractFileName(cPtr->getFileName())) + "Imp.js";
    if (TC_File::isFileExist(sFileName))
    {
        return ;
    }

    ostringstream str;
    str << printHeaderRemark("Imp");
    str << "\"use strict\";" << endl << endl;

    vector<NamespacePtr> namespaces = cPtr->getNamespaces();
    set<string> setNamespace;
    for(size_t i = 0; i < namespaces.size(); i++)
    {
        if (setNamespace.count(namespaces[i]->getId()) != 0)
        {
            continue;
        }
        setNamespace.insert(namespaces[i]->getId());

        str << "var " << namespaces[i]->getId() << " = require(\"./" 
            << TC_File::excludeFileExt(TC_File::extractFileName(cPtr->getFileName())) << ".js\")." 
            << namespaces[i]->getId() << ";" << endl;

        str << "module.exports." << namespaces[i]->getId() << " = " << namespaces[i]->getId() << ";" << endl;
    }
    str << endl;

    set<string> setInterface;
    for(size_t i = 0; i < namespaces.size(); i++) 
    {
        vector<InterfacePtr> & is = namespaces[i]->getAllInterfacePtr();
        for (size_t ii = 0; ii < is.size(); ii++)
        {
            if (setInterface.count(namespaces[i]->getId() + "::" + is[ii]->getId()) != 0)
            {
                continue;
            }
            setInterface.insert(namespaces[i]->getId() + "::" + is[ii]->getId());

            str << namespaces[i]->getId() << "." << is[ii]->getId() << "Imp.prototype.initialize = function () {" << endl;
            INC_TAB;
            str << TAB << "//TODO::" << endl;
            DEL_TAB;
            str << "};" << endl << endl;
        }
    }

    for(size_t i = 0; i < namespaces.size(); i++)
	{
		str << generateJSServerImp(cPtr, namespaces[i]);
	}

    TC_File::makeDirRecursive(_sToPath, 0755);
    makeUTF8File(sFileName, str.str());
}