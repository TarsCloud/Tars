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

#ifndef __TC_SOCKET_H
#define __TC_SOCKET_H

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <vector>
#include <string>
#include <sys/un.h>
#include "util/tc_ex.h"
using namespace std;

namespace tars
{
/////////////////////////////////////////////////
/** 
* @file tc_socket.h 
* @brief  socket封装类. 
* 
*/
/////////////////////////////////////////////////

/**
* @brief socket异常类
*/
struct TC_Socket_Exception : public TC_Exception
{
    TC_Socket_Exception(const string &buffer) : TC_Exception(buffer){};
    TC_Socket_Exception(const string &buffer, int err) : TC_Exception(buffer, err){};
    ~TC_Socket_Exception() throw() {};
};

/**
 * @brief 建立链接异常
 */
struct TC_SocketConnect_Exception : public TC_Socket_Exception
{
    TC_SocketConnect_Exception(const string &buffer) : TC_Socket_Exception(buffer){};
    TC_SocketConnect_Exception(const string &buffer, int err) : TC_Socket_Exception(buffer, err){};
    ~TC_SocketConnect_Exception() throw() {};
};


/**
* @brief  Socket类, 封装了socket的基本方法
*/
class TC_Socket
{
public:
    /**
     * @brief  构造函数
     */
    TC_Socket();

    /**
     * @brief  析够
     */
    virtual ~TC_Socket();

    /**
     * @brief  初始化. 
     *  
     * @param fd      socket句柄
     * @param bOwner  是否拥有socket
     * @param iDomain sokect协议族，缺省为AF_INET，代表TCP/IP协议族 
     */
    void init(int fd, bool bOwner, int iDomain = AF_INET);

    /**
     * @brief 设置是否拥有该fd. 
     *  
     * @param bOwner ture表示拥有，否则设置为false
     */
    void setOwner(bool bOwner)  { _bOwner = bOwner; }

    /**
     * @brief 置设套接字类型 
     *  
     * @param iDomain 要设置的套接字的类型 
     *               AF_INET：TCP/IP协议族
     *               AF_LOCAL：
     */
    void setDomain(int iDomain) { _iDomain = iDomain; }

    /**
    * @brief  生成socket, 如果已经存在以前的socket, 则释放掉, 
    *         生成新的.
    *  
    * @param iDomain      socket方式SOCK_STREAM|SOCK_DGRAM
    * @param iSocketType  socket类型，缺省AF_INET，TCP/IP族
    * @throws             TC_Socket_Exception
    * @return
    */
    void createSocket(int iSocketType = SOCK_STREAM, int iDomain = AF_INET);

    /**
    * @brief 获取socket句柄. 
    *  
    * @return  socket句柄
    */
    int getfd() const { return _sock; }

    /**
    * @brief  socket是否有效. 
    *  
    * @return true标识有效，否则返回false
    */
    bool isValid() const { return _sock != INVALID_SOCKET; }

    /**
    * @brief  关闭socket. 
    *  
    * @return void
    */
    void close();

    /**
    * @brief  获取对点的ip和端口,对AF_INET的socket有效. 
    *  
    * @param sPeerAddress  对点的ip地址
    * @param iPeerPort     对点的端口地址
    * @throws              TC_Socket_Exception
    * @return
    */
    void getPeerName(string &sPeerAddress, uint16_t &iPeerPort);

    /**
    * @brief  获取对点的ip和端口,对AF_LOCAL的socket有效. 
    *  
    * @param sPathName  文件路径
    * @throws           TC_Socket_Exception
    * @return
    */
    void getPeerName(string &sPathName);

    /**
    * @brief  获取自己的ip和端口,对AF_INET的socket有效. 
    *  
    * @param sSockAddress  ip地址
    * @param iSockPort     端口地址
    * @throws              TC_Socket_Exception
    * @return
    */
    void getSockName(string &sSockAddress, uint16_t &iSockPort);

    /**
     * @brief  获取socket文件路径,对AF_LOCAL的socket有效. 
     *  
     * @param sPathName
     * @param TC_Socket_Exception
     */
    void getSockName(string &sPathName);

    /**
    * @brief  修改socket选项. 
    *  
    * @param opt       选项名称
    * @param pvOptVal  选项值指针
    * @param optLen    pvOptVal对应的长度
    * @param level     socket操作层次, 默认是socket层
    * @return int
    */
    int setSockOpt(int opt, const void *pvOptVal, socklen_t optLen, int level = SOL_SOCKET);

    /**
    * @brief  获取socket选项值. 
    *  
    * @param opt       选项名称
    * @param pvOptVal  输出, 选项值指针
    * @param optLen    输入pvOptVal指向的缓存的长度
    * @param level     socket操作层次, 默认是socket层
    * @return          socket选项值
    */
    int getSockOpt(int opt, void *pvOptVal, socklen_t &optLen, int level = SOL_SOCKET);

