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

string CodeGenerator::generatePing(const NamespacePtr &nPtr, const InterfacePtr &pPtr)
{
    ostringstream str;

    str << TAB << "var __" << nPtr->getId() << "_" << pPtr->getId() << "$" << TC_Common::lower(IDL_NAMESPACE_STR) << "_ping$RE = function (_ret) {" << endl;
    INC_TAB;
    str << TAB << "if (this.getRequestVersion() === " << IDL_NAMESPACE_STR << "Stream.Wup.WUP_SIMPLE || this.getRequestVersion() === " << IDL_NAMESPACE_STR << "Stream.Wup.WUP_COMPLEX) {" << endl;
    INC_TAB;
    str << TAB << "var wup = new " << IDL_NAMESPACE_STR << "Stream.UniAttribute();" << endl; 
    str << TAB << "wup.wupVersion = this.getRequestVersion();" << endl; 
    str << TAB << "wup.writeInt32(\"\", _ret);" << endl << endl; 
    str << TAB << "this.doResponse(wup.encode());" << endl;
    DEL_TAB;
    str << TAB << "} else {" << endl;
    INC_TAB;
    str << TAB << "var os = new " << IDL_NAMESPACE_STR << "Stream." << IDL_TYPE << "OutputStream();" << endl; 
    str << TAB << "os.writeInt32(0, _ret);" << endl << endl; 
    str << TAB << "this.doResponse(os.getBinBuffer());" << endl;
    DEL_TAB;
    str << TAB << "}" << endl;
    DEL_TAB;
    str << TAB << "};" << endl << endl;

    str << TAB << nPtr->getId() << "." << pPtr->getId() << "Imp.prototype.__" << TC_Common::lower(IDL_NAMESPACE_STR) << "_ping = function (current) {" << endl;
    INC_TAB;
    str << TAB << "__" << nPtr->getId() << "_" << pPtr->getId() << "$" << TC_Common::lower(IDL_NAMESPACE_STR) << "_ping$RE.call(current, 0);" << endl << endl;
    str << TAB << "return " << IDL_NAMESPACE_STR << "Error.SUCCESS;" << endl;
    DEL_TAB;
    str << TAB << "};" << endl;

    return str.str();
}

string CodeGenerator::generateAsync(const NamespacePtr &nPtr, const InterfacePtr &pPtr, const OperationPtr &oPtr)
{
    ostringstream str;

    string sParams = "";
    if (oPtr->getReturnPtr()->getTypePtr())
    {
        sParams += "_ret";

        // 写入 Dependent 列表
        getDataType(oPtr->getReturnPtr()->getTypePtr());
    }

    vector<ParamDeclPtr> & vParamDecl = oPtr->getAllParamDeclPtr();
    for (size_t i = 0; i < vParamDecl.size(); i++)
    {
        if (!vParamDecl[i]->isOut()) continue;

        sParams += (sParams.empty()?"":", ") + vParamDecl[i]->getTypeIdPtr()->getId();
    }

    str << TAB << "var __" << nPtr->getId() << "_" << pPtr->getId() << "$" << oPtr->getId() << "$RE = function (" << sParams << ") {" << endl;

    INC_TAB;

    if (sParams.empty())
    {
        str << TAB << "this.doResponse(new " << IDL_NAMESPACE_STR << "Stream.BinBuffer());" << endl;
        DEL_TAB;
        str << TAB << "};" << endl;

        return str.str();
    }

    str << TAB << "if (this.getRequestVersion() === " << IDL_NAMESPACE_STR << "Stream.Wup.WUP_SIMPLE || this.getRequestVersion() === " << IDL_NAMESPACE_STR << "Stream.Wup.WUP_COMPLEX) {" << endl;
    INC_TAB;
    str << TAB << "var wup = new " << IDL_NAMESPACE_STR << "Stream.UniAttribute();" << endl;
    str << TAB << "wup.wupVersion = this.getRequestVersion();" << endl;
    if (oPtr->getReturnPtr()->getTypePtr())
    {
        str << TAB << "wup." << toFunctionName(oPtr->getReturnPtr(), "write") << "(\"\", _ret" 
            << (isRawOrString(oPtr->getReturnPtr()->getTypePtr()) ? ", 1" : "") << ");" << endl;
    }
    for (size_t i = 0; i < vParamDecl.size(); i++)
    {
        if (!vParamDecl[i]->isOut()) continue;

        str << TAB << "wup." << toFunctionName(vParamDecl[i]->getTypeIdPtr(), "write") << "(\"" 
            << vParamDecl[i]->getTypeIdPtr()->getId() << "\", " << vParamDecl[i]->getTypeIdPtr()->getId()
            << (isRawOrString(vParamDecl[i]->getTypeIdPtr()->getTypePtr()) ? ", 1" : "") << ");" << endl;
    }
    str << endl;
    str << TAB << "this.doResponse(wup.encode());" << endl;
    DEL_TAB;
    str << TAB << "} else {" << endl;

    INC_TAB;
    str << TAB << "var os = new " << IDL_NAMESPACE_STR << "Stream." << IDL_TYPE << "OutputStream();" << endl;
    if (oPtr->getReturnPtr()->getTypePtr())
    {
        str << TAB << "os." << toFunctionName(oPtr->getReturnPtr(), "write") << "(0, _ret" 
            << (isRawOrString(oPtr->getReturnPtr()->getTypePtr()) ? ", 1" : "") << ");" << endl;
    }
    for (size_t i = 0; i < vParamDecl.size(); i++)
    {
        if (!vParamDecl[i]->isOut()) continue;

        str << TAB << "os." << toFunctionName(vParamDecl[i]->getTypeIdPtr(), "write") << "(" 
            << (i + 1) << ", " << vParamDecl[i]->getTypeIdPtr()->getId()
            << (isRawOrString(vParamDecl[i]->getTypeIdPtr()->getTypePtr()) ? ", 1" : "") << ");" << endl;
    }
    str << endl;
    str << TAB << "this.doResponse(os.getBinBuffer());" << endl;
    DEL_TAB;
    str << TAB << "}" << endl;

    DEL_TAB;

    str << TAB << "};" << endl;

    return str.str();
}

