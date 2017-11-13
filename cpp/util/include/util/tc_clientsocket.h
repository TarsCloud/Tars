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

#ifndef _TC_CLIENTSOCKET_H__
#define _TC_CLIENTSOCKET_H__

#include "util/tc_socket.h"
#include <sstream>
#include "util/tc_http.h"

namespace tars
{
/////////////////////////////////////////////////
/**
 * @file  tc_clientsocket.h
 * @brief 客户端发包收包类.
 */
/////////////////////////////////////////////////
/**
*  @brief 解析endpoint异常类
*/
struct TC_EndpointParse_Exception : public TC_Exception
{
    TC_EndpointParse_Exception(const string &buffer) : TC_Exception(buffer){};
    ~TC_EndpointParse_Exception() throw() {};
};

/**
 *  @brief 表示一个网络端口,支持以下格式:
 *
 * 1:tcp -h 127.0.0.1 -p 2345 -t 10000
 *
 * 2:tcp -h /tmp/sock.sock -p 0 -t 10000
 *
 * 3:udp -h 127.0.0.1 -p 2345 -t 10000
 *
 * -p 0:表示本地套接字
 *
 * -q 0:表示qos的dscp值
 *
 * 此时-h表示的文件路径
 */
class TC_Endpoint
{
public:
	enum EType { UDP = 0, TCP = 1, SSL = 2 };
    /**
     *
     */
    TC_Endpoint();

    /**
     * @brief 构造函数
     * @param host
     * @param port
     * @param timeout, 超时时间, 毫秒
     * @param type, SOCK_STREAM或SOCK_DGRAM
     */
    TC_Endpoint(const string& host, int port, int timeout, int type = 1, int grid = 0, int qos = 0, int weight = -1, unsigned int weighttype = 0, int authType = 0)
    {
        init(host, port, timeout, type, grid, qos, weight, weighttype, authType);
    }

    /**
     * @brief 用字符串描述来构造
     * @param desc
     */
    TC_Endpoint(const string& desc)
    {
        parse(desc);
    }

    /**
     * @brief 拷贝构造
     * @param l
     */
    TC_Endpoint(const TC_Endpoint& l)
    {
        _host   = l._host;
        _port   = l._port;
        _timeout= l._timeout;
        _type   = l._type;
        _grid   = l._grid;
        _qos    = l._qos;
        _weight = l._weight;
        _weighttype = l._weighttype;
        _authType = l._authType;
    }

    /**
     * @brief 赋值函数
     * @param l
     *
     * @return TC_Endpoint&
     */
    TC_Endpoint& operator = (const TC_Endpoint& l)
    {
        if(this != &l)
        {
            _host   = l._host;
            _port   = l._port;
            _timeout= l._timeout;
            _type   = l._type;
            _grid   = l._grid;
            _qos    = l._qos;
            _weight = l._weight;
            _weighttype = l._weighttype;
            _authType = l._authType;
        }

        return *this;
    }

    /**
     * ==
     * @param l
     *
     * @return bool
     */
    bool operator == (const TC_Endpoint& l)
    {
        return (_host == l._host && _port == l._port && _timeout == l._timeout && _type == l._type && _grid == l._grid && _qos == l._qos && _weight == l._weight && _weighttype == l._weighttype && _authType == l._authType);
    }

    /**
     * @brief 设置ip
     * @param str
     */
    void setHost(const string& host)    { _host = host; }

    /**
     * @brief 获取ip
     *
     * @return const string&
     */
    string getHost() const              { return _host; }

    /**
     * @brief 设置端口
     * @param port
     */
    void setPort(int port)              { _port = port; }

    /**
     * @brief 获取端口
     *
     * @return int
     */
    int getPort() const                 { return _port; }

    /**
     * @brief 设置超时时间
     * @param timeout
     */
    void setTimeout(int timeout)        { _timeout = timeout; }

    /**
     * @brief 获取超时时间
     *
     * @return int
     */
    int getTimeout() const              { return _timeout; }

    /**
     * @brief  是否是TCP, 否则则为UDP
     *
     * @return bool
     */
    int  isTcp() const                  { return _type == TCP || _type == SSL; }
    /**
     * @brief  是否是SSL
     *
     * @return int
     */
    int isSSL() const                  { return _type == SSL; }

    /**
     * @brief 设置为TCP或UDP
     * @param bTcp
     */
    void setTcp(bool bTcp)              { _type = bTcp; }

