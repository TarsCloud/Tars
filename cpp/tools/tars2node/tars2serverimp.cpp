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


#include "tars2node.h"

#define TAB     g_parse->getTab()
#define INC_TAB g_parse->incTab()
#define DEL_TAB g_parse->delTab()

string Tars2Node::generateJSServerImp(const NamespacePtr &nPtr, const InterfacePtr &pPtr, const OperationPtr &oPtr)
{
    std::ostringstream str;
    str << nPtr->getId() << "." << pPtr->getId() << "Imp.prototype." << oPtr->getId() << " = function (current";

    vector<ParamDeclPtr> & vParamDecl = oPtr->getAllParamDeclPtr();
    for (size_t i = 0; i < vParamDecl.size(); i++)
    {
        str << ", " << vParamDecl[i]->getTypeIdPtr()->getId();
    }

    str << ") {" << endl;
    str << "    //TODO::\r\n" << endl;
    str << "}" << endl << endl;

    return str.str();
}

string Tars2Node::generateJSServerImp(const NamespacePtr &nPtr, const InterfacePtr &pPtr)
{
    ostringstream str;


    vector<OperationPtr> & vOperation = pPtr->getAllOperationPtr();
    for (size_t i = 0; i < vOperation.size(); i++)
    {
        str << generateJSServerImp(nPtr, pPtr, vOperation[i]);
    }

    return str.str();
}

string Tars2Node::generateJSServerImp(const ContextPtr &cPtr, const NamespacePtr &nPtr)
{
    std::ostringstream str;

    vector<InterfacePtr> & is = nPtr->getAllInterfacePtr();
    for (size_t i = 0; i < is.size(); i++)
    {
        str << generateJSServerImp(nPtr, is[i]) << endl;
    }

    return str.str();
}

void Tars2Node::generateJSServerImp(const ContextPtr &cPtr)
{
    std::string sFileName = tars::TC_File::excludeFileExt(_sToPath + tars::TC_File::extractFileName(cPtr->getFileName())) + "Imp.js";
    if (tars::TC_File::isFileExist(sFileName))
    {
        return ;
    }

    std::ostringstream str;
    str << g_parse->printHeaderRemark();

    vector<NamespacePtr> namespaces = cPtr->getNamespaces();
    std::set<string> setNamespace;
    for(size_t i = 0; i < namespaces.size(); i++)
    {
        if (setNamespace.count(namespaces[i]->getId()) != 0)
        {
            continue;
        }
        setNamespace.insert(namespaces[i]->getId());

        str << "var " << namespaces[i]->getId() << " = require('./" 
            << tars::TC_File::excludeFileExt(tars::TC_File::extractFileName(cPtr->getFileName())) << ".js')." 
            << namespaces[i]->getId() << ";" << endl;

        str << "module.exports." << namespaces[i]->getId() << " = " << namespaces[i]->getId() << ";" << endl;
    }
    str << endl;

    std::set<string> setInterface;
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

            str << namespaces[i]->getId() << "." << is[ii]->getId() << "Imp.prototype.initialize = function ( ) {" << endl;
            str << "    //TODO::" << endl << endl;
            str << "}" << endl << endl;
        }
    }

    for(size_t i = 0; i < namespaces.size(); i++)
	{
		str << generateJSServerImp(cPtr, namespaces[i]);
	}

    tars::TC_File::makeDirRecursive(_sToPath, 0755);
    makeUTF8File(sFileName, str.str());
}

