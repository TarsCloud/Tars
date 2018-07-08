// Generates C++ tars service interface out of Protobuf IDL.
//
// This is a Proto2 compiler plugin.  See net/proto2/compiler/proto/plugin.proto
// and net/proto2/compiler/public/plugin.h for more information on plugins.

#include <cassert>
#include "CppPbUtils.h"


std::string ProtoFileBaseName(const std::string& fullName) {
    std::size_t p = fullName.rfind(".");
    assert (p != std::string::npos);

    return fullName.substr(0, p);
}

const std::string kDeclaration = "/**\n" 
              " * Tencent is pleased to support the open source community by making Tars available.\n"
              " * Copyright (C) 2016 THL A29 Limited, a Tencent company. All rights reserved.\n"
              " * Licensed under the BSD 3-Clause License (the \"License\"); you may not use this file\n"
              " * except in compliance with the License. You may obtain a copy of the License at\n"
              " * https://opensource.org/licenses/BSD-3-Clause\n"
              " * Unless required by applicable law or agreed to in writing, software distributed\n"
              " * under the License is distributed on an \"AS IS\" BASIS, WITHOUT WARRANTIES OR\n"
              " * CONDITIONS OF ANY KIND, either express or implied. See the License for the\n"
              " * specific language governing permissions and limitations under the License.\n" 
              " */\n\n";

const std::string kIndent = "    "; 

std::string LineFeed(int indent) {
    std::string data("\n");

    for (int i = 0; i < indent; ++i)
        data += kIndent;

    return data;
}

