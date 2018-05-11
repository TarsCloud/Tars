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

#ifndef __TC_HTTP_ASYNC_H_
#define __TC_HTTP_ASYNC_H_

#include <functional>
#include "util/tc_thread_pool.h"
#include "util/tc_http.h"
#include "util/tc_autoptr.h"
#include "util/tc_socket.h"

namespace tars
{

/////////////////////////////////////////////////
/** 
* @file tc_http_async.h 
* @brief http异步调用类. 
*  
* http同步调用使用TC_HttpRequest::doRequest就可以了  
*/            
/////////////////////////////////////////////////


/**
* @brief 线程异常
*/
struct TC_HttpAsync_Exception : public TC_Exception
{
    TC_HttpAsync_Exception(const string &buffer) : TC_Exception(buffer){};
    TC_HttpAsync_Exception(const string &buffer, int err) : TC_Exception(buffer, err){};
    ~TC_HttpAsync_Exception() throw(){};
};

/**
 *  @brief 异步线程处理类.
 *  
 * 异步HTTP请求 ，使用方式示例如下: 
 *  
 *    实现异步回调对象
 *  
  *  异步对象回调执行的时候是在TC_HttpAsync中线程执行的
 *  
 *   用智能指针new出来, 不用管生命周期
 *  
 *   class AsyncHttpCallback : public
 *  
 *    TC_HttpAsync::RequestCallback
 *  
 *  {
 *  
 *   public:
 *  
 *       AsyncHttpCallback(const string &sUrl) : _sUrl(sUrl)
 *  
  *     {
 *  
  *     }
 *  
 *      virtual void onException(const string &ex)
 *  
 *     {
 *  
 *       cout << "onException:" << _sUrl << ":" << ex << endl;
 *  
  *       }
 *  
 *     //请求回来的时候onResponse被调用
 *  
  *   //bClose表示服务端关闭了连接 ,从而认为收到了一个完整的http响应
  *     
 *   virtual void onResponse(bool bClose, TC_HttpResponse
 *  
  *      &stHttpResponse)
 *  
  *       {
 *  
  *          cout << "onResponse:" << _sUrl << ":" <<
 *  
  *          TC_Common::tostr(stHttpResponse.getHeaders()) <<
 *  
  *          endl;
  *      }
  *  
  *      virtual void onTimeout()
 *  
 *      {
 *  
 *           cout << "onTimeout:" << _sUrl << endl;
 *  
  *      }
 *  
 *  
  *      //连接被关闭时调用
 *  
  *      virtual void onClose()
 *  
 *       {
 *  
 *           cout << "onClose:" << _sUrl << endl;
 *  
 *       }
 *  
 *   protected:
 *  
 *       string _sUrl;
 *  
  *  };
  *
 *   //封装一个函数, 根据实际情况处理
 *  
 *  int addAsyncRequest(TC_HttpAsync &ast, const string &sUrl) {
 *  
 *      TC_HttpRequest stHttpReq; stHttpReq.setGetRequest(sUrl);
 *  
  *
  *      //new出来一个异步回调对象
 *  
  *      TC_HttpAsync::RequestCallbackPtr p = new
 *  
  *      AsyncHttpCallback(sUrl);
  *
  *      return ast.doAsyncRequest(stHttpReq, p);
 *  
  *      }
  *
  *  //具体使用的示例代码如下:
 *  
 *   TC_HttpAsync ast;
 *  
 *   ast.setTimeout(1000);  //设置异步请求超时时间
 *  
 *   ast.start();
 *
 *   //真正的代码需要判断返回值,返回值=0才表示请求已经发送出去了
 *  
 *   int ret = addAsyncRequest(ast, "www.baidu.com");    
 *
 *   addAsyncRequest(ast, "www.qq.com");
 *  
 *   addAsyncRequest(ast, "www.google.com");
 *  
 *   addAsyncRequest(ast, "http://news.qq.com/a/20100108/002269.htm");
 *  
 *   addAsyncRequest(ast, "http://news.qq.com/zt/2010/mtjunshou/");
 *  
 *   addAsyncRequest(ast,"http://news.qq.com/a/20100108/000884.htm");
 *  
 *   addAsyncRequest(ast,"http://news.qq.com/a/20100108/000884.htm");
 *  
 *   addAsyncRequest(ast,"http://tech.qq.com/zt/2009/renovate/index.htm");
 * 
 *   ast.waitForAllDone();
 *  
 *   ast.terminate(); 
 */
class TC_HttpAsync : public TC_Thread, public TC_ThreadLock
{
public:
    /**
     * @brief 异步请求回调对象
     */
    class RequestCallback : public TC_HandleBase
    {
    public:
        /**
         * @brief 完整的响应回来了. 
         *  
         * @param bClose          因为远程服务器关闭连接认为http完整了
         * @param stHttpResponse  http响应包
         */
        virtual void onResponse(bool bClose, TC_HttpResponse &stHttpResponse) = 0;

