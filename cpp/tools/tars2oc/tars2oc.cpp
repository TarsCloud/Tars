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
#include "tars2oc.h"
#include "util/tc_md5.h"
#include "util/tc_file.h"
#include "util/tc_common.h"
#include <string>

#define TAB g_parse->getTab()
#define INC_TAB g_parse->incTab()
#define DEL_TAB g_parse->delTab()

//////////////////////////////////////////////////////////////////////////////////

string Tars2OC::toClassName(const TypePtr &pPtr) const
{
    string s;
    BuiltinPtr bPtr = BuiltinPtr::dynamicCast(pPtr);
    if (bPtr)
    {
        switch (bPtr->kind())
        {
            case Builtin::KindBool:     s = "bool";     break;
            case Builtin::KindByte:     s = "char";     break;
            case Builtin::KindShort:    s = "short";    break;
            case Builtin::KindInt:      s = "int32";    break;
            case Builtin::KindLong:     s = "int64";    break;
            case Builtin::KindFloat:    s = "float";    break;
            case Builtin::KindDouble:   s = "double";   break;
            case Builtin::KindString:   s = "string";   break;
            default:                    assert(false);  break;
        }

        return s;
    }

    VectorPtr vPtr = VectorPtr::dynamicCast(pPtr);
    if (vPtr) return tostrVector(vPtr);

    MapPtr mPtr = MapPtr::dynamicCast(pPtr);
    if (mPtr) return tostrMap(mPtr);

    StructPtr sPtr = StructPtr::dynamicCast(pPtr);
    if (sPtr) return tostrStruct(sPtr);

    EnumPtr ePtr = EnumPtr::dynamicCast(pPtr);
    if (ePtr) return tostrEnum(ePtr);

    if (!pPtr) return "void";

    assert(false);
    return "";
}

string Tars2OC::toAddtionalClassName(const TypePtr &pPtr) const
{
	BuiltinPtr bPtr = BuiltinPtr::dynamicCast(pPtr);
	if (bPtr)
	{
		string s = "";
		switch (bPtr->kind())
		{
			case Builtin::KindBool:
            case Builtin::KindByte:
			case Builtin::KindShort:
			case Builtin::KindInt:
			case Builtin::KindLong:
			case Builtin::KindFloat:
			case Builtin::KindDouble:
			{
				s = "ONSNumber";
				break;
			}
			case Builtin::KindString:
			{
				s = "ONSString";
				break;
			}
			default: assert(false);	break;
		}

		return s;
	}

	VectorPtr vPtr = VectorPtr::dynamicCast(pPtr);
	if (vPtr)
	{
		string s = "";
		//vector<byte>不用添加附加信息编码
		if (toClassName(vPtr) == "list<char>")
		{
			s = "ONSData";
		}
		else
		{
			s = "V" + toAddtionalClassName(vPtr->getTypePtr());

		}
		return s;
	}

	MapPtr mPtr = MapPtr::dynamicCast(pPtr);
	if (mPtr)
	{
		string sKey = toAddtionalClassName(mPtr->getLeftTypePtr());
		string sValue = toAddtionalClassName(mPtr->getRightTypePtr());

		ostringstream osKeyLen;
		{
			int iLen = sKey.length();
			//规定不能超过99
			assert(iLen <= 99 && iLen >=0);
			char sLen[3];
			snprintf(&sLen[0],3,"%.2d",iLen);
			osKeyLen <<sLen;
		}
		return "M" + osKeyLen.str() + sKey + sValue;
	}

	StructPtr sPtr = StructPtr::dynamicCast(pPtr);
	if (sPtr)
	{
		//struct当做Object处理
		return "O" + tostrStruct(sPtr);//TC_Common::replace(sPtr->getSid(), "::", "");
	}

	EnumPtr ePtr = EnumPtr::dynamicCast(pPtr);
	if (ePtr)
	{
		//Enum当做Object处理
		return "ONSNumber";
	}

	if (!pPtr) return "void";

	assert(false);
	return "";
}


