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

struct SortOperation
{
    bool operator()(const OperationPtr &o1, const OperationPtr &o2)
    {
        return o1->getId() < o2->getId();
    }
};

string CodeGenerator::generateJSProxy(const NamespacePtr &nPtr, const InterfacePtr &pPtr, const OperationPtr &oPtr)
{
    ostringstream str;

    //SETP01 生成编码接口
    str << TAB << "var __" << nPtr->getId() << "_" << pPtr->getId() << "$" << oPtr->getId() << "$EN = function (";
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

    str << TAB << "var os = new " << IDL_NAMESPACE_STR << "Stream." << IDL_TYPE << "OutputStream();" << endl;

    for (size_t i = 0; i < vParamDecl.size(); i++)
    {
        if (vParamDecl[i]->isOut()) continue;

        str << TAB << "os." << toFunctionName(vParamDecl[i]->getTypeIdPtr(), "write") << "(" 
            << (i + 1) << ", " << vParamDecl[i]->getTypeIdPtr()->getId() 
            << (isRawOrString(vParamDecl[i]->getTypeIdPtr()->getTypePtr()) ? ", 1" : "") << ");" << endl;

        // 写入 Dependent 列表
        getDataType(vParamDecl[i]->getTypeIdPtr()->getTypePtr());
    }

    str << TAB << "return os.getBinBuffer();" << endl;

    DEL_TAB;

    str << TAB << "};" << endl << endl;


    //STEP02 生成解码函数
    str << TAB << "var __" << nPtr->getId() << "_" << pPtr->getId() << "$" << oPtr->getId() << "$DE = function (data) {" << endl;
    INC_TAB;
    str << TAB << "try {" << endl;
    INC_TAB;

    bool bHasParamOut = false;
    if (vParamDecl.size() > 0) 
    {
        for (size_t i = 0; i < vParamDecl.size(); i++)
        {
            if (vParamDecl[i]->isOut()) {
                bHasParamOut = true;
                break;   
            }
        }
    }

    if (oPtr->getReturnPtr()->getTypePtr() || bHasParamOut)
    {
        str << TAB << "var is = new " << IDL_NAMESPACE_STR << "Stream." << IDL_TYPE << "InputStream(data.response.sBuffer);" << endl;
    }

    str << TAB << "return {" << endl;
    INC_TAB;
    str << TAB << "\"request\" : data.request," << endl;
    str << TAB << "\"response\" : {" << endl;
    INC_TAB;
    str << TAB << "\"costtime\" : data.request.costtime";

    if (oPtr->getReturnPtr()->getTypePtr())
    {
        str << "," << endl;
        str << TAB << "\"return\""
            << " : is." << toFunctionName(oPtr->getReturnPtr(), "read") << "(0, true, ";

        if (isSimple(oPtr->getReturnPtr()->getTypePtr()))
        {
            str << getDefault(oPtr->getReturnPtr(), oPtr->getReturnPtr()->def(), nPtr->getId())
                << (isRawOrString(oPtr->getReturnPtr()->getTypePtr()) ? ", 1" : "");
        }
        else
        {
            str << getDataType(oPtr->getReturnPtr()->getTypePtr());
        }

        str << ")";
    }

    if (bHasParamOut)
    {
        str << "," << endl;
        str << TAB << "\"arguments\" : {" << endl;

        INC_TAB;

        for (size_t i = 0; i < vParamDecl.size(); i++)
        {
            if (!vParamDecl[i]->isOut()) continue;

            str << TAB << "\"" << vParamDecl[i]->getTypeIdPtr()->getId() << "\""
                << " : is." << toFunctionName(vParamDecl[i]->getTypeIdPtr(), "read") << "(" << (i + 1) << ", true, ";

            if (isSimple(vParamDecl[i]->getTypeIdPtr()->getTypePtr()))
            {
                str << getDefault(vParamDecl[i]->getTypeIdPtr(), vParamDecl[i]->getTypeIdPtr()->def(), nPtr->getId()) 
                    << (isRawOrString(vParamDecl[i]->getTypeIdPtr()->getTypePtr()) ? ", 1" : "");
            }
            else
            {
                str << getDataType(vParamDecl[i]->getTypeIdPtr()->getTypePtr());
            }

            str << ")";

            if (i == vParamDecl.size() - 1)
            {
                str << endl;
            }
            else
            {
                str << "," << endl;
            }

        }

        DEL_TAB;
        str << TAB << "}";
    }

    str << endl;
    DEL_TAB;
    str << TAB << "}" << endl;
    DEL_TAB;
    str << TAB << "};" << endl;
    DEL_TAB;
    str << TAB << "} catch (e) {" << endl;
    INC_TAB;
    str << TAB << "throw {" << endl;
    INC_TAB;
    str << TAB << "\"request\" : data.request," << endl;
    str << TAB << "\"response\" : {" << endl;
    INC_TAB;
    str << TAB << "\"costtime\" : data.request.costtime," << endl;
    str << TAB << "\"error\" : {" << endl;
    INC_TAB;
    str << TAB << "\"code\" : " << IDL_NAMESPACE_STR << "Error.CLIENT.DECODE_ERROR," << endl;
    str << TAB << "\"message\" : e.message" << endl;
    DEL_TAB;
    str << TAB << "}" << endl;
    DEL_TAB;
    str << TAB << "}" << endl;
    DEL_TAB;
    str << TAB << "};" << endl;
    DEL_TAB;
    str << TAB << "}" << endl;
    DEL_TAB;
    str << TAB << "};" << endl << endl;


    //STEP03 生成框架调用错误处理函数
    str << TAB << "var __" << nPtr->getId() << "_" << pPtr->getId() << "$" << oPtr->getId() << "$ER = function (data) {" << endl;
    INC_TAB;
    str << TAB << "throw {" << endl;
    INC_TAB;
    str << TAB << "\"request\" : data.request," << endl;
    str << TAB << "\"response\" : {" << endl;
    INC_TAB;
    str << TAB << "\"costtime\" : data.request.costtime," << endl;
    str << TAB << "\"error\" : data.error" << endl;
    DEL_TAB;
    str << TAB << "}" << endl;
    DEL_TAB;
    str << TAB << "}" << endl;
    DEL_TAB;
    str << TAB << "};" << endl << endl;


    //SETP04 生成函数接口
    str << TAB << nPtr->getId() << "." << pPtr->getId() << "Proxy.prototype." << oPtr->getId() << " = function (";
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

    str << TAB << "return this._worker." << TC_Common::lower(IDL_NAMESPACE_STR) << "_invoke(\"" << oPtr->getId() << "\", ";
    str << "__" << nPtr->getId() << "_" << pPtr->getId() << "$" << oPtr->getId() << "$EN(";
    for (size_t i = 0; i < vParamDecl.size(); i++)
    {
        if (vParamDecl[i]->isOut())
        {
            continue;
        }

        str << (i == 0?"":", ") << vParamDecl[i]->getTypeIdPtr()->getId();
    }
    str << "), arguments[arguments.length - 1]).then(";
    str << "__" << nPtr->getId() << "_" << pPtr->getId() << "$" << oPtr->getId() << "$DE, ";
    str << "__" << nPtr->getId() << "_" << pPtr->getId() << "$" << oPtr->getId() << "$ER);" << endl;

    DEL_TAB;

    str << TAB << "};" << endl;

    return str.str();
}

