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

Tars2Node::Tars2Node():uiNameIndex(0), _sTarsLibPath("@tars/rpc"), _sTarsStreamPath("@tars/stream"), _sToPath("./"), _bClient(false), _bServer(false), _bRecursive(false), _bUseSpecialPath(false)
{

}

void Tars2Node::makeUTF8File(const string & sFileName, const string & sFileContent) 
{
    char header[3] = {(char)(0xef), (char)(0xbb), (char)(0xbf)};

    string sData(header, 3);

    sData += tars::TC_Encoder::gbk2utf8(sFileContent);

    tars::TC_File::save2file(sFileName, sData.c_str());
}

std::string Tars2Node::getRealFileInfo(const std::string & sPath) 
{
    return tars::TC_File::extractFilePath(sPath) + tars::TC_File::excludeFileExt(tars::TC_File::extractFileName(sPath)) + ".tars";
}

std::string Tars2Node::toFunctionName(const TypeIdPtr & pPtr, const std::string &sAction)
{
	BuiltinPtr bPtr = BuiltinPtr::dynamicCast(pPtr->getTypePtr());
	if (bPtr)
	{
		switch (bPtr->kind())
		{
			case Builtin::KindBool	 : return sAction + "Boolean";
			case Builtin::KindString : return sAction + "String";
			case Builtin::KindByte   : return sAction + "Int8";
			case Builtin::KindShort  : return sAction + (bPtr->isUnsigned()?"UInt8":"Int16");
			case Builtin::KindInt    : return sAction + (bPtr->isUnsigned()?"UInt16":"Int32");
			case Builtin::KindLong   : return sAction + (bPtr->isUnsigned()?"UInt32":"Int64");
			case Builtin::KindFloat  : return sAction + "Float";
			case Builtin::KindDouble : return sAction + "Double";
			default                  : assert(false);
		}
	}

	VectorPtr vPtr = VectorPtr::dynamicCast(pPtr->getTypePtr());
	if (vPtr)
	{
		BuiltinPtr vbPtr = BuiltinPtr::dynamicCast(vPtr->getTypePtr());
		if (vbPtr && vbPtr->kind() == Builtin::KindByte)
		{
			return sAction + "Bytes";
		}

		return sAction + "List";
	}

	StructPtr sPtr = StructPtr::dynamicCast(pPtr->getTypePtr());
	if (sPtr)
	{
		return sAction + "Struct";
	}

	EnumPtr ePtr = EnumPtr::dynamicCast(pPtr->getTypePtr());
	if (ePtr)
	{
		return sAction + "Int32";
	}

	MapPtr mPtr = MapPtr::dynamicCast(pPtr->getTypePtr());
	if (mPtr)
	{
		return sAction + "Map";
	}

	return "";
}

string Tars2Node::getDataType(const TypePtr & pPtr)
{
	BuiltinPtr bPtr = BuiltinPtr::dynamicCast(pPtr);
	if (bPtr)
	{
		switch (bPtr->kind())
		{
			case Builtin::KindBool	 :	return "TarsStream.Boolean";
            case Builtin::KindString :	return "TarsStream.String";
            case Builtin::KindByte   :  return "TarsStream.Int8";
			case Builtin::KindShort  :	return "TarsStream.Int16";
			case Builtin::KindInt    :	return "TarsStream.Int32";
			case Builtin::KindLong   : 	return "TarsStream.Int64";
			case Builtin::KindFloat  :	return "TarsStream.Float";
			case Builtin::KindDouble :	return "TarsStream.Double";
			default                  :  assert(false);
		}
	}

	VectorPtr vPtr = VectorPtr::dynamicCast(pPtr);
	if (vPtr)
	{
		BuiltinPtr vbPtr = BuiltinPtr::dynamicCast(vPtr->getTypePtr());
		if (vbPtr && vbPtr->kind() == Builtin::KindByte)
		{
			return "TarsStream.BinBuffer";
		}

		return "TarsStream.List(" + getDataType(vPtr->getTypePtr()) + ")";
	}

	StructPtr sPtr = StructPtr::dynamicCast(pPtr);
	if (sPtr)
	{
        vector<string> vecNames = tars::TC_Common::sepstr<string>(sPtr->getSid(), "::");
        assert(vecNames.size() == 2);

        std::string sName = findName(vecNames[0], vecNames[1]);

        return sName;
	}

	MapPtr mPtr = MapPtr::dynamicCast(pPtr);
	if (mPtr)
	{
		return "TarsStream.Map(" + getDataType(mPtr->getLeftTypePtr()) + ", " + getDataType(mPtr->getRightTypePtr()) + ")";
	}

	EnumPtr ePtr = EnumPtr::dynamicCast(pPtr);
	if (ePtr)
	{
        vector<string> vecNames = tars::TC_Common::sepstr<string>(ePtr->getSid(), "::");
        assert(vecNames.size() == 2);

        std::string sName = findName(vecNames[0], vecNames[1]);

        return sName;
	}

	assert(false);
	return "";
}