/*******************************BuiltinPtr********************************/
string Tars2OC::tostr(const TypePtr &pPtr) const
{
    BuiltinPtr bPtr = BuiltinPtr::dynamicCast(pPtr);
    if (bPtr) return tostrBuiltin(bPtr);

    VectorPtr vPtr = VectorPtr::dynamicCast(pPtr);
    if (vPtr)
    {
        if (toClassName(vPtr) == "list<char>")
        {
            return "NSData*";
        }
        else
        {
            return "NSArray*";
        }
    }

    MapPtr mPtr = MapPtr::dynamicCast(pPtr);
    if (mPtr) return "NSDictionary*";

    StructPtr sPtr = StructPtr::dynamicCast(pPtr);
    if (sPtr) return tostrStruct(sPtr)+ "*";

    EnumPtr ePtr = EnumPtr::dynamicCast(pPtr);
    if (ePtr) return tostrEnum(ePtr);

    if (!pPtr) return "void";

    assert(false);
    return "";
}

string Tars2OC::tostrBuiltin(const BuiltinPtr &pPtr) const
{
    string s;

    switch (pPtr->kind())
    {
    case Builtin::KindBool:
        s = "TarsBool";
        break;
    case Builtin::KindByte:
        s = "TarsInt8";
        break;
    case Builtin::KindShort:
        s = (pPtr->isUnsigned()?"TarsUInt8":"TarsInt16");
        break;
    case Builtin::KindInt:
        s = (pPtr->isUnsigned()?"TarsUInt16":"TarsInt32");
        break;
    case Builtin::KindLong:
        s =  (pPtr->isUnsigned()?"TarsUInt32":"TarsInt64");
        break;
    case Builtin::KindFloat:
        s = "TarsFloat";
        break;
    case Builtin::KindDouble:
        s = "TarsDouble";
        break;
    case Builtin::KindString:
        s = "NSString*";
        break;
    default:
        assert(false);
        break;
    }

    return s;
}
/*******************************VectorPtr********************************/
string Tars2OC::tostrVector(const VectorPtr &pPtr) const
{
    string s = string("list<") + toClassName(pPtr->getTypePtr());

    if (MapPtr::dynamicCast(pPtr->getTypePtr()) || VectorPtr::dynamicCast(pPtr->getTypePtr()))
    {
        s += " >";
    }
    else
    {
        s += ">";
    }
    return s;
}
/*******************************MapPtr********************************/
string Tars2OC::tostrMap(const MapPtr &pPtr) const
{
    string s = string("map<") + toClassName(pPtr->getLeftTypePtr()) + "," + toClassName(pPtr->getRightTypePtr());
    if (MapPtr::dynamicCast(pPtr->getRightTypePtr()) || VectorPtr::dynamicCast(pPtr->getRightTypePtr()))
    {
        s += " >";
    }
    else
    {
        s += ">";
    }
    return s;
}
string Tars2OC::getReqOptionType(const TypeIdPtr &pPtr) const
{
	return (pPtr->isRequire())?"r":"o";
}