    /**
     * @brief 设置为TCP/UDP/SSL
     * @param type
     */
    void setType(int type)              { _type = type; }
    /**
     * @brief 获取协议类型
     */
    int getType() const                { return _type; }
    /**
     * @brief 获取路由状态
     * @param grid
     */
    int getGrid() const                 { return _grid; }

    /**
     * @brief 设置路由状态
     * @param grid
     */
    void setGrid(int grid)              { _grid = grid; }

    /**
     * @brief 获取路由状态
     * @param grid
     */
    int getQos() const                 { return _qos; }

    /**
     * @brief 设置路由状态
     * @param grid
     */
    void setQos(int qos)              { _qos = qos; }

    /**
     * @brief 获取节点的静态权重值
     */
    int getWeight() const                 { return _weight; }

    /**
     * @brief 设置节点的静态权重值
     * @param weight
     */
    void setWeight(int weight)              { _weight = weight; }

    /**
     * @brief 获取节点的权重使用方式
     */
    unsigned int getWeightType() const                 { return _weighttype; }

    /**
     * @brief 设置节点的权重使用方式
     * @param weightway
     */
    void setWeightType(unsigned int weighttype)              { _weighttype = weighttype; }

    /**
     * @brief 是否是本地套接字
     *
     * @return bool
     */
    bool isUnixLocal() const            { return _port == 0; }

	/**
     * @brief 获取认证类型
     */
    int getAuthType() const             { return _authType; }

	/**
     * @brief 设置认证类型
     */
    void setAuthType(int type)          { _authType = type; }

    /**
     * @brief 字符串描述
     *
     * @return string
     */
    string toString()
    {
        ostringstream os;
        if (_type == TCP)
            os << "tcp";
        else if (_type == UDP)
            os << "udp";
        else 
            os << "ssl";

        os << " -h " << _host << " -p " << _port << " -t " << _timeout;
        if (_grid != 0) os << " -g " << _grid;
        if (_qos != 0) os << " -q " << _qos;
        if (_weight != -1) os << " -w " << _weight;
        if (_weighttype != 0) os << " -v " << _weighttype;
		if (_authType != 0) os << " -e " << _authType;
        return os.str();
    }

    /**
     * @brief  字符串形式的端口
     * tcp:SOCK_STREAM
     *
     * udp:SOCK_DGRAM
     *
     * -h: ip
     *
     * -p: 端口
     *
     * -t: 超时时间, 毫秒
     *
     * -p 和 -t可以省略, -t默认10s
     *
     * tcp -h 127.0.0.1 -p 2345 -t 10000
     *
     * @param desc
     */
    void parse(const string &desc);

private:
    void init(const string& host, int port, int timeout, int istcp, int grid, int qos, int weight, unsigned int weighttype, int authType);

protected:
    /**
     * ip
     */
    std::string _host;

    /**
     * 端口
     */
    int         _port;

    /**
     * 超时时间
     */
    int         _timeout;

    /**
     * 类型
     */
    int         _type;

    /**
     * 路由状态
     */
    int         _grid;

    /**
     *  网络Qos的dscp值
     */
    int         _qos;

    /**
     *  节点的静态权重值
     */
    int            _weight;

    /**
     *  节点的权重使用方式
     */
    unsigned int    _weighttype;
    /**
     *  鉴权类型
     */
    int         _authType;
};

/*************************************TC_ClientSocket**************************************/

/**
* @brief 客户端socket相关操作基类
*/
class TC_ClientSocket
{
public:

    /**
    *  @brief 构造函数
     */
    TC_ClientSocket() : _port(0),_timeout(3000) {}

    /**
     * @brief 析够函数
     */
    virtual ~TC_ClientSocket(){}

    /**
    * @brief 构造函数
    * @param sIP      服务器IP
    * @param iPort    端口, port为0时:表示本地套接字此时ip为文件路径
    * @param iTimeout 超时时间, 毫秒
    */
    TC_ClientSocket(const string &sIp, int iPort, int iTimeout) { init(sIp, iPort, iTimeout); }

    /**
    * @brief 初始化函数
    * @param sIP      服务器IP
    * @param iPort    端口, port为0时:表示本地套接字此时ip为文件路径
    * @param iTimeout 超时时间, 毫秒
    */
    void init(const string &sIp, int iPort, int iTimeout)
    {
        _socket.close();
        _ip         = sIp;
        _port       = iPort;
        _timeout    = iTimeout;
    }

