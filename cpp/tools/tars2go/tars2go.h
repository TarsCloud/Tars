#ifndef _TARS2GO_H
#define _TARS2GO_H

#include "parse.h"


#include <cassert>
#include <string>
#include <set>

using namespace tars;

class Tars2Go
{
    public:

        Tars2Go();

        void createFile(const string &file, const vector<string> &vsCoder);

        void setBaseDir(const std::string & sPath)
        {
            m_sBaseDir = sPath;
        }

        void setPDU(bool bPDU)
        {
            m_bPDU = bPDU;
        }

        void setCheckDefault(bool bCheck)
        {
            m_bCheckDefault = bCheck;
        }

        void setOnlyStruct(bool bOnlyStruct)
        {
            m_bOnlyStruct = bOnlyStruct;
        }

        void setTafMSF(std::string sTafMSF)
        {
            m_sNamespace = sTafMSF;
            m_bTafMSF = ("tafMSF" == sTafMSF);
        }

        void setUnknownField(bool bUnkownField)
        {
            m_bUnknownField = bUnkownField;
        }

        void setTafMaster(bool bTafMaster)
        {
            m_bTafMaster = bTafMaster;
        }

    protected:

        string readUnknown(const TypeIdPtr &pPtr) const;

        string writeUnknown() const;

    protected:

        string tostr(const TypePtr &pPtr) const;

        string tostrBuiltin(const BuiltinPtr &pPtr) const;

        string tostrVector(const VectorPtr &pPtr) const;

        string tostrMap(const MapPtr &pPtr) const;

        string tostrStruct(const StructPtr &pPtr) const;

        string tostrEnum(const EnumPtr &pPtr) const;

        string toStrSuffix(const TypeIdPtr &pPtr) const;

        int getSuffix(const TypeIdPtr &pPtr) const;

        string decode(const TypeIdPtr &pPtr) const;

        string encode(const TypeIdPtr &pPtr) const;

    protected:

        string MD5(const StructPtr &pPtr) const;

        string generateGo(const ParamDeclPtr &pPtr) const;
        string generateGo(const OperationPtr &pPtr, bool bVirtual, const string& interfaceId) const;
        string generateGo(const InterfacePtr& pPtr) const;
        string generateGo(const StructPtr &pPtr, const string& namespaceId) const;
        string generateGo(const EnumPtr &pPtr) const;
        string generateGo(const ConstPtr &pPtr) const;
        string generateGo(const NamespacePtr &pPtr) const;
        void generateGo(const ContextPtr &pPtr) const;
        string generateEncodeInvocation(TypeIdPtr& t, const std::string& bufstr, const std::string& id_prefix = "",
                bool first_upper = false) const;
        string generateDecodeInvocation(TypeIdPtr& t, const std::string& bufstr, const std::string& id_prefix,
                bool first_upper, bool is_pointer) const;

        void getAllNamespaceRef(const NamespacePtr& ns, std::set<std::string>& s) const;

        StructPtr findStruct(const ContextPtr &pPtr, const string &id);

        string generateInitValue(const TypeIdPtr &pPtr) const;
    private:
        std::string m_sBaseDir;

        bool m_bPDU;
        bool m_bCheckDefault;

        bool m_bOnlyStruct;
        bool m_bTafMSF;
        std::string m_sNamespace;
        bool m_bUnknownField;
        bool m_bTafMaster;
};

#endif

