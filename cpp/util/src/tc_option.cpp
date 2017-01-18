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

#include "util/tc_option.h"
#include "util/tc_common.h"

namespace tars
{

void TC_Option::decode(int argc, char *argv[])
{
    _mParam.clear();

    vector<string> v;
    for(int i = 1; i < argc; i++)
    {
        v.push_back(argv[i]);
    }

    for(size_t i = 0; i < v.size(); i++)
    {
        if(v[i].length() > 2 && v[i].substr(0,2) == "--")
        {
            parse(v[i]);
        }
        else
        {
            _vSingle.push_back(v[i]);
        }
    }
}

void TC_Option::parse(const string &s)
{
    string::size_type pos = s.find('=');
    if( pos != string::npos)
    {
        _mParam[s.substr(2, pos-2)] = s.substr(pos+1);
    }
    else
    {
        _mParam[s.substr(2, pos-2)] = "";
    }
}

string TC_Option::getValue(const string &sName)
{
    if(_mParam.find(sName) != _mParam.end())
    {
        return _mParam[sName];
    }
    return "";
}

bool TC_Option::hasParam(const string &sName)
{
    return _mParam.find(sName) != _mParam.end();
}

vector<string>& TC_Option::getSingle()
{
    return _vSingle;
}

map<string, string>& TC_Option::getMulti()
{
    return _mParam;
}

}


