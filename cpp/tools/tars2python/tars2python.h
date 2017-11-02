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

#ifndef _TARS2PYTHON_H
#define _TARS2PYTHON_H

#include "parse.h"

#include <cassert>
#include <set>
#include <string>

using namespace tars;

#if 0
#include "util/tc_md5.h"
#include "util/tc_file.h"
#include "util/tc_common.h"
#endif

class Tars2Python
{
public:
    void setBaseDir(const string &dir)
    {
        _baseDir = dir;
    }

    void setBasePackage(const string &prefix)
    {
        _packagePrefix = prefix;
        if (_packagePrefix.length() != 0 && _packagePrefix.substr(_packagePrefix.length()-1, 1) != ".")
        {
            _packagePrefix += ".";
        }
    }

	void createFile(const string &file);

private:
	string generatePython(const ConstPtr &pPtr, const std::string &sNamespace);

	string generatePython(const EnumPtr &pPtr, const std::string &sNamespace);

	string generatePython(const StructPtr & pPtr, const NamespacePtr &nPtr);

	string generatePython(const NamespacePtr &pPtr);

	string generatePython(const InterfacePtr &pPtr);

	string toFunctionName(const TypeIdPtr & pPtr, const string & sName);

	string getDataType(const TypePtr & pPtr);

	string getDefault(const TypeIdPtr & pPtr, const std::string &sDefault, const std::string & sCurStruct);

	string makeDataType(const TypePtr & pPtr);

	string getFilePath() const;

	void getNamespace(const string & sFile, set<string> & vec);

	void makePackages();

	void generatePython(const ContextPtr &pPtr) ;

	string makeParams(const OperationPtr &pPtr);

	string makeOperations(const OperationPtr &pPtr);
       string tostrEnum(const EnumPtr &pPtr);
private:
    string _packagePrefix;
	string _baseDir;
};

#endif