string Tars2OC::toTarsV2Procstr(const TypeIdPtr &pPtr) const
{
	ostringstream s;
	BuiltinPtr bPtr = BuiltinPtr::dynamicCast(pPtr->getTypePtr());

	if (bPtr)
	{
		//普通类型使用JV2_PROP_NM
		s << "JV2_PROP_NM(" << getReqOptionType(pPtr) << "," << pPtr->getTag() <<","<< pPtr->getId() << ")";
	}

	VectorPtr vPtr = VectorPtr::dynamicCast(pPtr->getTypePtr());
	if (vPtr)
	{
		//vector<byte>不用添加附加信息编码
		if (toClassName(vPtr) == "list<char>")
		{
			s << "JV2_PROP_NM(" << getReqOptionType(pPtr) << "," << pPtr->getTag() << "," << pPtr->getId() << ")";
		}
		else
		{
			s << "JV2_PROP_EX(" << getReqOptionType(pPtr) << "," << pPtr->getTag()<< "," <<pPtr->getId()<< "," <<toAddtionalClassName(vPtr)<< ")";
		}
	}

	MapPtr mPtr = MapPtr::dynamicCast(pPtr->getTypePtr());
	if (mPtr)
	{
		s << "JV2_PROP_EX(" << getReqOptionType(pPtr) << "," << pPtr->getTag()<< "," <<pPtr->getId()<< "," << toAddtionalClassName(mPtr) << ")";
	}

	StructPtr sPtr = StructPtr::dynamicCast(pPtr->getTypePtr());
	if (sPtr)
	{
		s << "JV2_PROP_NM(" << getReqOptionType(pPtr) << "," << pPtr->getTag() <<","<< pPtr->getId() << ")";
	}

	EnumPtr ePtr = EnumPtr::dynamicCast(pPtr->getTypePtr());
	if (ePtr)
	{
		s << "JV2_PROP_NM(" << getReqOptionType(pPtr) << "," << pPtr->getTag() <<","<< pPtr->getId() << ")";
	}

	return s.str();
}

/*******************************StructPtr********************************/
string Tars2OC::tostrStruct(const StructPtr &pPtr) const
{

	vector<string> vStr = TC_Common::sepstr<string>(pPtr->getSid(), "::");
	if(vStr.size() >= 2)
	{
		string sNamespace = getNamePrix(vStr[0]);
		return sNamespace + vStr[1];
	}

    return TC_Common::replace(pPtr->getSid(), "::", "");
}

/////////////////////////////////////////////////////////////////////
string Tars2OC::tostrEnum(const EnumPtr &pPtr) const
{
	vector<string> vStr = TC_Common::sepstr<string>(pPtr->getSid(), "::");
	if(vStr.size() >= 2)
	{
		string sNamespace = getNamePrix(vStr[0]);
		return sNamespace + vStr[1];
	}
    return TC_Common::replace(pPtr->getSid(), "::", "");
}

void Tars2OC::toIncludeName(const TypePtr &pPtr,map<string,int>& mReference) const
{
	//基本类型没有依赖其它结构
	BuiltinPtr bPtr = BuiltinPtr::dynamicCast(pPtr);
	if (bPtr)
	{
		return ;
	}

	VectorPtr vPtr = VectorPtr::dynamicCast(pPtr);
	if (vPtr)
	{
		BuiltinPtr bPtr = BuiltinPtr::dynamicCast(vPtr->getTypePtr());
		if (!bPtr)
		{
			toIncludeName(vPtr->getTypePtr(),mReference);
        }
	}

	MapPtr mPtr = MapPtr::dynamicCast(pPtr);
	if (mPtr)
	{
		BuiltinPtr bPtr = BuiltinPtr::dynamicCast(mPtr->getLeftTypePtr());
		if (!bPtr)//not builin type
		{
			toIncludeName(mPtr->getLeftTypePtr(),mReference);
		}

		BuiltinPtr _bPtr = BuiltinPtr::dynamicCast(mPtr->getRightTypePtr());
		if (!_bPtr)
		{
			toIncludeName(mPtr->getRightTypePtr(),mReference);
		}
	}

	StructPtr sPtr = StructPtr::dynamicCast(pPtr);
	if (sPtr)
	{
        mReference[tostrStruct(sPtr)] = EM_STRUCT_TYPE;
	}

	EnumPtr ePtr = EnumPtr::dynamicCast(pPtr);
	if (ePtr)
	{
        mReference[tostrEnum(ePtr)] = EM_ENUM_TYPE;
	}
}

