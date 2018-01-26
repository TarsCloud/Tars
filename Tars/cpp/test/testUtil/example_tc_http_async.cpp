/**
 * Tencent is pleased to support the open source community by making Tars available.
 *
 * Copyright (C) 2016 THL A29 Limited, a Tencent company. All rights reserved.
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

#include <iostream>
#include "util/tc_file.h"
#include "util/tc_http_async.h"

using namespace std;
using namespace tars;

class AsyncHttpCallback : public TC_HttpAsync::RequestCallback
{
public:
    AsyncHttpCallback(const string &sUrl) : _sUrl(sUrl)
    {
    }
    virtual void onException(const string &ex) 
    {
        cout << "onException:" << _sUrl << ":" << ex << endl;
    }
    virtual void onResponse(bool bClose, TC_HttpResponse &stHttpResponse) 
    {
        cout << "onResponse:" << _sUrl << endl;//<< ":" << TC_Common::tostr(stHttpResponse.getHeaders()) << endl;
        TC_File::save2file("tmp.html", stHttpResponse.getContent());
    }
    virtual void onTimeout() 
    {
        cout << "onTimeout:" << _sUrl << endl;
    }
    virtual void onClose()
    {
        cout << "onClose:" << _sUrl << endl;
    }
protected:
    string _sUrl;
};

int addAsyncRequest(TC_HttpAsync &ast, const string &sUrl)
{
    TC_HttpRequest stHttpReq;
    stHttpReq.setGetRequest(sUrl);

    TC_HttpAsync::RequestCallbackPtr p = new AsyncHttpCallback(sUrl);

    return ast.doAsyncRequest(stHttpReq, p);    
}

int main(int argc, char *argv[])
{
    try
    {
        uint64_t i = TC_Common::now2ms();
        TC_HttpAsync ast;
        ast.setTimeout(10000);
        ast.start(1);

//        int ret = addAsyncRequest(ast, "www.baidu.com");
        int ret = addAsyncRequest(ast, "http://www.qq.com");
        cout << ret << endl;
        /*
        addAsyncRequest(ast, "www.google.com");
        addAsyncRequest(ast, "http://news.qq.com/a/20100108/002269.htm");
        addAsyncRequest(ast, "http://news.qq.com/zt/2010/mtjunshou/");
        addAsyncRequest(ast, "http://news.qq.com/a/20100108/000884.htm");
        addAsyncRequest(ast, "http://tech.qq.com/zt/2009/renovate/index.htm");
        */
        ast.waitForAllDone();
        cout << (TC_Common::now2ms() - i) << endl;
    }
    catch(exception &ex)
    {
        cout << ex.what() << endl;
    }

    return 0;
}


