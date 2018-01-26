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

struct SortOperation
{
    bool operator()(const OperationPtr &o1, const OperationPtr &o2)
    {
        return o1->getId() < o2->getId();
    }
};

string Tars2Node::generateJSProxy(const NamespacePtr &nPtr, const InterfacePtr &pPtr, const OperationPtr &oPtr)
{
    std::ostringstream str;

    //SETP01 生成函数接口
    str << TAB << nPtr->getId() << "." << pPtr->getId() << "Proxy.prototype." << oPtr->getId() << " = function (";
    vector<ParamDeclPtr> & vParamDecl = oPtr->getAllParamDeclPtr();
    for (size_t i = 0; i < vParamDecl.size(); i++)
    {
        if (vParamDecl[i]->isOut())
        {
            continue;
        }

        str << (i == 0?"":", ") << vParamDecl[i]->getTypeIdPtr()->getId();
    }
    str << ") {" << endl;

    INC_TAB;

    //STEP02 生成函数结构体[生成编码函数]
    str << TAB << "var _encode = function () { " << endl;
    
    INC_TAB;
    str << TAB << "var os = new TarsStream.OutputStream();" << endl;

    for (size_t i = 0; i < vParamDecl.size(); i++)
    {
        if (vParamDecl[i]->isOut()) continue;

        str << TAB << "os." << toFunctionName(vParamDecl[i]->getTypeIdPtr(), "write") << "(" 
            << (i + 1) << ", " << vParamDecl[i]->getTypeIdPtr()->getId() << ");" << endl;
    }

    str << TAB << "return os.getBinBuffer();" << endl;

    DEL_TAB;
    str << TAB << "}" << endl << endl;

    //STEP03 生成函数结构体[生成解码函数]
    str << TAB << "var _decode = function (data) {" << endl;

    INC_TAB;

    str << TAB << "try {" << endl;

    INC_TAB;
    str << TAB << "var response = {arguments:{}};" << endl;
    if (oPtr->getReturnPtr()->getTypePtr() || vParamDecl.size() != 0)
    {
        str << TAB << "var is = new TarsStream.InputStream(data.response.sBuffer);" << endl << endl;

        str << TAB << "response.costtime = data.request.costtime;" << endl;

        if (oPtr->getReturnPtr()->getTypePtr())
        {
            str << TAB << "response.return   = is." << toFunctionName(oPtr->getReturnPtr(), "read") << "(0, true, "
                << getDataType(oPtr->getReturnPtr()->getTypePtr()) << ");" << endl;
        }

        for (size_t i = 0; i < vParamDecl.size(); i++)
        {
            if (!vParamDecl[i]->isOut()) continue;

            str << TAB << "response.arguments." << vParamDecl[i]->getTypeIdPtr()->getId() 
                << " = is." << toFunctionName(vParamDecl[i]->getTypeIdPtr(), "read") << "(" 
                << (i + 1) << ", true, " << getDataType(vParamDecl[i]->getTypeIdPtr()->getTypePtr()) << ");" << endl;
        }
    }

    str << endl;

    str << TAB << "return {request:data.request, response:response};" << endl;

    DEL_TAB;

    str << TAB << "} catch (e) {" << endl;

    INC_TAB;

    str << TAB << "var response = { };" << endl;
    str << TAB << "response.costtime      = data.request.costtime;" << endl;
    str << TAB << "response.error         = {};" << endl;
    str << TAB << "response.error.code    = TarsError.CLIENT.DECODE_ERROR;" << endl;
    str << TAB << "response.error.message = e.message;" << endl;

    str << endl;

    str << TAB << "throw { request : data.request, response : response};" << endl;

    DEL_TAB;

    str << TAB << "}" << endl;

    DEL_TAB;

    str << TAB << "}" << endl << endl;

    //STEP04 生成函数结构体[生成TARS框架调用错误处理函数]
    str << TAB << "var _error = function(data) {" << endl;
    INC_TAB;
    str << TAB << "var response = {};" << endl;
    str << TAB << "response.costtime = data.request.costtime;" << endl;
    str << TAB << "response.error    = data.error;" << endl;
    str << endl;
    str << TAB << "throw {request:data.request, response:response};" << endl;
    DEL_TAB;
    str << TAB << "}" << endl << endl;

    str << TAB << "return this._worker.tars_invoke('" << oPtr->getId() << "', _encode(), arguments.length != 0?arguments[arguments.length - 1]:undefined).then(_decode, _error);" << endl;

    DEL_TAB;

    str << TAB << "}" << endl << endl;

    return str.str();
}