map<string,int> Tars2OC::getReferences(const StructPtr &pPtr, const string& namespaceId) const
{
    map<string,int> mTemp;
	vector<TypeIdPtr>& vMember = pPtr->getAllMemberPtr();

	//查看成员变量是否依赖其它的结构体
	for (size_t j = 0; j < vMember.size(); j++)
	{
		BuiltinPtr bPtr = BuiltinPtr::dynamicCast(vMember[j]->getTypePtr());
		if (bPtr)
		{
			continue;
		}

		VectorPtr vPtr = VectorPtr::dynamicCast(vMember[j]->getTypePtr());
		if (vPtr)
		{
			//vector<byte>不用添加附加信息编码
			if (toClassName(vPtr) == "list<char>")
			{
				continue;
			}
			else
			{
				toIncludeName(vPtr,mTemp);
			}
		}

		MapPtr mPtr = MapPtr::dynamicCast(vMember[j]->getTypePtr());
		if (mPtr)
		{
			toIncludeName(vPtr,mTemp);
		}

		StructPtr sPtr = StructPtr::dynamicCast(vMember[j]->getTypePtr());
		if (sPtr)
		{
            mTemp[tostrStruct(sPtr)] = EM_STRUCT_TYPE;
		}

		EnumPtr ePtr = EnumPtr::dynamicCast(vMember[j]->getTypePtr());
		if (ePtr)
		{
           mTemp[tostrEnum(ePtr)] = EM_ENUM_TYPE;
		}

	}

	return mTemp;
}
bool Tars2OC::IsRetainType(const TypePtr &pPtr) const
{
	//对象类型用retain,enum is NSInteger
	BuiltinPtr bPtr  = BuiltinPtr::dynamicCast(pPtr);
	EnumPtr ePtr = EnumPtr::dynamicCast(pPtr);
	if((bPtr && bPtr->kind() != Builtin::KindString) || ePtr)
	{
		return false;
	}
	else
	{
		return true;
	}

}
string Tars2OC::getNamePrix(const string& sTarsNS) const
{
	string sNameSpace = "";
	if(m_bNeedNS)
	{
		if(!m_sNamespace.empty())
		{
			return m_sNamespace;
		}
		else
		{
			return sTarsNS;
		}
	}
	else
	{
		return sNameSpace;
	}
}

string Tars2OC::getPropertyName(const string& sId) const
{
	return "JV2_PROP(" + sId + ")";
}

string Tars2OC::getSetterName(const string& sId) const
{
	if(!sId.empty())
	{
		return "set" + TC_Common::upper(sId.substr(0,1)) + sId.substr(1) + ":";
	}
	else
	{
		return sId;
	}
}

///////////////////////////////////////////////////////////////////////
string Tars2OC::generateH(const StructPtr &pPtr, const string& namespaceId) const
{
    ostringstream s;

    string sStructName = getNamePrix(namespaceId) + pPtr->getId();
	string fileH = m_sBaseDir + "/" + sStructName + ".h";

    s << g_parse->printHeaderRemark();

    s << "#import \"TarsObjectV2.h\"" << endl;

    map<string,int> mTemp = getReferences(pPtr);
    for(map<string,int>::iterator it = mTemp.begin();it != mTemp.end();it++)
    {
        s << "#import \"" << it->first <<".h\""<<endl;
    }
    s << endl;

    s << "@interface " << sStructName << " : TarsObjectV2"<< endl;
	s << endl;

    //定义成员变量
    vector<TypeIdPtr>& vMember = pPtr->getAllMemberPtr();
    for (size_t j = 0; j < vMember.size(); j++)
    {
		string sPropType = IsRetainType(vMember[j]->getTypePtr())?"retain":"assign";

		s << "@property (nonatomic, "<<sPropType<<", JV2_PROP_GS_V2(" << vMember[j]->getId() << "," << getSetterName(vMember[j]->getId()) <<")) ";

        s << tostr(vMember[j]->getTypePtr()) << " " << toTarsV2Procstr(vMember[j]) <<";"<<endl;
    }
    s << endl;
    s << "@end" << endl;

	tars::TC_File::makeDirRecursive(m_sBaseDir, 0755);
	tars::TC_File::save2file(fileH, s.str());

    return "";
}


