#include "util/tc_md5.h"
#include "util/tc_file.h"
#include "util/tc_common.h"
#include <ctype.h>
#include <string>
#include <stdlib.h>
#include "tars2go.h"

#define TAB g_parse->getTab()
#define INC_TAB g_parse->incTab()
#define DEL_TAB g_parse->delTab()

static string k_current_namespace;

static string k_tafgo_package_prefix = "tars.";

static string getStructNameSapce(StructPtr s)
{
    std::string str = s->getSid();
    int count = 0;
    std::string newpat = ".";
    std::string pattern = "::";
    const size_t nsize = newpat.size();
    const size_t psize = pattern.size();

    for (size_t pos = str.find(pattern, 0); pos != std::string::npos; pos = str.find(pattern, pos + nsize))
    {
        str.replace(pos, psize, newpat);
        count++;
    }
    return str.substr(0, str.length() - ("." + s->getId()).length());
}

string Tars2Go::generateEncodeInvocation(TypeIdPtr& t, const std::string& bufstr, const std::string& id_prefix,
        bool first_upper) const
{
    ostringstream s;
    std::string str;

    std::string idstr = t->getId();
    if (first_upper)
    {
        idstr[0] = toupper(idstr[0]);
    }
    idstr = id_prefix + idstr;
    StructPtr sPtr = StructPtr::dynamicCast(t->getTypePtr());
    if (sPtr)
    {
        str.append(k_tafgo_package_prefix);
        if (first_upper)
        {
            str.append("EncodeTagStructValue(").append(bufstr).append(", &").append(idstr).append(", ");
        }
        else
        {
            str.append("EncodeTagStructValue(").append(bufstr).append(", ").append(idstr).append(", ");
        }
    }
    VectorPtr vPtr = VectorPtr::dynamicCast(t->getTypePtr());
    if (vPtr)
    {
        str.append(k_tafgo_package_prefix);
        std::string vstr = tostr(vPtr->getTypePtr());
        if ("byte" == vstr)
        {
            str.append("EncodeTagBytesValue(").append(bufstr).append(", ").append(idstr).append(", ");
        }
        else if ("string" == vstr)
        {
            str.append("EncodeTagStringsValue(").append(bufstr).append(", ").append(idstr).append(", ");
        }
        else
        {
            str.append("EncodeTagVectorValue(").append(bufstr).append(", ").append(idstr).append(", ");
        }
    }
    MapPtr mPtr = MapPtr::dynamicCast(t->getTypePtr());
    if (mPtr)
    {
        str.append(k_tafgo_package_prefix);
        str.append("EncodeTagMapValue(").append(bufstr).append(", ").append(idstr).append(", ");
        //s << TAB << "taf.EncodeTagMapValue(&osBuffer, " << inParas[i]->getTypeIdPtr()->getId() << ", ";
    }
    BuiltinPtr bPtr = BuiltinPtr::dynamicCast(t->getTypePtr());
    if (bPtr)
    {
        std::string typestr = tostrBuiltin(bPtr);
        typestr[0] = toupper(typestr[0]);
        str.append(k_tafgo_package_prefix);
        str.append("EncodeTag").append(typestr).append("Value(").append(bufstr).append(", ").append(idstr).append(", ");
    }
    EnumPtr ePtr = EnumPtr::dynamicCast(t->getTypePtr());
    if (ePtr)
    {
        str.append(k_tafgo_package_prefix);
        str.append("EncodeTagInt32Value(").append(bufstr).append(", ").append(idstr).append(", ");
    }
    if (str.empty())
    {
        printf("####Unsupported type:%s\n", tostr(t->getTypePtr()).c_str());
    }
    s << str << t->getTag() << ")";
    return s.str();
}

