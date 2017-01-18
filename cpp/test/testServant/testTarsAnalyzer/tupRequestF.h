/**
 * Tencent is pleased to support the open source community by making Tars available.
 *
 * Copyright (C) 2016 THL A29 Limited, a Tencent company. All rights reserved.
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
#ifndef _TUPREQUEST_H
#define _TUPREQUEST_H

string g_sTupPacket = "module Ttars\n"
"{\n"
"    struct RequestPacket\n"
"    {\n"
"        1  require short        iVersion;\n"
"       2  require byte         cPacketType  = 0;\n"
"        3  require int          iMessageType = 0;\n"
"        4  require int          iRequestId;\n"
"        5  require string       sServantName = \"\";\n"
"        6  require string       sFuncName    = \"\";\n"
"        7  require vector<byte> sBuffer;\n"
"        8  require int          iTimeout     = 0;\n"
"        9  require map<string, string> context;\n"
"        10 require map<string, string> status; \n"
"    };\n"

"    struct ResponsePacket\n"
"    {\n"
"        1 require short         iVersion;\n"
"        2 require byte          cPacketType  = 0;\n"
"        3 require int           iRequestId;\n"
"       4 require int           iMessageType = 0;\n"
"       5 require int           iRet         = 0;\n"
"        6 require vector<byte>  sBuffer;\n"
"        7 require map<string, string> status; \n"
"        8 optional string        sResultDesc; \n"
"    };\n"
"};\n" ;


#endif