bool Tars2Node::isSimple(const TypePtr & pPtr) const
{
    BuiltinPtr bPtr = BuiltinPtr::dynamicCast(pPtr);
    if (bPtr)
    {
        return true;
    }

    EnumPtr ePtr = EnumPtr::dynamicCast(pPtr);
    if (ePtr)
    {
        return true;
    }

    return false;
}

bool Tars2Node::isBinBuffer(const TypePtr & pPtr) const
{
    VectorPtr vPtr = VectorPtr::dynamicCast(pPtr);
    if (vPtr)
    {
        BuiltinPtr vbPtr = BuiltinPtr::dynamicCast(vPtr->getTypePtr());
        if (vbPtr && vbPtr->kind() == Builtin::KindByte)
        {
            return true;
        }
    }

    return false;
}

string Tars2Node::getDefault(const TypeIdPtr & pPtr, const std::string &sDefault, const std::string & sNamespace, const bool bGlobal)
{
	BuiltinPtr bPtr = BuiltinPtr::dynamicCast(pPtr->getTypePtr());
	if (bPtr)
	{
		switch (bPtr->kind())
		{
			case Builtin::KindBool		: return sDefault.empty()?"false":sDefault;
			case Builtin::KindString	: return "\"" + tars::TC_Common::replace(sDefault, "\"", "\\\"") +  "\"";
			case Builtin::KindByte		:
			case Builtin::KindShort		:
			case Builtin::KindInt		:
			case Builtin::KindLong		: 
            {
                string sTemp = tars::TC_Common::trim(sDefault);
                if (sTemp.empty())
                {
                    return "0";
                }

                //if (tars::TC_Common::isdigit(sTemp))
                if (tars::TC_Common::tostr(tars::TC_Common::strto<long>(sTemp)) ==  sTemp)
                {
                    return sTemp;
                }

                //有可能是枚举值，在枚举值中查找
                vector<string> vecNames = tars::TC_Common::sepstr<string>(sDefault, "::");
                if (vecNames.size() == 2)
                {
                    sTemp = findName(vecNames[0], vecNames[1]);
                }
                else 
                {
                    sTemp = findName(sNamespace, sDefault);
                }

                return sTemp.empty()?"0":sTemp;
            }
			case Builtin::KindFloat		:
			case Builtin::KindDouble	: return sDefault.empty()?"0.0":sDefault;
			default						: assert(false);
		}
	}

    EnumPtr ePtr = EnumPtr::dynamicCast(pPtr->getTypePtr());
	if (ePtr)
	{
        if (sDefault.empty())
        {
            vector<TypeIdPtr>& eMember = ePtr->getAllMemberPtr();
            if (eMember.size() > 0)
            {
                vector<string> vecNames = tars::TC_Common::sepstr<string>(ePtr->getSid(), "::");

                std::string sModule = findName(vecNames[0], eMember[0]->getId());

                return sModule;
            }
        }

        if (!tars::TC_Common::isdigit(sDefault))
        {
            std::string s1 = sNamespace;
            std::string s2 = sDefault;

            std::string::size_type index = sDefault.find("::");
            if (index != std::string::npos)
            {
                s1 = sDefault.substr(0, index);
                s2 = sDefault.substr(index + 2);
            }

            std::string sModule = findName(s1, s2);

            assert(!sModule.empty());

            return sModule;
        }
	}

	VectorPtr vPtr = VectorPtr::dynamicCast(pPtr->getTypePtr());
	if (vPtr && bGlobal)
	{
		BuiltinPtr vbPtr = BuiltinPtr::dynamicCast(vPtr->getTypePtr());
		if (vbPtr && vbPtr->kind() == Builtin::KindByte)
		{
			return "new TarsStream.BinBuffer()";
		}

		return "new TarsStream.List(" + getDataType(vPtr->getTypePtr()) + ")";
	}

	StructPtr sPtr = StructPtr::dynamicCast(pPtr->getTypePtr());
	if (sPtr && bGlobal)
	{
        vector<string> vecNames = tars::TC_Common::sepstr<string>(sPtr->getSid(), "::");

        std::string sModule = findName(vecNames[0], vecNames[1]);

        return "new " + sModule + "()";
	}

	MapPtr mPtr = MapPtr::dynamicCast(pPtr->getTypePtr());
	if (mPtr && bGlobal)
	{
		return "new TarsStream.Map(" + getDataType(mPtr->getLeftTypePtr()) + ", " + getDataType(mPtr->getRightTypePtr()) + ")";
	}

	return sDefault;
}