string CodeGenerator::generateDispatch(const NamespacePtr &nPtr, const InterfacePtr &pPtr, const OperationPtr &oPtr)
{
    ostringstream str;
    vector<ParamDeclPtr> & vParamDecl = oPtr->getAllParamDeclPtr();

    str << TAB << nPtr->getId() << "." << pPtr->getId() << "Imp.prototype.__" << oPtr->getId() << " = function (current" << (vParamDecl.size() != 0 ? ", binBuffer" : "") << ") {" << endl;

    INC_TAB;

    ostringstream dstr;

    for (size_t i = 0; i < vParamDecl.size(); i++)
    {
        dstr << TAB << "var " << vParamDecl[i]->getTypeIdPtr()->getId() << " = null;" << endl;
    }
    if (vParamDecl.size() != 0)
    {
        dstr << endl;
    }

    dstr << TAB << "if (current.getRequestVersion() === " << IDL_NAMESPACE_STR << "Stream.Wup.WUP_SIMPLE || current.getRequestVersion() === " << IDL_NAMESPACE_STR << "Stream.Wup.WUP_COMPLEX) {" << endl;
    INC_TAB;
    dstr << TAB << "var wup = new " << IDL_NAMESPACE_STR << "Stream.UniAttribute();" << endl;
    dstr << TAB << "wup.wupVersion = current.getRequestVersion();" << endl;
    dstr << TAB << "wup.decode(binBuffer);" << endl;

    for (size_t i = 0; i < vParamDecl.size(); i++)
    {
        dstr << TAB << vParamDecl[i]->getTypeIdPtr()->getId()
                << " = wup." << toFunctionName(vParamDecl[i]->getTypeIdPtr(), "read")
                << "(\"" << vParamDecl[i]->getTypeIdPtr()->getId() << "\"";

        if (!isSimple(vParamDecl[i]->getTypeIdPtr()->getTypePtr()) && !isBinBuffer(vParamDecl[i]->getTypeIdPtr()->getTypePtr()))
        {
            dstr << ", " << getDataType(vParamDecl[i]->getTypeIdPtr()->getTypePtr());
        }

        if (vParamDecl[i]->isOut())
        {
            dstr << ", " << getDefault(vParamDecl[i]->getTypeIdPtr(), "", nPtr->getId(), true)
                    << (isRawOrString(vParamDecl[i]->getTypeIdPtr()->getTypePtr()) ? ", 1" : "");
        }
        
        dstr << ");" << endl;
    }
    DEL_TAB;
    dstr << TAB << "} else {" << endl;

    INC_TAB;
    dstr << TAB << "var is = new " << IDL_NAMESPACE_STR << "Stream." << IDL_TYPE << "InputStream(binBuffer);" << endl;
    string sParams = "";
    for (size_t i = 0; i < vParamDecl.size(); i++)
    {
        sParams += ", " + vParamDecl[i]->getTypeIdPtr()->getId();

        dstr << TAB << vParamDecl[i]->getTypeIdPtr()->getId()
                << " = is." << toFunctionName(vParamDecl[i]->getTypeIdPtr(), "read") << "("
                << (i + 1) << ", " << (vParamDecl[i]->isOut() ? "false" : "true") << ", ";

        if (isSimple(vParamDecl[i]->getTypeIdPtr()->getTypePtr()))
        {
            dstr << getDefault(vParamDecl[i]->getTypeIdPtr(), vParamDecl[i]->getTypeIdPtr()->def(), nPtr->getId())
                    << (isRawOrString(vParamDecl[i]->getTypeIdPtr()->getTypePtr()) ? ", 1" : "");
        }
        else 
        {
            dstr << getDataType(vParamDecl[i]->getTypeIdPtr()->getTypePtr());
        }

        dstr << ");" << endl;
    }
    DEL_TAB;
    dstr << TAB << "}" << endl << endl;

    if (!sParams.empty())
    {
        str << dstr.str();
    }

    str << TAB << "current.sendResponse = __" << nPtr->getId() << "_" << pPtr->getId() << "$" << oPtr->getId() << "$RE;" << endl << endl;

    str << TAB << "this." << oPtr->getId() << "(current" << sParams << ");" << endl << endl;

    str << TAB << "return " << IDL_NAMESPACE_STR << "Error.SUCCESS;" << endl;

    DEL_TAB;

    str << TAB << "};" << endl;

    return str.str();
}