    /**
    * @brief  accept. 
    *  
    * @param tcSock       客户端socket结构
    * @param pstSockAddr  客户端地址
    * @param iSockLen     pstSockAddr长度
    * @return             int : > 0 ,客户端socket; <0, 出错
    */
    int accept(TC_Socket &tcSock, struct sockaddr *pstSockAddr, socklen_t &iSockLen);

    /**
    * @brief 绑定,对AF_INET的socket有效. 
    *  
    * @param port          端口
    * @param sServerAddr   服务器地址
    * @throws              TC_Socket_Exception
    * @return 
    */
    void bind(const string &sServerAddr, int port);

    /**
     * @brief  绑定域套接字,对AF_LOCAL的socket有效. 
     *  
     * @param sPathName
     */
    void bind(const char *sPathName);

    /**
    * @brief  连接其他服务,对AF_INET的socket有效(同步连接). 
    *  
    * @param sServerAddr  ip地址
    * @param port         端口
    * @throws             TC_Socket_Exception
    * @return
    */
    void connect(const string &sServerAddr, uint16_t port);

    /**
     * @brief 连接本地套接字,对AF_LOCAL的socket有效(同步连接). 
     *  
     * @param sPathName
     * @throws TC_Socket_Exception
     */
    void connect(const char *sPathName);

    /**
     * @brief 发起连接，连接失败的状态不通过异常返回, 
     *        通过connect的返回值,在异步连接的时候需要
     * @param sServerAddr  ip地址
     * @param port         端口
     * @throws             TC_Socket_Exception: 
     *                     其他错误还是通过异常返回(例如),例如地址错误
     * @return int
     */
    int connectNoThrow(const string &sServerAddr, uint16_t port);


    /**
     * @brief 发起连接，连接失败的状态不通过异常返回, 
     *        通过connect的返回值,在异步连接的时候需要
     * @param addr socket直接可用的地址
     * @throws TC_Socket_Exception  
     *        其他错误还是通过异常返回(例如),例如地址错误
     * @return int
     */
    int connectNoThrow(struct sockaddr* addr);

    /**
     * @brief 发起连接，连接失败的状态不通过异常返回, 
     *        通过connect的返回值,在异步连接的时候需要
     * @param sPathName
     * @throws TC_Socket_Exception:其他错误还是通过异常返回(例如),例如地址错误
     * @return int
     */
    int connectNoThrow(const char *sPathName);

    /**
    * @brief 在socket上监听. 
    *  
    * @param connBackLog  连接队列个数
    * @throws             TC_Socket_Exception
    */
    void listen(int connBackLog);

    /**
    * @brief  接收数据(一般用于tcp). 
    *  
    * @param pvBuf  接收buffer
    * @param iLen   buffer长度
    * @param iFlag  标示
    * @return int   接收的数据长度
    */
    int  recv(void *pvBuf, size_t iLen, int iFlag = 0);

    /**
    * @brief  发送数据(一般用于tcp). 
    *  
    * @param pvBuf 发送buffer
    * @param iLen  buffer长度
    * @param iFlag 标示
    * @return int  发送了的数据长度
    */
    int  send(const void *pvBuf, size_t iLen, int iFlag = 0);

    /**
    * @brief  接收数据(一般用于udp). 
    *  
    * @param pvBuf      发送buffer
    * @param iLen        buffer长度
    * @param sFromAddr  输出, 服务端ip地址
    * @param iFromPort  输出, 服务端端口
    * @param iFlag      标示
    * @return int       接收了的数据长度
    */
    int recvfrom(void *pvBuf, size_t iLen, string &sFromAddr, uint16_t &iFromPort, int iFlags = 0);

    /**
    * @brief  接收数据(一般用于udp). 
    *  
    * @param pvBuf      发送buffer
    * @param iLen        buffer长度
    * @param pstFromAddr 地址
    * @param iFromLen    输出, pstFromAddr长度
    * @param iFlag       标示
    * @return int        接收了的数据长度
    */
    int recvfrom(void *pvBuf, size_t iLen, struct sockaddr *pstFromAddr, socklen_t &iFromLen, int iFlags = 0);

    /**
    * @brief 发送数据(一般用于udp). 
    *  
    * @param pvBuf    发送buffer
    * @param iLen     buffer长度
    * @param sToAddr  服务端ip地址, 如果sToAddr为空, 则udp广播
    * @param iToPort  服务端端口
    * @param iFlag    标示
    * @return          int : >0 发送的数据长度 ;<=0, 出错
    */
    int sendto(const void *pvBuf, size_t iLen, const string &sToAddr, uint16_t iToPort, int iFlags = 0);

    /**
    * @brief  发送数据(一般用于udp). 
    *  
    * @param pvBuf       发送buffer
    * @param iLen        buffer长度
    * @param pstToAddr   服务端地址
    * @param iToLen      pstToAddr长度
    * @param iFlag      标示
    * @return           int : >0 发送的数据长度 ;<=0, 出错
    */
    int sendto(const void *pvBuf, size_t iLen, struct sockaddr *pstToAddr, socklen_t iToLen, int iFlags = 0);

