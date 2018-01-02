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

#ifndef __TARS_TC_EPOLL_SERVER_H_
#define __TARS_TC_EPOLL_SERVER_H_

#include <string>
#include <memory>
#include <map>
#include <vector>
#include <list>
#include <algorithm>
#include "util/tc_epoller.h"
#include "util/tc_thread.h"
#include "util/tc_clientsocket.h"
#include "util/tc_logger.h"
#include "util/tc_shm.h"
#include "util/tc_common.h"
#include "util/tc_mem_queue.h"
#include "util/tc_squeue.h"
#include "util/tc_mmap.h"
#include "util/tc_fifo.h"
#include "util/tc_buffer.h"
#include "util/tc_buffer_pool.h"

using namespace std;

namespace tars
{

#if TARS_SSL
    class TC_OpenSSL;
#endif

/////////////////////////////////////////////////
/**
 * @file  tc_epoll_server.h
 * @brief  EpollServer类
 *
 */
/////////////////////////////////////////////////
/**
 * Server基类
 * 注册协议解析器
 * 注册逻辑处理器
 * 注册管理端口处理器
 */

class PropertyReport;

class TC_EpollServer : public TC_ThreadLock, public TC_HandleBase
{
public:
    ////////////////////////////////////////////////////////////////////////////
    /**
     * 定义协议解析的返回值
     */
    enum
    {
        PACKET_LESS = 0,
        PACKET_FULL = 1,
        PACKET_ERR  = -1,
    };

    enum EM_CLOSE_T
    {
        EM_CLIENT_CLOSE = 0,         //客户端主动关闭
        EM_SERVER_CLOSE = 1,        //服务端业务主动调用close关闭连接,或者框架因某种异常主动关闭连接
        EM_SERVER_TIMEOUT_CLOSE = 2  //连接超时了，服务端主动关闭
    };

    /**
     * 定义协议解析接口的操作对象
     * 注意必须是线程安全的或是可以重入的
     */
    typedef TC_Functor<int, TL::TLMaker<string &, string&>::Result> protocol_functor;
    typedef TC_Functor<int, TL::TLMaker<int, string&>::Result>      header_filter_functor;

    class NetThread;

    class BindAdapter;
    typedef TC_AutoPtr<BindAdapter> BindAdapterPtr;

    class Handle;
    typedef TC_AutoPtr<Handle> HandlePtr;

    class HandleGroup;
    typedef TC_AutoPtr<HandleGroup> HandleGroupPtr;


    ////////////////////////////////////////////////////////////////////////////
    /**定义数据队列中的结构*/
    struct tagRecvData
    {
        uint32_t        uid;            /**连接标示*/
        string          buffer;         /**需要发送的内容*/
        string          ip;             /**远程连接的ip*/
        uint16_t        port;           /**远程连接的端口*/
        int64_t         recvTimeStamp;  /**接收到数据的时间*/
        bool            isOverload;     /**是否已过载 */
        bool            isClosed;       /**是否已关闭*/
        int                fd;                /*保存产生该消息的fd，用于回包时选择网络线程*/
        BindAdapterPtr  adapter;        /**标识哪一个adapter的消息*/
        int             closeType;     /*如果是关闭消息包，则标识关闭类型,0:表示客户端主动关闭；1:服务端主动关闭;2:连接超时服务端主动关闭*/
    };

    struct tagSendData
    {
        char            cmd;            /**命令:'c',关闭fd; 's',有数据需要发送*/
        uint32_t        uid;            /**连接标示*/
        string          buffer;         /**需要发送的内容*/
        string          ip;             /**远程连接的ip*/
        uint16_t        port;           /**远程连接的端口*/
    };

    typedef TC_ThreadQueue<tagRecvData*, deque<tagRecvData*> > recv_queue;
    typedef TC_ThreadQueue<tagSendData*, deque<tagSendData*> > send_queue;
    typedef recv_queue::queue_type recv_queue_type;

    ////////////////////////////////////////////////////////////////////////////
    /**
     * 链接状态
     */
    struct ConnStatus
    {
        string          ip;
        int32_t         uid;
        uint16_t        port;
        int             timeout;
        int             iLastRefreshTime;
    };
    ////////////////////////////////////////////////////////////////////////////
    /**
     * 按name对handle分组，
     * 每组handle处理一个或多个Adapter消息
     * 每个handle对象一个线程
     */
    struct HandleGroup : public TC_HandleBase
    {
        string                      name;
        TC_ThreadLock               monitor;
        vector<HandlePtr>           handles;
        map<string, BindAdapterPtr> adapters;
    };
    ////////////////////////////////////////////////////////////////////////////
    /**
     * @brief 定义服务逻辑处理的接口
     *
     */
    /**
     * 服务的逻辑处理代码
     */
    class Handle : public TC_Thread, public TC_ThreadLock, public TC_HandleBase
    {
    public:
        /**
         * 构造, 默认没有请求, 等待10s
         */
        Handle();

