#ifndef _DemoServantImp_H_
#define _DemoServantImp_H_

#include "servant/Application.h"
#include "DemoServant.h"

/**
 *
 *
 */
class DemoServantImp : public DemoApp::DemoServant
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
    virtual int test(tars::TarsCurrentPtr current) { return 0;};
};
/////////////////////////////////////////////////////
#endif
