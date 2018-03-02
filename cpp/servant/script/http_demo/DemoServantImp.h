#ifndef _DemoServantImp_H_
#define _DemoServantImp_H_

#include "servant/Application.h"

/**
 *
 *
 */
class DemoServantImp : public Servant
{
public:
    /**
     *
     */
    virtual ~DemoServantImp() {}

    /**
     *
     */
    virtual void initialize();

    /**
     *
     */
    virtual void destroy();

    /**
     *
     */
	int doRequest(TarsCurrentPtr current, vector<char> &buffer);

private:
	int doRequest(const TC_HttpRequest &req, TC_HttpResponse &rsp);
};
/////////////////////////////////////////////////////
#endif
