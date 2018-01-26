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

#ifndef __TARS_CURRENT_H_
#define __TARS_CURRENT_H_

#include "util/tc_epoll_server.h"
#include "tup/RequestF.h"
#include "servant/BaseF.h"

namespace tars
{

class ServantHandle;

//////////////////////////////////////////////////////////////
/**
 * 当前请求的上下文
 */
class TarsCurrent : public TC_HandleBase
{
public:
    typedef std::map<string, string>    TARS_STATUS;

    typedef std::vector<char>           TARS_BUFFER;

    /**
     * 构造函数
     * @param pServantHandle
     */
    TarsCurrent(ServantHandle *pServantHandle);

    /**
     * 析构
     */
    ~TarsCurrent();

    /**
     * 获取IP
     * @return string
     */
    string getIp() const;

    /**
     * 获取端口
     * @return int
     */
    int getPort() const;

    /**
     * 获取uid
     * @return uint32
     */
    uint32_t getUId() const;

    /**
     * 获取fd
     * @return int
     */
    int getFd() const { return _fd; }

    /**
     * 是否函数返回时发送响应包给客户端
     * @return bool
     */
    bool isResponse() const;

    /**
     * 设置连接的关闭类型，详情参看TC_EpollServer::EM_CLOSE_T
     */
    void setCloseType(int type);

    /**
     * 获取连接关闭类型，详情请参考TC_EpollServer::EM_CLOSE_T类型
     */
    int getCloseType() const;

    /**
     * 设置是否自动回响应包
     */
    void setResponse(bool value) { _response = value; }

    /**
     * 设置返回的context(仅TARS协议有效)
     */
    void setResponseContext(const map<std::string, std::string> & context){_responseContext = context;}

    /**
     * 获取返回的context(仅TARS协议有效)
     */
    const map<std::string, std::string> & getResponseContext() const {return _responseContext;}

    /**
     * 关闭当前连接
     */
    void close();

    /**
     * 获取所属的ServantHandle
     */
    ServantHandle* getServantHandle();

    /**
     * 获取来源的Adapter
     * @return TC_EpollServer::BindAdapter*
     */
    TC_EpollServer::BindAdapter* getBindAdapter();

    /**
     * 获取请求buffer
     * @return string
     */
    const vector<char> &getRequestBuffer() const;

    /**
     * 获取服务Servant名称
     * @return string
     */
    string getServantName() const;

    /**
     * 请求的协议的版本号(仅TARS协议有效)
     *
     * @return short
     */
    short getRequestVersion() const;

    /**
     * 扩展map(仅TARS协议有效)
     * @return map<string,string>&
     */
    map<string, string>& getContext();

    /**
     * 获取保存状态信息，比如染色等(仅TARS协议有效)
     * @return map<string,string>&
     */
    const map<string, string>& getRequestStatus() const;

    /**
     * 函数名称(仅TARS协议有效)
     * @return string
     */
    string getFuncName() const;

    /**
     * 请求ID(仅TARS协议有效)
     * @return int
     */
    uint32_t getRequestId() const;

    /**
     * 获取包类型(仅TARS协议有效)
     * @return char
     */
    char getPacketType() const;

    /**
     * 获取消息类型(仅TARS协议有效)
     * @return tars::Int32
     */
    tars::Int32 getMessageType() const;

    /**
     * 获取接收到请求的时间
     */
    struct timeval getRecvTime() const;

    /**
     * 设置是否上报状态报告
     */
    void setReportStat(bool bReport);

    /**
     * tars协议的发送响应数据(仅TARS协议有效)
     * @param iRet
     * @param status
     * @param buffer
     */
    void sendResponse(int iRet, const vector<char>& buffer = TARS_BUFFER(),
                      const map<string, string>& status = TARS_STATUS(),
                      const string & sResultDesc = "");

    /**
     * 普通协议的发送响应数据(非TARS协议有效)
     * @param buff
     * @param len
     */
    void sendResponse(const char* buff, uint32_t len);

protected:

    friend class ServantHandle;

    friend class Application;

    /**
     * 初始化
     * @param stRecvData
     */
    void initialize(const TC_EpollServer::tagRecvData &stRecvData);

    /**
     * 初始化
     * @param stRecvData
     * @param beginTime
     */
    void initialize(const TC_EpollServer::tagRecvData &stRecvData, int64_t beginTime);

    /**
     * 初始化
     * @param stRecvData
     */
    void initializeClose(const TC_EpollServer::tagRecvData &stRecvData);

    /**
     * 初始化
     * @param sRecvBuffer
     */
    void initialize(const string &sRecvBuffer);

    /**
     * 服务端上报状态，针对单向调用及TUP调用(仅对TARS协议有效)
     */
    void reportToStat(const string & sObj);

protected:
    /**
     * 操作类指针
     */
    ServantHandle*            _servantHandle;

    /**
     * 消息_bindAdapter
     */
    TC_EpollServer::BindAdapter* _bindAdapter;

    /**
     * 唯一标识
     */
    uint32_t                _uid;

    /**
     * ip地址
     */
    string                  _ip;

    /**
     * 端口
     */
    int                     _port;

    /**
     * 用于回包时选择网络线程
     */
    int                        _fd;

    /**
     * 客户端请求包
     */
    RequestPacket            _request;

    /**
     * 响应
     */
    bool                    _response;

    /**
     * 收到请求时间
     */
    int64_t                _begintime;

    /**
     * 接口处理的返回值
     */
    int                     _ret;

    /**
     * 是否上报stat
     */
    bool                    _reportStat;

    /**
     * 连接关闭的类型，初始值是-1，非法值
     */
    int                     _closeType;

    /**
     * 设置额外返回的内容
     */
    map<std::string, std::string> _responseContext;
};
//////////////////////////////////////////////////////////////
}
#endif
