#ifndef _TestPushServer_H_
#define _TestPushServer_H_

#include <iostream>
#include "servant/Application.h"
#include "TestPushThread.h"


using namespace tars;

/**
 *
 **/
class TestPushServer : public Application
{
public:
    /**
     *
     **/
    virtual ~TestPushServer() {};

    /**
     *
     **/
    virtual void initialize();

    /**
     *
     **/
    virtual void destroyApp();

    private:
    //用于push消息的线程
    PushInfoThread  pushThread;

};

extern TestPushServer g_app;

////////////////////////////////////////////
#endif
