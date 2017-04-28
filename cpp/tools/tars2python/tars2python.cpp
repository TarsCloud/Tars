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
#include "tars2python.h"
// begin modify
#include "util/tc_md5.h"
#include "util/tc_file.h"
#include "util/tc_common.h"
#include <string>
// end modify

#define TAB g_parse->getTab()
#define INC_TAB g_parse->incTab()
#define DEL_TAB g_parse->delTab()

const string MAP_PREFIX = "mapcls_";
const string VEC_PREFIX = "vctcls_";

string Tars2Python::generatePython(const EnumPtr &pPtr, const std::string &sNamespace)
{
	std::ostringstream s;
	s << TAB << "class " << pPtr->getId() << ":" << endl;
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

		s << TAB << member[i]->getId()<< " = " << tars::TC_Common::tostr(nenum) << ";" << endl;
	}

	DEL_TAB;

	return s.str();
}

std::string Tars2Python::toFunctionName(const TypeIdPtr & pPtr, const string & sName)
{
	BuiltinPtr bPtr = BuiltinPtr::dynamicCast(pPtr->getTypePtr());
	if (bPtr)
	{
		switch (bPtr->kind())
		{
			case Builtin::KindBool	: 	return "tarscore.boolean";
			case Builtin::KindString:	return "tarscore.string";
			case Builtin::KindByte: 	return "tarscore.int8";
			case Builtin::KindShort:	return "tarscore." + string((bPtr->isUnsigned()?"uint8":"int16"));
			case Builtin::KindInt:		return "tarscore." + string((bPtr->isUnsigned()?"uint16":"int32"));
			case Builtin::KindLong: 	return "tarscore." + string((bPtr->isUnsigned()?"uint32":"int64"));
			case Builtin::KindFloat:	return "tarscore.float";
			case Builtin::KindDouble:	return "tarscore.double";
			default:					assert(false);
		}
	}

	StructPtr sPtr = StructPtr::dynamicCast(pPtr->getTypePtr());
	if (sPtr)
	{
		return sPtr->getId();
	}

	EnumPtr ePtr = EnumPtr::dynamicCast(pPtr->getTypePtr());
	if (ePtr)
	{
		return "tarscore.int32";
	}

	VectorPtr vPtr = VectorPtr::dynamicCast(pPtr->getTypePtr());
	if (vPtr)
	{
		BuiltinPtr vbPtr = BuiltinPtr::dynamicCast(vPtr->getTypePtr());
		if (vbPtr && vbPtr->kind() == Builtin::KindByte)
		{
			return "tarscore.bytes";
		}

		return "value." + VEC_PREFIX + sName;
	}

	MapPtr mPtr = MapPtr::dynamicCast(pPtr->getTypePtr());
	if (mPtr)
	{
		return "value." + MAP_PREFIX + sName;
	}

	assert(false);
	return "";
}