string Tars2OC::writeInit(const vector<TypeIdPtr>& vMember) const
{
	ostringstream s;

	s << "- (id)init" <<endl;
	s << "{" <<endl;
	INC_TAB;
	s << TAB << "if (self = [super init]) {" <<endl;
	INC_TAB;
	for (size_t j = 0; j < vMember.size(); j++)
	{
		//基本类型初始化,有默认值或者说string类型才初始化
		BuiltinPtr bPtr  = BuiltinPtr::dynamicCast(vMember[j]->getTypePtr());
		if(bPtr)
		{
			if(bPtr->kind() == Builtin::KindBool)
			{
				string sBool = (vMember[j]->def() == "true")?"YES":"NO";
				s <<TAB << getPropertyName(vMember[j]->getId()) << " = " << sBool << ";" << endl;
			}
			else if(bPtr->kind() == Builtin::KindString)
			{
				string tmp = tars::TC_Common::replace(vMember[j]->def(), "\"", "\\\"");
				if(!tmp.empty())
				{
					s <<TAB << getPropertyName(vMember[j]->getId()) << " = @\"" << tmp << "\";" << endl;
				}
				else
				{
					s <<TAB << getPropertyName(vMember[j]->getId()) << " = DefaultTarsString;" << endl;
				}
			}
			else if((vMember[j]->def() != "0"))
			{
				s <<TAB << getPropertyName(vMember[j]->getId()) << " = " << vMember[j]->def() << ";" << endl;
			}
			//其它情况的基本类型不用初始化，系统提供默认值
		}

		//是否require字段,且是retain属性
		if(vMember[j]->isRequire() && IsRetainType(vMember[j]->getTypePtr()))
		{
			VectorPtr vPtr = VectorPtr::dynamicCast(vMember[j]->getTypePtr());
			if (vPtr)
			{
				if (toClassName(vPtr) == "list<char>")
				{
					s <<TAB << getPropertyName(vMember[j]->getId()) << " = DefaultTarsData;" << endl;
				}
				else
				{
					s <<TAB << getPropertyName(vMember[j]->getId()) << " = DefaultTarsArray;" << endl;
				}
			}

			MapPtr mPtr = MapPtr::dynamicCast(vMember[j]->getTypePtr());
			if (mPtr)
			{
				s <<TAB << getPropertyName(vMember[j]->getId()) << " = DefaultTarsDictionary;" << endl;
			}

			StructPtr sPtr = StructPtr::dynamicCast(vMember[j]->getTypePtr());
			if (sPtr)
			{
				s <<TAB << getPropertyName(vMember[j]->getId()) << " = ["<<tostrStruct(sPtr)<<" object];" << endl;
			}
		}
	}
	DEL_TAB;
	s << TAB <<"}" <<endl;
	s << TAB <<"return self;"<<endl;
	DEL_TAB;
	s << TAB <<"}" <<endl;
	return s.str();
}

string Tars2OC::writedealloc(const vector<TypeIdPtr>& vMember) const
{
	ostringstream s;

	s << "- (void)dealloc" <<endl;
	s << "{" <<endl;
	INC_TAB;
	for (size_t j = 0; j < vMember.size(); j++)
	{
		//对象类型才需要=nil,NSString是对象类型
		if(IsRetainType(vMember[j]->getTypePtr()))
		{
			s <<TAB << getPropertyName(vMember[j]->getId()) << " = nil;" <<endl;
		}
	}

	s << TAB << "[super dealloc];" <<endl;
	DEL_TAB;
	s << TAB <<"}" <<endl;

	return s.str();
}

string Tars2OC::writesynthesize(const vector<TypeIdPtr>& vMember) const
{
	ostringstream s;
	s <<endl;

	for (size_t j = 0; j < vMember.size(); j++)
	{
		s <<TAB << "@synthesize " << toTarsV2Procstr(vMember[j]) <<";"<<endl;
	}
	return s.str();
}

string Tars2OC::writeTarsType(const StructPtr &pPtr) const
{
	ostringstream s;

	s << "+ (NSString*)tarsType" <<endl;
	s << "{" <<endl;

	INC_TAB;
	s << TAB <<"return @"<< "\""<<TC_Common::replace(pPtr->getSid(), "::", ".")<< "\";" <<endl;
	DEL_TAB;

	s << "}" <<endl;

	return s.str();
}

