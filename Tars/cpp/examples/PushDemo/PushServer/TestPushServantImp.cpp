#include "TestPushServantImp.h"
#include "servant/Application.h"
#include "TestPushThread.h"

using namespace std;

//////////////////////////////////////////////////////
void TestPushServantImp::initialize()
{
    //initialize servant here:
    //...
}

//////////////////////////////////////////////////////
void TestPushServantImp::destroy()
{
    //destroy servant here:
    //...
}


int TestPushServantImp::doRequest(tars::TarsCurrentPtr current, vector<char>& response)
{
	//保存客户端的信息，以便对客户端进行push消息
	(PushUser::mapMutex).lock();
	map<string, TarsCurrentPtr>::iterator it = PushUser::pushUser.find(current->getIp());
	if(it == PushUser::pushUser.end())
	{
		PushUser::pushUser.insert(map<string, TarsCurrentPtr>::value_type(current->getIp(), current));
		LOG->debug() << "connect ip: " << current->getIp() << endl;
	}
	(PushUser::mapMutex).unlock();
	//返回给客户端它自己请求的数据包，即原包返回
	const vector<char>& request = current->getRequestBuffer();
	response = request;

	return 0;
}
//客户端关闭到服务端的连接，或者服务端发现客户端长时间未发送包过来，然后超过60s就关闭连接
//调用的方法
int TestPushServantImp::doClose(TarsCurrentPtr current)
{
	(PushUser::mapMutex).lock();
	map<string, TarsCurrentPtr>::iterator it = PushUser::pushUser.find(current->getIp());
	if(it != PushUser::pushUser.end())
	{
		PushUser::pushUser.erase(it);
		LOG->debug() << "close ip: " << current->getIp() << endl;
	}
	(PushUser::mapMutex).unlock();

	return 0;
}