string Tars2Node::generateJS(const EnumPtr &pPtr, const std::string &sNamespace)
{
	ostringstream s;
    s << TAB << sNamespace << "." << pPtr->getId() << " = {" << endl;
    INC_TAB;

    //成员变量
    int nenum = -1;
    vector<TypeIdPtr>& member = pPtr->getAllMemberPtr();
    for (size_t i = 0; i < member.size(); i++)
    {
		if (member[i]->hasDefault())
		{
			nenum = tars::TC_Common::strto<int>(member[i]->def());
		}
		else
		{
			nenum++;
		}

        s << TAB << member[i]->getId()<< " : " << tars::TC_Common::tostr(nenum) << (i == member.size() - 1?"":",") << endl;
    }

	DEL_TAB;
	s << TAB << "};" << endl;

    //函数
    s << TAB << sNamespace << "." << pPtr->getId() << "._write = function(os, tag, val) { return os.writeInt32(tag, val); }" << endl;
    s << TAB << sNamespace << "." << pPtr->getId() << "._read  = function(is, tag, def) { return is.readInt32(tag, true, def); }" << endl;

    return s.str();
}

string Tars2Node::generateJS(const ConstPtr &pPtr, const std::string &sNamespace)
{
    ostringstream s;

    if (pPtr->getConstGrammarPtr()->t == ConstGrammar::STRING)
    {
        string tmp = tars::TC_Common::replace(pPtr->getConstGrammarPtr()->v, "\"", "\\\"");
        s << TAB << sNamespace << "." << pPtr->getTypeIdPtr()->getId() << " = \"" << tmp << "\";" << endl;
    }
    else
    {
        s << TAB << sNamespace << "." << pPtr->getTypeIdPtr()->getId() << " = " << pPtr->getConstGrammarPtr()->v << ";" << endl;
    }

    return s.str();
}