string Tars2OC::writeInitialize(const StructPtr &pPtr) const
{
	ostringstream s;

	s << "+ (void)initialize" <<endl;
	s << "{" <<endl;

	INC_TAB;

	string sStructName = tostrStruct(pPtr);//TC_Common::replace(pPtr->getSid(), "::", "");

	s << TAB << "if (self == [" << sStructName << " class]) {" << endl;

	INC_TAB;
    map<string,int> mTemp = getReferences(pPtr);
    for(map<string,int>::iterator it = mTemp.begin();it != mTemp.end();it++)
    {
       if(it->second == EM_STRUCT_TYPE)
       {
          s << TAB << "[" << it->first <<" initialize];"<< endl;
       }
    }

	s << TAB <<"[super initialize];" <<endl;
	DEL_TAB;

	s << TAB << "}" <<endl;

	DEL_TAB;
	s << "}" <<endl;

	return s.str();
}

///////////////////////////////////////////////////////////////////////
string Tars2OC::generateM(const EnumPtr &pPtr, const string& namespaceId) const
{
	ostringstream s;

	string sEnumPrefix = getNamePrix(namespaceId) + pPtr->getId();

	string fileH = m_sBaseDir + "/" + sEnumPrefix + ".m";

	s << g_parse->printHeaderRemark();

	s << "#import \"TarsObjectV2.h\"" << endl;

	s << "#import \"" <<sEnumPrefix<<".h\""<<endl;

	s << endl;

	s << "#if TARSV2_ENUM_ETOS_AND_STOE_SUPPORTED" <<endl;
	s <<endl;

	s << "@implementation " <<sEnumPrefix<<"Helper"<<endl;
	s << endl;
	//生成枚举转字符串函数
	s << "+ (NSString *)etos:(" << sEnumPrefix << ")e" << endl;
	s << "{" << endl;

	INC_TAB;
	s << TAB << "switch(e){" << endl;

	INC_TAB;

	vector<TypeIdPtr>& member = pPtr->getAllMemberPtr();
	for(size_t i = 0; i < member.size(); i++)
	{
		string sMem = sEnumPrefix + "_" + member[i]->getId();

		s << TAB << "case " << sMem << ": return "<< "@\"" << sMem << "\";" << endl;
	}
	s << TAB << "default: return @\"\";" << endl;

	DEL_TAB;
	s << TAB << "}" << endl;
	DEL_TAB;
	s << TAB << "}" << endl;

	s << endl;

	//生成字符串转枚举函数
	s << "+ (" << sEnumPrefix << ")stoe:(NSString *)s" << endl;
	s << "{" << endl;

	INC_TAB;
	for(size_t i = 0; i < member.size(); i++)
	{
		string sMem = sEnumPrefix + "_" + member[i]->getId();

		s << TAB << "if(isTarsEnumStringEqual(s, @\"" << sMem << "\")) return " << sMem <<";"<<endl;
	}
	s << TAB << "return NSIntegerMin;" << endl;

	DEL_TAB;
	s << TAB << "}" << endl;

	s <<endl;
	s << "@end"<<endl;
	s <<endl;

	s << "#endif" <<endl;

	tars::TC_File::makeDirRecursive(m_sBaseDir, 0755);
	tars::TC_File::save2file(fileH, s.str());

	return "";

}

string Tars2OC::generateM(const StructPtr &pPtr, const string& namespaceId) const
{
    string sStructName = getNamePrix(namespaceId) + pPtr->getId();

    ostringstream s;

    string fileM = m_sBaseDir + "/" + sStructName + ".m";

    s << g_parse->printHeaderRemark();
    //定义成员变量
    vector<TypeIdPtr>& member = pPtr->getAllMemberPtr();

    ////////////////////////////////////////////////////////////
    s << "#import \"" <<sStructName<<".h\""<<endl;
    s << endl;
    s << "@implementation " <<sStructName<< endl;

	//build properties
	s << writesynthesize(member) <<endl;

    //build initialize
    s << writeInitialize(pPtr) <<endl;

	//build init method
	s << writeInit(member) <<endl;

	//build dealloc method
	if(!m_bARC)
	{
		s << writedealloc(member) <<endl;
	}
	//build tarsType method
	s << writeTarsType(pPtr) <<endl;

    s << "@end" << endl;

    tars::TC_File::makeDirRecursive(m_sBaseDir, 0755);
    tars::TC_File::save2file(fileM, s.str());
    return "";
}


