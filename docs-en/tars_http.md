# tars supports http1/2 protocol

Tars supports the http2 protocol by using the[nghttp2 library](https://www.nghttp2.org), and its RPC encapsulation of protocol library
makes it almost transparent to bussiness development and is very convenient to use. Because of the Tars rpc, it also has the features
of synchronous and asynchronous as well as timeout, and can use tars stat to report the call quality.

This article briefly describes the methods and steps for using http2, including synchronous calls and asynchronous calls. But be aware
that it does not have a field similar to the request id for http1 which can not be mapped to the synchronous response and request, so
you can only snyc requests by using http1. 

## Usage of http rpc

Just follow the steps below to set it up:

### 1. compile tars framework, to support nghttp2
the tars framework does not support http2 by default, change TARS_HTTP2 definition to 1 in cpp/build/CMakeLists.txt at first:
```cpp
set(TARS_HTTP2 1)
```
Now recompile and install the tars framework.
modify the makefile of bussiness and add a line definition like that:
```cpp
TARS_HTTP2 = 1
```
Note that this line ** must be placed on include /usr/local/tars/cpp/makefile/makefile.tars**

### 2. Get the client agent, set the http2 codec function

```cpp
CommunicatorPtr& comm = Application::getCommunicator();
YourPrx prx = comm->stringToProxy<YourPrx>("test.server.yourobj");

// set protocol
ProxyProtocol proto;
proto.requestFunc = tars::http2Request;
proto.responseExFunc = tars::http2Response;
prox->tars_set_protocol(proto);
```

### 3. Initiate a synchronous call

```cpp
map<string, string> headers;
headers[":authority"] = "domain.com";
headers[":scheme"] = "http";

map<string, string> rspHeaders;
string rspBody;

prx->http_call("GET", "/", headers, "", rspHeaders, rspBody);
```

### 4. Initiate a asynchronous call

```cpp
// write callback class
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

// initiate a call
headers[":path"] = "/";
headers[":method"] = "GET";
prx->http_call_async(headers, "", new MyHttpCb());
```