string Tars2Python::getDefault(const TypeIdPtr & pPtr, const std::string &sDefault, const std::string & sCurStruct)
{
	BuiltinPtr bPtr = BuiltinPtr::dynamicCast(pPtr->getTypePtr());
	if (bPtr)
	{
		switch (bPtr->kind())
		{
			case Builtin::KindBool		: return sDefault.empty()?"True": (sDefault == "true"?"True":"False");
			case Builtin::KindString	: return "\"" + tars::TC_Common::replace(sDefault, "\"", "\\\"") +  "\"";
			case Builtin::KindByte		:
			case Builtin::KindShort		:
			case Builtin::KindInt		:
			case Builtin::KindLong		: return sDefault.empty()?"0":sDefault;
			case Builtin::KindFloat		:
			case Builtin::KindDouble	: return sDefault.empty()?"0.0":sDefault;
			default						: assert(false);
		}
	}

	VectorPtr vPtr = VectorPtr::dynamicCast(pPtr->getTypePtr());
	if (vPtr)
	{
		BuiltinPtr vbPtr = BuiltinPtr::dynamicCast(vPtr->getTypePtr());
		if (vbPtr && vbPtr->kind() == Builtin::KindByte)
		{
			return "bytes()";
		}

		return sCurStruct + "." + VEC_PREFIX + pPtr->getId() + "()";
	}

	EnumPtr ePtr = EnumPtr::dynamicCast(pPtr->getTypePtr());
	if (ePtr)
	{
		string sid = ePtr->getSid();
		size_t pos = sid.find_last_of(":");
		return sDefault.empty()?"0":(sid.substr((pos == string::npos)?0:pos + 1) + "." + sDefault);
	}

	StructPtr sPtr = StructPtr::dynamicCast(pPtr->getTypePtr());
	if (sPtr)
	{
		return sPtr->getId() + "()";
	}

	MapPtr mPtr = MapPtr::dynamicCast(pPtr->getTypePtr());
	if (mPtr)
	{
		return sCurStruct + "." + MAP_PREFIX + pPtr->getId() + "()";
	}

	assert(false);
	return sDefault;
}

string Tars2Python::tostrEnum(const EnumPtr &pPtr)
{
	return "tarscore.int8";
}
string Tars2Python::makeDataType(const TypePtr & pPtr)
{
	BuiltinPtr bPtr = BuiltinPtr::dynamicCast(pPtr);
	if (bPtr)
	{
		switch (bPtr->kind())
		{
			case Builtin::KindBool	: 	return "tarscore.boolean";
			case Builtin::KindString:	return "tarscore.string";
			case Builtin::KindByte: 	return "tarscore.int8";
			case Builtin::KindShort:	return "tarscore." + string((bPtr->isUnsigned()?"uint8":"int16"));
			case Builtin::KindInt:		return "tarscore." + string((bPtr->isUnsigned()?"uint16":"int32"));
			case Builtin::KindLong: 	return "tarscore." + string((bPtr->isUnsigned()?"uint32":"int64"));
			case Builtin::KindFloat:	return "tarscore.float";
			case Builtin::KindDouble:	return "tarscore.double";
			default:					assert(false);
		}
	}

	VectorPtr vPtr = VectorPtr::dynamicCast(pPtr);
	if (vPtr)
	{
		BuiltinPtr vbPtr = BuiltinPtr::dynamicCast(vPtr->getTypePtr());
		if (vbPtr && vbPtr->kind() == Builtin::KindByte)
		{
			return "tarscore.bytes";
		}

		return "tarscore.vctclass(" + makeDataType(vPtr->getTypePtr()) + ")";
	}

       EnumPtr ePtr = EnumPtr::dynamicCast(pPtr);
       if(ePtr) return tostrEnum(ePtr);
	
	StructPtr sPtr = StructPtr::dynamicCast(pPtr);
	if (sPtr)
	{
		return sPtr->getId();
	}

	MapPtr mPtr = MapPtr::dynamicCast(pPtr);
	if (mPtr)
	{
		return "tarscore.mapclass(" + makeDataType(mPtr->getLeftTypePtr()) + ", " + makeDataType(mPtr->getRightTypePtr()) + ")";
	}

	assert(false);
	return "";
}

