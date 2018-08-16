#ifndef __TC_HTTP2_H__
#define __TC_HTTP2_H__

#if TARS_HTTP2

#include "util/tc_thread.h"
#include "util/tc_autoptr.h"
#include "util/tc_http.h"
#include "nghttp2/nghttp2.h"

namespace tars
{

typedef enum
{
    REQUEST_GET,
    REQUEST_POST,
    REQUEST_OPTIONS,
    REQUEST_HEAD,
    REQUEST_PUT,
    REQUEST_DELETE
}Req_Type;

typedef int (*ResponseFunc)(const Req_Type reqtype, 
                         const string &requri, 
                         const TC_Http::http_header_type &reqHeader, 
                         const string &reqBody,
                         int &resopnseStatus,
                         string &resopnseAbout,
                         TC_Http::http_header_type &responseHeader,
                         string &responseBody);


class TC_Http2Session: public TC_HandleBase
{
public:

    struct Http2Response
    {
        int status;
        string about;
        TC_Http::http_header_type header;
        string body;
    };

    int parse(string &in, string &out);

    int getRequest(const vector<char> &request, vector<int32_t>& vtReqid);

    int doResopnse(int32_t reqid, const Http2Response &response, vector<char>& out);

    int getMethod(int32_t reqid, Req_Type &method)
    {
        TC_ThreadLock::Lock lock(reqLock_);
        map<int32_t, RequestPack>::iterator it = mReq_.find(reqid);
        if (it != mReq_.end()) 
            method = it->second.method;
        else
            return -1;
    }

    int getUri(int32_t reqid, string &uri)
    {
        TC_ThreadLock::Lock lock(reqLock_);
        map<int32_t, RequestPack>::iterator it = mReq_.find(reqid);
        if (it != mReq_.end()) 
            uri = it->second.uri;
        else
            return -1;
    }

    int getHeader(int32_t reqid, TC_Http::http_header_type &header)
    {
        TC_ThreadLock::Lock lock(reqLock_);
        map<int32_t, RequestPack>::iterator it = mReq_.find(reqid);
        if (it != mReq_.end()) 
            header = it->second.header;
        else
            return -1;
    }

    int getBody(int32_t reqid, string &body)
    {
        TC_ThreadLock::Lock lock(reqLock_);
        map<int32_t, RequestPack>::iterator it = mReq_.find(reqid);
        if (it != mReq_.end()) 
            body = it->second.body;
        else
            return -1;
    }

    int doRequest(const vector<char> &request, vector<char>& response);

    void setResponseFunc(ResponseFunc func)
    {
        responseFunc_ = func;
    }

    struct RequestPack
    {
        RequestPack():streamId(0), bFinish(false){}

        Req_Type method;
        string uri;
        TC_Http::http_header_type header;
        string body;
        int32_t streamId;
        bool bFinish;
    };

    struct DataPack
    {
        DataPack(){}

        DataPack(string &data, int pos):dataBuf(data), readPos(pos){}

        string dataBuf;
        unsigned int readPos;
    };

    TC_Http2Session();

    ~TC_Http2Session();

    TC_ThreadLock responseBufLock_;
    string responseBuf_;

    TC_ThreadLock reqLock_;
    map<int32_t, RequestPack> mReq_;

    string reqout_;

private:

    int (*responseFunc_)(const Req_Type reqtype, 
                         const string &requri, 
                         const TC_Http::http_header_type &reqHeader, 
                         const string &reqBody,
                         int &resopnseStatus,
                         string &resopnseAbout,
                         TC_Http::http_header_type &responseHeader,
                         string &responseBody);

    nghttp2_session *session_;

    bool bNewCon_;

    TC_ThreadLock nghttpLock;

    bool bOldVersion_;
    bool bUpgrade_;
};

typedef TC_AutoPtr<TC_Http2Session> TC_Http2SessionPtr;

}

#endif

#endif
