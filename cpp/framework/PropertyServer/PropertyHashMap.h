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

#ifndef __PROPERTY_HASHMAP_H_
#define __PROPERTY_HASHMAP_H_

#include "util/tc_common.h"
#include "jmem/jmem_hashmap.h"
#include "servant/PropertyF.h"
#include "servant/TarsLogger.h"

using namespace tars;

typedef StatPropMsgBody PropBody;
typedef StatPropMsgHead PropHead;
typedef TarsHashMap<PropHead,PropBody, ThreadLockPolicy, FileStorePolicy> PropHashMap;

typedef std::map<PropHead, PropBody, std::less<PropHead>, __gnu_cxx::__pool_alloc<std::pair<PropHead const, PropBody> > > PropertyMsg;

class PropertyHashMap : public PropHashMap
{
public:


    /**
    * 增加数据
    * @param Key
    * @param Value
    *
    * @return int
    */
    int add(const PropHead &head, const StatPropMsgBody &body)
    {
        int ret = TC_HashMap::RT_OK;
        tars::TarsOutputStream<BufferWriter> osk;
        head.writeTo(osk);
        string sk(osk.getBuffer(), osk.getLength());
        {
            TC_LockT<ThreadLockPolicy::Mutex> lock(ThreadLockPolicy::mutex());
            string sv;
            time_t t = 0;
            ret = this->_t.get(sk, sv,t);
            if ( ret < 0 || ret == TC_HashMap::RT_ONLY_KEY || ret == TC_HashMap::RT_NO_DATA)
            {

                tars::TarsOutputStream<BufferWriter> osv;
                body.writeTo(osv);
                string stemp(osv.getBuffer(), osv.getLength());
                vector<TC_HashMap::BlockData> vtData;
                return    this->_t.set(sk, stemp,true,vtData);
            }

            //读取到数据了, 解包
            if (ret == TC_HashMap::RT_OK)
            {
                tars::TarsInputStream<BufferReader> is;
                is.setBuffer(sv.c_str(), sv.length());

                PropBody stBody;
                stBody.readFrom(is);
                if(LOG->IsNeedLog(TarsRollLogger::INFO_LOG))
                {
                    ostringstream os;
                    head.displaySimple(os);
                    os<< " "<<endl;
                    stBody.displaySimple(os);
                    TLOGINFO("read hash body|" <<os.str()<< endl);
                }

                map<string, string> mSumib;

                for (size_t i=0; i< stBody.vInfo.size(); i++)
                {
                    mSumib.insert(make_pair(stBody.vInfo[i].policy, stBody.vInfo[i].value));
                }

                for (size_t i=0; i< body.vInfo.size(); i++)
                {
                    string sPolicy = body.vInfo[i].policy;
                    string inValue = body.vInfo[i].value;
                    map<string, string>::iterator it = mSumib.find(sPolicy);
                    if(it != mSumib.end())
                    {
                        if (sPolicy == "Count")
                        {
                            long long count = TC_Common::strto<long long >(it->second) + TC_Common::strto<long long >(inValue);
                            it->second      = TC_Common::tostr(count);
                        }
                        else if (sPolicy == "Sum")
                        {
                            long long sum =  TC_Common::strto<long long >(it->second) + TC_Common::strto<long long >(inValue);
                            it->second = TC_Common::tostr(sum);
                        }
                        else if (sPolicy == "Min")
                        {
                            long long  s  = TC_Common::strto<long long >(it->second);
                            long long  in = TC_Common::strto<long long >(inValue);
                            long long min =  s < in ? s : in;
                            it->second    = TC_Common::tostr(min);
                        }
                        else if (sPolicy == "Max")
                        {
                            long long  s   = TC_Common::strto<long long >(it->second);
                            long long  in  = TC_Common::strto<long long >(inValue);
                            long long max  =  s > in ? s : in;
                            it->second     = TC_Common::tostr(max);
                        }
                        else if (sPolicy == "Distr")
                        {
                            vector<string> fields;
                            vector<string> fieldIn;
                            fields = TC_Common::sepstr<string>(it->second, ",");
                            fieldIn= TC_Common::sepstr<string>(inValue, ",");
                            string tmpValue = "";
                            for (size_t k=0; k<fields.size(); k++)

                            {
                                vector<string> sTmp  = TC_Common::sepstr<string>(fields[k], "|");
                                vector<string> inTmp = TC_Common::sepstr<string>(fieldIn[k], "|");
                                long long  tmp       = TC_Common::strto<long long >(sTmp[1]) + TC_Common::strto<long long>(inTmp[1]);
                                sTmp[1] = TC_Common::tostr(tmp);
                                fields[k] = sTmp[0]+ "|" +sTmp[1];

                                if (k==0)
                                {
                                    tmpValue = fields[k];
                                }
                                else
                                {
                                    tmpValue = tmpValue + "," + fields[k];
                                }
                            }
                            it->second = tmpValue;
                        }
                        else if(sPolicy == "Avg")
                        {
                            //double avg = (TC_Common::strto<double>(sValue) + TC_Common::strto<double>(inValue))/2;
                            vector<string> sTmp  = TC_Common::sepstr<string>(it->second,"=");
                            vector<string> inTmp = TC_Common::sepstr<string>(inValue,"=");

                            //总值求和
                            double tmpValueSum = TC_Common::strto<double>(sTmp[0]) + TC_Common::strto<double>(inTmp[0]);
                            //新版本平均值带有记录数,记录求和
                            long tmpCntSum = (2 == inTmp.size()?(TC_Common::strto<long>(inTmp[1])):1) +
                                                   (2 == sTmp.size()?(TC_Common::strto<long>(sTmp[1])):1);

                            it->second = TC_Common::tostr(tmpValueSum) + "=" + TC_Common::tostr(tmpCntSum);
                        }
                    }
                    else
                    {
                        mSumib.insert(make_pair(body.vInfo[i].policy, body.vInfo[i].value));
                    }
                }

                stBody.vInfo.clear();

                map<string, string>::iterator it = mSumib.begin();
                while(it != mSumib.end())
                {
                    StatPropInfo tempProp;
                    tempProp.policy = it->first;
                    tempProp.value  = it->second;

                    stBody.vInfo.push_back(tempProp);
                    
                    ++it;
                }

                if(LOG->IsNeedLog(TarsRollLogger::INFO_LOG))
                {
                    ostringstream os;
                    head.displaySimple(os);
                    os<< " "<<endl;
                    stBody.displaySimple(os);
                    TLOGINFO("reset hash body|" <<os.str()<< endl);
                }

                tars::TarsOutputStream<BufferWriter> osv;
                stBody.writeTo(osv);
                string stemp(osv.getBuffer(), osv.getLength());
                vector<TC_HashMap::BlockData> vtData;
                ret = this->_t.set(sk, stemp,true,vtData);
            }
        }
        return ret;
    }
};


extern PropertyHashMap g_hashmap;






#endif