string Tars2Python::generatePython(const StructPtr & pPtr, const NamespacePtr & nPtr)
{
	std::ostringstream s;
	vector<string> key = pPtr->getKey();
	vector<TypeIdPtr> & member = pPtr->getAllMemberPtr();

	s << TAB << "class " << pPtr->getId() << "(tarscore.struct):"<< endl;
	INC_TAB;

	//STEP01 开始定义类需要的类型
	s << TAB << "__tars_class__ = \"" << nPtr->getId() << "." << pPtr->getId() << "\";" << endl;
	for (size_t i = 0, len = member.size(); i < len; i++)
	{
		VectorPtr vPtr = VectorPtr::dynamicCast(member[i]->getTypePtr());
		if (vPtr)
		{
			BuiltinPtr vbPtr = BuiltinPtr::dynamicCast(vPtr->getTypePtr());
			if (vbPtr && vbPtr->kind() == Builtin::KindByte) continue;

			string st = makeDataType(member[i]->getTypePtr());
			s << TAB << VEC_PREFIX << member[i]->getId() << " = " << st << ";" << endl;
		}

		MapPtr mPtr = MapPtr::dynamicCast(member[i]->getTypePtr());
		if (mPtr)
		{
			string st = makeDataType(member[i]->getTypePtr());
			s << TAB << MAP_PREFIX << member[i]->getId() << " = " << st << ";" << endl;
		}
	}

	//STEP02 定义数据成员
	s << TAB << endl;
	s << TAB << "def __init__(self):" << endl;
	INC_TAB;
	for (size_t i = 0; i < member.size(); i++)
	{
		s << TAB << "self." << member[i]->getId() << " = " << getDefault(member[i], member[i]->def(), pPtr->getId()) << ";" << endl;
	}
	DEL_TAB;

	//STEP03 开始生成writeTo函数
	s << TAB << endl;
	s << TAB << "@staticmethod" << endl;
	s << TAB << "def writeTo(oos, value):" << endl;
	INC_TAB;
	for (size_t i = 0; i < member.size(); i++)
	{
		std::string sFuncName = toFunctionName(member[i], member[i]->getId());
       	s << TAB << "oos.write(" << sFuncName << ", " << member[i]->getTag() << ", value." << member[i]->getId() << ");" << endl;
	}
	DEL_TAB;

   	//STEP04 开始生成readFrom函数
	s << TAB << endl;
	s << TAB << "@staticmethod" << endl;
	s << TAB << "def readFrom(ios):" << endl;
	INC_TAB;
	s << TAB << "value = " << pPtr->getId() << "();" << endl;
	for (size_t i = 0; i < member.size(); i++)
	{
		std::string sFuncName = toFunctionName(member[i], member[i]->getId());
		s << TAB << "value." << member[i]->getId() << "= ios.read(" << sFuncName << ", " << member[i]->getTag()
            	<< ", " << (member[i]->isRequire()?"True":"False") << ", value." << member[i]->getId() << ");" << endl;
	}
	s << TAB << "return value;" << endl;
	DEL_TAB;

	DEL_TAB;
	return s.str();
}

string Tars2Python::makeParams(const OperationPtr &pPtr)
{
	 ostringstream str;
	 vector<ParamDeclPtr>& vParamDecl = pPtr->getAllParamDeclPtr();

	 for (size_t i = 0; i < vParamDecl.size(); i++)
	 {
		 if (vParamDecl[i]->isOut()) continue;

		 str << vParamDecl[i]->getTypeIdPtr()->getId() << ", ";
	 }

	 str << "context = ServantProxy.mapcls_context()";

	 return str.str();
}