    /**
    * @brief 发送到服务器
    * @param sSendBuffer 发送buffer
    * @param iSendLen    发送buffer的长度
    * @return            int 0 成功,<0 失败
    */
    virtual int send(const char *sSendBuffer, size_t iSendLen) = 0;

    /**
    * @brief 从服务器返回不超过iRecvLen的字节
    * @param sRecvBuffer 接收buffer
    * @param iRecvLen    指定接收多少个字符才返回,输出接收数据的长度
    * @return            int 0 成功,<0 失败
    */
    virtual int recv(char *sRecvBuffer, size_t &iRecvLen) = 0;

    /**
    * @brief  定义发送的错误
    */
    enum
    {
        EM_SUCCESS  = 0,          /** EM_SUCCESS:发送成功*/
        EM_SEND     = -1,        /** EM_SEND:发送错误*/
        EM_SELECT   = -2,        /** EM_SELECT:select 错误*/
        EM_TIMEOUT  = -3,        /** EM_TIMEOUT:select超时*/
        EM_RECV     = -4,        /** EM_RECV: 接受错误*/
        EM_CLOSE    = -5,        /**EM_CLOSE: 服务器主动关闭*/
        EM_CONNECT  = -6,        /** EM_CONNECT : 服务器连接失败*/
        EM_SOCKET   = -7        /**EM_SOCKET : SOCKET初始化失败*/
    };

protected:
    /**
     * 套接字句柄
     */
    TC_Socket     _socket;

    /**
     * ip或文件路径
     */
    string        _ip;

    /**
     * 端口或-1:标示是本地套接字
     */
    int         _port;

    /**
     * 超时时间, 毫秒
     */
    int            _timeout;
};

/**
 * @brief TCP客户端Socket
 * 多线程使用的时候，不用多线程同时send/recv，小心串包；
 */
class TC_TCPClient : public TC_ClientSocket
{
public:
    /**
    * @brief  构造函数
     */
    TC_TCPClient(){}

    /**
    * @brief  构造函数
    * @param sIp       服务器Ip
    * @param iPort     端口
    * @param iTimeout  超时时间, 毫秒
    */
    TC_TCPClient(const string &sIp, int iPort, int iTimeout) : TC_ClientSocket(sIp, iPort, iTimeout)
    {
    }

    /**
    * @brief  发送到服务器
    * @param sSendBuffer  发送buffer
    * @param iSendLen     发送buffer的长度
    * @return             int 0 成功,<0 失败
    */
    int send(const char *sSendBuffer, size_t iSendLen);

    /**
    * @brief  从服务器返回不超过iRecvLen的字节
    * @param sRecvBuffer 接收buffer
    * @param iRecvLen    指定接收多少个字符才返回,输出接收数据的长度
    * @return            int 0 成功,<0 失败
    */
    int recv(char *sRecvBuffer, size_t &iRecvLen);

    /**
    *  @brief 从服务器直到结束符(注意必须是服务器返回的结束符,
    *         而不是中间的符号 ) 只能是同步调用
    * @param sRecvBuffer 接收buffer, 包含分隔符
    * @param sSep        分隔符
    * @return            int 0 成功,<0 失败
    */
    int recvBySep(string &sRecvBuffer, const string &sSep);

    /**
     * @brief 接收倒服务器关闭连接为止
     * @param recvBuffer
     *
     * @return int 0 成功,<0 失败
     */
    int recvAll(string &sRecvBuffer);

    /**
     * @brief  从服务器返回iRecvLen的字节
     * @param sRecvBuffer, sRecvBuffer的buffer长度必须大于等于iRecvLen
     * @param iRecvLen
     * @return int 0 成功,<0 失败
     */
    int recvLength(char *sRecvBuffer, size_t iRecvLen);

    /**
    * @brief  发送到服务器, 从服务器返回不超过iRecvLen的字节
    * @param sSendBuffer  发送buffer
    * @param iSendLen     发送buffer的长度
    * @param sRecvBuffer  接收buffer
    * @param iRecvLen     接收buffer的长度指针[in/out],
    *                     输入时表示接收buffer的大小,返回时表示接收了多少个字符
    * @return             int 0 成功,<0 失败
    */
    int sendRecv(const char* sSendBuffer, size_t iSendLen, char *sRecvBuffer, size_t &iRecvLen);