string Tars2Node::generateJS(const StructPtr & pPtr, const NamespacePtr & nPtr)
{
    ostringstream s;
    vector<TypeIdPtr>& member = pPtr->getAllMemberPtr();

    s << TAB << nPtr->getId() << "." << pPtr->getId() << " = function() {"<< endl;

    INC_TAB;

    for (size_t i = 0; i < member.size(); i++)
    {
        s << TAB << "this." << member[i]->getId() << " = " << getDefault(member[i], member[i]->def(), nPtr->getId()) << ";" << endl;
    }

    {
        s << TAB << "this._classname = \"" << nPtr->getId() << "." << pPtr->getId() << "\";" << endl; 
    }

    DEL_TAB;

    s << TAB << "};" << endl;
    s << TAB << nPtr->getId() << "." << pPtr->getId() << "._classname = \"" << nPtr->getId() << "." << pPtr->getId() << "\";" << endl << endl; 

	std::string sProto = tars::TC_Common::replace(pPtr->getSid(), "::", ".");
	s << TAB << sProto << "._write = function (os, tag, value) { os.writeStruct(tag, value); }" << endl;
	s << TAB << sProto << "._read  = function (is, tag, def) { return is.readStruct(tag, true, def); }" << endl;

    //_readFrom
    s << TAB << nPtr->getId() << "." << pPtr->getId() << "._readFrom = function (is) {" << endl;
    INC_TAB;
    s << TAB << "var tmp = new " << nPtr->getId() << "." << pPtr->getId() << "();" << endl;
    for (size_t i = 0; i < member.size(); i++)
    {
        std::string sFuncName = toFunctionName(member[i], "read");
		s << TAB << "tmp." << member[i]->getId() << " = is." << sFuncName << "(" << member[i]->getTag()
            << ", " << (member[i]->isRequire()?"true":"false") << string(", ");

        if (isSimple(member[i]->getTypePtr()))
        {
            s << getDefault(member[i], member[i]->def(), nPtr->getId());
        }
        else
        {
            s << getDataType(member[i]->getTypePtr());
        }

        s << ");" << endl;
    }
    s << TAB << "return tmp;" << endl;
    DEL_TAB;
    s << TAB << "};" << endl;

    //_writeTo
    s << TAB << nPtr->getId() << "." << pPtr->getId() << ".prototype._writeTo = function (os) {" << endl;

    INC_TAB;
    for (size_t i = 0; i < member.size(); i++)
    {
        std::string sFuncName = toFunctionName(member[i], "write");

       	s << TAB << "os." << sFuncName << "(" << member[i]->getTag() << ", this." << member[i]->getId() << ");" << endl;
    }
    DEL_TAB;
    s << TAB << "};" << endl;

    //_equal
    s << TAB << nPtr->getId() << "." << pPtr->getId() << ".prototype._equal = function (anItem) {" << endl;
    INC_TAB;

    vector<string> key = pPtr->getKey();
    if (key.size() > 0)
    {
        s << TAB << "return ";

        for (size_t i = 0; i < key.size(); i++)
        {
            for (size_t ii =0; ii < member.size(); ii++)
            {
                if (key[i] != member[ii]->getId())
                {
                    continue;
                }

                if (isSimple(member[i]->getTypePtr()))
                {
                    s << (i==0?"":TAB + "    ") << "this." << key[i] << " === " << "anItem." << key[i];
                }
                else 
                {
                    s << (i==0?"":TAB + "    ") << "this._equal(" << "anItem)";
                }
            }

            if (i != key.size() - 1) 
            {
                s << " && " << endl;
            }
        }

        s << ";" << endl;

    }
    else 
    {
        s << TAB << "assert(false, 'this structure not define key operation');" << endl;
    }

    DEL_TAB;
    s << TAB << "}" << endl;

    //_genKey
    s << TAB << nPtr->getId() << "." << pPtr->getId() << ".prototype._genKey = function () {" << endl;
    INC_TAB;
    s << TAB << "if (!this._proto_struct_name_) {" << endl;
    s << TAB << "    this._proto_struct_name_ = 'STRUCT' + Math.random();" << endl;
    s << TAB << "}" << endl;
    s << TAB << "return this._proto_struct_name_;" << endl;
    DEL_TAB;
    s << TAB << "}" << endl;

    //toObject
    s << TAB << nPtr->getId() << "." << pPtr->getId() << ".prototype.toObject = function() { "<< endl;
    INC_TAB;
    s << TAB << "var tmp = {};" << endl << endl;

    for (size_t i = 0; i < member.size(); i++)
    {
        if (isSimple(member[i]->getTypePtr())) {
            s << TAB << "tmp." << member[i]->getId() << " = this." << member[i]->getId() << ";" << endl;
        }
        else {
            s << TAB << "tmp." << member[i]->getId() << " = this." << member[i]->getId() << ".toObject();" << endl;
        }
    }

    s << TAB << endl;
    s << TAB << "return tmp;" << endl;
    DEL_TAB;
    s << TAB << "}" << endl;

    //readFromJson
    s << TAB << nPtr->getId() << "." << pPtr->getId() << ".prototype.readFromObject = function(json) { "<< endl;
    INC_TAB;

    for (size_t i = 0; i < member.size(); i++)
    {
        if (isSimple(member[i]->getTypePtr())) {
            s << TAB << "!json.hasOwnProperty(\"" << member[i]->getId() << "\") || (this." << member[i]->getId() << " = json." << member[i]->getId() << ");" << endl;
        } else {
            s << TAB << "!json.hasOwnProperty(\"" << member[i]->getId() << "\") || (this." << member[i]->getId() << ".readFromObject(json." << member[i]->getId() << "));" << endl;
        }
    }

    DEL_TAB;
    s << TAB << "}" << endl;

    //toBinBuffer
    s << TAB << nPtr->getId() << "." << pPtr->getId() << ".prototype.toBinBuffer = function () {" << endl;
    INC_TAB;
    s << TAB << "var os = new TarsStream.OutputStream();" << endl;
    s << TAB << "this._writeTo(os);" << endl;
    s << TAB << "return os.getBinBuffer();" << endl;
    DEL_TAB;
    s << TAB << "}" << endl;

    //new
    s << TAB << nPtr->getId() << "." << pPtr->getId() << ".new = function () {" << endl;
    s << TAB << "    return new " << nPtr->getId() << "." << pPtr->getId() << "();" << endl;
    s << TAB << "}" << endl;

    //create
    s << TAB << nPtr->getId() << "." << pPtr->getId() << ".create = function (is) {" << endl;
    s << TAB << "    return " << nPtr->getId() << "." << pPtr->getId() << "._readFrom(is);" << endl;
    s << TAB << "}" << endl;

    return s.str();
}

