# The push function of the Tars

# Contents
> * [1.Background] 
> * [2.Flow chart of the push mode]
> * [3.Implement server-to-client push mode in Tars] 
> * [4.Server function implementation] 
> * [5.Client function implementation] 
> * [6.Test results] 


## Background

In the actual application scenario, server-to-client push modes need to be supported in the TARS service framework.

For example, see cpp/examples/pushDemo/.

## Flow chart of the push mode
Here's a flow chart of the push mode

![tars](../docs/images/tars_flow.PNG)

- The black line represents the data flow : data(client) -〉 request packet encoder(client) -〉 protocol parser(server) -〉 doRequest protocol processor(server) -〉 return data generation(server) -〉 response packet decoder(client) -〉 response data (client)
- The yellow line represents the client access server
- The blue line represents the server pushing the message to the client.
- The **request packet encoder**(client)  is responsible for packaging and encoding the data sent by the client. The **protocol parser**(server) is responsible for unpacking the received data and handing it over to **protocol processor**(server).
- The **protocol processor**(server) processes and generates the return data, while the **response packet decoder**(client) is responsible for decoding the returned data.

## Implement server-to-client push mode in Tars:

- For the server, first, the server needs to implement the protocol parser according to the mode of developing the third-party protocol (that is, the non-TARS protocol) and load it into the service. Then server need to establish a non-TARS service object, which inherits from the Servant class of the Tars framework, and establishes the protocol processor between the client and the server by overloading the doRequest method in the Servant class. The information of the client, who connected to the server, is saved in the method, so that the server can push the message to the client according to that information. Finally, the doClose method in the Servant class needs to be reloaded. After the server knows that the client closes the connection, the client's information saved in the doRequest method is released, so that the server does not need to push the message to the disconnected client. In addition, the server needs to establish a thread dedicated to push messages to the client.

- For the client, the codec function of the protocol packet is implemented according to the mode of developing the third-party protocol, and it is set to the protocol parser of the corresponding ServantProxy proxy, and implemented by the `tars_set_protocol()` method of the `ServantProxy` class. Then you need to customize a callback class that inherits the `ServantProxyCallback` class. (Because the client receives the message from the server in an asynchronous manner, the client processes the message in an asynchronous manner.) At the same time, you need to override the `onDispatch()` method. In this method, the protocol defined between the client and the server is parsed. Finally, you need to create an instance of the callback class described above, and then pass it as a parameter to the `tars_set_push_callback()` method of `ServantProxy` class. In addition, the client needs to periodically send a message to the server (equivalent to a heartbeat packet) to tell the server that the client is alive. This is done because the server does not receive a message from the client within a certain period of time and automatically closes the connection between them. Before the server interacts with the client in push mode, the client needs to access the service by calling the rpc related function of the ServantProxy class.

## Server function implementation

### Server Implementation Overview
First we deploy a TestPushServant service in accordance with the code of the third-party protocol. 
Deploy a server on the management platform as shown below

![tars](../docs/images/tars_push_deploy.PNG)

Refer to the code that Tars supports third-party protocols:

- The `initialize()` of the `TestPushServer` class loads the service object `TestPushServantImp` and sets a third-party protocol `parser`. The `parser` does not do any processing, and passes the received data packet to the service object for processing. But usually, the data is parsed before being handed over to the service object for processing.
- `TestPushServantImp` overrides the `doRequest()` method that inherits from the `Servant` class, which is a protocol processor for third-party services. The processor is responsible for processing the data passed to it by the protocol parser and is responsible for generating the response returned to the client（This service is an echo service, so the response is directly equal to the received packet）. At the same time, the server will save the information state of the client, so that the `pushThread` thread can push the message to the client.
- In addition, `TestPushServantImp` overrides the `doClose()` method that inherits from the `Servant` class, and is used to clear the saved related customer information after the client closes the connection or the connection times out.


