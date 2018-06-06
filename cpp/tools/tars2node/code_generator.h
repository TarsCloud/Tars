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

#ifndef _CODEGENERATOR_H
#define _CODEGENERATOR_H

#include "parse.h"
#include "util/tc_file.h"
#include "util/tc_encoder.h"
#include <set>

#ifndef TAB
    #define TAB     g_parse->getTab()
#endif

#ifndef INC_TAB
    #define INC_TAB g_parse->incTab()
#endif

#ifndef DEL_TAB
    #define DEL_TAB g_parse->delTab()
#endif

#define DEFINE_STRING(str) string(CSTR(str))
#define CSTR(str) #str
#define IDL_NAMESPACE_STR DEFINE_STRING(IDL_NAMESPACE)

#define GET_CONST_GRAMMAR_PTR_V(name, ptr) \
    ptr->getConst##name##Ptr()
#define GET_CONST_GRAMMAR_PTR_BASE(name, ptr) \
    GET_CONST_GRAMMAR_PTR_V(name, ptr)
#define GET_CONST_GRAMMAR_PTR(ptr) \
    GET_CONST_GRAMMAR_PTR_BASE(GRAMMAR_NAME, ptr)

#define CONST_GRAMMAR_V(name, val) \
    Const##name::val
#define CONST_GRAMMAR_BASE(name, val) \
    CONST_GRAMMAR_V(name, val)
#define CONST_GRAMMAR(val) \
    CONST_GRAMMAR_BASE(GRAMMAR_NAME, val)

using namespace TC_NAMESPACE;

class CodeGenerator
{
public:
    CodeGenerator()
        : uiNameIndex(0),
          _sRpcPath(RPC_MODULE_PATH),
          _sStreamPath(STREAM_MODULE_PATH),
          _sToPath("./"),
          _bClient(false),
          _bServer(false),
          _bRecursive(false),
          _bUseSpecialPath(false),
          _bUseStringRepresent(false),
          _bStringBinaryEncoding(false),
          _bMinimalMembers(false),
          _bDTS(false) {}

    void createFile(const string &file, const bool bEntry = true);

    void setRpcPath(const string & sPath) { _sRpcPath = sPath; }

    void setStreamPath(const string & sPath) { _sStreamPath = sPath; }

    void setEnableClient(bool bEnable) { _bClient = bEnable; }

    void setEnableServer(bool bEnable) { _bServer = bEnable; }

    void setTargetPath(const string & sPath) { _sToPath = sPath + "/"; }

    void setRecursive(bool bEnable) { _bRecursive = bEnable; }

    void setUseSpecialPath(bool bEnable) { _bUseSpecialPath = bEnable; }

    void setUseStringRepresent(bool bEnable) { _bUseStringRepresent = bEnable; }

    void setStringBinaryEncoding(bool bEnable) { _bStringBinaryEncoding = bEnable; }

    void setMinimalMembers(bool bEnable) { _bMinimalMembers = bEnable; }

    void setDependent(set<string> & deps) { _depMembers = deps; }

    void setEnableDTS(bool bEnable) { _bDTS = bEnable; }

private:
    struct ImportFileType
    {
        enum TYPE_T {EN_ENUM = 10000, EN_ENUM_VALUE, EN_STRUCT};
        int         iType;
        string      sNamespace;
        string      sTypeName;
        string      sName;
    };

    struct ImportFile 
    {
        string                      sFile;
        string                      sModule;
        map<string, ImportFileType> mapVars;
    };

    uint32_t uiNameIndex;

    map<string, ImportFile> _mapFiles;

    set<string> _depMembers;

    void   scan(const string & sFile, bool bNotPrefix);

    string makeName();

    string findName(const string & sNamespace, const string & sName);

private:
    string toFunctionName(const TypeIdPtr & pPtr, const string &sAction);

    string getDataType(const TypePtr & pPtr);

    string getDtsType(const TypePtr &pPtr, const bool bStream = true);

    string getDefault(const TypeIdPtr & pPtr, const string &sDefault, const string & sNamespace);

