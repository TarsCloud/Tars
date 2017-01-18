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

#ifndef __ACTIVATOR_H_
#define __ACTIVATOR_H_
#include "Node.h"
#include <unistd.h>
#include "util/tc_file.h"
#include "util/tc_monitor.h"
#include <iostream>

using namespace tars;
using namespace std;
////////////////////////////////////////////////////
// 

//用来标志脚本结束
/////////////////////////////////////////////////////////
// 环境变量
struct EnvVal : std::unary_function<string, string>
{
    string operator()(const std::string& value)
    {
        string::size_type pos = value.find("=");
        if(pos == string::npos || pos >= value.size() - 1)
        {
            return value;
        }

        string v = value.substr(pos + 1);
        assert(v.size());

        string::size_type start = 0;
        string::size_type finish;

        //宏替换
        while((start = v.find("$", start)) != string::npos && start < v.size() - 1)
        {
            string var;
            if(v[start + 1] == '{')
            {
                finish = v.find("}");
                if(finish == string::npos)
                {
                    break;
                }
                var = v.substr(start + 2, finish - start - 2);
            }
            else
            {
                finish = start + 1;
                while((isalnum(v[finish]) || v[finish] == '_')  && finish < v.size())
                {
                    ++finish;
                }
                var = v.substr(start + 1, finish - start - 1);
                --finish;
            }

            char* p = getenv(var.c_str());
            string str = p ? string(p) : "";

            v.replace(start, finish - start + 1, str);

            start += str.size();
        }

        //此处需要马上设置，否则以上宏替换中获取的环境变量为空
        setenv((value.substr(0, pos)).c_str(), v.c_str(), true);

        return value.substr(0, pos) + "=" + v;
    }
};

class Activator : public TC_ThreadLock, public TC_HandleBase
{
public:
    /**
     * 构造服务
     * iTimeInterval秒内最多进行iMaxCount次启动。 达到最大启动次数仍失败后iPunishInterval秒重试启动一次
     * @param iTimeInterval
     * @param iMaxCount 
     * @param iPunishInterval 惩罚时间间隔 
     *
     */
    Activator(int iTimeInterval,int iMaxCount,int iPunishInterval)
    : _maxCount(iMaxCount)
    , _timeInterval(iTimeInterval)
    , _punishInterval(iPunishInterval)
    , _termSignal(false)
    , _redirectPath("")
    {
        clearRunntimeData();
    };

    ~Activator()
    {
    };
    
    /**
     * 启动服务
     * 
     * @param strExePath 可执行文件路径
     * @param strPwdPath 当前路径 
     * @param strRollLogPath 日志路径 
     * @param vOptions 启动参数 
     * @param vEnvs 环境变量 
     * @return pid_t 生成子进程id 
     *
     */
    pid_t activate(const string& strExePath, const string& strPwdPath, const string &strRollLogPath, const vector<string>& vOptions, vector<string>& vEnvs);

    /**
     * 脚本启动服务
     * 
     * @param sServerId 服务id
     * @param strStartScript 脚本路径  
     * @param strMonitorScript 脚本路径 
     * @param sResult 执行结果  
     * @return pid_t 生成子进程id 
     *
     */
    pid_t activate(const string &strServerId, const string& strStartScript, const string &strMonitorScript, string &strResult);

    /**
     * 停止服务
     * 
     * @param pid 进程id
     * @return int 0 成功  其它失败
     */
    int deactivate( int pid );

    /**
     * 停止服务 并生成core文件
     * 
     * @param pid 进程id
     * @return int 0 成功  其它失败
     */
    int deactivateAndGenerateCore( int pid );
    
    /**
     * 发送信号
     * 
     * @param pid   进程id 
     * @param signal 信号
     * @return int 0 成功  其它失败
     */
    int sendSignal( int pid, int signal ) const;
public:
    
    bool isActivatingLimited (); //启动限制,用来防止问题服务不断重启影响其它服务

    void addActivatingRecord();

    //运行脚本
    bool doScript(const string &sServerId, const string &strScript, string &strResult, map<string,string> &mResult,const string &sEndMark = "");

    map <string,string> parseScriptResult(const string &strServerId,const string &strResult);

    void setRedirectPath(const string& sRedirectpath) {_redirectPath = sRedirectpath;}

    void clearRunntimeData()
    {
        _activingRecord.clear();
        _limited = false;
        _curCount = 0;
    };

    void setLimitInfo(int iTimeInterval,int iMaxCount,int iPunishInterval)
    {
        _timeInterval = iTimeInterval;
        _maxCount = iMaxCount;
        _punishInterval = iPunishInterval;
        clearRunntimeData();
    }

private:
    int pclose2(FILE *fp);
    FILE* popen2(const char *cmdstring, const char *type);
       
private:
    vector <time_t> _activingRecord;   //运行时
    bool    _limited;                  //是否启动受限，运行时
    int     _curCount;                 //当前启动次数，运行时
    int     _maxCount;                 //最大启动次数,配置
    int     _timeInterval;             //时间,单位分钟,配置
    int     _punishInterval;           //惩罚受限时间间隔,单位分钟,配置
    
private:
    bool    _termSignal;               //非tars服务脚本运行超时停止信号
    string  _redirectPath;               //标准输出和错误输出重定向目录
};

typedef TC_AutoPtr<Activator> ActivatorPtr;
#endif

