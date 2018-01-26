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

#ifndef __NODE_IMP_H_
#define __NODE_IMP_H_
#include "Node.h"
#include <unistd.h>
#include "PlatformInfo.h"
#include "Activator.h"
#include "KeepAliveThread.h"
#include "tars_patch.h"

using namespace tars;
using namespace std;

class NodeImp : public Node
{
public:
    /**
     * 构造函数
     */
    NodeImp()
    {
    }

    /**
     * 析构函数
     */
    ~NodeImp()
    {
    }

    /**
     * 初始化
     */
    virtual void initialize();

    /**
     * 退出
     */
    virtual void destroy()
    {
    }

    /**
     * 销毁指定服务
     * @param application    服务所属应用名
     * @param serverName  服务名
     * @return  int
    */
    virtual int destroyServer( const string& application, const string& serverName, string &result,TarsCurrentPtr current );

    /**
     * 增强的发布接口
     * pushRequest 插入发布请求到队列
     * @param req  发布请求
     * @return  int 0成功 其它失败
     */
    int patchPro(const tars::PatchRequest & req, string & result, TarsCurrentPtr current);

    /**
     * 加载指定文件
     * @param out result  失败说明
     * @return  int 0成功  非0失败
     */
    virtual int addFile(const string &application,const string &serverName,const string &file, string &result, TarsCurrentPtr current);

    /**
     * 获取node名称
     * @return  string
     */
    virtual string getName( TarsCurrentPtr current ) ;

    /**
     * 获取node上负载
     * @return  LoadInfo
     */
    virtual tars::LoadInfo getLoad( TarsCurrentPtr current ) ;

    /**
     * 关闭node
     * @return  int
     */
    virtual int shutdown( string &result, TarsCurrentPtr current );

    /**
     * 关闭nodes上所有服务
     * @return  int
     */
    virtual int stopAllServers( string &result,TarsCurrentPtr current );

    /**
     * 载入指定服务
     * @param application    服务所属应用名
     * @param serverName  服务名
     * @return  int
     */
    virtual int loadServer( const string& application, const string& serverName, string &result, TarsCurrentPtr current );

    /**
     * 启动指定服务
     * @param application    服务所属应用名
     * @param serverName  服务名
     * @return  int
     */
    virtual int startServer( const string& application, const string& serverName, string &result, TarsCurrentPtr current ) ;

    /**
     * 停止指定服务
     * @param application    服务所属应用名
     * @param serverName  服务名
     * @return  int
     */
    virtual int stopServer( const string& application, const string& serverName, string &result, TarsCurrentPtr current ) ;

    /**
     * 通知服务
     * @param application
     * @param serverName
     * @param result
     * @param current
     *
     * @return int
     */
    virtual int notifyServer( const string& application, const string& serverName, const string &command, string &result, TarsCurrentPtr current );

    /**
     *  获取指定服务pid进程号
     * @param application  服务所属应用名
     * @param serverName  服务名
     * @return  int
     */
    int getServerPid( const string& application, const string& serverName, string &result, TarsCurrentPtr current);

    /**
     *  获取指定服务registry设置的状态
     * @param application    服务所属应用名
     * @param serverName  服务名
     * @return  int
     */
    virtual ServerState getSettingState( const string& application, const string& serverName, string &result, TarsCurrentPtr current ) ;

    /**
     * 获取指定服务状态
     * @param application    服务所属应用名
     * @param serverName  服务名
     * @return  ServerState
     */
    virtual ServerState getState( const string& application, const string& serverName, string &result, TarsCurrentPtr current ) ;

    /**
     * 获取指定服务在node的信息
     * @param application    服务所属应用名
     * @param serverName  服务名
     * @return  ServerState
     */
    virtual int getStateInfo(const std::string & application,const std::string & serverName,tars::ServerStateInfo &info,std::string &result,tars::TarsCurrentPtr current);

    /**
     * 同步指定服务状态
     * @param application    服务所属应用名
     * @param serverName  服务名
     * @return  int
     */
    virtual int synState( const string& application, const string& serverName, string &result, TarsCurrentPtr current ) ;

    /**
     * 发布服务进度
     * @param application  服务所属应用名
     * @param serverName  服务名
     * @out tPatchInfo  下载信息
     * @return  int
     */
    virtual int getPatchPercent( const string& application, const string& serverName, PatchInfo &tPatchInfo, TarsCurrentPtr current);

    virtual tars::Int32 delCache(const std::string & sFullCacheName, const std::string &sBackupPath, const std::string & sKey, std::string &result,TarsCurrentPtr current);

    virtual tars::Int32 getUnusedShmKeys(tars::Int32 count,vector<tars::Int32> &shm_keys,tars::TarsCurrentPtr current);

private:
    string keyToStr(key_t key_value);

private:

    string          _downloadPath; //文件下载目录
    NodeInfo        _nodeInfo;     //node信息
    PlatformInfo    _platformInfo; //平台信息
    RegistryPrx     _registryPrx;  //主控代理

};

typedef TC_AutoPtr<NodeImp> NodeImpPtr;

#endif

