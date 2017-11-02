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

string Tars2Node::makeName()
{
    int iHigh = uiNameIndex/26;
    int iLow  = uiNameIndex%26;

    uiNameIndex++;

    if (iHigh != 0)
    {
        return "_TARS_MODULE_" + string(1, (char)(iHigh + 65)) + string(1, (char)(iLow + 65)) + "_"; 
    }

    return "_TARS_MODULE_" + string(1, (char)(iLow + 65)) + "_";
}

string Tars2Node::findName(const std::string & sNamespace, const std::string & sName) 
{
    #ifdef DUMP_FIND_NAME
    cout << "FINDNAME BEGIN:" << sNamespace << "|" << sName << endl;
    #endif

    for (map<string, ImportFile>::iterator it = _mapFiles.begin(); it != _mapFiles.end(); it++)
    {
        #ifdef DUMP_FIND_NAME
        for (map<string, ImportFileType>::iterator demo = it->second.mapVars.begin(); demo != it->second.mapVars.end(); demo++)
        {
            cout << "FINDNAME:" << it->second.sModule << "|" << demo->first << "|" << demo->second.sNamespace << "|" << demo->second.sName << endl;
        }
        #endif

        std::map<string, ImportFileType>::iterator inIter = it->second.mapVars.find(sNamespace + "::" + sName);
        if (inIter == it->second.mapVars.end())
        {
            continue;
        }

        if (inIter->second.iType == ImportFileType::EN_ENUM)
        {
            return it->second.sModule + (it->second.sModule.empty()?"":".") + inIter->second.sNamespace + "." + inIter->second.sName;
        }

        if (inIter->second.iType == ImportFileType::EN_ENUM_VALUE)
        {
            return it->second.sModule + (it->second.sModule.empty()?"":".") + inIter->second.sNamespace + "." + inIter->second.sTypeName + "." + inIter->second.sName;
        }

        if (inIter->second.iType == ImportFileType::EN_STRUCT)
        {
            return it->second.sModule + (it->second.sModule.empty()?"":".") + inIter->second.sNamespace + "." + inIter->second.sName;
        }

        return it->second.sModule;
    }

    return "";
}

void Tars2Node::scan(const std::string & sFile, bool bNotPrefix)
{
    if (_mapFiles.find(sFile) != _mapFiles.end())
    {
        return ;
    }

    std::string sTarsFile = getRealFileInfo(sFile);
    g_parse->parse(sTarsFile);

    std::vector<ContextPtr> contexts = g_parse->getContexts();

	for(size_t i = 0; i < contexts.size(); i++)
	{
		if (sTarsFile == contexts[i]->getFileName())
		{
            ImportFile item;
            item.sFile   = "./" + tars::TC_File::excludeFileExt(tars::TC_File::extractFileName(sFile)) + "Tars.js";
            item.sModule = bNotPrefix?"":makeName();

            vector<NamespacePtr> namespaces = contexts[i]->getNamespaces();
            for (size_t ii = 0; ii < namespaces.size(); ii++)
            {
                std::string sNamespace = namespaces[ii]->getId();

                vector<StructPtr> & ss = namespaces[ii]->getAllStructPtr();
                for (size_t iii = 0; iii < ss.size(); iii++)
                {
                    vector<string> vecNames = tars::TC_Common::sepstr<string>(ss[iii]->getSid(), "::");

                    ImportFileType temp;
                    temp.iType      = ImportFileType::EN_STRUCT;
                    temp.sNamespace = sNamespace;
                    temp.sName      = vecNames[1];

                    item.mapVars.insert(make_pair(temp.sNamespace + "::" + temp.sName, temp));
                }

                vector<EnumPtr> & es = namespaces[ii]->getAllEnumPtr();
                for (size_t iii = 0; iii < es.size(); iii++)
                {
                    vector<string> vecNames = tars::TC_Common::sepstr<string>(es[iii]->getSid(), "::");

                    ImportFileType temp;
                    temp.iType      = ImportFileType::EN_ENUM;
                    temp.sNamespace = sNamespace;
                    temp.sName      = vecNames[1];

                    item.mapVars.insert(make_pair(temp.sNamespace + "::" + temp.sName, temp));

                    vector<TypeIdPtr> & eMember = es[iii]->getAllMemberPtr();
                    for (size_t iiii = 0; iiii < eMember.size(); iiii++)
                    {
                        ImportFileType temp;
                        temp.iType      = ImportFileType::EN_ENUM_VALUE;
                        temp.sNamespace = sNamespace;
                        temp.sTypeName  = vecNames[1];
                        temp.sName      = eMember[iiii]->getId();

                        #ifdef DUMP_FIND_NAME
                        cout << "GET::::" << temp.sNamespace << "|" << temp.sName << endl;
                        #endif

                        item.mapVars.insert(make_pair(temp.sNamespace + "::" + temp.sName, temp));
                    }
                }
            }

            _mapFiles.insert(make_pair(sFile, item));

            vector<string> vecFiles = contexts[i]->getIncludes();
            for (size_t ii = 0; ii < vecFiles.size(); ii++)
            {
                std::string sFileName = tars::TC_File::extractFilePath(vecFiles[ii]) + tars::TC_File::excludeFileExt(tars::TC_File::extractFileName(vecFiles[ii])) + ".tars";

                scan(sFileName, false);
            }
		}
	}
}