### Server-implemented code
TestPushServantImp.h
```cpp
#ifndef _TestPushServantImp_H_
#define _TestPushServantImp_H_

#include "servant/Application.h"
//#include "TestPushServant.h"

/**
 *
 *
 */
class TestPushServantImp : public  tars::Servant
{
public:
    /**
     *
     */
    virtual ~TestPushServantImp() {}

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


    //Overloading the doRequest method of the Servant class
    int doRequest(tars::TarsCurrentPtr current, vector<char>& response);

    //Overloading the doClose method of the Servant class
    int doClose(tars::TarsCurrentPtr current);

};
/////////////////////////////////////////////////////
#endif
```
TestPushServantImp.cpp
```cpp
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
//Save the client's information so that the client can push the message later.
	(PushUser::mapMutex).lock();
	map<string, TarsCurrentPtr>::iterator it = PushUser::pushUser.find(current->getIp());
	if(it == PushUser::pushUser.end())
	{
		PushUser::pushUser.insert(map<string, TarsCurrentPtr>::value_type(current->getIp(), current));
		LOG->debug() << "connect ip: " << current->getIp() << endl;
	}
	(PushUser::mapMutex).unlock();
//Return the requested packet to the client, that is, return the original packet.
	const vector<char>& request = current->getRequestBuffer();
	response = request;

	return 0;
}
//The client closes its connection with the server, or the server finds that the client has not 
//sent the packet for a long time (more than 60s), and then closes the connection.
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

```
TestPushThread.h
```cpp

#ifndef __TEST_PUSH_THREAD_H
#define __TEST_PUSH_THREAD_H

#include "servant/Application.h"

class PushUser
{
public:
	static map<string, TarsCurrentPtr> pushUser;
	static TC_ThreadMutex mapMutex;
};

class PushInfoThread : public TC_Thread, public TC_ThreadLock
{
public:
	PushInfoThread():_bTerminate(false),_tLastPushTime(0),_tInterval(10),_iId(0){}

	virtual void run();

	void terminate();

	void setPushInfo(const string &sInfo);

private:
	bool _bTerminate;
	time_t _tLastPushTime;
	time_t _tInterval;
	unsigned int _iId;
	string _sPushInfo;
};
#endif


```
TestPushThread.cpp
```cpp
#include "TestPushThread.h"
#include <arpa/inet.h>

map<string, TarsCurrentPtr> PushUser::pushUser;
TC_ThreadMutex PushUser::mapMutex;


void PushInfoThread::terminate(void)
{
	_bTerminate = true;
	{
	    tars::TC_ThreadLock::Lock sync(*this);
	    notifyAll();
	}
}

void PushInfoThread::setPushInfo(const string &sInfo)
{
	  unsigned int iBuffLength = htonl(sInfo.size()+8);
    unsigned char * pBuff = (unsigned char*)(&iBuffLength);

    _sPushInfo = "";
    for (int i = 0; i<4; ++i)
    {
        _sPushInfo += *pBuff++;
    }

    unsigned int iRequestId = htonl(_iId);
    unsigned char * pRequestId = (unsigned char*)(&iRequestId);

    for (int i = 0; i<4; ++i)
    {
        _sPushInfo += *pRequestId++;
    }

    _sPushInfo += sInfo;
}
//Push messages to customers on a regular basis
void PushInfoThread::run(void)
{
	time_t iNow;

	setPushInfo("hello world");

	while (!_bTerminate)
	{
		iNow =  TC_TimeProvider::getInstance()->getNow();

		if(iNow - _tLastPushTime > _tInterval)
		{
			_tLastPushTime = iNow;

			(PushUser::mapMutex).lock();
			for(map<string, TarsCurrentPtr>::iterator it = (PushUser::pushUser).begin(); it != (PushUser::pushUser).end(); ++it)
			{
				(it->second)->sendResponse(_sPushInfo.c_str(), _sPushInfo.size());
				LOG->debug() << "sendResponse: " << _sPushInfo.size() <<endl;
			}
			(PushUser::mapMutex).unlock();
		}

		{
            TC_ThreadLock::Lock sync(*this);
            timedWait(1000);
		}
	}
}

```
TestPushServer.h
```cpp
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
    //Thread for push messages
    PushInfoThread  pushThread;

};

extern TestPushServer g_app;

////////////////////////////////////////////
#endif

```

TestPushServer.cpp
```cpp
#include "TestPushServer.h"
#include "TestPushServantImp.h"

using namespace std;

TestPushServer g_app;

/////////////////////////////////////////////////////////////////

static int parse(string &in, string &out)
{
    if(in.length() < sizeof(unsigned int))
    {
        return TC_EpollServer::PACKET_LESS;
    }

    unsigned int iHeaderLen;

    memcpy(&iHeaderLen, in.c_str(), sizeof(unsigned int));

    iHeaderLen = ntohl(iHeaderLen);

    if(iHeaderLen < (unsigned int)(sizeof(unsigned int))|| iHeaderLen > 1000000)
    {
        return TC_EpollServer::PACKET_ERR;
    }

    if((unsigned int)in.length() < iHeaderLen)
    {
        return TC_EpollServer::PACKET_LESS;
    }

    out = in.substr(0, iHeaderLen);

    in  = in.substr(iHeaderLen);

    return TC_EpollServer::PACKET_FULL;
}


void
TestPushServer::initialize()
{
    //initialize application here:
    //...

    addServant<TestPushServantImp>(ServerConfig::Application + "." + ServerConfig::ServerName + ".TestPushServantObj");

    addServantProtocol("Test.TestPushServer.TestPushServantObj", parse);

    pushThread.start();

}
/////////////////////////////////////////////////////////////////
void
TestPushServer::destroyApp()
{
    //destroy application here:
    //...
    pushThread.terminate();
    pushThread.getThreadControl().join();

}
/////////////////////////////////////////////////////////////////
int
main(int argc, char* argv[])
{
    try
    {
        g_app.main(argc, argv);
        g_app.waitForShutdown();
    }
    catch (std::exception& e)
    {
        cerr << "std::exception:" << e.what() << std::endl;
    }
    catch (...)
    {
        cerr << "unknown exception." << std::endl;
    }
    return -1;
}
/////////////////////////////////////////////////////////////////
```

