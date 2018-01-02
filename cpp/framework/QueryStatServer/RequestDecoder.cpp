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

#include "DbProxy.h"
#include "util/tc_config.h"
#include "QueryServer.h"
#include "RequestDecoder.h"

static string skey = "";



RequestDecoder::RequestDecoder(const string& input)
: _input(input)
, _slaveName("")
{
}
RequestDecoder::~RequestDecoder()
{
    
}

int  RequestDecoder::generateVector(Document &d ,const string s,std::vector<string> &v)
{   

     Value::ConstMemberIterator iter = d.FindMember(s.c_str());
     int i=0;
    if (iter == d.MemberEnd())
    {
        return 0;
    }
    else 
    {
        if(iter->value.IsArray())
        {
            for (Value::ConstValueIterator itr = iter->value.Begin(); itr != iter->value.End(); ++itr)
            {
                 v.push_back(itr->GetString());
                 ++i;
            }
        }

        else if  (iter->value.IsString())
        {
            v.push_back(iter->value.GetString());
            i=1;
        }

        else
        {
            return 0;
        }

        return i;
    }

}
int RequestDecoder::composeSqlPartition()
{
    vector<string> vConditions;
    int iRet=0;
    
     iRet=generateVector(_req,"filter",vConditions);

    if(iRet==0)
    {
        return -1;
    }


    string date1,date2;

    int ret = find_field(vConditions, "f_date", date1);
    date1 = TC_Common::trim(date1, " ''");
    date1 = TC_Common::trim(date1, " ");
    if (ret == EQ || ret ==-1)
    {
        
        date2 = date1;
    }
    else //NOT EQ, find  again
    {
        ret = find_field(vConditions, "f_date", date2);
        date2 = TC_Common::trim(date2, " ''");
        date2 = TC_Common::trim(date2, " ");
        if (date1 < date2)
        {
            
        }
        else
        {
            date1.swap(date2);
        }
    }

    _sql["date1"] = date1;
    _sql["date2"] = date2;
    //get date end

    //get hour
    string tflag1,tflag2;
    ret = find_field(vConditions, "f_tflag", tflag1);
    tflag1 = TC_Common::trim(tflag1, " \"'");
    if (ret == EQ || ret == -1)
    {
        
        tflag2 = tflag1;
    }
    else //NOT EQ, find  again
    {
        ret = find_field(vConditions, "f_tflag", tflag2);
        tflag2 = TC_Common::trim(tflag2, " \"'");
        if (tflag1 < tflag2)
        {
            
        }
        else
        {
            tflag1.swap(tflag2);
        }
    }

    _sql["tflag1"] = tflag1;
    _sql["tflag2"] = tflag2;
    //get hour end
    
    iRet=generateVector(_req,"filter",vConditions);
    if (!vConditions.empty())
    {
        string whereCond = "";
        vector<string>::iterator it = vConditions.begin();
        while(it != vConditions.end())
        {
            string &sTmp = *it;
            string::size_type pos = sTmp.find("slave_name");
            if(pos != string::npos)
            {
                _slaveName = *it;
            }

            whereCond += (whereCond.empty()?"":" and ") + *it ;
             it++;
        }

        string sWhere("");
        string::size_type ipos = whereCond.find("and istars=");

        if(ipos != string::npos)
        {

            string spre = whereCond.substr(0, ipos);

            string snext = whereCond.substr(ipos+9);

            string::size_type ipos1 = snext.find("and");

            if(ipos1 != string::npos)
            {
                string s = snext.substr(ipos1);
                sWhere = spre;
                sWhere += s;

                string sistars = whereCond.substr(ipos+4, (ipos + 9) + ipos1 - (ipos + 4));

                string value("");

                int flag = parseCondition(sistars, value);

                TLOGDEBUG("RequestDecoder::composeSqlPartition istars set|sistars:" << sistars << "|flag:" << flag << "|ipos:" << ipos << "|ipos1:" << ipos1 << "|snext:" << snext << endl);

                if(flag == EQ)
                {
                    value = TC_Common::trim(value, " \"'");
                    _sql["istars"] = value;
                }
                else if(flag == NE)
                {
                    value = TC_Common::trim(value, " \"'");
                    int iFlag = TC_Common::strto<int>(value);
                    iFlag = !iFlag;
                    value = TC_Common::tostr(iFlag);
                    _sql["istars"] = value;
                }
            }
            else
            {
                sWhere = spre;

                string sistars = whereCond.substr(ipos+4);

                string value("");

                int flag = parseCondition(sistars, value);

                if(flag == EQ)
                {
                    value = TC_Common::trim(value, " \"'");
                    _sql["istars"] = value;
                }
                else if(flag == NE)
                {
                    value = TC_Common::trim(value, " \"'");
                    int iFlag = TC_Common::strto<int>(value);
                    iFlag = !iFlag;
                    value = TC_Common::tostr(iFlag);
                    _sql["istars"] = value;
                }
            }
        }
        else
        {
            sWhere = whereCond;
        }

        _sql["whereCond"] = " where " + sWhere;

        
    }

    if(_slaveName != "")
    {
        string::size_type pos1 = _slaveName.find("'");
        if(pos1 != string::npos)
        {
            string::size_type pos2 = _slaveName.rfind("'");
            if(pos2 != pos1 && pos2 != string::npos)
            {
                _slaveName = _slaveName.substr(pos1, (pos2 - pos1 -1));
            }
        }
    }



    vConditions.clear();
    iRet=generateVector(_req,"groupby",vConditions);

    if (!vConditions.empty())
    {
        string groupCond = "";
        string groupField = "";
        vector<string>::iterator it = vConditions.begin();
        while(it != vConditions.end())
        {
            groupCond += (groupCond.empty()?"":", ") + *it;
            groupField += (groupField.empty()?"":", ") + *it;
            it++;
        }
        _sql["groupCond"] = " group by " + groupCond;
        _sql["groupField"] = groupField;
    }

    string sumField = "";
    vConditions.clear();
    iRet=generateVector(_req,"indexs",vConditions);
    if (vConditions.empty())
    {
        sumField = " sum(succ_count) ";
    }
    else
    {
        vector<string>::iterator it = vConditions.begin();
        while(it != vConditions.end())
        {
            sumField += string((sumField.empty()?"":", ")) + " sum(" + *it + ")" ;
             it++;
        }
    }

    _sql["sumField"]= sumField;
    return 0;
}