    /**
    * @brief 关闭. 
    *  
    * @param iHow 关闭方式:SHUT_RD|SHUT_WR|SHUT_RDWR
    * @throws     TC_Socket_Exception
    * @return
    */
    void shutdown(int iHow);

    /**
    * @brief 设置socket方式 .
    *  
    * @param   bBlock true, 阻塞; false, 非阻塞
    * @throws  TC_Socket_Exception
    * @return
    */
    void setblock(bool bBlock = false);

    /**
    * @brief 设置非closewait状态, 可以重用socket. 
    *  
    * @throws TC_Socket_Exception
    * @return void
    */
    void setNoCloseWait();

    /**
     * @brief 设置为closewait状态, 最常等待多久. 
     *  
     * @param delay  等待时间秒
     * @throws       TC_Socket_Exception
     */
    void setCloseWait(int delay = 30);

    /**
     * @brief 设置closewait缺省状态， 
     *        close以后马上返回并尽量把数据发送出去
     * @throws TC_Socket_Exception
     */
    void setCloseWaitDefault();

    /**
     * @brief 设置nodelay(只有在打开keepalive才有效).
     *  
     * @throws TC_Socket_Exception
     */
    void setTcpNoDelay();

    /**
     * @brief 设置keepalive. 
     *  
     * @throws TC_Socket_Exception
     * @return 
     */
    void setKeepAlive();

    /**
    * @brief 获取recv buffer 大小. 
    *  
    * @throws TC_Socket_Exception
    * @return recv buffer 的大小
    */
    int getRecvBufferSize();

    /**
    * @brief 设置recv buffer 大小. 
    * @param  recv buffer 大小 
    * @throws TC_Socket_Exception 
    */
    void setRecvBufferSize(int sz);

    /**
    * @brief 获取发送buffer大小.
    * @param 发送buffer大小  
    * @throws TC_Socket_Exception 
     */
    int getSendBufferSize();

    /**
     * @brief 设置发送buffer大小. 
     *  
     * @throws TC_Socket_Exception
     * @param  发送buffer大小
     */
    void setSendBufferSize(int sz);

    /**
     * @brief  获取本地所有ip.
     * 
     * @throws TC_Socket_Exception
     * @return 本地所有ip
     */
    static vector<string> getLocalHosts();

    /**
    * @brief 设置socket方式. 
    *  
    * @param fd      句柄
    * @param bBlock  true, 阻塞; false, 非阻塞
    * @throws        TC_Socket_Exception
    * @return
    */
    static void setblock(int fd, bool bBlock);

    /**
     * @brief 生成管道,抛出异常时会关闭fd. 
     *  
     * @param fds    句柄
     * @param bBlock true, 阻塞; false, 非阻塞
     * @throws       TC_Socket_Exception
     */
    static void createPipe(int fds[2], bool bBlock);

    /**
    * @brief 解析地址, 从字符串(ip或域名), 解析到in_addr结构. 
    *  
    * @param sAddr   字符串
    * @param stAddr  地址
    * @throws        TC_Socket_Exception
    * @return
    */
    static void parseAddr(const string &sAddr, struct in_addr &stAddr);

    /**
    * @brief 绑定. 
    *  
    * @param pstBindAddr  需要绑定的地址
    * @param iAddrLen      pstBindAddr指向的结构的长度
    * @throws              TC_Socket_Exception
    * @return
    */
    void bind(struct sockaddr *pstBindAddr, socklen_t iAddrLen);

    #if 0
    /**
    * @brief no implementation. 
    *  
    */
    TC_Socket(const TC_Socket &tcSock);

    /**
    * @brief  no implementation.
    */
    TC_Socket& operator=(const TC_Socket &tcSock);
    #endif


protected:

    /**
    * @brief 连接其他服务. 
    *  
    * @param pstServerAddr  服务地址
    * @param serverLen      pstServerAddr指向的结构的长度
    * @return int
    */
    int connect(struct sockaddr *pstServerAddr, socklen_t serverLen);    

    /**
    * @brief 获取对点的地址. 
    *  
    * @param pstPeerAddr 地址指针
    * @param iPeerLen    pstPeerAddr指向的结构长度
    * @throws            TC_Socket_Exception
    * @return 
    */
    void getPeerName(struct sockaddr *pstPeerAddr, socklen_t &iPeerLen);

    /**
    * @brief 获取自己的的ip和端口. 
    *  
    * @param pstSockAddr 地址指针
    * @param iSockLen    pstSockAddr
    * @throws            TC_Socket_Exception
    * @return
    */
    void getSockName(struct sockaddr *pstSockAddr, socklen_t &iSockLen);

private:

protected:
    static const int INVALID_SOCKET = -1;

    /**
     * socket句柄
     */
    int  _sock;

    /**
     * 是否拥有socket
     */
    bool _bOwner;

    /**
     * socket类型
     */
    int  _iDomain;
};

}
#endif