## Client function implementation

### Client Implementation Overview
This section describes how the client accesses the server through the proxy mode. The specific steps are as follows:
- The client first establishes a communicator (Communicator _comm) and obtains a proxy through the communicator. The code is as follows:

 ```cpp
  string sObjName = "Test.TestPushServer.TestPushServantObj";
  string sObjHost = "tcp -h 10.120.129.226 -t 60000 -p 10099";
  _prx = _comm.stringToProxy<ServantPrx>(sObjName+"@"+sObjHost);
```
-  Write and set the request packet encoder and response packet decoder for the proxy. The code is as follows:
  ```
request packet encoder:
static void FUN1(const RequestPacket& request, string& buff)
response packet decoder:
static size_t FUN2(const char* recvBuffer, size_t length, list<ResponsePacket>& done)
The code to set the proxy:
ProxyProtocol prot;
prot.requestFunc = FUN1;
prot.responseFunc = FUN2 ;
_prx->tars_set_protocol(prot);

```
 - Synchronous or asynchronous methods to access the server
 
    - Synchronization method: access the service by calling the proxy rpc_call method
   ```
	 virtual void rpc_call(uint32_t requestId, const string& sFuncName,const char* buff, uint32_t len, ResponsePacket& rsp);  
   ```
   The requestId parameter needs to be unique within the object, and a unique id in the object can be obtained through the `uint32_t tars_gen_requestid()` interface of the proxy. sFuncName is mainly used for statistical analysis of interface calls to the framework layer. It can be "" by default. Buff is the content to be sent, and len is the length of the buff. Rsp is the ResponsePacket package obtained for this call.

    - Asynchronous method: access the service by calling the proxy rpc_call_asyc method    
      ```
			virtual void rpc_call_async(uint32_t requestId, const string& sFuncName, const char* buff, uint32_t len,  const ServantProxyCallbackPtr& callback);
      ```
      The requestId parameter needs to be unique within the object, and a unique id in the object can be obtained through the `uint32_t tars_gen_requestid()` interface of the proxy. sFuncName is the name of the function called after the response object responds. Buff is the content to be sent, and len is the length of the buff. Callback is the callback object that is responded to after this call returns the result (that is, after the server returns the processing result).

- Set the push message method to accept the server:
```
TestPushCallBackPtr cbPush = new TestPushCallBack();
_prx->tars_set_push_callback(cbPush);
 ``` 
  
### Client-implemented code