int RequestDecoder::parseCondition(const string& sCond, string& value)
{
    string::size_type pos =0;
    pos= sCond.find(">=");
    if (pos != string::npos )
    {
        value = sCond.substr(pos+2);
        return GE;
    }

    pos = sCond.find("<=");
    if (pos != string::npos )
    {
        value = sCond.substr(pos+2);
        return LE;
    }

    pos = sCond.find("!=");
    if (pos != string::npos )
    {
        value = sCond.substr(pos+2);
        return NE;
    }
    pos = sCond.find("<");
    if (pos != string::npos )
    {
        value = sCond.substr(pos+1);
        return LT;
    }

    pos = sCond.find(">");
    if (pos != string::npos )
    {
        value = sCond.substr(pos+1);
        return GT;
    }

    pos = sCond.find("=");
    if (pos != string::npos )
    {
        value = sCond.substr(pos+1);
        return EQ;
    }
    return 0;
}

int RequestDecoder::find_field(vector<string>& vCond, const string& field /*f_tflag, f_date*/, string& value )
{
    vector<string>::iterator it = vCond.begin();
    while(it != vCond.end()){

        string::size_type pos = it->find(field);

        if(pos != string::npos)
        {
            string temp = *it;
            vCond.erase(it);
            return parseCondition(temp, value);
        }
        it++;
    }
    return -1;
}

/*
name : indexs, filter, groupby
*/
int RequestDecoder::decodeDataid()
{

    string::size_type pos = _input.find("dataid");
    if (pos == string::npos)
    {
        return -1;
    }
    string::size_type endpos = _input.find(",", pos);
    _sql["dataid"] = _input.substr(pos + 9, endpos - pos -10);
    LOG->debug() << "decodeDataid: " << _sql["dataid"] <<endl;
    return 0;
}

int RequestDecoder::decodeMethod()
{

    string::size_type pos = _input.find("method");
    if (pos == string::npos)
    {
        return -1;
    }
    string::size_type endpos = _input.find(",", pos);
    if (endpos!=string::npos)
    {
        _sql["method"] = _input.substr(pos + 9, endpos - pos -10);
    }
    else
    {
        _sql["method"] = "timecheck"; //
    }
    LOG->debug() << "decodeMethod: " << _sql["method"] <<endl;
    return 0;
}

int RequestDecoder::decodeArray(const string& arr, vector<string> &vFields)
{

    string::size_type pos1 = arr.find_first_of("[");
    if(pos1 == string::npos)
    {
        LOG->debug() << ("paramh '" + arr + "' is invalid!" );
        return -1;
    }
    string::size_type pos2 = arr.find_first_of("]");
    if(pos2 == string::npos)
    {
        LOG->debug() << ("paramh '" + arr + "' is invalid!" );
        return -1;
    }
    vFields = TC_Common::sepstr<string>(arr.substr(pos1, pos2 -pos1-1), ",");
    vector<string>::iterator it = vFields.begin();
    while(it != vFields.end())
    {

        LOG->debug() << "indexs bt " << *it << endl;
        string s = TC_Common::trim(*it, "[]\"\"");
        *it = s;
        LOG->debug() << "indexs at " << *it << endl;
        it++;
    }
    return 0;


}


map<string, string>& RequestDecoder::getSql()
{
    return _sql;
}


int RequestDecoder::decode()
{
    
     _req.Parse(_input.c_str());

    try
    {
        vector<string> vConditions ;
        generateVector(_req,"method",vConditions);
        if(vConditions.size())
        {
            _sql["method"] =vConditions[0];
            TLOGDEBUG("Decode"<<_sql["method"]<<endl);
            vConditions.clear();
        }

         generateVector(_req,"dataid",vConditions);
        if(vConditions.size())
        {
            _sql["dataid"] =vConditions[0];
            TLOGDEBUG("Decode"<<_sql["dataid"]<<endl);
        }

        if (_sql["method"] == "timecheck")
        {
            return TIMECHECK; // 查看最后入库时间
        }
        else if(_sql["method"] == "query")
        {
            composeSqlPartition();
            return QUERY;
        }

    }
    catch (exception& ex)
    {
        LOG->error() << "RequestDecoder::decode exception:"<< ex.what() << endl;
        return -1;
    }
    return -1;
}

//传入sUid，供打印使用
int RequestDecoder::addUid(const string& sUid)
{
    _sql["uid"] = sUid;
    return 0;
}

string RequestDecoder::getLine(const char** ppChar)
{
    string sTmp;

    sTmp.reserve(512);

    while((**ppChar) != '\r' && (**ppChar) != '\n' && (**ppChar) != '\0')
    {
        sTmp.append(1, (**ppChar));
        (*ppChar)++;
    }

    if((**ppChar) == '\r')
    {
        (*ppChar)++;   /* pass the char '\n' */
    }

    (*ppChar)++;

    return sTmp;
}

