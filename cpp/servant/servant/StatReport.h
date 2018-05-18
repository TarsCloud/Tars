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

#ifndef __TARS_STAT_REPORT_H_
#define __TARS_STAT_REPORT_H_

#include "util/tc_thread.h"
#include "util/tc_readers_writer_data.h"
#include "servant/PropertyReport.h"
#include "servant/StatF.h"
#include "servant/PropertyF.h"

/////////////////////////////////////////////////////////////////////////
/*
    属性上报使用方式:
    StatReport report;
    report->setReportInfo( pPtr,strMasterName,strMasterIp, 5 );
    vector<int> v;
    v.push_back(1000);
    v.push_back(1001);
    v.push_back(1002);
    v.push_back(1003);
    v.push_back(1004);

    PropertyReportPtr srPtr = report->createPropertyReport("test", PropertyReport::sum(), PropertyReport::dist(v));
    srPtr->report(i);
*/

namespace tars
{

struct StatSampleMsgHead
{
    string slaveName;
    string interfaceName;
    string ip;
    bool operator <(const StatSampleMsgHead& m)const
    {
        if(slaveName != m.slaveName)
        {
            return slaveName < m.slaveName;
        }
        if(interfaceName != m.interfaceName)
        {
            return interfaceName < m.interfaceName;
        }
        return ip < m.ip;
    }
};

/////////////////////////////////////////////////////////////////////////
/**
 * 状态上报类, 上报的信息包括:
 * 1 模块间调用的信息
 * 2 业务自定义的属性统计
 */
class StatReport : public TC_HandleBase, public TC_Thread, public TC_ThreadLock
{
public:
    typedef  map<StatMicMsgHead, StatMicMsgBody>        MapStatMicMsg;
    typedef  map<StatPropMsgHead, StatPropMsgBody>      MapStatPropMsg;
    typedef  multimap<StatSampleMsgHead,StatSampleMsg>  MMapStatSampleMsg;
    typedef  TC_LoopQueue<MapStatMicMsg*>                 stat_queue;

    const static int MAX_MASTER_NAME_LEN   = 127;
    const static int MAX_MASTER_IP_LEN     = 20;
    const static int MAX_REPORT_SIZE       = 1400;    //上报的最大大小限制
    const static int MIN_REPORT_SIZE       = 500;     //上报的最小大小限制
    const static int STAT_PROTOCOL_LEN     = 100;     //一次stat mic上报纯协议部分占用大小，用来控制udp大小防止超MTU
    const static int PROPERTY_PROTOCOL_LEN = 50;      //一次property上纯报协议部分占用大小，用来控制udp大小防止超MTU
    const static int MAX_STAT_QUEUE_SIZE   = 10000;   //上报队列缓存大小限制
    
    enum StatResult
    {
        STAT_SUCC       = 0,
        STAT_TIMEOUT    = 1,
        STAT_EXCE       = 2,
    };
public:
    /**
     * 构造函数
     */
    StatReport(size_t iEpollNum=0);

    /**
     * 析够函数
     */
    ~StatReport();

    /**
     * 初始化
     * @param statPrx, 模块间调用服务器地址
     * @param propertyPrx, 用户自定义属性服务器地址
     * @param strModuleName, 模块名
     * @param strModuleIp, 模块ip
     * @param iReportInterval, 上报间隔单位秒
     * @param iMaxReporSize一次最大上报包长度。 跟udp最大允许包8k、MTU长度1472有关，暂定取值范围[500-1400]
     * @param iSampleRate, 采样比率1/1000
     * @param iMaxSampleCount, 最大采样数
     * @param iReportTimeout, 上报接口调用的超时时间
     * @param sContainer, 设置上报的容器名
     */
    void setReportInfo(const StatFPrx& statPrx,
                       const PropertyFPrx& propertyPrx,
                       const string& strModuleName,
                       const string& strModuleIp,
                       const string& strSetDivision,
                       int iReportInterval = 60,
                       int iSampleRate = 1000,
                       unsigned int iMaxSampleCount = 100,
                       int iMaxReportSize = 1400,
                       int iReportTimeout = 5000);

    /**
     * 设置模块间调用数据
     * @param strModuleName, 被调模块. 一般采用app.servername 例如:Comm.BindServer
     * @param setdivision,被调set信息,=>>MTT.s.s
     * @param strInterfaceName,被调接口.一般为函数名
     * @param strModuleIp,被调ip
     * @param shSlavePort,被调port
     * @param eResult,成功STAT_SUCC，超时 STAT_TIMEOUT，异常STAT_EXC.
     * @param iSptime,耗时
     * @param iReturnValue,返回值
     * @param bFromClient,从客户端采集 false从服务端采集
     * @param sContainer, 设置上报的容器名
     * 。
     */
    void report(const string& strModuleName,
                 const string& setdivision,
                const string& strInterfaceName,
                const string& strModuleIp,
                uint16_t iPort,
                StatResult eResult,
                int  iSptime,
                int  iReturnValue = 0,
                bool bFromClient = true);

