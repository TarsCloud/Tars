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


string Tars2Node::generateAsync(const NamespacePtr &nPtr, const InterfacePtr &pPtr, const OperationPtr &oPtr)
{
    std::ostringstream str;

    std::string sParams = "";
    if (oPtr->getReturnPtr()->getTypePtr())
    {
        sParams += "_ret";
    }

    vector<ParamDeclPtr> & vParamDecl = oPtr->getAllParamDeclPtr();
    for (size_t i = 0; i < vParamDecl.size(); i++)
    {
        if (!vParamDecl[i]->isOut()) continue;

        sParams += (sParams.empty()?"":", ") + vParamDecl[i]->getTypeIdPtr()->getId();
    }

    str << TAB << "current.sendResponse = function (" << sParams << ") {" << endl;

    if (sParams.empty())
    {
        str << endl;
        INC_TAB;
        str << TAB << "current.doResponse(new TarsStream.BinBuffer());" << endl;
        DEL_TAB;
        str << TAB << "}" << endl;

        return str.str();
    }

    INC_TAB;

    str << TAB << "if (current.getRequestVersion() === TarsStream.Tup.TUP_SIMPLE || current.getRequestVersion() === TarsStream.Tup.TUP_COMPLEX) {" << endl;
    INC_TAB;
    str << TAB << "var tup = new TarsStream.UniAttribute();" << endl;
    str << TAB << "tup.tupVersion = current.getRequestVersion();" << endl;
    if (oPtr->getReturnPtr()->getTypePtr())
    {
        str << TAB << "tup." << toFunctionName(oPtr->getReturnPtr(), "write") << "(\"\", _ret);" << endl;
    }
    for (size_t i = 0; i < vParamDecl.size(); i++)
    {
        if (!vParamDecl[i]->isOut()) continue;

        str << TAB << "tup." << toFunctionName(vParamDecl[i]->getTypeIdPtr(), "write") << "(\"" 
            << vParamDecl[i]->getTypeIdPtr()->getId() << "\", " << vParamDecl[i]->getTypeIdPtr()->getId() << ");" << endl;
    }
    str << endl;
    str << TAB << "current.doResponse(tup.encode());" << endl;
    DEL_TAB;
    str << TAB << "} else {" << endl;

    INC_TAB;
    str << TAB << "var os = new TarsStream.OutputStream();" << endl;
    if (oPtr->getReturnPtr()->getTypePtr())
    {
        str << TAB << "os." << toFunctionName(oPtr->getReturnPtr(), "write") << "(0, _ret);" << endl;
    }
    for (size_t i = 0; i < vParamDecl.size(); i++)
    {
        if (!vParamDecl[i]->isOut()) continue;

        str << TAB << "os." << toFunctionName(vParamDecl[i]->getTypeIdPtr(), "write") << "(" 
            << (i + 1) << ", " << vParamDecl[i]->getTypeIdPtr()->getId() << ");" << endl;
    }
    str << endl;
    str << TAB << "current.doResponse(os.getBinBuffer());" << endl;
    DEL_TAB;
    str << TAB << "}" << endl;

    DEL_TAB;

    str << TAB << "}" << endl;

    return str.str();
}