string Tars2Node::generateJS(const NamespacePtr &pPtr)
{
	std::ostringstream str;

	vector<ConstPtr> &cs = pPtr->getAllConstPtr();
	for (size_t i = 0; i < cs.size(); i++)
	{
		str << generateJS(cs[i], pPtr->getId());
	}
    if (cs.size() != 0) str << endl;

	vector<EnumPtr> & es = pPtr->getAllEnumPtr();
    for (size_t i = 0; i < es.size(); i++)
    {
        str << generateJS(es[i], pPtr->getId());
    }
    if (es.size() != 0) str << endl;

	vector<StructPtr> & ss = pPtr->getAllStructPtr();
	for (size_t i = 0; i < ss.size(); i++)
	{
		str << generateJS(ss[i], pPtr);
	}
    if (ss.size() != 0) str << endl;

	return str.str();
}

void Tars2Node::generateJS(const ContextPtr &pPtr)
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

    //先生成编解码体
    std::ostringstream estr;
    for(size_t i = 0; i < namespaces.size(); i++)
    {
        estr << generateJS(namespaces[i]);
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
    sstr << "\"use strict;\"" << endl << endl;
    sstr << "var assert     = require('assert');" << endl;
    sstr << "var TarsStream = require('" << _sTarsStreamPath << "');" << endl;
    sstr << ostr.str() << endl << endl;
    sstr << istr.str();
    sstr << estr.str() << endl;

    std::string sFileName = tars::TC_File::excludeFileExt(_sToPath + tars::TC_File::extractFileName(pPtr->getFileName())) + "Tars.js";
    tars::TC_File::makeDirRecursive(_sToPath, 0755);
    makeUTF8File(sFileName, sstr.str());
}

void Tars2Node::createFile(const string &file)
{
    std::string sTarsFile = getRealFileInfo(file);
    g_parse->parse(sTarsFile);

	std::vector<ContextPtr> contexts = g_parse->getContexts();

	for(size_t i = 0; i < contexts.size(); i++)
	{
		if (sTarsFile == contexts[i]->getFileName())
		{
            scan(sTarsFile, true);                                  //分析枚举值、结构体所在的文件

            if (!_bClient && !_bServer) generateJS(contexts[i]);    //生成当前文件的编解码文件

			if (_bClient) generateJSProxy(contexts[i]);             //生成当前文件的客户端代理类文件

            if (_bServer) generateJSServer(contexts[i]);            //生成当前文件的服务端代理类文件

            if (_bServer) generateJSServerImp(contexts[i]);         //生成当前文件的服务端实现类文件

            vector<string> files = contexts[i]->getIncludes();
            for (size_t ii = 0; _bRecursive && ii < files.size(); ii++)
            {
                Tars2Node node;
                node.setTarsLibPath(_sTarsLibPath);
                node.setTarsStreamPath(_sTarsStreamPath);
                node.setTargetPath(_sToPath);
                node.setRecursive(_bRecursive);
                node.setUseSpecialPath(_bUseSpecialPath);
                node.createFile(files[ii]);
            }
        }
	}
}

