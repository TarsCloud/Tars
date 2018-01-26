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

#ifndef __NODE_ROLL_LOGGER_H__
#define __NODE_ROLL_LOGGER_H__

#include "util/tc_logger.h"
#include "servant/TarsLogger.h"

/**
 * node使用的日志，主要是为了支持多个滚动日志
 */

///////////////////////////////////////////////////////////////////////////////

/**
 * 本地日志帮助类, 单件
 * 循环日志单件是永生不死的, 保证任何地方都可以使用
 * 当该对象析够以后, 则直接cout出来
 */
class RollLoggerManager : public TC_ThreadLock, public TC_Singleton<RollLoggerManager, CreateUsingNew, DefaultLifetime>
{
public:
    enum
    {
        NONE_LOG    = 1,    /**所有的log都不写*/
        ERROR_LOG   = 2,    /**写错误log*/
        WARN_LOG    = 3,    /**写错误,警告log*/
        DEBUG_LOG   = 4,    /**写错误,警告,调试log*/
        INFO_LOG    = 5        /**写错误,警告,调试,Info log*/
    };
public:

    typedef TC_Logger<RollWriteT, TC_RollBySize> RollLogger;

    RollLoggerManager();

    ~RollLoggerManager();

    /**
     * 设置本地信息
     * @param app, 业务名称
     * @param server, 服务名称
     * @param logpath, 日志路径
     * @param iMaxSize, 文件最大大小,字节
     * @param iMaxNum, 文件最大数
     */
    void setLogInfo(const string &sApp, const string &sServer, const string &sLogpath, int iMaxSize = 1024*1024*50, int iMaxNum = 10, const CommunicatorPtr &comm=NULL, const string &sLogObj="");

    /**
     * 设置同步写日志
     *
     * @param bSync
     */
    void sync(RollLogger *pRollLogger, bool bSync = true);

    /**
     * 获取循环日志
     *
     * @return RollLogger
     */
    RollLogger *logger(const string &sFile);

private:
    void initRollLogger(RollLogger *pRollLogger, const string &sFile, const string &sFormat);

protected:

    /**
     * 应用
     */
    string                  _app;

    /**
     * 服务名称
     */
    string                  _server;

    /**
     * 日志路径
     */
    string                  _logpath;

    int                        _maxSize;
    int                        _maxNum;
    CommunicatorPtr         _comm;
    string                     _logObj;

    /**
     * 本地线程组
     */
    TC_LoggerThreadGroup    _local;

    /**
     * 远程日志
     */
    map<string, RollLogger*>    _loggers;

};


/**
 * 循环日志
 */
#define NODE_LOG(x)             (RollLoggerManager::getInstance()->logger(x))
#endif