        /**
         * 析构函数
         */
        virtual ~Handle();

        /**
         * 设置服务
         * @param pEpollServer
         */
        void setEpollServer(TC_EpollServer *pEpollServer);

        /**
         * 获取服务
         * @return TC_EpollServer*
         */
        TC_EpollServer* getEpollServer();

        /**
         * 设置所属的Group
         * @param pHandleGroup
         */
        void setHandleGroup(HandleGroupPtr& pHandleGroup);

        /**
         * 获取所属Group
         * @return HandleGroup*
         */
        HandleGroupPtr& getHandleGroup();

        /**
         * 线程处理方法
         */
        virtual void run();

    public:
        /**
         * 发送数据
         * @param stRecvData
         * @param sSendBuffer
         */
        void sendResponse(unsigned int uid, const string &sSendBuffer, const string &ip, int port, int fd);

        /**
         * 关闭链接
         * @param stRecvData
         */
        void close(unsigned int uid, int fd);

        /**
         * 设置等待时间
         * @param iWaitTime
         */
        void setWaitTime(uint32_t iWaitTime);

        /**
         * 对象初始化
         */
        virtual void initialize() {};

        /**
         * 唤醒HandleGroup中的handle线程
         */
        virtual void notifyFilter();

    protected:
        /**
         * 具体的处理逻辑
         */
        virtual void handleImp();

        /**
         * 处理函数
         * @param stRecvData: 接收到的数据
         */
        virtual void handle(const tagRecvData &stRecvData) = 0;

        /**
         * 处理超时数据, 即数据在队列中的时间已经超过
         * 默认直接关闭连接
         * @param stRecvData: 接收到的数据
         */
        virtual void handleTimeout(const tagRecvData &stRecvData);

        /**
         * 处理连接关闭通知，包括
         * 1.close by peer
         * 2.recv/send fail
         * 3.close by timeout or overload
         * @param stRecvData:
         */
        virtual void handleClose(const tagRecvData &stRecvData);

        /**
         * 处理overload数据 即数据队列中长度已经超过允许值
         * 默认直接关闭连接
         * @param stRecvData: 接收到的数据
         */
        virtual void handleOverload(const tagRecvData &stRecvData);

        /**
         * 处理Filter的消息
         */
        //virtual void handleFilter() {};
        /**
         * 处理异步回调队列
         */
        virtual void handleAsyncResponse() {}
           /**
         * handleFilter拆分的第二部分，处理用户自有数据
         * 非游戏逻辑可忽略bExpectIdle参数
         */
        virtual void handleCustomMessage(bool bExpectIdle = false) {}

        /**
         * 心跳(每处理完一个请求或者等待请求超时都会调用一次)
         */
        virtual void heartbeat() {}

        /**
         * 线程已经启动, 进入具体处理前调用
         */
        virtual void startHandle() {}

        /**
         * 线程马上要退出时调用
         */
        virtual void stopHandle() {}

        /**
         * 是否所有的Adpater队列都为空
         * @return bool
         */
        virtual bool allAdapterIsEmpty();

        /**
         * 是否所有的servant都没有resp消息待处理
         * @return bool
         */
        virtual bool allFilterIsEmpty();

        /**
         * 友元类
         */
        friend class BindAdapter;
    protected:
        /**
         * 服务
         */
        TC_EpollServer  *_pEpollServer;

        /**
         * 所属handle组
         */
        HandleGroupPtr _handleGroup;

        /**
         * 等待时间
         */
        uint32_t  _iWaitTime;

    };

    typedef TC_Functor<bool /*processed*/, TL::TLMaker<void* /*conn*/, const std::string& /*data*/ >::Result> auth_process_wrapper_functor;

    ////////////////////////////////////////////////////////////////////////////
    // 服务端口管理,监听socket信息
    class BindAdapter : public TC_ThreadLock, public TC_HandleBase
    {
    public:
        /**
         * 缺省的一些定义
         */
        enum
        {
            DEFAULT_QUEUE_CAP       = 10*1024,    /**流量*/
            MIN_QUEUE_TIMEOUT       = 3*1000,     /**队列最小超时时间(ms)*/
            DEFAULT_MAX_CONN        = 1024,       /**缺省最大连接数*/
            DEFAULT_QUEUE_TIMEOUT   = 60*1000,    /**缺省的队列超时时间(ms)*/
        };
        /**
         * 顺序
         */
        enum EOrder
        {
            ALLOW_DENY,
            DENY_ALLOW
        };


        BindAdapter()
        {
            _pReportQueue = NULL;
            _pReportConRate = NULL;
            _pReportTimeoutNum = NULL;
        }

        /**
         * 构造函数
         */
        BindAdapter(TC_EpollServer *pEpollServer);

        /**
         * 析够函数
         */
        ~BindAdapter();

        /**
         * 设置adapter name
         * @param name
         */
        void setName(const string &name);

        /**
         * 获取adapter name
         * @return string
         */
        string getName() const;

