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


#ifndef _TARS2NODE_H_
#define _TARS2NODE_H_

#include "parse.h"

#include "util/tc_md5.h"
#include "util/tc_file.h"
#include "util/tc_common.h"
#include "util/tc_encoder.h"

#include <set>
#include <cstring>

class Tars2Node
{
public:
    Tars2Node();

    void createFile(const string &file);

    void setTarsLibPath(const string & sPath) { _sTarsLibPath = sPath; }

    void setTarsStreamPath(const string & sPath) { _sTarsStreamPath = sPath; }

    void setEnableClient(bool bEnable) { _bClient = bEnable; }

    void setEnableServer(bool bEnable) { _bServer = bEnable; }

    void setTargetPath(const string & sPath) { _sToPath = sPath + "/"; }

    void setRecursive(bool bEnable) { _bRecursive = bEnable; }

    void setUseSpecialPath(bool bEnable) { _bUseSpecialPath = bEnable; }

private:
    struct ImportFileType
    {
        enum TYPE_T {EN_ENUM = 10000, EN_ENUM_VALUE, EN_STRUCT};
        int         iType;
        std::string sNamespace;
        std::string sTypeName;
        std::string sName;
    };

    struct ImportFile 
    {
        std::string sFile;
        std::string sModule;                        //Module模块的名称
        std::map<string, ImportFileType> mapVars;
    };

    uint32_t uiNameIndex;

    map<string, ImportFile> _mapFiles;

    void   scan(const std::string & sFile, bool bNotPrefix);

    string makeName();

    string findName(const std::string & sNamespace, const std::string & sName);

private:
    string toFunctionName(const TypeIdPtr & pPtr, const std::string &sAction);

    string getDataType(const TypePtr & pPtr);

	string getDefault(const TypeIdPtr & pPtr, const std::string &sDefault, const std::string & sNamespace, const bool bGlobal = true);

    string generateJS(const StructPtr & pPtr, const NamespacePtr &sNamespace);

    string generateJS(const ConstPtr &pPtr, const std::string &sNamespace);

	string generateJS(const EnumPtr &pPtr, const std::string &sNamespace);

    string generateJS(const NamespacePtr &pPtr);

    void   generateJS(const ContextPtr &pPtr);

private:
    void   generateJSProxy(const ContextPtr &pPtr);

    string generateJSProxy(const NamespacePtr &nPtr);

    string generateJSProxy(const NamespacePtr &nPtr, const InterfacePtr &pPtr);

    string generateJSProxy(const NamespacePtr &nPtr, const InterfacePtr &pPtr, const OperationPtr &oPtr);

private:
    void   generateJSServer(const ContextPtr &pPtr);

    string generateJSServer(const NamespacePtr &pPtr);

    string generateAsync(const NamespacePtr &nPtr, const InterfacePtr &pPtr, const OperationPtr &oPtr);

    string generateDispatch(const NamespacePtr &nPtr, const InterfacePtr &pPtr, const OperationPtr &oPtr);

    string generateJSServer(const NamespacePtr &nPtr, const InterfacePtr &pPtr, const OperationPtr &oPtr); //生成函数接口

    string generateJSServer(const InterfacePtr &pPtr, const NamespacePtr &nPtr); //生成

private:
    string generateJSServerImp(const NamespacePtr &nPtr, const InterfacePtr &pPtr, const OperationPtr &oPtr);

    string generateJSServerImp(const NamespacePtr &nPtr, const InterfacePtr &pPtr);

    string generateJSServerImp(const ContextPtr &cPtr, const NamespacePtr &nPtr);

    void   generateJSServerImp(const ContextPtr &cPtr);

private:
    void   makeUTF8File(const string & sFileName, const string & sFileContent);

    bool   isSimple(const TypePtr & pPtr) const;

    bool   isBinBuffer(const TypePtr & pPtr) const;

    string getRealFileInfo(const std::string & sPath);

private:
    string _sTarsLibPath;

    string _sTarsStreamPath;

    string _sToPath;

    bool   _bClient;

    bool   _bServer;

    bool   _bRecursive;

    bool   _bUseSpecialPath;
};

#endif