main.cpp
```cpp
#include "servant/Application.h"
#include "TestRecvThread.h"
#include <iostream>

using namespace std;
using namespace tars;

int main(int argc,char**argv)
{
    try
    {
		RecvThread thread;
		thread.start();

		int c;
		while((c = getchar()) != 'q');

		thread.terminate();
		thread.getThreadControl().join();
    }
    catch(std::exception&e)
    {
        cerr<<"std::exception:"<<e.what()<<endl;
    }
    catch(...)
    {
        cerr<<"unknown exception"<<endl;
    }
    return 0;
}

```
TestRecvThread.h
```
#ifndef __TEST_RECV_THREAD_H
#define __TEST_RECV_THREAD_H

#include "servant/Application.h"

class TestPushCallBack : public ServantProxyCallback
{
public:
	virtual int onDispatch(ReqMessagePtr msg);
};
typedef tars::TC_AutoPtr<TestPushCallBack> TestPushCallBackPtr;

class RecvThread : public TC_Thread, public TC_ThreadLock
{
public:
	RecvThread();

	virtual void run();

	void terminate();
private:
	bool _bTerminate;

	Communicator _comm;

	ServantPrx  _prx;
};
#endif

```
TestRecvThread.cpp
```
#include "TestRecvThread.h"
#include <iostream>
#include <arpa/inet.h>

/*
 Response packet decoding function: Decode data received from the server according to a specific format, and parse it into ResponsePacket
*/
static size_t pushResponse(const char* recvBuffer, size_t length, list<ResponsePacket>& done)
{
	size_t pos = 0;
    while (pos < length)
    {
        unsigned int len = length - pos;
        if(len < sizeof(unsigned int))
        {
            break;
        }

        unsigned int iHeaderLen = ntohl(*(unsigned int*)(recvBuffer + pos));

        //Do a length protection: the length cannot be greater than M or less than sizeof (unsigned int)
        if (iHeaderLen > 100000 || iHeaderLen < sizeof(unsigned int))
        {
            throw TarsDecodeException("packet length too long or too short,len:" + TC_Common::tostr(iHeaderLen));
        }

        //Did not receive the complete packet
        if (len < iHeaderLen)
        {
            break;
        }
        else
        {
            ResponsePacket rsp;
			rsp.iRequestId = ntohl(*((unsigned int *)(recvBuffer + pos + sizeof(unsigned int))));
			rsp.sBuffer.resize(iHeaderLen - 2*sizeof(unsigned int));
		  ::memcpy(&rsp.sBuffer[0], recvBuffer + pos + 2*sizeof(unsigned int), iHeaderLen - 2*sizeof(unsigned int));

			pos += iHeaderLen;

            done.push_back(rsp);
        }
    }

    return pos;
}
/*
   Request packet encoding function
   The packing format of this function: the entire packet length (bytes) + iRequestId (bytes) + package contents
*/
static void pushRequest(const RequestPacket& request, string& buff)
{
    unsigned int net_bufflength = htonl(request.sBuffer.size()+8);
    unsigned char * bufflengthptr = (unsigned char*)(&net_bufflength);

    buff = "";
    for (int i = 0; i<4; ++i)
    {
        buff += *bufflengthptr++;
    }

    unsigned int netrequestId = htonl(request.iRequestId);
    unsigned char * netrequestIdptr = (unsigned char*)(&netrequestId);

    for (int i = 0; i<4; ++i)
    {
        buff += *netrequestIdptr++;
    }

    string tmp;
    tmp.assign((const char*)(&request.sBuffer[0]), request.sBuffer.size());
    buff+=tmp;
}

static void printResult(int iRequestId, const string &sResponseStr)
{
	cout << "request id: " << iRequestId << endl;
	cout << "response str: " << sResponseStr << endl;
}
static void printPushInfo(const string &sResponseStr)
{
	cout << "push message: " << sResponseStr << endl;
}

int TestPushCallBack::onDispatch(ReqMessagePtr msg)
{
	if(msg->request.sFuncName == "printResult")
	{
		string sRet;
		cout << "sBuffer: " << msg->response.sBuffer.size() << endl;
		sRet.assign(&(msg->response.sBuffer[0]), msg->response.sBuffer.size());
		printResult(msg->request.iRequestId, sRet);
		return 0;
	}
	else if(msg->response.iRequestId == 0)
	{
		string sRet;
		sRet.assign(&(msg->response.sBuffer[0]), msg->response.sBuffer.size());
		printPushInfo(sRet);
		return 0;
	}
	else
	{
		cout << "no match func!" <<endl;
	}
	return -3;
}

RecvThread::RecvThread():_bTerminate(false)
{
	string sObjName = "Test.TestPushServer.TestPushServantObj";
    string sObjHost = "tcp -h 10.120.129.226 -t 60000 -p 10099";

    _prx = _comm.stringToProxy<ServantPrx>(sObjName+"@"+sObjHost);

	ProxyProtocol prot;
    prot.requestFunc = pushRequest;
    prot.responseFunc = pushResponse;

    _prx->tars_set_protocol(prot);
}

void RecvThread::terminate()
{
	_bTerminate = true;
	{
	    tars::TC_ThreadLock::Lock sync(*this);
	    notifyAll();
	}
}

void RecvThread::run(void)
{
	TestPushCallBackPtr cbPush = new TestPushCallBack();
	_prx->tars_set_push_callback(cbPush);	

	string buf("heartbeat");

	while(!_bTerminate)
	{
		{
			try
			{
				TestPushCallBackPtr cb = new TestPushCallBack();
				_prx->rpc_call_async(_prx->tars_gen_requestid(), "printResult", buf.c_str(), buf.length(), cb);
			}
			catch(TarsException& e)
			{     
				cout << "TarsException: " << e.what() << endl;
			}
			catch(...)
			{
				cout << "unknown exception" << endl;
			}
		}

		{
            TC_ThreadLock::Lock sync(*this);
            timedWait(5000);
		}
	}
}

```


## Client test results

If the server pushes to the client successfully, the result is as follows:

![tars](../docs/images/tars_result.PNG)