        /**
         * 获取queue capacity
         * @return int
         */
        int getQueueCapacity() const;

        /**
         * 设置queue capacity
         * @param n
         */
        void setQueueCapacity(int n);

        /**
         * 设置协议名称
         * @param name
         */
        void setProtocolName(const string& name);

        /**
         * 获取协议名称
         * @return const string&
         */
        const string& getProtocolName();

        /**
         * 是否tars协议
         * @return bool
         */
        bool isTarsProtocol();

        /**
         * 判断是否需要过载保护
         * @return bool
         */
        int isOverloadorDiscard();

        /**
         * 设置消息在队列中的超时时间, t为毫秒
         * (超时时间精度只能是s)
         * @param t
         */
        void setQueueTimeout(int t);

        /**
         * 获取消息在队列中的超时时间, 毫秒
         * @return int
         */
        int getQueueTimeout() const;

        /**
         * 设置endpoint
         * @param str
         */
        void setEndpoint(const string &str);

        /**
         * 获取ip
         * @return const string&
         */
        TC_Endpoint getEndpoint() const;

        /**
         * 监听socket
         * @return TC_Socket
         */
        TC_Socket &getSocket();

        /**
         * 设置最大连接数
         * @param iMaxConns
         */
        void setMaxConns(int iMaxConns);

        /**
         * 获取最大连接数
         * @return size_t
         */
        size_t getMaxConns() const;

        /**
         * 设置HeartBeat时间
         * @param n
         */
        void setHeartBeatTime(time_t t);

        /**
         * 获取HeartBeat时间
         * @return size_t
         */
        time_t getHeartBeatTime() const;

        /**
         * 设置allow deny次序
         * @param eOrder
         */
        void setOrder(EOrder eOrder);

        /**
         * 设置允许ip
         * @param vtAllow
         */
        void setAllow(const vector<string> &vtAllow);

        /**
         * 设置禁止ip
         * @param vtDeny
         */
        void setDeny(const vector<string> &vtDeny);

        /**
         * 获取允许ip
         * @return vector<string>: ip列表
         */
        vector<string> getAllow() const;

         /**
         * 获取禁止ip
         * @return vector<string>: ip列表
         */
        vector<string> getDeny() const;

         /**
         * 获取allow deny次序
         * @return EOrder
         */
        EOrder getOrder() const;

        /**
         * 是否Ip被允许
         * @param ip
         * @return bool
         */
        bool isIpAllow(const string& ip) const;

        /**
         * 是否超过了最大连接数
         * @return bool
         */
        bool isLimitMaxConnection() const;

        /**
         * 减少当前连接数
         */
        void decreaseNowConnection();

        /**
         * 减少当前连接数
         */
        void increaseNowConnection();

        /**
         * 获取所有链接状态
         * @return ConnStatus
         */
        vector<ConnStatus> getConnStatus();

        /**
         * 获取当前连接数
         * @return size_t
         */
        size_t getNowConnection() const;

        /**
         * 获取EpollServer
         * @return TC_EpollServer*
         */
        TC_EpollServer* getEpollServer();

        /**
         * 注册协议解析器
         * @param pp
         */
        void setProtocol(const protocol_functor& pf, int iHeaderLen = 0, const header_filter_functor& hf = echo_header_filter);

        /**
         * 获取协议解析器
         * @return protocol_functor&
         */
        protocol_functor &getProtocol();

        /**
         * 解析包头处理对象
         * @return protocol_functor&
         */
        header_filter_functor &getHeaderFilterFunctor();

        /**
         * 增加数据到队列中
         * @param vtRecvData
         * @param bPushBack 后端插入
         * @param sBuffer
         */
        void insertRecvQueue(const recv_queue::queue_type &vtRecvData,bool bPushBack = true);

        /**
         * 通知等待在接收队列上面的线程醒过来
         */
        void notifyRecvQueue();

        /**
         * 等待数据
         * @return bool
         */
        bool waitForRecvQueue(tagRecvData* &recv, uint32_t iWaitTime);

        /**
         * 接收队列的大小
         * @return size_t
         */
        size_t getRecvBufferSize();

        /**
         * 默认的协议解析类, 直接echo
         * @param r
         * @param o
         * @return int
         */
        static int echo_protocol(string &r, string &o);

        /**
         * 默认的包头处理
         * @param i
         * @param o
         * @return int
         */
        static int echo_header_filter(int i, string &o);

        /**
         * 获取需要过滤的包头长度
         */
        int getHeaderFilterLen();

        /**
         * 设置所属的handle组名
         * @param handleGroupName
         */
        void setHandleGroupName(const string& handleGroupName);

        /**
         * 获得所属的handle组名
         * @return string
         */
        string getHandleGroupName() const;

        /**
         * 获得所属的handle
         * @return HandleGroupPtr
         */

        HandleGroupPtr getHandleGroup() const
        {
            return _handleGroup;
        }

        /**
         * 设置ServantHandle数目
         * @param n
         */
        void setHandleNum(int n);

