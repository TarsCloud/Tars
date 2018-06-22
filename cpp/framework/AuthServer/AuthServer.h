#ifndef _AUTH_SERVER_H_
#define _AUTH_SERVER_H_

#include "servant/Application.h"

using namespace tars;

class AuthServer : public Application
{
protected:
    /**
     * 初始化, 只会进程调用一次
     */
    virtual void initialize();

    /**
     * 析构, 每个进程都会调用一次
     */
    virtual void destroyApp();
};

#endif

