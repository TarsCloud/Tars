#include "servant/Auth.h"

namespace tars
{

/**
 * server :默认鉴权逻辑
 */
bool processAuth(void* c, const string& data);

/**
 * server :默认鉴权逻辑
 */
int processAuthReqHelper(const BasicAuthPackage& pkg, const BasicAuthInfo& info);

/**
 * server :默认鉴权方法
 */
int defaultProcessAuthReq(const char* request, size_t len, const string& expectObj);
int defaultProcessAuthReq(const string& request, const string& expectObj);

/**
 * client:默认生成鉴权请求方法
 */
string defaultCreateAuthReq(const BasicAuthInfo& info, const string& hashMethod = "sha1");

} // end namespace taf

