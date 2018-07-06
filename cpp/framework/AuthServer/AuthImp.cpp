#include "AuthImp.h"
#include "AuthServer.h"
#include "servant/TarsLogger.h"

using namespace tars;

std::string tableName = "t_auth_info";

AuthImp::AuthImp() : _mysql(NULL)
{
}

AuthImp::~AuthImp()
{
    delete _mysql;
}

void AuthImp::initialize()
{
    assert (!_mysql);
    _mysql = new TC_Mysql();

    extern TC_Config * g_pconf;
           
    TC_DBConf dbConf;
    dbConf.loadFromMap(g_pconf->getDomainMap("/tars/db"));
    _mysql->init(dbConf);
    
    std::string createTbl = (*g_pconf)["/tars/<sql>"];
    _mysql->execute(createTbl);
}

int AuthImp::authProcess(const AuthRequest& request, TarsCurrentPtr current)
{
    string sSql("select token from ");
    sSql += tableName;
    sSql += " where application ='";
    sSql += request.sKey.sApplication;
    sSql += "' and server ='";
    sSql += request.sKey.sServer;
    sSql += "' and objname ='";
    sSql += request.sKey.sObjName;
    sSql += "';";

    TC_Mysql::MysqlData result;
    try {
        result = _mysql->queryRecord(sSql);
    }
    catch (const TC_Mysql_Exception& e) {
        TLOGDEBUG("exp : " << e.what() << endl);
        return AUTH_ERROR;
    }

    assert (result.size() <= 1);
    if (result.size() == 0)
    {
        return AUTH_WRONG_OBJ;
    }
    else
    {
        string token = result[0]["token"];
        TLOGDEBUG("got token from mysql " << token << endl);
        if (token != request.sToken)
        {
            TLOGERROR("wrong token " << request.sToken << endl);
            return AUTH_WRONG_TOKEN;
        }
    }

	return AUTH_SUCC;
}

    
vector<tars::TokenResponse> AuthImp::getTokens(const tars::TokenRequest& request,tars::TarsCurrentPtr current)
{
    vector<tars::TokenResponse> rsp;
    for (vector<string>::const_iterator it(request.vObjName.begin()); it != request.vObjName.end(); ++ it)
    {
        string sSql;
        sSql += "select application, server, token from " + tableName + " where objname = '";
        sSql += *it;
        sSql += "';";

        TC_Mysql::MysqlData result;
        try {
            result = _mysql->queryRecord(sSql);
        }
        catch (const TC_Mysql_Exception& e) {
            TLOGDEBUG("queryRecord exp " << e.what() << endl);
            continue;
        }

        tars::TokenResponse tmp;
        tmp.sObjName = *it;
        for (size_t i = 0; i < result.size(); ++ i)
        {
            // app.server
            std::string key = result[i]["application"] + "." + result[i]["server"];
            tmp.mTokens[key] = result[i]["token"]; 
        }
        rsp.push_back(tmp);
    }

    return rsp;
}

    
#if 0
   id          | int(11)      | NO   | PRI | NULL    | auto_increment |
 | application | varchar(128) | YES  | MUL |         |                |
 | server      | varchar(128) | YES  |     | NULL    |                |
 | objname     | varchar(128) | YES  |     |         |                |
 | token       | varchar(128) | NO   |     |         |                |
#endif
tars::ApplyTokenResponse AuthImp::applyToken(const tars::ApplyTokenRequest& request,tars::TarsCurrentPtr current)
{
    // gen secret key;
    char token[32] = "";
    TC_Common::getRandomHexChars(token, 16);
    std::string sToken(token);

    std::ostringstream sql;
    sql << "insert into " + tableName + " (application, server, objname, token) values('";
    sql << request.sKey.sApplication << "','" << request.sKey.sServer << "','" << request.sKey.sObjName << "','";
    sql << sToken << "');";

    try {
        string sSql = sql.str();
        TLOGDEBUG("exec insert:" <<  sSql << endl);
        _mysql->execute(sSql);
    }
    catch (const TC_Mysql_Exception& e) {
        TLOGDEBUG("applySecret exp " << e.what() << endl);
        sToken.clear();
    }

    tars::ApplyTokenResponse rsp;
    rsp.sKey = request.sKey;
    rsp.sToken = sToken;
    return rsp;
}
    
int AuthImp::deleteToken(const tars::DeleteTokenRequest& request,tars::TarsCurrentPtr current)
{
    //DELETE  FROM tbl_name
     //   WHERE  which rows to delete;

    std::ostringstream sql;
    sql << "delete from " + tableName + " where application='";
    sql << request.sKey.sApplication << "' and server='" << request.sKey.sServer << "' and objname='" << request.sKey.sObjName << "';";

    try {
        string sSql = sql.str();
        TLOGDEBUG("exec delete :" <<  sSql << endl);
        _mysql->execute(sSql);
    }
    catch (const TC_Mysql_Exception& e) {
        TLOGDEBUG("delete exp " << e.what() << endl);
        return -1;
    }

    return 0;
}