/*******************************ContainerPtr********************************/
string Tars2OC::generateH(const ContainerPtr &pPtr) const
{
    ostringstream s;
    for (size_t i = 0; i < pPtr->getAllNamespacePtr().size(); i++)
    {
        s << generateH(pPtr->getAllNamespacePtr()[i]) << endl;
        s << endl;
    }
    return s.str();
}

/******************************EnumPtr***************************************/

string Tars2OC::generateH(const EnumPtr &pPtr, const string& namespaceId) const
{
    ostringstream s;

	string sEnumPrefix = getNamePrix(namespaceId) + pPtr->getId();

	string fileH = m_sBaseDir + "/" + sEnumPrefix + ".h";

    s << g_parse->printHeaderRemark();

    s << "#import \"TarsObjectV2.h\"" << endl;

	s << endl;
    s << "enum {"<<endl;

	INC_TAB;
    vector<TypeIdPtr>& member = pPtr->getAllMemberPtr();
    for (size_t i = 0; i < member.size(); i++)
    {
        s << TAB << sEnumPrefix << "_" << member[i]->getId();
        if (member[i]->hasDefault())
        {
            s << " = " << member[i]->def();
        }
        s << (i==member.size()-1 ? "" : ",") << endl;
    }
	DEL_TAB;
    s <<"};" << endl;

	s <<"#define "<<sEnumPrefix<<" NSInteger"<<endl;
	s <<endl;

	s << "#if TARSV2_ENUM_ETOS_AND_STOE_SUPPORTED" <<endl;
	s <<endl;

	s << "@interface " <<sEnumPrefix << "Helper: TarsEnumHelper" <<endl;
	s <<endl;

	s << "+ (NSString *)etos:(" << sEnumPrefix <<")e;"<<endl;

	s << "+ (" << sEnumPrefix << ")stoe:(NSString *)s;" <<endl;

	s <<endl;

	s << "@end" <<endl;
	s << endl;
	s << "#endif" <<endl;
    tars::TC_File::makeDirRecursive(m_sBaseDir, 0755);
    tars::TC_File::save2file(fileH, s.str());

    return "";
}


/******************************NamespacePtr***************************************/

string Tars2OC::generate(const NamespacePtr &pPtr) const
{
    vector<StructPtr>&ss = pPtr->getAllStructPtr();
    vector<EnumPtr>&es = pPtr->getAllEnumPtr();

	//先遍历枚举类型,单独生成文件
	for (size_t i = 0; i < es.size(); i++)
	{
		generateH(es[i], pPtr->getId());
		generateM(es[i], pPtr->getId());
	}

	//遍历结构体类型,单独生成文件
	for (size_t i = 0; i < ss.size(); i++)
	{
		generateH(ss[i], pPtr->getId());
        generateM(ss[i], pPtr->getId());
	}

    return "";
}



/******************************Tars2OC***************************************/

void Tars2OC::generate(const ContextPtr &pPtr) const
{
    vector<NamespacePtr> namespaces = pPtr->getNamespaces();
    for (size_t i = 0; i < namespaces.size(); i++)
    {
        generate(namespaces[i]);
    }
}



void Tars2OC::createFile(const string &file)
{
    std::vector<ContextPtr> contexts = g_parse->getContexts();
    for (size_t i = 0; i < contexts.size(); i++)
    {
        if (file == contexts[i]->getFileName())
        {
           generate(contexts[i]);
          //generateM(contexts[i]);
        }
    }
}