string CodeGenerator::generateJSProxy(const NamespacePtr &nPtr, const InterfacePtr &pPtr)
{
    ostringstream str;

    vector<OperationPtr> & vOperation = pPtr->getAllOperationPtr();
    sort(vOperation.begin(), vOperation.end(), SortOperation());
    for (size_t i = 0; i < vOperation.size(); i++)
    {
        str << generateJSProxy(nPtr, pPtr, vOperation[i]) << endl;
    }

    return str.str();
}

string CodeGenerator::generateJSProxy(const NamespacePtr &nPtr, bool &bNeedRpc, bool &bNeedStream)
{
    ostringstream str;

    vector<InterfacePtr> & is = nPtr->getAllInterfacePtr();
    for (size_t i = 0; i < is.size(); i++)
    {
        str << generateJSProxy(nPtr, is[i]) << endl;
    }
    if (is.size() != 0)
    {
        bNeedRpc = true;
        bNeedStream = true;
    }

    return str.str();
}

bool CodeGenerator::generateJSProxy(const ContextPtr &cPtr)
{
    vector<NamespacePtr> namespaces = cPtr->getNamespaces();

    ostringstream istr;
    set<string> setNamespace;
    for(size_t i = 0; i < namespaces.size(); i++)
    {
        if (setNamespace.count(namespaces[i]->getId()) == 0)
        {
            istr << TAB << "var " << namespaces[i]->getId() << " = " << namespaces[i]->getId() << " || {};" << endl;
            istr << TAB << "module.exports." << namespaces[i]->getId() << " = " << namespaces[i]->getId() << ";" << endl << endl;

            setNamespace.insert(namespaces[i]->getId());
        }
    }

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

            istr << TAB << namespaces[i]->getId() << "." << is[ii]->getId() << "Proxy = function () {" << endl;
            INC_TAB;
            istr << TAB << "this._name   = undefined;" << endl;
            istr << TAB << "this._worker = undefined;" << endl;
            DEL_TAB;
            istr << TAB << "};" << endl << endl;

            istr << TAB << namespaces[i]->getId() << "." << is[ii]->getId() << "Proxy.prototype.setTimeout = function (iTimeout) {" << endl;
            INC_TAB;
            istr << TAB << "this._worker.timeout = iTimeout;" << endl;
            DEL_TAB;
            istr << TAB << "};" << endl << endl;

            istr << TAB << namespaces[i]->getId() << "." << is[ii]->getId() << "Proxy.prototype.getTimeout = function () {" << endl;
            INC_TAB;
            istr << TAB << "return this._worker.timeout;" << endl;
            DEL_TAB;
            istr << TAB << "};" << endl << endl;
        }
    }

    //先生成编解码 + 代理类
    ostringstream estr;
    bool bNeedAssert = false;
    bool bNeedStream = false;
	for(size_t i = 0; i < namespaces.size(); i++)
	{
		estr << generateJS(namespaces[i], bNeedStream, bNeedAssert);
	}

    bool bNeedRpc = false;
    for(size_t i = 0; i < namespaces.size(); i++)
    {
        estr << generateJSProxy(namespaces[i], bNeedRpc, bNeedStream);
    }

    if (estr.str().empty())
    {
        return false;
    }

    //再生成导入模块
    ostringstream ostr;
    for (map<string, ImportFile>::iterator it = _mapFiles.begin(); it != _mapFiles.end(); it++)
    {
        if (it->second.sModule.empty()) continue;

        if (estr.str().find(it->second.sModule + ".") == string::npos) continue;

        ostr << "var " << it->second.sModule << " = require(\"" << it->second.sFile << "\");" << endl;
    }

    //生成文件内容    
    ostringstream sstr;
    sstr << printHeaderRemark("Client");
    sstr << "\"use strict\";" << endl << endl;
    if (bNeedAssert)
    {
        sstr << "var assert    = require(\"assert\");" << endl;
    }
    if (bNeedStream)
    {
        sstr << "var " << IDL_NAMESPACE_STR << "Stream = require(\"" << _sStreamPath << "\");" << endl;
    }
    if (bNeedRpc)
    {
        sstr << "var " << IDL_NAMESPACE_STR << "Error  = require(\"" << _sRpcPath << "\").error;" << endl;
    }

    sstr << ostr.str() << endl;
    sstr << istr.str() << endl;
    sstr << estr.str() << endl;

    string sFileName = TC_File::excludeFileExt(_sToPath + TC_File::extractFileName(cPtr->getFileName())) + "Proxy.js";

    TC_File::makeDirRecursive(_sToPath, 0755);
    makeUTF8File(sFileName, sstr.str());

    return true;
}