string Tars2Node::generateDispatch(const NamespacePtr &nPtr, const InterfacePtr &pPtr, const OperationPtr &oPtr)
{
    std::ostringstream dstr;
    vector<ParamDeclPtr> & vParamDecl = oPtr->getAllParamDeclPtr();

    INC_TAB;

    dstr << TAB << "if (current.getRequestVersion() === TarsStream.Tup.TUP_SIMPLE || current.getRequestVersion() === TarsStream.Tup.TUP_COMPLEX) {" << endl;
    INC_TAB;
    dstr << TAB << "var tup = new TarsStream.UniAttribute();" << endl;
    dstr << TAB << "tup.tupVersion = current.getRequestVersion();" << endl;
    dstr << TAB << "tup.decode(BinBuffer);" << endl;

    for (size_t i = 0; i < vParamDecl.size(); i++)
    {
        if (isSimple(vParamDecl[i]->getTypeIdPtr()->getTypePtr()) || isBinBuffer(vParamDecl[i]->getTypeIdPtr()->getTypePtr()))
        {
            dstr << TAB << "var " << vParamDecl[i]->getTypeIdPtr()->getId()
                << " = tup." << toFunctionName(vParamDecl[i]->getTypeIdPtr(), "read")
                << "(\"" << vParamDecl[i]->getTypeIdPtr()->getId() << "\"";

            if (vParamDecl[i]->isOut()) {
                dstr << ", " << getDefault(vParamDecl[i]->getTypeIdPtr(), "", nPtr->getId(), true);
            }

            dstr << ");" << endl;
        } else {
            dstr << TAB << "var " << vParamDecl[i]->getTypeIdPtr()->getId() 
                << " = tup." << toFunctionName(vParamDecl[i]->getTypeIdPtr(), "read") 
                << "(\"" << vParamDecl[i]->getTypeIdPtr()->getId() << "\", " 
                << getDataType(vParamDecl[i]->getTypeIdPtr()->getTypePtr());

            if (vParamDecl[i]->isOut()) {
                dstr << ", " << getDefault(vParamDecl[i]->getTypeIdPtr(), "", nPtr->getId(), true);
            }

            dstr << ");" << endl;
        }
    }
    DEL_TAB;
    dstr << TAB << "} else {" << endl;

    INC_TAB;
    dstr << TAB << "var is = new TarsStream.InputStream(BinBuffer);" << endl;
    string sParams = "";
    for (size_t i = 0; i < vParamDecl.size(); i++)
    {
        sParams += ", " + vParamDecl[i]->getTypeIdPtr()->getId();

        if (isSimple(vParamDecl[i]->getTypeIdPtr()->getTypePtr())) {
            dstr << TAB << "var " << vParamDecl[i]->getTypeIdPtr()->getId() 
                << " = is." << toFunctionName(vParamDecl[i]->getTypeIdPtr(), "read") << "(" 
                << (i + 1) << ", " << (vParamDecl[i]->isOut()?"false":"true") << ");" << endl;
        } else {
            dstr << TAB << "var " << vParamDecl[i]->getTypeIdPtr()->getId() 
                << " = is." << toFunctionName(vParamDecl[i]->getTypeIdPtr(), "read") << "(" 
                << (i + 1) << ", " << (vParamDecl[i]->isOut()?"false":"true") << ", " << getDataType(vParamDecl[i]->getTypeIdPtr()->getTypePtr()) << ");" << endl;
        }
    }
    DEL_TAB;
    dstr << TAB << "}" << endl;

    std::ostringstream str;
    if (!sParams.empty())
    {
        str << dstr.str();
    }

    str << generateAsync(nPtr, pPtr, oPtr) << endl;

    str << TAB << "this." << oPtr->getId() << "(current" << sParams << ");" << endl << endl;

    str << TAB << "return TarsError.SUCCESS;" << endl;

    DEL_TAB;

    return str.str();
}

string Tars2Node::generateJSServer(const NamespacePtr &nPtr, const InterfacePtr &pPtr, const OperationPtr &oPtr)
{
    std::ostringstream str;

    str << TAB << nPtr->getId() << "." << pPtr->getId() << "Imp.prototype." << oPtr->getId() << " = function (current";
    vector<ParamDeclPtr> & vParamDecl = oPtr->getAllParamDeclPtr();
    for (size_t i = 0; i < vParamDecl.size(); i++)
    {
        str << ", " << vParamDecl[i]->getTypeIdPtr()->getId();
    }
    str << ") {" << endl;

    str << TAB << "    //TODO:" << endl;
    str << TAB << "    assert.fail('" << oPtr->getId() << " function not implemented');" << endl;
    str << TAB << "}" << endl << endl;

    return str.str();
}