string Tars2Node::generateJSProxy(const NamespacePtr &nPtr, const InterfacePtr &pPtr)
{
    std::ostringstream str;

    vector<OperationPtr> & vOperation = pPtr->getAllOperationPtr();
    std::sort(vOperation.begin(), vOperation.end(), SortOperation());
    for (size_t i = 0; i < vOperation.size(); i++)
    {
        str << generateJSProxy(nPtr, pPtr, vOperation[i]);
    }

    return str.str();
}

string Tars2Node::generateJSProxy(const NamespacePtr &nPtr)
{
    std::ostringstream str;

    vector<InterfacePtr> & is = nPtr->getAllInterfacePtr();
    for (size_t i = 0; i < is.size(); i++)
    {
        str << generateJSProxy(nPtr, is[i]) << endl;
    }

    return str.str();
}

void Tars2Node::generateJSProxy(const ContextPtr &cPtr)
{
    vector<NamespacePtr> namespaces = cPtr->getNamespaces();

    std::ostringstream istr;
    std::set<string> setNamespace;
    for(size_t i = 0; i < namespaces.size(); i++)
    {
        if (setNamespace.count(namespaces[i]->getId()) == 0)
        {
            istr << TAB << "var " << namespaces[i]->getId() << " = " << namespaces[i]->getId() << " || {};" << endl;
            istr << TAB << "module.exports." << namespaces[i]->getId() << " = " << namespaces[i]->getId() << ";" << endl << endl;

            setNamespace.insert(namespaces[i]->getId());
        }
    }

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

            istr << TAB << namespaces[i]->getId() << "." << is[ii]->getId() << "Proxy = function () {" << endl;
            istr << TAB << "    this._name   = undefined;" << endl;
            istr << TAB << "    this._worker = undefined;" << endl;
            istr << TAB << "}" << endl << endl;

            istr << TAB << namespaces[i]->getId() << "." << is[ii]->getId() << "Proxy.prototype.setTimeout = function (iTimeout) {" << endl;
            istr << TAB << "    this._worker.timeout = iTimeout;" << endl;
            istr << TAB << "}" << endl << endl;

            istr << TAB << namespaces[i]->getId() << "." << is[ii]->getId() << "Proxy.prototype.getTimeout = function ( ) {" << endl;
            istr << TAB << "    return this._worker.timeout;" << endl;
            istr << TAB << "}" << endl << endl;
        }
    }

    //先生成编解码 + 代理类
    std::ostringstream estr;
	for(size_t i = 0; i < namespaces.size(); i++)
	{
		estr << generateJS(namespaces[i]);
	}

    for(size_t i = 0; i < namespaces.size(); i++)
    {
        estr << generateJSProxy(namespaces[i]);
    }

    //再生成导入模块
    std::ostringstream ostr;
    for (std::map<string, ImportFile>::iterator it = _mapFiles.begin(); it != _mapFiles.end(); it++)
    {
        if (it->second.sModule.empty()) continue;

        ostr << "var " << it->second.sModule << " = require('" << it->second.sFile << "');" << endl;
    }

    //生成文件内容    
    std::ostringstream sstr;
    sstr << g_parse->printHeaderRemark(); 
    sstr << "var TarsStream = require('" << _sTarsStreamPath << "');" << endl;
    sstr << "var TarsError  = require('" << _sTarsLibPath << "').error;" << endl;

    sstr << ostr.str() << endl;
    sstr << istr.str() << endl;
    sstr << estr.str() << endl;

    std::string sFileName = tars::TC_File::excludeFileExt(_sToPath + tars::TC_File::extractFileName(cPtr->getFileName())) + "Proxy.js";

    tars::TC_File::makeDirRecursive(_sToPath, 0755);
    makeUTF8File(sFileName, sstr.str());
}