    /**
    * @brief  发送倒服务器, 并等待服务器直到结尾字符, 包含结尾字符
    * sSep必须是服务器返回的结束符,而不是中间的符号，只能是同步调用
    * (一次接收一定长度的buffer,如果末尾是sSep则返回,
    * 否则继续等待接收)
    *
    * @param sSendBuffer  发送buffer
    * @param iSendLen     发送buffer的长度
    * @param sRecvBuffer  接收buffer
    * @param sSep         结尾字符
    * @return             int 0 成功,<0 失败
    */
    int sendRecvBySep(const char* sSendBuffer, size_t iSendLen, string &sRecvBuffer, const string &sSep);

    /**
    * @brief  发送倒服务器, 并等待服务器直到结尾字符(\r\n), 包含\r\n
    * 注意必须是服务器返回的结束符,而不是中间的符号
    * 只能是同步调用
    * (一次接收一定长度的buffer,如果末尾是\r\n则返回,否则继续等待接收)
    *
    * @param sSendBuffer  发送buffer
    * @param iSendLen     发送buffer的长度
    * @param sRecvBuffer  接收buffer
    * @param sSep         结尾字符
    * @return             int 0 成功,<0 失败
    */
    int sendRecvLine(const char* sSendBuffer, size_t iSendLen, string &sRecvBuffer);

    /**
     * @brief  发送到服务器, 接收直到服务器关闭连接为止
     * 此时服务器关闭连接不作为错误
     * @param sSendBuffer
     * @param iSendLen
     * @param sRecvBuffer
     *
     * @return int
     */
    int sendRecvAll(const char* sSendBuffer, size_t iSendLen, string &sRecvBuffer);

protected:
    /**
     * @brief  获取socket
     *
     * @return int
     */
    int checkSocket();
};

/*************************************TC_TCPClient**************************************/
 /**
  * @brief  多线程使用的时候，不用多线程同时send/recv，小心串包
  */
class TC_UDPClient : public TC_ClientSocket
{
public:
    /**
    * @brief  构造函数
     */
    TC_UDPClient(){};

    /**
    * @brief  构造函数
    * @param sIp       服务器IP
    * @param iPort     端口
    * @param iTimeout  超时时间, 毫秒
    */
    TC_UDPClient(const string &sIp, int iPort, int iTimeout) : TC_ClientSocket(sIp, iPort, iTimeout)
    {
    }

    /**
     * @brief  发送数据
     * @param sSendBuffer 发送buffer
     * @param iSendLen    发送buffer的长度
     *
     * @return            int 0 成功,<0 失败
     */
    int send(const char *sSendBuffer, size_t iSendLen);

    /**
     * @brief  接收数据
     * @param sRecvBuffer  接收buffer
     * @param iRecvLen     输入/输出字段
     * @return             int 0 成功,<0 失败
     */
    int recv(char *sRecvBuffer, size_t &iRecvLen);

    /**
     * @brief  接收数据, 并返回远程的端口和ip
     * @param sRecvBuffer 接收buffer
     * @param iRecvLen    输入/输出字段
     * @param sRemoteIp   输出字段, 远程的ip
     * @param iRemotePort 输出字段, 远程端口
     *
     * @return int 0 成功,<0 失败
     */
    int recv(char *sRecvBuffer, size_t &iRecvLen, string &sRemoteIp, uint16_t &iRemotePort);

    /**
     * @brief  发送并接收数据
     * @param sSendBuffer 发送buffer
     * @param iSendLen    发送buffer的长度
     * @param sRecvBuffer 输入/输出字段
     * @param iRecvLen    输入/输出字段
     *
     * @return int 0 成功,<0 失败
     */
    int sendRecv(const char *sSendBuffer, size_t iSendLen, char *sRecvBuffer, size_t &iRecvLen);

    /**
     * @brief  发送并接收数据, 同时获取远程的ip和端口
     * @param sSendBuffer  发送buffer
     * @param iSendLen     发送buffer的长度
     * @param sRecvBuffer  输入/输出字段
     * @param iRecvLen     输入/输出字段
     * @param sRemoteIp    输出字段, 远程的ip
     * @param iRemotePort  输出字段, 远程端口
     *
     * @return int 0 成功,<0 失败
     */
    int sendRecv(const char *sSendBuffer, size_t iSendLen, char *sRecvBuffer, size_t &iRecvLen, string &sRemoteIp, uint16_t &iRemotePort);

protected:
    /**
     * @brief  获取socket
     *
     * @return TC_Socket&
     */
    int checkSocket();
};

}

#endif