string Tars2Node::generateJSServer(const InterfacePtr &pPtr, const NamespacePtr &nPtr)
{
    std::ostringstream str;
    vector<OperationPtr> & vOperation = pPtr->getAllOperationPtr();

    //生成类
    str << TAB << nPtr->getId() << "." << pPtr->getId() << "Imp = function () { " << endl;
    str << TAB << "    this._name   = undefined;" << endl;
    str << TAB << "    this._worker = undefined;" << endl;
    str << TAB << "}" << endl << endl;

    //生成初始化函数
    str << TAB << nPtr->getId() << "." << pPtr->getId() << "Imp.prototype.initialize = function () { " << endl;
    str << TAB << "    //TODO:" << endl;
    str << TAB << "}" << endl << endl;

    //生成分发函数
    str << TAB << nPtr->getId() << "." << pPtr->getId() << "Imp.prototype.onDispatch = function (current, FuncName, BinBuffer) { " << endl;
    INC_TAB;
    str << TAB << "switch (FuncName) {" << endl;
    INC_TAB;

    //生成 ping 函数
    str << TAB << "case 'tars_ping': {" << endl;       
    str << TAB << TAB << "current.sendResponse = function (_ret) {" << endl; 
    str << TAB << TAB << TAB << "if (current.getRequestVersion() === TarsStream.Tup.TUP_SIMPLE || current.getRequestVersion() === TarsStream.Tup.TUP_COMPLEX){" << endl; 
    str << TAB << TAB << TAB << TAB << "var tup = new TarsStream.UniAttribute();" << endl; 
    str << TAB << TAB << TAB << TAB << "tup.tupVersion = current.getRequestVersion();" << endl; 
    str << TAB << TAB << TAB << TAB << "tup.writeInt32(\"\", _ret);" << endl; 
    str << TAB << TAB << TAB << TAB << "current.doResponse(tup.encode());" << endl; 
    str << TAB << TAB << TAB << "}else{" << endl;  
    str << TAB << TAB << TAB << TAB << "var os = new TarsStream.OutputStream();" << endl; 
    str << TAB << TAB << TAB << TAB << "os.writeInt32(0, _ret);" << endl; 
    str << TAB << TAB << TAB << TAB << "current.doResponse(os.getBinBuffer());" << endl; 
    str << TAB << TAB << TAB << "}" << endl;     
    str << TAB << TAB << "}" << endl; 
    str << TAB << TAB << TAB << "current.sendResponse(0);" << endl; 
    str << TAB << TAB << "return TarsError.SUCCESS;" << endl; 
    str << TAB << "}" << endl;

    for (size_t i = 0; i < vOperation.size(); i++)
    {
        str << TAB << "case '" << vOperation[i]->getId() << "' : {" << endl;
        
        str << generateDispatch(nPtr, pPtr, vOperation[i]);
        
        str << TAB << "}" << endl;
    }

    DEL_TAB;
    str << TAB << "}" << endl;

    str << TAB << "return TarsError.SERVER.FUNC_NOT_FOUND;" << endl;
    DEL_TAB;
    str << TAB << "}" << endl << endl;

    //生成接口函数
    for (size_t i = 0; i < vOperation.size(); i++)
    {
        str << generateJSServer(nPtr, pPtr, vOperation[i]);
    }

    str << endl;

    return str.str();
}

string Tars2Node::generateJSServer(const NamespacePtr &pPtr)
{
	std::ostringstream str;

	vector<ConstPtr> &cs = pPtr->getAllConstPtr();
	for (size_t i = 0; i < cs.size(); i++)
	{
		str << generateJS(cs[i], pPtr->getId()) << endl;
	}
	str << endl;

	vector<EnumPtr> & es = pPtr->getAllEnumPtr();
    for (size_t i = 0; i < es.size(); i++)
    {
        str << generateJS(es[i], pPtr->getId()) << endl;
    }

	vector<StructPtr> & ss = pPtr->getAllStructPtr();
	for (size_t i = 0; i < ss.size(); i++)
	{
		str << generateJS(ss[i], pPtr) << endl;
	}

    vector<InterfacePtr> & is = pPtr->getAllInterfacePtr();
    for (size_t i = 0; i < is.size(); i++)
    {
        str << generateJSServer(is[i], pPtr) << endl;
    }

	return str.str();
}

void Tars2Node::generateJSServer(const ContextPtr &pPtr)
{
    vector<NamespacePtr> namespaces = pPtr->getNamespaces();

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

    //生成编解码
    std::ostringstream estr;
	for(size_t i = 0; i < namespaces.size(); i++)
	{
		estr << generateJSServer(namespaces[i]) << endl;
	}

    //再生成导入模块
    std::ostringstream ostr;
    ostr << TAB << "var assert     = require('assert');" << endl;
    ostr << TAB << "var TarsStream = require('" << _sTarsStreamPath << "');" << endl;
    ostr << TAB << "var TarsError  = require('" << _sTarsLibPath << "').error;" << endl;
    for (std::map<string, ImportFile>::iterator it = _mapFiles.begin(); it != _mapFiles.end(); it++)
    {
        if (it->second.sModule.empty()) continue;

        ostr << TAB << "var " << it->second.sModule << " = require('" << it->second.sFile << "');" << endl;
    }

    std::ostringstream str;

    str << g_parse->printHeaderRemark();
    str << "\"use strict\";" << endl << endl;
    str << ostr.str() << endl;
    str << istr.str();
    str << estr.str() << endl;

    std::string sFileName = tars::TC_File::excludeFileExt(_sToPath + tars::TC_File::extractFileName(pPtr->getFileName())) + ".js";

    tars::TC_File::makeDirRecursive(_sToPath, 0755);
    makeUTF8File(sFileName, str.str());
}

