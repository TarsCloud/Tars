#include "servant/Auth.h"

namespace tars
{

/**
 * server :Ĭ�ϼ�Ȩ�߼�
 */
bool processAuth(void* c, const string& data);

/**
 * server :Ĭ�ϼ�Ȩ�߼�
 */
int processAuthReqHelper(const BasicAuthPackage& pkg, const BasicAuthInfo& info);

/**
 * server :Ĭ�ϼ�Ȩ����
 */
int defaultProcessAuthReq(const char* request, size_t len, const string& expectObj);
int defaultProcessAuthReq(const string& request, const string& expectObj);

/**
 * client:Ĭ�����ɼ�Ȩ���󷽷�
 */
string defaultCreateAuthReq(const BasicAuthInfo& info, const string& hashMethod = "sha1");

} // end namespace taf

