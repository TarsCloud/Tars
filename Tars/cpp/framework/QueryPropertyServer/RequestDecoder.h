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

#ifndef __REQUEST_DECODE_H__
#define __REQUEST_DECODE_H__
#include <iostream>
#include "util/tc_thread.h"
#include "util/tc_mysql.h"
#include "servant/StatF.h"
#include "util/tc_common.h"
#include "servant/TarsLogger.h"
#include "util/tc_thread.h"
#include "../../thirdparty/rapidjson/include/rapidjson/document.h"
#include "../../thirdparty/rapidjson/include/rapidjson/writer.h"
#include "../../thirdparty/rapidjson/include/rapidjson/stringbuffer.h"

using namespace rapidjson;

class RequestDecoder
{
public:
    static string getLine(const char** ppChar);
public:
    enum Op
    {
        SET,
        ADD,
        SUB,
        EQ,
        NE,
        GT,
        LT,
        LE,
        GE,
        LIMIT,
    };

    enum METHOD
    {
        QUERY = 0,
        TIMECHECK,
    };
    RequestDecoder(const string& input);

    ~RequestDecoder();

    int parseCondition(const string& sCond, string& value);

    /**
     * 从vCond中解析出field对应的值
     *
     */
    int find_field(vector<string>& vCond, const string& field /*f_tflag, f_date*/, string& value );

    int decode();

    int decodeDataid();

    int decodeMethod();

    int decodeArray(const string& arr, vector<string> &vFields);

    int addUid(const string& sUid);

    map<string, string>& getSql();

    int composeSqlPartition();

    string getQuerySlaveName() { return _slaveName; }

    int  generateVector(Document &d ,const string s,std::vector<string> &v);

private:
    //待解析的json请求串
    string _input;

    //解析json得到的数据库查询slave_name
    string _slaveName;

    //解析json请求串得到的Dom 文件
    Document _req;

    //解析json得到的数据库查询条件值
    map<string, string> _sql;
};
#endif