    string getDefault(const TypeIdPtr & pPtr, const string &sDefault, const string & sNamespace, const bool bGlobal);

    string generateJS(const StructPtr & pPtr, const string &sNamespace, bool &bNeedAssert);

    string generateJS(const ConstPtr &pPtr, const string &sNamespace, bool &bNeedStream);

    string generateJS(const EnumPtr &pPtr, const string &sNamespace);

    string generateJS(const NamespacePtr &pPtr, bool &bNeedStream, bool &bNeedAssert);

    bool   generateJS(const ContextPtr &pPtr);

private:
    string generateJSProxy(const NamespacePtr &nPtr, bool &bNeedRpc, bool &bNeedStream);

    string generateJSProxy(const NamespacePtr &nPtr, const InterfacePtr &pPtr);

    string generateJSProxy(const NamespacePtr &nPtr, const InterfacePtr &pPtr, const OperationPtr &oPtr);

    bool   generateJSProxy(const ContextPtr &pPtr);

private:
    string generateJSServer(const NamespacePtr &pPtr, bool &bNeedStream, bool &bNeedRpc, bool &bNeedAssert);

    string generatePing(const NamespacePtr &nPtr, const InterfacePtr &pPtr);

    string generateAsync(const NamespacePtr &nPtr, const InterfacePtr &pPtr, const OperationPtr &oPtr);

    string generateDispatch(const NamespacePtr &nPtr, const InterfacePtr &pPtr, const OperationPtr &oPtr);

    string generateJSServer(const NamespacePtr &nPtr, const InterfacePtr &pPtr, const OperationPtr &oPtr);

    string generateJSServer(const InterfacePtr &pPtr, const NamespacePtr &nPtr);

    bool   generateJSServer(const ContextPtr &pPtr);

private:
    string generateJSServerImp(const NamespacePtr &nPtr, const InterfacePtr &pPtr, const OperationPtr &oPtr);

    string generateJSServerImp(const NamespacePtr &nPtr, const InterfacePtr &pPtr);

    string generateJSServerImp(const ContextPtr &cPtr, const NamespacePtr &nPtr);

    void   generateJSServerImp(const ContextPtr &cPtr);

private:
    string generateDTS(const StructPtr &pPtr, const string &sNamespace);

    string generateDTS(const ConstPtr &pPtr, const string &sNamespace, bool &bNeedStream);

    string generateDTS(const EnumPtr &pPtr, const string &sNamespace);

    string generateDTS(const NamespacePtr &pPtr, bool &bNeedStream);

    string generateDTS(const NamespacePtr &pPtr, const string &sContent);

    void   generateDTS(const ContextPtr &cPtr);

private:
    string generateDTSServer(const NamespacePtr &pPtr, bool &bNeedStream, bool &bNeedRpc);

    string generateDTSServer(const NamespacePtr &nPtr, const InterfacePtr &pPtr);

    void   generateDTSServer(const ContextPtr &cPtr);

private:
    string generateDTSProxy(const InterfacePtr &pPtr);

    string generateDTSProxy(const NamespacePtr &pPtr, bool &bNeedStream, bool &bNeedRpc);

    void   generateDTSProxy(const ContextPtr &pPtr);

private:
    void   makeUTF8File(const string & sFileName, const string & sFileContent);

    bool   isSimple(const TypePtr & pPtr) const;

    bool   isBinBuffer(const TypePtr & pPtr) const;

    bool   isRawOrString(const TypePtr & pPtr) const;

    bool   isDependent(const string & sNamespace, const string & sName) const;

    string getRealFileInfo(const string & sPath);

    string printHeaderRemark(const string & sTypeName);

private:
    string _sRpcPath;

    string _sStreamPath;

    string _sToPath;

    bool   _bClient;

    bool   _bServer;

    bool   _bRecursive;

    bool   _bUseSpecialPath;

    bool   _bUseStringRepresent;

    bool   _bStringBinaryEncoding;

    bool   _bMinimalMembers;

    bool   _bEntry;

    string _sIdlFile;

    bool   _bDTS;
};

#endif