        /**
         * 所属handle组的handle数(每个handle一个对象)
         * @return int
         */
        int getHandleNum();

        /**
         * 绑定两个Adapter到同一个Group
         * @param otherAdapter
         */
        void setHandle(BindAdapterPtr& otherAdapter)
        {
            _pEpollServer->setHandleGroup(otherAdapter->getHandleGroupName(), this);
        }

        /**
         * 初始化处理线程,线程将会启动
         */
        template<typename T> void setHandle()
        {
            _pEpollServer->setHandleGroup<T>(_handleGroupName, _iHandleNum, this);
        }

        /**
         * 设置服务端回包缓存的大小限制
         */
        void setBackPacketBuffLimit(size_t iLimitSize);

        /**
         * 获取服务端回包缓存的大小限制
         */
        size_t getBackPacketBuffLimit();

        /**
         * 注册鉴权包裹函数
         * @param apwf
         */
        void setAuthProcessWrapper(const auth_process_wrapper_functor& apwf) { _authWrapper = apwf; }

        void setAkSk(const std::string& ak, const std::string& sk) { _accessKey = ak; _secretKey = sk; }

        bool checkAkSk(const std::string& ak, const std::string& sk) { return ak == _accessKey && sk == _secretKey; }

        std::string getSk(const std::string& ak) const { return (_accessKey == ak) ? _secretKey : ""; }

    public:

        //统计上报的对象
        PropertyReport * _pReportQueue;
        PropertyReport * _pReportConRate;
        PropertyReport * _pReportTimeoutNum;

    protected:
        friend class TC_EpollServer;
        friend class NetThread;

        /**
         * 服务
         */
        TC_EpollServer  *_pEpollServer;

        /**
         * Adapter所用的HandleGroup
         */
        HandleGroupPtr  _handleGroup;

        /**
         * 协议解析
         */
        protocol_functor _pf;

        /**
         * 首个数据包包头过滤
         */
        header_filter_functor _hf;

        /**
         * adapter的名字
         */
        string          _name;

        /**
         * handle分组名称
         */
        string      _handleGroupName;

        /**
         * 监听fd
         */
        TC_Socket       _s;

        /**
         * 绑定的IP
         */
        TC_Endpoint     _ep;

        /**
         * 最大连接数
         */
        int             _iMaxConns;

        /**
         * 当前连接数
         */
        TC_Atomic      _iCurConns;

        /**
         * Handle个数
         */
        size_t          _iHandleNum;

        /**
         * 允许的Order
         */
        volatile EOrder _eOrder;

        /**
         * 允许的ip
         */
        vector<string>  _vtAllow;

        /**
         * 禁止的ip
         */
        vector<string>  _vtDeny;

        /**
         * 接收的数据队列
         */
        recv_queue      _rbuffer;

        /**
         * 队列最大容量
         */
        int             _iQueueCapacity;

        /**
         * 消息超时时间（从入队列到出队列间隔)(毫秒）
         */
        int             _iQueueTimeout;

        /**
         * 首个数据包包头长度
         */
        int             _iHeaderLen;

        /**
         * 上次心跳发送时间
         */
        volatile time_t          _iHeartBeatTime;

        /**
         * 协议名称,缺省为"tars"
         */
        string          _protocolName;

        //回包缓存限制大小
        size_t                    _iBackPacketBuffLimit;

        /**
         * 包裹认证函数,不能为空
         */
        auth_process_wrapper_functor _authWrapper;

        /**
         * 该obj的AK SK
         */
        std::string                 _accessKey;
        std::string                 _secretKey;
    };

    ////////////////////////////////////////////////////////////////////////////
    class NetThread : public TC_Thread, public TC_ThreadLock, public TC_HandleBase
    {
    public:
        
        ////////////////////////////////////////////////////////////////////////////
        // 服务连接管理
        /**
         *  建立连接的socket信息
         */
        class Connection
        {
        public:
            enum EnumConnectionType
            {
                EM_TCP = 0,
                EM_UDP = 1,
            };

            /**
             * 构造函数
             * @param lfd
             * @param s
             * @param ip
             * @param port
             */
            Connection(BindAdapter *pBindAdapter, int lfd, int timeout, int fd, const string& ip, uint16_t port);

            /**
             * udp连接
             * @param fd
             */
            Connection(BindAdapter *pBindAdapter, int fd);
            /**
             * 通讯组件初始化
             */
            Connection(BindAdapter *pBindAdapter);

            /**
             * 析构函数
             */
           virtual ~Connection();

            /**
             * 链接所属的adapter
             */
            BindAdapter* getBindAdapter()       { return _pBindAdapter; }

            /**
             * 初始化
             * @param id, 连接的唯一id
             */
            void init(unsigned int uid)         { _uid = uid; }

            /**
             * 获取连接超时时间
             *
             * @return int
             */
            int getTimeout() const              { return _timeout; }

            /**
             * 获取线程的惟一id
             *
             * @return unsigned int
             */
            uint32_t getId() const              { return _uid; }

