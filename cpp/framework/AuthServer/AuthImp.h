#ifndef _AUTHIMP_H_
#define _AUTHIMP_H_

#include "Auth.h"
#include "util/tc_common.h"
#include "util/tc_config.h"
#include "util/tc_mysql.h"

class AuthImp : public tars::Auth
{
public:
    AuthImp();
    ~AuthImp();

    /**
     * ³õÊ¼»¯
     *
     * @return int
     */
    virtual void initialize();

    /**
     * ÍË³ö
     */
    virtual void destroy() {}

    /**
     * process
     */
    virtual int authProcess(const tars::AuthRequest & request,tars::TarsCurrentPtr current);
    virtual vector<tars::TokenResponse> getTokens(const tars::TokenRequest& request,tars::TarsCurrentPtr current);
    virtual tars::ApplyTokenResponse applyToken(const tars::ApplyTokenRequest& request,tars::TarsCurrentPtr current);
    virtual int deleteToken(const tars::DeleteTokenRequest& request,tars::TarsCurrentPtr current);

private:
    /**
     * mysql
     */
    TC_Mysql* _mysql;
};

#endif