#if 0
std::string Tars2Python::innerDataType(const TypeIdPtr & pPtr)
{
	BuiltinPtr bPtr = BuiltinPtr::dynamicCast(pPtr->getTypePtr());
	if (bPtr)
	{
		switch (bPtr->kind())
		{
			case Builtin::KindBool	: 	return "tarscore.boolean";
			case Builtin::KindString:	return "tarscore.string";
			case Builtin::KindByte: 	return "tarscore.int8";
			case Builtin::KindShort:	return "tarscore." + string((bPtr->isUnsigned()?"uint8":"int16"));
			case Builtin::KindInt:		return "tarscore." + string((bPtr->isUnsigned()?"uint16":"int32"));
			case Builtin::KindLong: 	return "tarscore." + string((bPtr->isUnsigned()?"uint32":"int64"));
			case Builtin::KindFloat:	return "tarscore.float";
			case Builtin::KindDouble:	return "tarscore.double";
			default:					assert(false);
		}
	}

	StructPtr sPtr = StructPtr::dynamicCast(pPtr->getTypePtr());
	if (sPtr)
	{
		return sPtr->getId();
	}

	EnumPtr ePtr = EnumPtr::dynamicCast(pPtr->getTypePtr());
	if (ePtr)
	{
		return "tarscore.int32";
	}

	VectorPtr vPtr = VectorPtr::dynamicCast(pPtr->getTypePtr());
	if (vPtr)
	{
		BuiltinPtr vbPtr = BuiltinPtr::dynamicCast(vPtr->getTypePtr());
		if (vbPtr && vbPtr->kind() == Builtin::KindByte)
		{
			return "tarscore.bytes";
		}

		return "tarscore.vctclass(" + innerDataType(vPtr->getTypePtr()) + ")";
	}

	MapPtr mPtr = MapPtr::dynamicCast(pPtr->getTypePtr());
	if (mPtr)
	{
		return "tarscore.mapclass(" + innerDataType() + ")";
	}

	assert(false);
	return "";
}

#endif

string Tars2Python::makeOperations(const OperationPtr &pPtr)
{
	ostringstream str;
	vector<ParamDeclPtr>& vParamDecl = pPtr->getAllParamDeclPtr();

	str << TAB << "oos = tarscore.TarsOutputStream();" << endl;
    for(size_t i = 0; i < vParamDecl.size(); i++)
    {
        if(vParamDecl[i]->isOut()) continue;
        str << TAB << "oos.write(" << makeDataType(vParamDecl[i]->getTypeIdPtr()->getTypePtr()) << ", " << (i + 1) << ", " << vParamDecl[i]->getTypeIdPtr()->getId() << ");" << endl;
    }
    str << endl;

    str << TAB << "rsp = self.tars_invoke(ServantProxy.TARSNORMAL, \"" << pPtr->getId() << "\", oos.getBuffer(), context, None);" << endl << endl;

    if(vParamDecl.size() > 0 || pPtr->getReturnPtr()->getTypePtr())
    {
    	str << TAB << "ios = tarscore.TarsInputStream(rsp.sBuffer);" << endl;

    	string ret;
    	if (pPtr->getReturnPtr()->getTypePtr())
    	{
    		str << TAB << "ret = ios.read(" << makeDataType(pPtr->getReturnPtr()->getTypePtr()) << ", 0, True);" << endl;
    		ret += "ret, ";
    	}

        for(size_t i = 0; i < vParamDecl.size(); i++)
        {
            if(!vParamDecl[i]->isOut()) continue;
            str << TAB << vParamDecl[i]->getTypeIdPtr()->getId() << " = ios.read(" << makeDataType(vParamDecl[i]->getTypeIdPtr()->getTypePtr()) << ", " << (i + 1) << ", True);" << endl;
			ret += vParamDecl[i]->getTypeIdPtr()->getId() + ", ";
        }

        str << endl;

        if (!ret.empty())
        {
        	str << TAB << "return (" << ret.substr(0, ret.find_last_of(",")) << ");" << endl;
        }
    }

    return str.str();
}

string Tars2Python::generatePython(const InterfacePtr &pPtr)
{
	std::ostringstream str;
	str << "#proxy for client" << endl;

	str << TAB << "class " << pPtr->getId() << "Proxy(ServantProxy):" << endl;
	INC_TAB;

	vector<OperationPtr>& vOperation = pPtr->getAllOperationPtr();
	for(size_t i = 0; i < vOperation.size(); i++)
	{
		//生成每个接口的编解码
		str << TAB << "def " << vOperation[i]->getId() << "(self, " << makeParams(vOperation[i]) << "):" << endl;
		INC_TAB;
		str << makeOperations(vOperation[i]) << endl;
		DEL_TAB;
	}

	DEL_TAB;
	return str.str();
}