     /**
     * 设置模块间调用数据
     * @param strMasterName     主调用方的名字
     * @param strMasterIp       主调用方的地址
     * @param strSlaveName      被调用方的名字
     * @param strSlaveIp        被调用方的地址
     * @param iSlavePort        被调用方的端口
     * @param strInterfaceName  被调接口.一般为函数名
     * @param eResult           成功STAT_SUCC，超时 STAT_TIMEOUT，异常STAT_EXC.
     * @param iSptime           耗时(单位毫秒)
     * @param iReturnValue      返回值
     */
    void report(const string& strMasterName,
                const string& strMasterIp,
                const string& strSlaveName,
                const string& strSlaveIp,
                uint16_t iSlavePort,
                const string& strInterfaceName,
                StatResult eResult,
                int  iSptime,
                int  iReturnValue = 0);

    /**
     * 根据名字获取属性上报对象
     * @param sProperty
     *
     * @return PropertyReportPtr
     */
    PropertyReportPtr getPropertyReport(const string& strProperty)
    {
         Lock lock(*this);
         if(_statPropMsg.find(strProperty) != _statPropMsg.end())
         {
             return _statPropMsg[strProperty];
         }

         return NULL;
    }

    /**
     * 生成属性上报对象
     * @param strProperty
     * @param t1
     *
     * @return PropertyReportPtr
     */
    template<typename... Args>
    PropertyReportPtr createPropertyReport(const string& strProperty, Args&&... args)
    {
        Lock lock(*this);

        if(_statPropMsg.find(strProperty) != _statPropMsg.end())
        {
            return _statPropMsg[strProperty];
        }

         PropertyReportPtr srPtr = new PropertyReportImp<decltype(std::forward<Args>(args))...>(std::forward<Args>(args)...);

         _statPropMsg[strProperty] = srPtr;

         return srPtr;
    }

public:
    void report(size_t iSeq,MapStatMicMsg * pmStatMicMsg);

    /*
    * 获取stat代理
    */
    StatFPrx getStatPrx() {return _statPrx; }

    /*
    * 采样
    */
    void doSample(const string& strSlaveName,
                      const string& strInterfaceName,
                      const string& strSlaveIp,
                      map<string, string> &status);
    /*
    * 采样id
    */
    string sampleUnid();

    /**
     * 增加关注时间点.  调用方式addStatInterv(5)
     * @param strTimePoint  时间点序列
     * @return
     */
    void addStatInterv(int  iInterv);

    /**
     * 重置关注时间点.
     * @return
     */
    void resetStatInterv();

    /**
     *
     * @param body
     * @param time
     * @return void
     */
    void getIntervCount(int time,StatMicMsgBody& body);

    /**
     * 结束线程
     */
    void terminate();

    /**
     * 轮询函数
     */
    virtual void run();
public:
    /**
     * 限制长度
     * @param str
     * @param limitlen
     *
     * @return string
     */
    static string trimAndLimitStr(const string& str, uint32_t limitlen);

    /*
    * func: tars.s.1 =>> MTT s  1
    */
    static bool divison2SetInfo(const string& str, vector<string>& vtSetInfo);

    /*
    * func: tars.xxxServer =>> xxxServer
    */
    static string getServerName(string sModuleName);

private:

    /**
     *
     * @param head
     * @param body
     * @param msg
     * @param bFromClient
     */
    void submit(StatMicMsgHead& head, StatMicMsgBody& body, bool bFromClient);

    /**
     * 上报模块间调用信息  Mic = module interval call
     * @param msg
     * @param bFromClient  是否为客户端上报
     * @return int
     */
    int reportMicMsg(MapStatMicMsg &msg, bool bFromClient);

    /**
     * 上报属性信息  Prop = property
     * @return int
     */
    int reportPropMsg();

    /**
     * 上报多维度属性信息  Prop = property
     * @return int
     */
    int reportPropPlusMsg();

    /**
     * stat 采样
     */
    int reportSampleMsg();


    //合并两个MicMsg
    void addMicMsg(MapStatMicMsg & old,MapStatMicMsg & add);

private:
    time_t              _time;

    int                 _reportInterval;

    int                    _reportTimeout;

    int                 _maxReportSize;

    bool                _terminate;

    string              _moduleName;

    string              _setName; // 主调上报，表示主调set名，被调上报，表示被调set名

    string              _setArea; //

    string              _setID; //

    string              _ip;

    int                 _sampleRate; //生成模块间调用时序图的采样比率

    unsigned int        _maxSampleCount; //1分钟内最大采样条数

    StatFPrx            _statPrx;

    MapStatMicMsg       _statMicMsgClient;

    MapStatMicMsg       _statMicMsgServer;

    MMapStatSampleMsg   _statSampleMsg;

    vector<int>         _timePoint;

    PropertyFPrx        _propertyPrx;

    map<string, PropertyReportPtr>          _statPropMsg;

private:

    size_t _epollNum;
    vector<stat_queue*>   _statMsg;

    size_t                _retValueNumLimit;    

};
///////////////////////////////////////////////////////////
}

#endif