string CodeGenerator::generateJSServer(const NamespacePtr &nPtr, const InterfacePtr &pPtr, const OperationPtr &oPtr)
{
    ostringstream str;

    str << TAB << nPtr->getId() << "." << pPtr->getId() << "Imp.prototype." << oPtr->getId() << " = function () {" << endl;

    INC_TAB;
    str << TAB << "assert.fail(\"" << oPtr->getId() << " function not implemented\");" << endl;
    DEL_TAB;
    str << TAB << "};" << endl;

    return str.str();
}

string CodeGenerator::generateJSServer(const InterfacePtr &pPtr, const NamespacePtr &nPtr)
{
    ostringstream str;
    vector<OperationPtr> & vOperation = pPtr->getAllOperationPtr();

    //生成类
    str << TAB << nPtr->getId() << "." << pPtr->getId() << "Imp = function () { " << endl;
    INC_TAB;
    str << TAB << "this._name   = undefined;" << endl;
    str << TAB << "this._worker = undefined;" << endl;
    DEL_TAB;
    str << TAB << "};" << endl << endl;

    //生成初始化函数
    str << TAB << nPtr->getId() << "." << pPtr->getId() << "Imp.prototype.initialize = function () {};" << endl << endl;

    //生成分发函数
    str << TAB << nPtr->getId() << "." << pPtr->getId() << "Imp.prototype.onDispatch = function (current, funcName, binBuffer) { " << endl;
    INC_TAB;
    str << TAB << "if (\"__\" + funcName in this) {" << endl;
    INC_TAB;
    str << TAB << "return this[\"__\" + funcName](current, binBuffer);" << endl;
    DEL_TAB;
    str << TAB << "} else {" << endl;
    INC_TAB;
    str << TAB << "return " << IDL_NAMESPACE_STR << "Error.SERVER.FUNC_NOT_FOUND;" << endl;
    DEL_TAB;
    str << TAB << "}" << endl;
    DEL_TAB;
    str << TAB << "};" << endl << endl;

    //生成 PING 方法
    str << generatePing(nPtr, pPtr) << endl;

    //生成接口函数
    for (size_t i = 0; i < vOperation.size(); i++)
    {
        str << generateJSServer(nPtr, pPtr, vOperation[i]) << endl;
        str << generateAsync(nPtr, pPtr, vOperation[i]) << endl;
        str << generateDispatch(nPtr, pPtr, vOperation[i]) << endl;
    }

    str << endl;

    return str.str();
}

string CodeGenerator::generateJSServer(const NamespacePtr &pPtr, bool &bNeedStream, bool &bNeedRpc, bool &bNeedAssert)
{
	ostringstream str;

    vector<InterfacePtr> & is = pPtr->getAllInterfacePtr();
    for (size_t i = 0; i < is.size(); i++)
    {
        str << generateJSServer(is[i], pPtr) << endl;
    }
    if (is.size() != 0)
    {
        bNeedRpc = true;
        bNeedStream = true;
        bNeedAssert = true;
    }

	return str.str();
}

bool CodeGenerator::generateJSServer(const ContextPtr &pPtr)
{
    vector<NamespacePtr> namespaces = pPtr->getNamespaces();

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

    //生成编解码 + 服务类
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
        estr << generateJSServer(namespaces[i], bNeedStream, bNeedRpc, bNeedAssert) << endl;
    }

    if (estr.str().empty())
    {
        return false;
    }

    //再生成导入模块
    ostringstream ostr;
    if (bNeedAssert)
    {
        ostr << TAB << "var assert    = require(\"assert\");" << endl;
    }
    if (bNeedStream)
    {
        ostr << TAB << "var " << IDL_NAMESPACE_STR << "Stream = require(\"" << _sStreamPath << "\");" << endl;
    }
    if (bNeedRpc)
    {
        ostr << TAB << "var " << IDL_NAMESPACE_STR << "Error  = require(\"" << _sRpcPath << "\").error;" << endl;
    }
    for (map<string, ImportFile>::iterator it = _mapFiles.begin(); it != _mapFiles.end(); it++)
    {
        if (it->second.sModule.empty()) continue;

        if (estr.str().find(it->second.sModule + ".") == string::npos) continue;

        ostr << TAB << "var " << it->second.sModule << " = require(\"" << it->second.sFile << "\");" << endl;
    }

    ostringstream str;

    str << printHeaderRemark("Server");
    str << "\"use strict\";" << endl << endl;
    str << ostr.str() << endl;
    str << istr.str();
    str << estr.str() << endl;

    string sFileName = TC_File::excludeFileExt(_sToPath + TC_File::extractFileName(pPtr->getFileName())) + ".js";

    TC_File::makeDirRecursive(_sToPath, 0755);
    makeUTF8File(sFileName, str.str());

    return true;
}