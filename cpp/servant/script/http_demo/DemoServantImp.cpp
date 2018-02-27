#include "DemoServantImp.h"
#include "servant/Application.h"

using namespace std;

//////////////////////////////////////////////////////
void DemoServantImp::initialize()
{
    //initialize servant here:
    //...
}

//////////////////////////////////////////////////////
void DemoServantImp::destroy()
{
    //destroy servant here:
    //...
}

int DemoServantImp::doRequest(TarsCurrentPtr current, vector<char> &buffer)
{
    TC_HttpRequest req;
    TC_HttpResponse rsp;

	// parse request header
    vector<char> v = current->getRequestBuffer();
    string sBuf;
    sBuf.assign(&v[0], v.size());
    req.decode(sBuf);

    int ret = doRequest(req, rsp);

    rsp.encode(buffer);

    return ret;
}

int DemoServantImp::doRequest(const TC_HttpRequest &req, TC_HttpResponse &rsp)
{
	string msg = "Hello Tars!";
    rsp.setContentType("html/text");
    rsp.setResponse(msg.c_str(), msg.size());
    return 0;   
}
