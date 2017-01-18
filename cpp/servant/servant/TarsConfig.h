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

#ifndef __TARS_CONFIG_H_
#define __TARS_CONFIG_H_

#include "util/tc_autoptr.h"
#include "util/tc_singleton.h"
#include "servant/Global.h"
#include "servant/ConfigF.h"

using namespace std;

namespace tars
{
     
/**
 *
 * 功能：获取远程系统配置，生成本地文件，支持备份和回滚
 *
 * 说明：该对象只实现将ConfigServer上指定的配置文件
 * 读取到本地(保存到应用程序执行目录)，具体配置解析
 * 由客户端实现
 *
 * 客户端通过调用addConfig接口为
 * 每个配置文件创建一个TarsRemoteConfig实例
 *
 * 备份文件数目在对象创建时指定，缺省为5个，
 * 能回滚的次数等于备份文件数目
 *
 */

class TarsRemoteConfig : public TC_Singleton<TarsRemoteConfig>
{
public:
    /**
     * 初始化
     * @param comm, 通信器
     * @param obj, 对象名称
     * @param app, 应用名称
     * @param serverName, 服务名称
     * @param basePath, 基本路径
     * @param maxBakNum, 最大备份文件个数
     *
     * @return int
     */
    int setConfigInfo(const CommunicatorPtr &comm, const string &obj, const string & app, const string &serverName, const string& basePath,const string& setdivision="",int maxBakNum=5);

    /**
     * 读取ConfigServer上配置文件到本地，并备份原文件
     * @param  sFullFileName 文件名称
     * @param  result        结果
     * @param  bAppOnly      是否只获取应用级别的配置
     *
     * @return bool
     */
    bool addConfig(const string & filename, string &result, bool bAppConfigOnly = false);

private:
    /**
     *  实现请求ConfigServer并将结果以文件形式保存到本地目录
     * @param  sFullFileName 文件名称
     * @param  bAppOnly      是否只获取应用级别的配置
     *
     * @return string       生成的文件名称
     */
    string getRemoteFile(const string & sFullFileName, bool bAppConfigOnly = false);

    /**
     * 实现本地文件的回滚，可回滚次数等于最大备份文件数，每次
     * 都使用最近的备份文件覆盖当前配置文件
     *
     * @return string
     */
    string recoverSysConfig(const string & sFullFileName);

    /**
     * 备份文件名称 Config.conf.1.bak,Config.conf.2.bak ...
     * 该方法提供下标到文件名的转化
     *
     * @param index         第几个备份文件
     *
     * @return string       配置文件全路径
     */
    inline string index2file(const string & sFullFileName, int index);

    /**
     *  rename系统操作的封装，当oldFile不存在时抛出异常
     *
     * @param oldFile   原文件路径和名称
     * @param newFile   新文件逻辑和名称
     */
    inline void localRename(const string& oldFile, const string& newFile);

protected:

    /**
     * 通信器
     */
    CommunicatorPtr _comm;

    /**
     * 配置代理
     */
    ConfigPrx       _configPrx;

    /**
     * 应用
     */
    string          _app;

    /**
     * 服务名称
     */
    string          _serverName;

    /**
     * 路径
     */
    string          _basePath;

    /**
     * set信息
     */

    string          _setdivision;

    /**
     * 最大备份数
     */
    int             _maxBakNum;

    /**
     * 线程锁
     */
    TC_ThreadMutex     _mutex;
};

}

#endif
