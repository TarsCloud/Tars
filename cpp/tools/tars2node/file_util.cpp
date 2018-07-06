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

#include "code_generator.h"

void CodeGenerator::makeUTF8File(const string& sFileName, const string& sFileContent) 
{
    char header[3] = {(char)(0xef), (char)(0xbb), (char)(0xbf)};

    string sData(header, 3);

    sData += TC_Encoder::gbk2utf8(sFileContent);

    TC_File::save2file(sFileName, sData.c_str());
}

string CodeGenerator::getRealFileInfo(const string& sPath) 
{
    return TC_File::extractFilePath(sPath) + TC_File::excludeFileExt(TC_File::extractFileName(sPath)) + "." + TC_Common::lower(IDL_TYPE);
}