            /**
             * 获取监听fd
             *
             * @return int
             */
            int getListenfd() const             { return _lfd; }

            /**
             * 当前连接fd
             *
             * @return int
             */
            virtual int getfd() const                   { return _sock.getfd(); }

            /**
             * 是否有效
             *
             * @return bool
             */
            bool isValid() const                { return _sock.isValid();}

            /**
             * 远程IP
             *
             * @return string
             */
            string getIp() const                { return _ip; }

            /**
             * 远程端口
             *
             * @return uint16_t
             */
            uint16_t getPort() const            { return _port; }

            /**
             * 设置首个数据包包头需要过滤的字节数
             */
            void setHeaderFilterLen(int iHeaderLen)     { _iHeaderLen = iHeaderLen; }

            /**
             * 设置关闭,发送完当前数据就关闭连接
             */
            bool setClose();//                             { _bClose = true; return _sendbuffer.empty(); }

            /**
             * 获取连接类型
             */
            EnumConnectionType getType() const          { return _enType; }

            bool IsEmptyConn() const  {return _bEmptyConn;}

            /**
             * Init Auth State;
             */
            void tryInitAuthState(int initState);

        protected:
            /**
             * 关闭连接
             * @param fd
             */
            void close();

            /**
             * 添加发送buffer
             * @param buffer
             * @return int, -1:发送出错, 0:无数据, 1:发送完毕, 2:还有数据
             */
            virtual int send(const string& buffer, const string &ip, uint16_t port, bool byEpollout = false);

            /**
             * 发送数据
             *
             * @return int
             */
             virtual int send();

             /**
              * 发送buffer-slices
              * @param slices
              * @return int, -1:发送出错, >= 0:发送的字节数
              */
             int send(const std::vector<TC_Slice>& slices);


            /**
             * 读取数据
             * @param fd
             * @return int, -1:接收出错, 0:接收不全, 1:接收到一个完整包
             */
            virtual int recv(recv_queue::queue_type &o);

            /**
             * 解析协议
             * @param o
             * @return int: <0:协议错误, 0:没有一个完整的包, 1:收到至少一个包
             */
            int parseProtocol(recv_queue::queue_type &o);

            /**
             * 增加数据到队列中
             * @param vtRecvData
             */

            void insertRecvQueue(recv_queue::queue_type &vRecvData);

            /**
             * 对于udp方式的连接，分配指定大小的接收缓冲区
             *@param nSize
             */
            bool setRecvBuffer(size_t nSize=DEFAULT_RECV_BUFFERSIZE);

            friend class NetThread;

        private:
            /**
             * tcp发送数据
             */
            int tcpSend(const void* data, size_t len);
            int tcpWriteV(const std::vector<iovec>& buffers);

            /**
             * 清空buffer-slices
             * @param slices
             */
            void clearSlices(std::vector<TC_Slice>& slices);

            /**
             * 整理buffer-slices
             * @param slices
             * @param toSkippedBytes 
             */
            void adjustSlices(std::vector<TC_Slice>& slices, size_t toSkippedBytes);

        public:
            /**
             * 最后刷新时间
             */
            time_t              _iLastRefreshTime;

        protected:

            /**
             * 适配器
             */
            BindAdapter         *_pBindAdapter;

            /**
             * TC_Socket
             */
            TC_Socket           _sock;

            /**
             * 连接的唯一编号
             */
            volatile uint32_t   _uid;

            /**
             * 监听的socket
             */
            int                 _lfd;

            /**
             * 超时时间
             */
            int                 _timeout;

            /**
             * ip
             */
            string              _ip;

            /**
             * 端口
             */
            uint16_t             _port;

            /**
             * 接收数据buffer
             */
            string              _recvbuffer;

            /**
             * 发送数据buffer
             */
            std::vector<TC_Slice>  _sendbuffer;

            /**
             * 需要过滤的头部字节数
             */
            int                 _iHeaderLen;

            /**
             * 发送完当前数据就关闭连接
             */
            bool                _bClose;

            /**
            * 临时队列的最大长度
            */
            int                 _iMaxTemQueueSize;
            /**
             * 连接类型
             */
            EnumConnectionType  _enType;

            bool                _bEmptyConn;

            /*
             *接收数据的临时buffer,加这个目的是对udp接收数据包大小进行设置
             */
            char                *_pRecvBuffer;

            size_t                _nRecvBufferSize;
        public:
            /*
             *该连接的鉴权状态
             */
            int                 _authState;
            /*
             *该连接的鉴权状态是否初始化了
             */
            bool                _authInit;
#if TARS_SSL
            TC_OpenSSL*         _openssl;
#endif
        };
        ////////////////////////////////////////////////////////////////////////////
        /**
         * 带有时间链表的map
         */
        class ConnectionList : public TC_ThreadLock
        {
        public:
            /**
             * 构造函数
             */
            ConnectionList(NetThread *pEpollServer);

