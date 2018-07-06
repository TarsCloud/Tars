# tars http1/2支持

Tars使用[nghttp2库](https://www.nghttp2.org)支持了http2协议，对协议库做了RPC封装，使得对业务开发几乎透明，用起来非常方便。由于复用了
Tars rpc，因此也拥有了同步和异步以及超时的特性，并能够利用tars stat上报调用质量。

本文简单介绍一下使用http2的方法和步骤，包括同步调用和异步调用。但要注意的是，如果使用的是http1，则只能同步请求。因为http1没有类似
请求id的字段，无法将异步响应和请求对应上。

## http rpc的使用

只需要按照下面几个步骤设置：

### 1. 编译tars框架，支持nghttp2
tars框架默认不开启http2，首先改动cpp/build/CMakeLists.txt，将TARS_HTTP2定义改为1：
```cpp
set(TARS_HTTP2 1)
```
现在重新编译安装tars框架。
修改业务makefile，添加一行定义：
```cpp
TARS_HTTP2 = 1
```
注意这一行**一定要放在include /usr/local/tars/cpp/makefile/makefile.tars上面**。

### 2. 获取客户端代理，设置http2编解码函数

```cpp
CommunicatorPtr& comm = Application::getCommunicator();
YourPrx prx = comm->stringToProxy<YourPrx>("test.server.yourobj");

// set protocol
ProxyProtocol proto;
proto.requestFunc = tars::http2Request;
proto.responseExFunc = tars::http2Response;
prox->tars_set_protocol(proto);
```

### 3. 发起同步调用

```cpp
map<string, string> headers;
headers[":authority"] = "domain.com";
headers[":scheme"] = "http";

map<string, string> rspHeaders;
string rspBody;

prx->http_call("GET", "/", headers, "", rspHeaders, rspBody);
```

### 4. 发起异步调用

```cpp
// 编写callback
class MyHttpCb : public HttpCallback
{
    public:
            int onHttpResponse(const map<string, string>& reqHeaders,
                               const map<string, string>& rspHeaders,
                               const vector<char>& rspBody)
            {
               // your process
               return 0;
            }
            int onHttpResponseException(const map<string, string>& reqHeaders, int eCode)
            {
                //  process exception
                return 0;
            }
};

// 发起调用
headers[":path"] = "/";
headers[":method"] = "GET";
prx->http_call_async(headers, "", new MyHttpCb());
```