string Tars2Python::generatePython(const NamespacePtr &pPtr)
{
	std::ostringstream str;

	/*
	//STEP01 开始生成常量
	vector<ConstPtr> &cs = pPtr->getAllConstPtr();
	for (size_t i = 0; i < cs.size(); i++)
	{
		str << generatePython(cs[i], pPtr->getId()) << endl;
	}
	str << endl;
	*/

	//STEP02 开始生成枚举值
	vector<EnumPtr> & es = pPtr->getAllEnumPtr();
	for (size_t i = 0; i < es.size(); i++)
	{
		str << generatePython(es[i], pPtr->getId()) << endl;
	}

	//STEP03 开始生成结构体编解码函数
	vector<StructPtr> & ss = pPtr->getAllStructPtr();
	for (size_t i = 0; i < ss.size(); i++)
	{
		str << generatePython(ss[i], pPtr) << endl;
	}

	//STEP04 开始生成Proxy函数
	vector<InterfacePtr> & is = pPtr->getAllInterfacePtr();
    for(size_t i = 0; i < is.size(); i++)
    {
        str << generatePython(is[i]) << endl << endl;
    }

	return str.str();
}

void Tars2Python::generatePython(const ContextPtr &pPtr)
{
	vector<NamespacePtr> namespaces = pPtr->getNamespaces();
	for(size_t i = 0; i < namespaces.size(); i++)
	{
		std::ostringstream str;
		str << "from tars.core import tarscore;" << endl;
		str << "from tars.__rpc import ServantProxy;" << endl;

		vector<string> include = pPtr->getIncludes();
	    for(size_t j = 0; j < include.size(); j++)
	    {
	    	set<string> vec;
 	    	getNamespace(tars::TC_File::excludeFileExt(include[j]) + ".tars", vec);

 	    	for (set<string>::iterator it = vec.begin(); it != vec.end(); it++)
 	    	{
 	    		str << "from " << _packagePrefix << *it << "." << tars::TC_File::excludeFileExt(include[j]) << " import *;" << endl;
 	    	}
	    }
	    str << endl << endl;

		str << generatePython(namespaces[i]) << endl;

		string sPath = getFilePath() + "/" + namespaces[i]->getId() + "/";
		string sFileName = sPath + tars::TC_File::excludeFileExt(tars::TC_File::extractFileName(pPtr->getFileName())) + ".py";
		tars::TC_File::makeDirRecursive(sPath, 0755);
		tars::TC_File::save2file(sFileName, str.str());
	}

	makePackages();
}

void Tars2Python::getNamespace(const string & sFile, set<string> & vec)
{
	g_parse->parse(sFile);
	vector<ContextPtr> contexts = g_parse->getContexts();
	for (size_t i = 0; i < contexts.size(); i++)
	{
		if(sFile != contexts[i]->getFileName()) continue;

		vector<NamespacePtr> namespaces = contexts[i]->getNamespaces();
		for (size_t j = 0; j < namespaces.size(); j++)
			vec.insert(namespaces[j]->getId());
	}
}

void Tars2Python::createFile(const string &file)
{
	std::vector<ContextPtr> contexts = g_parse->getContexts();
	for(size_t i = 0; i < contexts.size(); i++)
	{
		if(file == contexts[i]->getFileName())
		{
			generatePython(contexts[i]);
		}
	}
}

void Tars2Python::makePackages()
{
	string root = _packagePrefix.substr(0, _packagePrefix.find("."));
	vector<string> files;
	tars::TC_File::listDirectory(_baseDir + "/" + root, files, true);
	files.push_back(_baseDir + "/" + root);

	for (size_t i = 0; i < files.size(); i++)
	{
		if (tars::TC_File::isFileExist(files[i], S_IFDIR) == true)
		{
			tars::TC_File::save2file(files[i] + "/__init__.py", "");
		}
	}
}

string Tars2Python::getFilePath() const
{
	return _baseDir + "/" + tars::TC_Common::replace(_packagePrefix, ".", "/");
}