string Tars2Go::generateDecodeInvocation(TypeIdPtr& t, const std::string& bufstr, const std::string& id_prefix,
        bool first_upper, bool is_pointer) const
{
    ostringstream s;
    std::string str;
    std::string idstr = t->getId();
    if (first_upper)
    {
        idstr[0] = toupper(idstr[0]);
    }
    idstr = id_prefix + idstr;
    if (!is_pointer)
    {
        idstr = "&" + idstr;
    }

    StructPtr sPtr = StructPtr::dynamicCast(t->getTypePtr());
    if (sPtr)
    {
        str.append(k_tafgo_package_prefix);
        str.append("DecodeTagStructValue(").append(bufstr).append(", ").append(idstr).append(", ");

    }
    VectorPtr vPtr = VectorPtr::dynamicCast(t->getTypePtr());
    if (vPtr)
    {
        str.append(k_tafgo_package_prefix);
        std::string vstr = tostr(vPtr->getTypePtr());
        if ("byte" == vstr)
        {
            str.append("DecodeTagBytesValue(").append(bufstr).append(", ").append(idstr).append(", ");
        }
        else if ("string" == vstr)
        {
            str.append("DecodeTagStringsValue(").append(bufstr).append(", ").append(idstr).append(", ");
        }
        else
        {
            str.append("DecodeTagVectorValue(").append(bufstr).append(", ").append(idstr).append(", ");
        }
    }
    MapPtr mPtr = MapPtr::dynamicCast(t->getTypePtr());
    if (mPtr)
    {
        str.append(k_tafgo_package_prefix);
        str.append("DecodeTagMapValue(").append(bufstr).append(", ").append(idstr).append(", ");
        //s << TAB << "taf.EncodeTagMapValue(&osBuffer, " << inParas[i]->getTypeIdPtr()->getId() << ", ";
    }
    BuiltinPtr bPtr = BuiltinPtr::dynamicCast(t->getTypePtr());
    if (bPtr)
    {
        std::string typestr = tostrBuiltin(bPtr);
        typestr[0] = toupper(typestr[0]);
        str.append(k_tafgo_package_prefix);
        str.append("DecodeTag").append(typestr).append("Value(").append(bufstr).append(", ").append(idstr).append(
                ", ");
    }
    EnumPtr ePtr = EnumPtr::dynamicCast(t->getTypePtr());
    if (ePtr)
    {
        str.append(k_tafgo_package_prefix);
        str.append("DecodeTagInt32Value(").append(bufstr).append(", ").append(idstr).append(", ");
    }
    if (str.empty())
    {
        printf("####Unsupported type:%s\n", tostr(t->getTypePtr()).c_str());
    }
    s << str << t->getTag() << ", " << (t->isRequire() ? "true" : "false") << ")";
    return s.str();
}

//////////////////////////////////////////////////////////////////////////////////
//
Tars2Go::Tars2Go() :
        m_bPDU(false), m_bCheckDefault(false), m_bOnlyStruct(false), m_bTafMSF(
                false), m_sNamespace("tars"), m_bUnknownField(false), m_bTafMaster(false)
{

}

string Tars2Go::readUnknown(const TypeIdPtr &pPtr) const
{
    ostringstream s;
    s << TAB << "_is.readUnknown(sUnknownField, " << pPtr->getTag() << ");" << endl;
    return s.str();
}
string Tars2Go::writeUnknown() const
{
    ostringstream s;
    s << TAB << "_os.writeUnknown(sUnknownField);" << endl;
    return s.str();
}

int Tars2Go::getSuffix(const TypeIdPtr &pPtr) const
{
    BuiltinPtr bPtr = BuiltinPtr::dynamicCast(pPtr->getTypePtr());
    if (bPtr && bPtr->kind() == Builtin::KindString && bPtr->isArray())
    {
        return bPtr->getSize();
    }

    VectorPtr vPtr = VectorPtr::dynamicCast(pPtr->getTypePtr());
    if (vPtr && vPtr->isArray())
    {
        return vPtr->getSize();
    }

    return -1;
}

string Tars2Go::toStrSuffix(const TypeIdPtr &pPtr) const
{
    ostringstream s;

    int i = getSuffix(pPtr);

    if (i >= 0)
    {
        s << "[" << i << "]";
    }
    return s.str();
}