        /**
         * @brief 每次收到数据且http头收全了都会调用， 
         * stHttpResponse的数据可能不是完全的http响应数据 ,只有部分body数据 
         * @param stHttpResponse  收到的http数据
         * @return                true:继续收取数据, false:不收取数据了
         */
        virtual bool onReceive(TC_HttpResponse &stHttpResponse) { return true;};

        /**
         * @brief 异常. 
         *  
         * @param ex 异常原因
         */
        virtual void onException(const string &ex) = 0;

        /**
         * @brief 超时没有响应
         */
        virtual void onTimeout() = 0;

        /**
         * @brief 连接被关闭
         */
        virtual void onClose() = 0;
    };

    typedef TC_AutoPtr<RequestCallback> RequestCallbackPtr;

protected:
    /**
     * @brief 异步http请求(短连接)
     */
    class AsyncRequest : public TC_HandleBase
    {
    public:
        /**
         * @brief 构造. 
         *  
         * @param stHttpRequest
         * @param callbackPtr
         */
        AsyncRequest(TC_HttpRequest &stHttpRequest, RequestCallbackPtr &callbackPtr);

        /**
         * @brief 析构
         */
        ~AsyncRequest();

        /**
         * @brief 获取句柄
         * 
         * @return int
         */
        int getfd() { return _fd.getfd(); }

        /**
         * @brief 发起建立连接.
         * 
         * @return int
         */
        int doConnect();

        /**
         * @brief 发起建立到addr的连接,不用DNS解析. 
         *  
         * @param addr 可用于直接连接透明代理或者连接通过DNS解析后的地址
         * @return int
         */
        int doConnect(struct sockaddr* addr);

        /**
         * @brief 发生异常
         */
        void doException();

        /**
         * @brief 发送请求
         */
        void doRequest();

        /**
         * @brief 接收响应
         */
        void doReceive();

        /**
         * @brief 关闭连接
         */
        void doClose();

        /**
         * @brief 超时
         */
        void timeout();

        /**
         * @brief 设置唯一ID. 
         *  
         * @param uniqId
         */
        void setUniqId(uint32_t uniqId)    { _iUniqId = uniqId;}

        /**
         * @brief 获取唯一ID.
         * 
         * @return uint32_t
         */
        uint32_t getUniqId() const         { return _iUniqId; }
           
        /**
         * @brief 设置处理请求的http异步线程.
         * 
         * @param pHttpAsync ：异步线程处理对象
         */
        void setHttpAsync(TC_HttpAsync *pHttpAsync) { _pHttpAsync = pHttpAsync; }

        /**
         * @brief 设置发网络请求时绑定的ip地址. 
         *  
         * @param addr
         */
        void setBindAddr(const struct sockaddr* addr);

    protected:
        /**
         * @brief 接收请求. 
         *  
         * @param buf
         * @param len
         * @param flag 
         * @return int
         */
        int recv(void* buf, uint32_t len, uint32_t flag);