            /**
             * 析够函数
             */
            ~ConnectionList() { if(_vConn) { delete[] _vConn; } }

            /**
             * 初始化大小
             * @param size
             */
            void init(uint32_t size, uint32_t iIndex = 0);

            /**
             * 获取惟一ID
             *
             * @return unsigned int
             */
            uint32_t getUniqId();

            /**
             * 添加连接
             * @param cPtr
             * @param iTimeOutStamp
             */
            void add(Connection *cPtr, time_t iTimeOutStamp);

            /**
             * 刷新时间链
             * @param uid
             * @param iTimeOutStamp, 超时时间点
             */
            void refresh(uint32_t uid, time_t iTimeOutStamp);

            /**
             * 检查超时数据
             */
            void checkTimeout(time_t iCurTime);

            /**
             * 获取某个监听端口的连接
             * @param lfd
             * @return vector<TC_EpollServer::ConnStatus>
             */
            vector<ConnStatus> getConnStatus(int lfd);

            /**
             * 获取某一个连接
             * @param p
             * @return T
             */
            Connection* get(uint32_t uid);

            /**
             * 删除连接
             * @param uid
             */
            void del(uint32_t uid);

            /**
             * 大小
             * @return size_t
             */
            size_t size();

        protected:
            typedef pair<Connection*, multimap<time_t, uint32_t>::iterator> list_data;

            /**
             * 内部删除, 不加锁
             * @param uid
             */
            void _del(uint32_t uid);

        protected:
            /**
             * 服务
             */
            NetThread                        *_pEpollServer;

            /**
             * 总计连接数
             */
            volatile uint32_t               _total;

            /**
             * 空闲链表
             */
            list<uint32_t>                  _free;

            /**
             * 空闲链元素个数
             */
            volatile size_t                 _free_size;

            /**
             * 链接
             */
            list_data                       *_vConn;

            /**
             * 超时链表
             */
            multimap<time_t, uint32_t>      _tl;

            /**
             * 上次检查超时时间
             */
            time_t                          _lastTimeoutTime;

            /**
             * 链接ID的魔数
             */
            uint32_t                        _iConnectionMagic;
        };
        ////////////////////////////////////////////////////////////////////////////
    public:
        /**
         * 构造函数
         */
        NetThread(TC_EpollServer *epollServer);

        /**
         * 析构函数
         */
        virtual ~NetThread();

        /**
         * 绑定监听socket
         * @param ls
         */
        int bind(BindAdapterPtr &lsPtr);

        /**
         * 网络线程执行函数
         */
        virtual void run();

        /**
         * 停止网络线程
         */
        void terminate();

        /**
         * 生成epoll
         */
        void createEpoll(uint32_t iIndex = 0);

        /**
         * 初始化udp监听
         */
        void initUdp();

        /**
         * 是否服务结束了
         *
         * @return bool
         */
        bool isTerminate() const    { return _bTerminate; }

        /**
         * 获取Epoller对象
         * @return TC_Epoller*
         */
        TC_Epoller* getEpoller()    { return &_epoller; }

        /**
         * 获取监听socket信息
         *
         * @return map<int,ListenSocket>
         */
        map<int, BindAdapterPtr> getListenSocketInfo();

        /**
         * 根据名称获取BindAdapter
         * @param sName
         * @return BindAdapterPtr
         */
        BindAdapterPtr getBindAdapter(const string &sName);

        /**
         * 关闭连接
         * @param uid
         */
        void close(unsigned int uid);

         /**
         * 发送数据
         * @param uid
         * @param s
         */
        void send(unsigned int uid, const string &s, const string &ip, uint16_t port);

        /**
         * 获取某一监听端口的连接数
         * @param lfd
         *
         * @return vector<TC_EpollServer::ConnStatus>
         */
        vector<TC_EpollServer::ConnStatus> getConnStatus(int lfd);

        /**
         * 获取连接数
         *
         * @return size_t
         */
        size_t getConnectionCount()     { return _list.size(); }

        /**
         * 记录日志
         * @param s
         */
        void debug(const string &s);

        /**
         * INFO日志
         * @param s
         */
        void info(const string &s);

        /**
         * 记录错误日志
         * @param s
         */
        void error(const string &s);

        /**
         * 是否启用防止空链接攻击的机制
         * @param bEnable
         */
        void EnAntiEmptyConnAttack(bool bEnable);

        /**
         *设置空连接超时时间
         */
        void setEmptyConnTimeout(int timeout);

        /**
         *设置udp的接收缓存区大小，单位是B,最小值为8192，最大值为DEFAULT_RECV_BUFFERSIZE
         */
        void setUdpRecvBufferSize(size_t nSize=DEFAULT_RECV_BUFFERSIZE);

        /*
         *当网络线程中listeners没有监听socket时，使用adapter中设置的最大连接数
         */
        void setListSize(size_t iSize) { _listSize += iSize; }

        /**
         * 发送队列的大小
         * @return size_t
         */
        size_t getSendRspSize();