string Tars2Go::tostr(const TypePtr &pPtr) const
{
    BuiltinPtr bPtr = BuiltinPtr::dynamicCast(pPtr);
    if (bPtr) return tostrBuiltin(bPtr);

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

string Tars2Go::tostrBuiltin(const BuiltinPtr &pPtr) const
{
    string s;

    switch (pPtr->kind())
    {
        case Builtin::KindBool:
//            s = m_sNamespace + ".Bool";
            s = "bool";
            break;
        case Builtin::KindByte:
//            s = m_sNamespace + ".Char";
            s = "byte";
            break;
        case Builtin::KindShort:
//            s = (pPtr->isUnsigned() ? m_sNamespace + ".UInt8" : m_sNamespace + ".Short");
            s = "int16";
            break;
        case Builtin::KindInt:
//            s = (pPtr->isUnsigned() ? m_sNamespace + ".UInt16" : m_sNamespace + ".Int32");
            s = "int32";
            break;
        case Builtin::KindLong:
//            s = (pPtr->isUnsigned() ?
//                    m_sNamespace + "." + (m_bTafMSF ? "taf" : "") + ".UInt32" : m_sNamespace + ".Int64");
            s = "int64";
            break;
        case Builtin::KindFloat:
//            s = m_sNamespace + ".Float";
            s = "float32";
            break;
        case Builtin::KindDouble:
//            s = m_sNamespace + ".Double";
            s = "float64";
            break;
        case Builtin::KindString:
//            if (pPtr->isArray()) s = m_sNamespace + ".Char"; //char a [8];
//            else s = "string"; //string a;
            s = "string";
            break;
        case Builtin::KindVector:
            s = "std::vector";
            break;
        case Builtin::KindMap:
            s = "std::map";
            break;
        default:
            assert(false);
            break;
    }

    return s;
}
/*******************************VectorPtr********************************/
string Tars2Go::tostrVector(const VectorPtr &pPtr) const
{

//    if (pPtr->isArray())
//    {
//        return tostr(pPtr->getTypePtr());
//    }
//
//    if (pPtr->isPointer())
//    {
//        return tostr(pPtr->getTypePtr()) + " *";
//    }
//
//    string s = Builtin::builtinTable[Builtin::KindVector] + string("<") + tostr(pPtr->getTypePtr());
//
//    if (MapPtr::dynamicCast(pPtr->getTypePtr()) || VectorPtr::dynamicCast(pPtr->getTypePtr()))
//    {
//        s += " >";
//    }
//    else
//    {
//        s += ">";
//    }
    string s = "[]" + tostr(pPtr->getTypePtr());
    return s;
}
/*******************************MapPtr********************************/
string Tars2Go::tostrMap(const MapPtr &pPtr) const
{
//    string s = Builtin::builtinTable[Builtin::KindMap] + string("<") + tostr(pPtr->getLeftTypePtr()) + ", "
//            + tostr(pPtr->getRightTypePtr());
//    if (MapPtr::dynamicCast(pPtr->getRightTypePtr()) || VectorPtr::dynamicCast(pPtr->getRightTypePtr()))
//    {
//        s += " >";
//    }
//    else
//    {
//        s += ">";
//    }
    string s = "map[" + tostr(pPtr->getLeftTypePtr()) + "]" + tostr(pPtr->getRightTypePtr());
    return s;
}

/*******************************StructPtr********************************/
string Tars2Go::tostrStruct(const StructPtr &pPtr) const
{
    string ns = getStructNameSapce(pPtr);
    if (ns == k_current_namespace)
    {
        return pPtr->getId();
    }
    else
    {
        return ns + "." + pPtr->getId();
    }
//    std::string str = pPtr->getSid();
//    int count = 0;
//    std::string newpat = ".";
//    std::string pattern = "::";
//    const size_t nsize = newpat.size();
//    const size_t psize = pattern.size();
//
//    for (size_t pos = str.find(pattern, 0); pos != std::string::npos; pos = str.find(pattern, pos + nsize))
//    {
//        str.replace(pos, psize, newpat);
//        count++;
//    }
//    return str;
}

string Tars2Go::MD5(const StructPtr &pPtr) const
{
    string s;
    vector<TypeIdPtr>& member = pPtr->getAllMemberPtr();
    for (size_t j = 0; j < member.size(); j++)
    {
        s += "_" + tostr(member[j]->getTypePtr());
    }

    return "\"" + tars::TC_MD5::md5str(s) + "\"";
}

/////////////////////////////////////////////////////////////////////
string Tars2Go::tostrEnum(const EnumPtr &pPtr) const
{
    return "int32";
    //return pPtr->getSid();
}
///////////////////////////////////////////////////////////////////////

string Tars2Go::generateInitValue(const TypeIdPtr &pPtr) const
{
    BuiltinPtr bPtr = BuiltinPtr::dynamicCast(pPtr->getTypePtr());
    string init = "";
    if (bPtr && Builtin::KindBool == bPtr->kind())
    {
        init = " = false";
    }

    EnumPtr ePtr = EnumPtr::dynamicCast(pPtr->getTypePtr());
    if (ePtr)
    {
        if (pPtr->hasDefault())
        {
            string sid = ePtr->getSid();
            init = " = " + pPtr->def() + ";";
        }
        else
        {
            vector<TypeIdPtr>& eMember = ePtr->getAllMemberPtr();
            if (eMember.size() > 0)
            {
                string sid = ePtr->getSid();
                init = " = " + sid.substr(0, sid.find_first_of("::")) + "::" + eMember[0]->getId() + ";";
            }
        }
    }
    return init;
}

struct SortOperation
{
        bool operator()(const OperationPtr &o1, const OperationPtr &o2)
        {
            return o1->getId() < o2->getId();
        }
};

//struct SortOperation
//{
//    bool operator()(const OperationPtr &o1, const OperationPtr &o2)
//    {
//        return o1->getId() < o2->getId();
//    }
//};

/******************************EnumPtr***************************************/

/******************************NamespacePtr***************************************/

/******************************Jce2Cpp***************************************/

string Tars2Go::generateGo(const ConstPtr &pPtr) const
{
    ostringstream s;

    if (pPtr->getConstGrammarPtr()->t == ConstGrammar::STRING)
    {
        string tmp = tars::TC_Common::replace(pPtr->getConstGrammarPtr()->v, "\"", "\\\"");
        s << TAB << pPtr->getTypeIdPtr()->getId() << " " << tostr(pPtr->getTypeIdPtr()->getTypePtr()) << " = \"" << tmp
                << "\";" << endl;
    }
    else
    {
        s << TAB << pPtr->getTypeIdPtr()->getId() << " " << tostr(pPtr->getTypeIdPtr()->getTypePtr()) << " = "
                << pPtr->getConstGrammarPtr()->v
                << ((tostr(pPtr->getTypeIdPtr()->getTypePtr()) == m_sNamespace + ".Int64") ? "LL" : "") << endl;
    }

    return s.str();
}

string Tars2Go::generateGo(const EnumPtr &pPtr) const
{
    ostringstream s;
    vector<TypeIdPtr>& member = pPtr->getAllMemberPtr();
    for (size_t i = 0; i < member.size(); i++)
    {
        s << TAB << member[i]->getId();
        if (member[i]->hasDefault())
        {
            s << " = int32(" << member[i]->def() << ")";
        }
        else
        {
            s << " = int32(" << i << ")";
        }
        s << endl;
    }
    return s.str();
}

string Tars2Go::generateGo(const StructPtr &pPtr, const string& namespaceId) const
{
    ostringstream s;
    vector<TypeIdPtr>& member = pPtr->getAllMemberPtr();
    s << "type " << pPtr->getId() << " struct{" << endl;
    INC_TAB;
    for (size_t j = 0; j < member.size(); j++)
    {
        if (member[j]->getTypePtr()->isArray() || member[j]->getTypePtr()->isPointer()) //�������͡�ָ��������Ҫ���峤��
        {
            s << TAB << m_sNamespace + "::" << (m_bTafMSF ? "taf" : "") << "UInt32 " << member[j]->getId() << "Len"
                    << ";" << endl;
        }

        std::string mid = member[j]->getId();
        mid[0] = toupper(mid[0]);
        s << TAB << mid << " " << tostr(member[j]->getTypePtr()) << toStrSuffix(member[j]);
        s << "  `tag:\"" << member[j]->getTag() << "\"";
        s << "  required:\"" << (member[j]->isRequire() ? "true" : "false") << "\"";
        s << "  json:\"" << member[j]->getId() << "\"`";
        s << endl;
    }
    if (m_bUnknownField)
    {
        s << TAB << "UnknownField string" << endl; //���ߴ�--unknown�����Զ������ֶ�,���δ֪tag����." << endl;
    }
    DEL_TAB;
    s << "}" << endl;
    s << TAB << endl;

    s << TAB << "func (p *" << pPtr->getId() << ") ClassName() string{" << endl;
    INC_TAB;
    s << TAB << "return " << "\"" << namespaceId << "." << pPtr->getId() << "\"" << endl;
    DEL_TAB;
    s << TAB << "}" << endl;

    s << TAB << "func (p *" << pPtr->getId() << ") MD5() string{" << endl;
    INC_TAB;
    s << TAB << "return " << MD5(pPtr) << endl;
    DEL_TAB;
    s << TAB << "}" << endl;

    s << TAB << "func (p *" << pPtr->getId() << ") ResetDefautlt(){" << endl;
    INC_TAB;
    s << TAB << "var empty " << pPtr->getId() << endl;
    s << TAB << "*p = empty" << endl;
    DEL_TAB;
    s << TAB << "}" << endl;

    s << TAB << "func (p *" << pPtr->getId() << ") Encode(buf *bytes.Buffer) error{" << endl;
    INC_TAB;
    s << TAB << "var err error " << endl;
    for (size_t j = 0; j < member.size(); j++)
    {
        s << TAB << "err = " << generateEncodeInvocation(member[j], "buf", "p.", true) << endl;
        s << TAB << "if nil != err{" << endl;
        INC_TAB;
        s << TAB << "return err" << endl;
        DEL_TAB;
        s << TAB << "}" << endl;
    }
    s << TAB << "return nil" << endl;
    DEL_TAB;
    s << TAB << "}" << endl;

    s << TAB << "func (p *" << pPtr->getId() << ") Decode(buf *bytes.Buffer) error{" << endl;
    INC_TAB;
    s << TAB << "var err error " << endl;
    for (size_t j = 0; j < member.size(); j++)
    {
        s << TAB << "err = " << generateDecodeInvocation(member[j], "buf", "p.", true, false) << endl;
        s << TAB << "if nil != err{" << endl;
        INC_TAB;
        s << TAB << "return err" << endl;
        DEL_TAB;
        s << TAB << "}" << endl;
    }
    s << TAB << "return err" << endl;
    DEL_TAB;
    s << TAB << "}" << endl;

    return s.str();
}

string Tars2Go::generateGo(const ParamDeclPtr &pPtr) const
{
    ostringstream s;
    s << pPtr->getTypeIdPtr()->getId();
    s << " ";
    StructPtr sPtr = StructPtr::dynamicCast(pPtr->getTypeIdPtr()->getTypePtr());
    if (sPtr || pPtr->isOut())
    {
        s << "*" << tostr(pPtr->getTypeIdPtr()->getTypePtr());
    }
    else
    {
        s << tostr(pPtr->getTypeIdPtr()->getTypePtr());
    }
    return s.str();
}

string Tars2Go::generateGo(const OperationPtr &pPtr, bool bVirtual, const string& interfaceId) const
{
    string proxyStructName = interfaceId + "Proxy";
    ostringstream s;
    std::string funcName = pPtr->getId();
    funcName[0] = toupper(funcName[0]);
    s << TAB << "func(p *" << proxyStructName << ") " << funcName << "(";
    vector<ParamDeclPtr>& vParamDecl = pPtr->getAllParamDeclPtr();
    string routekey = "";
    vector<ParamDeclPtr> inParas, outParas;
    for (size_t i = 0; i < vParamDecl.size(); i++)
    {
        if (vParamDecl[i]->isOut())
        {
            outParas.push_back(vParamDecl[i]);
        }
        else
        {
            inParas.push_back(vParamDecl[i]);
        }
    }
    for (size_t i = 0; i < vParamDecl.size(); i++)
    {
        s << generateGo(vParamDecl[i]);
        if (i != vParamDecl.size() - 1)
        {
            s << ", ";
        }
        else
        {
            s << ", context map[string]string)";
        }
        if (routekey.empty() && vParamDecl[i]->isRouteKey())
        {
            routekey = vParamDecl[i]->getTypeIdPtr()->getId();
        }
    }
    if (vParamDecl.size() == 0)
    {
        s << "context map[string]string)";
    }
    string returnType = tostr(pPtr->getReturnPtr()->getTypePtr());
    s << "(";
    if (returnType != "void")
    {
        s << "_ret " << returnType << ", ";
    }
    s << "respContext map[string]string, tarsErr error){" << endl;
//    for (size_t i = 0; i < outParas.size(); i++)
//    {
//        s << generateGo(outParas[i]);
//        if (i != outParas.size() - 1)
//        {
//            s << ", ";
//        }
//        else
//        {
//            s << ", respContext map[string]string";
//        }
//        if (routekey.empty() && vParamDecl[i]->isRouteKey())
//        {
//            routekey = vParamDecl[i]->getTypeIdPtr()->getId();
//        }
//    }
//    if (outParas.empty())
//    {
//        s << "respContext map[string]string,";
//    }
//    if (outParas.size() > 0)
//    {
//        s << ",";
//    }
//    s << "tarsErr error){" << endl;

    INC_TAB;
    s << TAB << "var osBuffer bytes.Buffer" << endl;

//    if (m_bTafMaster)
//    {
//        s << TAB << "this->taf_setMasterFlag(true);" << endl;
//    }

//    s << TAB << m_sNamespace + "::JceOutputStream<" + m_sNamespace + "::BufferWriter> _os;" << endl;

    for (size_t i = 0; i < inParas.size(); i++)
    {
        s << TAB << generateEncodeInvocation(inParas[i]->getTypeIdPtr(), "&osBuffer") << endl;

        //printf("####Unsupoorted type:%s\n", tostr(member[j]->getTypePtr()).c_str());
        //if(vParamDecl[i]->isOut()) continue;
        //s << writeTo(vParamDecl[i]->getTypeIdPtr());
//        s << TAB << "taf.EncodeTagValue(" << inParas[i]->getTypeIdPtr()->getId() << ","
//                << inParas[i]->getTypeIdPtr()->getTag() << ", &osBuffer)" << endl;
    }

//    s << TAB << "var rep taf.ResponsePacket" << endl;
//
//    s << TAB << "std::map<string, string> _mStatus;" << endl;

//    if (!routekey.empty())
//    {
//        ostringstream os;
//
//        os << routekey;
//
//        s << TAB << "_mStatus.insert(std::make_pair(ServantProxy::STATUS_GRID_KEY, " << os.str() << "));" << endl;
//    }

    s << TAB << "rep, err := p.TarsClient.Invoke(" << k_tafgo_package_prefix << "JCENORMAL,\"" << pPtr->getId()
            << "\", &osBuffer, context)" << endl;
    s << TAB << "if nil != err{" << endl;
    INC_TAB;
    s << TAB << "tarsErr = err" << endl;
    s << TAB << "return" << endl;
    DEL_TAB;
    s << TAB << "}" << endl;
    s << endl;
    s << TAB << "respContext = rep.Context" << endl;
    s << TAB << "respBuffer := bytes.NewBuffer(rep.SBuffer)" << endl;
    if (vParamDecl.size() > 0 || pPtr->getReturnPtr()->getTypePtr())
    {
//        s << TAB << m_sNamespace + "::JceInputStream<" + m_sNamespace + "::BufferReader> _is;" << endl;
//        s << TAB << "_is.setBuffer(rep.sBuffer);" << endl;
        if (pPtr->getReturnPtr()->getTypePtr())
        {
//            s << TAB << tostr(pPtr->getReturnPtr()->getTypePtr()) << " " << pPtr->getReturnPtr()->getId()
//                    << generateInitValue(pPtr->getReturnPtr()) << ";" << endl;
//            s << TAB << "tarsErr = taf.DecodeTagValue(" << pPtr->getReturnPtr()->getTag() << ", "
//                    << (pPtr->getReturnPtr()->isRequire() ? "true" : "false") << ", &_ret, " << "&rep.ResponseBuffer)"
//                    << endl;
            if (StructPtr::dynamicCast(pPtr->getReturnPtr()->getTypePtr()))
            {
                s << TAB << pPtr->getReturnPtr()->getId() << " = &" << tostr(pPtr->getReturnPtr()->getTypePtr()) << "{}"
                        << endl;
            }
            s << TAB << "tarsErr = " << generateDecodeInvocation(pPtr->getReturnPtr(), "respBuffer", "", false, false) << endl;
            s << TAB << "if nil != tarsErr{" << endl;
            INC_TAB;
            s << TAB << "return" << endl;
            DEL_TAB;
            s << TAB << "}" << endl;
//            s << readFrom(pPtr->getReturnPtr());
        }
        for (size_t i = 0; i < outParas.size(); i++)
        {
//            s << TAB << "tarsErr = taf.DecodeTagValue(" << outParas[i]->getTypeIdPtr()->getTag() << ", "
//                    << (outParas[i]->getTypeIdPtr()->isRequire() ? "true" : "false") << ", &"
//                    << outParas[i]->getTypeIdPtr()->getId() << " ,&rep.ResponseBuffer)" << endl;
//            if (StructPtr::dynamicCast(outParas[i]->getTypeIdPtr()->getTypePtr()))
//            {
//                s << TAB << outParas[i]->getTypeIdPtr()->getId() << " = &"
//                        << tostr(outParas[i]->getTypeIdPtr()->getTypePtr()) << "{}" << endl;
//            }
            s << TAB << "tarsErr = " << generateDecodeInvocation(outParas[i]->getTypeIdPtr(), "respBuffer", "", false, true) << endl;
            s << TAB << "if nil != tarsErr{" << endl;
            INC_TAB;
            s << TAB << "return" << endl;
            DEL_TAB;
            s << TAB << "}" << endl;
        }
    }
    s << TAB << "return" << endl;
    DEL_TAB;
    s << TAB << "}" << endl;
    return s.str();
}

string Tars2Go::generateGo(const InterfacePtr &pPtr) const
{
    ostringstream s;
    vector<OperationPtr>& vOperation = pPtr->getAllOperationPtr();

    std::string interfaceId = pPtr->getId();
    interfaceId[0] = toupper(interfaceId[0]);

    s << TAB << "type " << interfaceId << " interface{" << endl;
    INC_TAB;
    for (size_t i = 0; i < vOperation.size(); i++)
    {
        std::string funcName = vOperation[i]->getId();
        funcName[0] = toupper(funcName[0]);
        s << TAB << funcName << "(";
        vector<ParamDeclPtr>& vParamDecl = vOperation[i]->getAllParamDeclPtr();
        for (size_t j = 0; j < vParamDecl.size(); j++)
        {
            s << generateGo(vParamDecl[j]);
            if (j != vParamDecl.size() - 1)
            {
                s << ", ";
            }
            else
            {
                s << ", context map[string]string)";
            }
        }
        if (vParamDecl.size() == 0)
        {
            s << "context map[string]string)";
        }
        string returnType = tostr(vOperation[i]->getReturnPtr()->getTypePtr());
        s << "(";
        if (returnType != "void")
        {
            s << returnType << ", ";
        }
        s << "map[string]string, error)" << endl;
    }
    DEL_TAB;
    s << TAB << "};" << endl;

    s << TAB << "/* proxy for client */" << endl;
    s << TAB << "type " << interfaceId << "Proxy  struct{" << endl;
    INC_TAB;
    s << TAB << "TarsClient *" << k_tafgo_package_prefix << "Client" << endl;
    DEL_TAB;
    s << TAB << "};" << endl;

    s << endl;
    for (size_t i = 0; i < vOperation.size(); i++)
    {
        s << generateGo(vOperation[i], false, pPtr->getId()); // << endl;
    }

    s << endl;
    s << TAB << "func New" << interfaceId << "Proxy(obj string, timeout time.Duration) *" << interfaceId<<"Proxy{" << endl;
    INC_TAB;
    s << TAB << "c := " << k_tafgo_package_prefix  << "NewClient(obj, timeout)"<< endl;
    s << TAB << "proxy := &" << interfaceId<< "Proxy{c}" << endl;
    s << TAB << "return proxy" << endl;
    DEL_TAB;
    s << TAB << "}" << endl;
    return s.str();
}

string Tars2Go::generateGo(const NamespacePtr &pPtr) const
{
    ostringstream s;
    vector<InterfacePtr> &is = pPtr->getAllInterfacePtr();
    vector<StructPtr> &ss = pPtr->getAllStructPtr();
    vector<EnumPtr> &es = pPtr->getAllEnumPtr();
    vector<ConstPtr> &cs = pPtr->getAllConstPtr();

    std::set<std::string> ns;
    getAllNamespaceRef(pPtr, ns);
    k_current_namespace = pPtr->getId();
    k_tafgo_package_prefix = (pPtr->getId() != "tafgo") ? "tars." : "";
    s << endl;
    s << "package " << pPtr->getId() << endl << endl;
    s << "import(" << endl;
    INC_TAB;
    if(is.size() > 0){
        s << TAB << "\"time\"" << endl;
    }
    s << TAB << "\"bytes\"" << endl << endl;
    std::set<std::string>::iterator ns_it = ns.begin();
    while (ns_it != ns.end())
    {
        const std::string& nn = *ns_it;
        if (nn != k_current_namespace)
        {
            s << TAB << "\"" << nn << "\"" << endl;
        }
        ns_it++;
    }
    if (!k_tafgo_package_prefix.empty())
    {
        s << TAB << "tars \"github.com/yinqiwen/tafgo\" " << endl;
    }

    DEL_TAB;
    s << ")" << endl;
    //s << TAB << "{" << endl;
    //INC_TAB;
    if (cs.size() > 0 || es.size() > 0)
    {
        s << TAB << "const (" << endl;
        INC_TAB;
    }
    if (cs.size() > 0)
    {
        for (size_t i = 0; i < cs.size(); i++)
        {
            s << generateGo(cs[i]) << endl;
        }
    }

    for (size_t i = 0; i < es.size(); i++)
    {
        s << generateGo(es[i]) << endl;
    }

    if (cs.size() > 0 || es.size() > 0)
    {
        DEL_TAB;
        s << TAB << ")" << endl;
    }

    for (size_t i = 0; i < ss.size(); i++)
    {
        s << generateGo(ss[i], pPtr->getId()) << endl;
    }

    s << endl;

    for (size_t i = 0; i < is.size() && m_bOnlyStruct == false; i++)
    {
        s << generateGo(is[i]) << endl;
        s << endl;
    }

    DEL_TAB;
    //s << "}";

    s << endl << endl;

    return s.str();
}

void Tars2Go::generateGo(const ContextPtr &pPtr) const
{
    string n = tars::TC_File::excludeFileExt(tars::TC_File::extractFileName(pPtr->getFileName()));

    vector<NamespacePtr> namespaces = pPtr->getNamespaces();
    //string define = taf::TC_Common::upper("__" + n + "_h_");
    ostringstream s;

    s << g_parse->printHeaderRemark();
    for (size_t i = 0; i < namespaces.size(); i++)
    {
        s << generateGo(namespaces[i]) << endl;
        string fileGo = m_sBaseDir + "/" + namespaces[i]->getId() + "/" + n + ".go";
        tars::TC_File::makeDirRecursive(m_sBaseDir + "/" + namespaces[i]->getId(), 0755);
        tars::TC_File::save2file(fileGo, s.str());
        system(("gofmt  -w " + fileGo).c_str());
    }

    //pPtr->getIncludes();
}

void Tars2Go::createFile(const string &file, const vector<string> &vsCoder)
{
    std::vector<ContextPtr> contexts = g_parse->getContexts();
    for (size_t i = 0; i < contexts.size(); i++)
    {
        if (file == contexts[i]->getFileName())
        {
            if (vsCoder.size() == 0)
            {
                generateGo(contexts[i]);

                if (m_bOnlyStruct == false)
                {
                    //generateCpp(contexts[i]);
                }
            }
            else
            {
                for (size_t j = 0; j < vsCoder.size(); j++)
                {
                    //generateCoder(contexts[i], vsCoder[j]);
                }
            }
        }
    }
}

StructPtr Tars2Go::findStruct(const ContextPtr &pPtr, const string &id)
{
    string sid = id;
    vector<NamespacePtr> namespaces = pPtr->getNamespaces();
    for (size_t i = 0; i < namespaces.size(); i++)
    {
        NamespacePtr np = namespaces[i];
        vector<StructPtr> structs = np->getAllStructPtr();

        for (size_t i = 0; i < structs.size(); i++)
        {
            if (structs[i]->getSid() == sid)
            {
                return structs[i];
            }
        }
    }

    return NULL;
}

void Tars2Go::getAllNamespaceRef(const NamespacePtr& pPtr, std::set<std::string>& s) const
{
    vector<InterfacePtr> &is = pPtr->getAllInterfacePtr();
    vector<StructPtr> &ss = pPtr->getAllStructPtr();

    for (size_t i = 0; i < ss.size(); i++)
    {
        vector<TypeIdPtr>& members = ss[i]->getAllMemberPtr();
        for (size_t j = 0; j < members.size(); j++)
        {
            StructPtr sPtr = StructPtr::dynamicCast(members[j]->getTypePtr());
            if (sPtr)
            {
                s.insert(getStructNameSapce(sPtr));
            }
        }
    }

    for (size_t i = 0; i < is.size(); i++)
    {
        vector<OperationPtr>& vOperation = is[i]->getAllOperationPtr();
        for (size_t j = 0; j < vOperation.size(); j++)
        {
            vector<ParamDeclPtr>& vParamDecl = vOperation[j]->getAllParamDeclPtr();

            for (size_t k = 0; k < vParamDecl.size(); k++)
            {
                StructPtr sPtr = StructPtr::dynamicCast(vParamDecl[k]->getTypeIdPtr()->getTypePtr());
                if (sPtr)
                {
                    s.insert(getStructNameSapce(sPtr));
                }
            }
        }
    }
}