        /**
         * @brief 发送请求. 
         *  
         * @param buf 发送内容
         * @param len 发送长度
         * @param flag  
         * @return int
         */
        int send(const void* buf, uint32_t len, uint32_t flag);

    protected:
        TC_HttpAsync               *_pHttpAsync;
        TC_HttpResponse             _stHttpResp;
        TC_Socket                   _fd;
        string                      _sHost;
        uint32_t                    _iPort;
        uint32_t                    _iUniqId;
        string                      _sReq;
        string                      _sRsp;
        RequestCallbackPtr          _callbackPtr;
        bool                        _bindAddrSet;
        struct sockaddr             _bindAddr;
    };

    typedef TC_AutoPtr<AsyncRequest> AsyncRequestPtr;

public:

    typedef TC_TimeoutQueue<AsyncRequestPtr> http_queue_type;

    /**
     * @brief 构造函数
     */
    TC_HttpAsync();

    /**
     * @brief 析构函数
     */
    ~TC_HttpAsync();

    /**
     * @brief 异步发起请求. 
     *  
     * @param stHttpRequest
     * @param httpCallbackPtr
     * @param bUseProxy,是否使用代理方式连接
     * @param addr, bUseProxy为false 直接连接指定的地址 
     * @return int, <0:发起连接失败, 可以通过strerror(返回值)
     *             =0:成功
     */
    int doAsyncRequest(TC_HttpRequest &stHttpRequest, RequestCallbackPtr &callbackPtr, bool bUseProxy=false, struct sockaddr* addr=NULL);

    /**
     * @brief 设置proxy地址
     * 
     */
    int setProxyAddr(const char* Host,uint16_t Port);

    /**
     * @brief 设置代理的地址. 
     *  
     * 不通过域名解析发送,直接发送到代理服务器的ip地址) 
     * @param sProxyAddr 格式 192.168.1.2:2345 或者 sslproxy.qq.com:2345
     */
    int setProxyAddr(const char* sProxyAddr);

    /**
     * @brief 设置绑定的地址. 
     *  
     * @param sProxyAddr 格式 192.168.1.2
     */
    int setBindAddr(const char* sBindAddr);

    /**
     * @brief 设置绑定的地址. 
     *  
     * @param addr 直接用 addr 赋值
     */
    void setProxyAddr(const struct sockaddr* addr);

    /**
     * @brief 启动异步处理. 
     *  
     * 参数已经无效(网络层有且只有一个线程)
     * @param num, 异步处理的线程数
     */
    void start(int iThreadNum = 1);

    /**
     * @brief 设置超时(所有请求都只能用一种超时时间). 
     *  
     * @param timeout: 毫秒, 但是具体的超时精度只能在s左右
     */
    void setTimeout(int millsecond) { _data->setTimeout(millsecond); }

    /**
     * @brief 等待请求全部结束(等待毫秒精度在100ms左右). 
     *  
     * @param millsecond, 毫秒 -1表示永远等待
     */
    void waitForAllDone(int millsecond = -1);

    /**
     * @brief 结束线程
     */
    void terminate();

protected:

    using async_process_type = std::function<void (AsyncRequestPtr&, int)>;

    /**
     * @brief 超时处理. 
     *  
     * @param ptr
     */
    static void timeout(AsyncRequestPtr& ptr);

    /**
     * @brief 具体网络处理
     */
    static void process(AsyncRequestPtr &p, int events);

    /**
     * @brief 具体的网络处理逻辑
     */
    void run() ;

    /**
     * @brief 删除异步请求对象
     */
    void erase(uint32_t uniqId); 

    friend class AsyncRequest;

protected:
    TC_ThreadPool               _tpool;

    http_queue_type             *_data;
    
    TC_Epoller                  _epoller;

    bool                        _terminate;

    struct sockaddr             _proxyAddr;

    struct sockaddr             _bindAddr;

    bool                        _bindAddrSet;
};

}
#endif