    protected:

        /**
         * 获取连接
         * @param id
         *
         * @return ConnectionPtr
         */
        Connection *getConnectionPtr(uint32_t uid)      { return _list.get(uid); }

        /**
         * 添加tcp链接
         * @param cPtr
         * @param iIndex
         */
        void addTcpConnection(Connection *cPtr);

        /**
         * 添加udp连接
         * @param cPtr
         * @param index
         */
        void addUdpConnection(Connection *cPtr);

        /**
         * 删除链接
         * @param cPtr
         * @param bEraseList 是否是超时连接的删除
         * @param closeType  关闭类型,0:表示客户端主动关闭；1:服务端主动关闭;2:连接超时服务端主动关闭
         */
        void delConnection(Connection *cPtr, bool bEraseList = true,EM_CLOSE_T closeType=EM_CLIENT_CLOSE);

        /**
         * 发送数据
         * @param cPtr
         * @param buffer
         */
        int sendBuffer(Connection *cPtr, const string &buffer, const string &ip, uint16_t port);

        /**
         * 发送数据
         * @param cPtr
         * @return int
         */
        int sendBuffer(Connection *cPtr);

        /**
         * 接收buffer
         * @param cPtr
         * @param buffer
         * @return int
         */
        int recvBuffer(Connection *cPtr, recv_queue::queue_type &v);

        /**
         * 处理管道消息
         */
        void processPipe();

        /**
         * 处理网络请求
         */
        void processNet(const epoll_event &ev);

        /**
         * 停止线程
         */
        void stopThread();

        /**
         * 新连接建立
         * @param fd
         */
        bool accept(int fd);

        /**
         * 绑定端口
         * @param ep
         * @param s
         */
        void bind(const TC_Endpoint &ep, TC_Socket &s);

        /**
         * 空连接超时时间
         */
        int getEmptyConnTimeout() const;

        /**
         *是否空连接检测
         */
        bool IsEmptyConnCheck() const;

        //定义事件类型
        enum
        {
            ET_LISTEN = 1,
            ET_CLOSE  = 2,
            ET_NOTIFY = 3,
            ET_NET    = 0,
        };
        enum
        {
            MIN_EMPTY_CONN_TIMEOUT  = 2*1000,    /*空链接超时时间(ms)*/
            DEFAULT_RECV_BUFFERSIZE = 64*1024    /*缺省数据接收buffer的大小*/
        };
        //定义加入到网络线程的fd类别
        enum
        {
            TCP_CONNECTION = 0,
            UDP_CONNECTION = 1,
        };
        friend class BindAdapter;
        friend class ConnectionList;
        friend class TC_EpollServer;

    private:
        /**
         * 服务
         */
        TC_EpollServer            *_epollServer;

        /**
         * 监听socket
         */
        map<int, BindAdapterPtr>    _listeners;

        /**
         * 没有监听socket的网络线程时，使用此变量保存adapter信息
         */
        size_t                        _listSize;

        /**
         * epoll
         */
        TC_Epoller                  _epoller;

        /**
         * 停止
         */
        bool                        _bTerminate;

        /**
         * epoll是否已经创建
         */
        bool                        _createEpoll;

        /**
         * handle是否已经启动
         */
        bool                        _handleStarted;

        /**
         * 管道(用于关闭服务)
         */
        TC_Socket                   _shutdown;

        //管道(用于通知有数据需要发送就)
        TC_Socket                   _notify;

        /**
         * 管理的连接链表
         */
        ConnectionList              _list;

        /**
         * 发送队列
         */
        send_queue                  _sbuffer;

        /**
         * BindAdapter是否有udp监听
         */
        bool                        _hasUdp;

        /**
         *空连接检测机制开关
         */
        bool                         _bEmptyConnAttackCheck;

        /**
         * 空连接超时时间,单位是毫秒,默认值2s,
         * 该时间必须小于等于adapter自身的超时时间
         */
        int                            _iEmptyCheckTimeout;

        /**
         * udp连接时接收包缓存大小,针对所有udp接收缓存有效
         */
        size_t                         _nUdpRecvBufferSize;

        /**
         * 属于该网络线程的内存池,目前主要用于发送使用
         */
        TC_BufferPool*                 _bufferPool;

        /**
         * 该网络线程的内存池所负责分配的最小字节和最大字节(2的幂向上取整)
         */
        size_t                         _poolMinBlockSize;
        size_t                         _poolMaxBlockSize;

        /**
         * 该网络线程的内存池hold的最大字节
         */
        size_t                         _poolMaxBytes;
    };
    ////////////////////////////////////////////////////////////////////////////
public:
    /**
     * 构造函数
     */
    TC_EpollServer(unsigned int iNetThreadNum = 1);

    /**
     * 析构函数
     */
    ~TC_EpollServer();

    /**
     * 是否启用防止空链接攻击的机制
     * @param bEnable
     */
    void EnAntiEmptyConnAttack(bool bEnable);

    /**
     *设置空连接超时时间
     */
    void setEmptyConnTimeout(int timeout);

    /**
     *设置NetThread的内存池信息
     */
    void setNetThreadBufferPoolInfo(size_t minBlock, size_t maxBlock, size_t maxBytes);

    /**
     * 设置本地日志
     * @param plocalLogger
     */
    void setLocalLogger(RollWrapperInterface *pLocalLogger)       { _pLocalLogger = pLocalLogger; }

    /**
     * 绑定到一个已经有的handle组上
     * @param groupName
     * @param handleNum
     * @param adapter
     */
    void setHandleGroup(const string& groupName, BindAdapterPtr adapter)
    {
        map<string, HandleGroupPtr>::iterator it = _handleGroups.find(groupName);

        if (it != _handleGroups.end())
        {
            it->second->adapters[adapter->getName()] = adapter;
            adapter->_handleGroup = it->second;
        }
    }

    /**
     * 创建一个handle对象组，如果已经存在则直接返回
     * @param name
     * @return HandlePtr
     */
    template<class T> void setHandleGroup(const string& groupName, int32_t handleNum, BindAdapterPtr adapter)
    {
        map<string, HandleGroupPtr>::iterator it = _handleGroups.find(groupName);

        if (it == _handleGroups.end())
        {
            HandleGroupPtr hg = new HandleGroup();

            hg->name = groupName;

            adapter->_handleGroup = hg;

            for (int32_t i = 0; i < handleNum; ++i)
            {
                HandlePtr handle = new T();

                handle->setEpollServer(this);

                handle->setHandleGroup(hg);

                hg->handles.push_back(handle);
            }

            _handleGroups[groupName] = hg;

            it = _handleGroups.find(groupName);
        }
        it->second->adapters[adapter->getName()] = adapter;

        adapter->_handleGroup = it->second;
    }

    /**
     * 选择网络线程
     * @param fd
     */
    NetThread* getNetThreadOfFd(int fd)
    {
        return _netThreads[fd % _netThreads.size()];
    }

    /**
     * 绑定监听socket
     * @param ls
     */
    int  bind(BindAdapterPtr &lsPtr);

    /**
     * 启动业务处理线程
     */
    void startHandle();

    /**
     * 生成epoll
     */
    void createEpoll();

    /**
     * 运行
     */
    void waitForShutdown();

    /**
     * 停止服务
     */
    void terminate();

    /**
     * 是否服务结束了
     *
     * @return bool
     */
    bool isTerminate() const    { return _bTerminate; }

    /**
     * 根据名称获取BindAdapter
     * @param sName
     * @return BindAdapterPtr
     */
    BindAdapterPtr getBindAdapter(const string &sName);

    /**
     * 向网络线程添加连接
     */
    void addConnection(NetThread::Connection * cPtr, int fd, int iType);

    /**
     * 关闭连接
     * @param uid
     */
    void close(unsigned int uid, int fd);

    /**
     * 发送数据
     * @param uid
     * @param s
     */
    void send(unsigned int uid, const string &s, const string &ip, uint16_t port, int fd);

    /**
     * 获取某一监听端口的连接数
     * @param lfd
     *
     * @return vector<TC_EpollServer::ConnStatus>
     */
    vector<ConnStatus> getConnStatus(int lfd);

    /**
     * 获取监听socket信息
     *
     * @return map<int,ListenSocket>
     */
    map<int, BindAdapterPtr> getListenSocketInfo();

    /**
     * 获取所有连接的数目
     *
     * @return size_t
     */
    size_t getConnectionCount();

    /**
     * 记录日志
     * @param s
     */
    void debug(const string &s);

    /**
     * INFO日志
     * @param s
     */
    void info(const string &s);

     /**
     * 记录错误日志
     * @param s
     */
    void error(const string &s);

    /**
     * 获取网络线程的数目
     */
    unsigned int getNetThreadNum() { return _netThreadNum; }

    /**
     * 获取网络线程的指针集合
     */
    vector<TC_EpollServer::NetThread*> getNetThread() { return _netThreads; }

    /**
     * 停止线程
     */
    void stopThread();

    /**
     * 获取所有业务线程的数目
     */
    unsigned int getLogicThreadNum();

protected:

    friend class BindAdapter;

public:

    //统计服务端相应队列大小的上报的对象
    PropertyReport *            _pReportRspQueue;

private:
    /**
     * 网络线程
     */
    std::vector<NetThread*>        _netThreads;

    /*
     * 网络线程数目
     */
    unsigned int                _netThreadNum;

    /*
     * 服务是否停止
     */
    bool                        _bTerminate;

    /*
     * 业务线程是否启动
     */
    bool                        _handleStarted;

    /**
     * 本地循环日志
     */
    RollWrapperInterface        *_pLocalLogger;

    /**
     * 处理handle对象
     */
    map<string, HandleGroupPtr> _handleGroups;
};
typedef TC_AutoPtr<TC_EpollServer> TC_EpollServerPtr;
}

#endif
