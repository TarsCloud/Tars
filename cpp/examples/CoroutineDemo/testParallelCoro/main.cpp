/**
 * Tencent is pleased to support the open source community by making Tars available.
 *
 * Copyright (C) 2016THL A29 Limited, a Tencent company. All rights reserved.
 *
 * Licensed under the BSD 3-Clause License (the "License"); you may not use this file except 
 * in compliance with the License. You may obtain a copy of the License at
 *
 * https://opensource.org/licenses/BSD-3-Clause
 *
 * Unless required by applicable law or agreed to in writing, software distributed 
 * under the License is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR 
 * CONDITIONS OF ANY KIND, either express or implied. See the License for the 
 * specific language governing permissions and limitations under the License.
 */

#include "BServant.h"
#include "servant/Communicator.h"
#include "servant/CoroutineScheduler.h"
#include <iostream>

using namespace std;
using namespace Test;
using namespace tars;

class BServantCoroCallback : public BServantCoroPrxCallback
{
public:
    virtual ~BServantCoroCallback(){}

    virtual void callback_testCoroSerial(tars::Int32 ret, const std::string &sOut) // override
    {
    		_iRet = ret;
    		_sOut = sOut;
    }
    virtual void callback_testCoroSerial_exception(tars::Int32 ret) // override
    {
    		_iException = ret;
    }

  virtual void callback_testCoroParallel(tars::Int32 ret, const std::string &sOut) // override
    {
    		_iRet = ret;
    		_sOut = sOut;
    }

  virtual void callback_testCoroParallel_exception(tars::Int32 ret) // override
  {
    		_iException = ret;
  }

public:
	int		_iException;
	int		_iRet;
	int		_iOut;
	string	_sOut;
};
typedef tars::TC_AutoPtr<BServantCoroCallback> BServantCoroCallbackPtr;

//自定义协程类
class CoroutineClass : public TC_Thread
{
public:
	/**
     * 构造函数
     */
	CoroutineClass();

	/**
     * 析构函数
     */
	virtual ~CoroutineClass();

	/**
     * 返回0，代表成功，-1，表示失败
     */
	int registerFunc(const vector< tars::TC_Callback<void ()> > &vFunc);

	/**
     * 线程初始化
     */
	virtual void initialize() {}

	/**
	 * 线程处理方法
	 */
	virtual void run();

	/**
	 * 停止线程
	 */
	void terminate();

protected:
	/**
	 * 线程已经启动, 进入具体协程处理前调用
	 */
	virtual void startCoro() {}

	/**
	 * 线程马上要退出时调用
	 */
	virtual void stopCoro() {}

	/**
	 * 具体的处理逻辑
	 */
	virtual void handleCoro();

protected:
	CoroutineScheduler *_coroSched;
	uint32_t			_iPoolSize;
	size_t				_iStackSize;
	vector< tars::TC_Callback<void ()> > _vFunc;
};

CoroutineClass::CoroutineClass()
: _coroSched(NULL)
, _iPoolSize(1024)
, _iStackSize(128*1024)
{
}

CoroutineClass::~CoroutineClass()
{
	if(isAlive())
    {
        terminate();

        getThreadControl().join();
    }
}

int CoroutineClass::registerFunc(const vector< tars::TC_Callback<void ()> > &vFunc)
{
	if(vFunc.size() > _iPoolSize || vFunc.size() <= 0)
	{
		return -1;
	}

	_vFunc = vFunc;

	return 0;
}

void CoroutineClass::run()
{
	initialize();

	startCoro();

	handleCoro();

	stopCoro();
}

void CoroutineClass::terminate()
{
	if(_coroSched)
	{
		_coroSched->terminate();
	}
}

void CoroutineClass::handleCoro()
{
	_coroSched = new CoroutineScheduler();

	_coroSched->init(_iPoolSize, _iStackSize);

	ServantProxyThreadData * pSptd = ServantProxyThreadData::getData();

	assert(pSptd != NULL);

	pSptd->_sched = _coroSched;

	for(size_t i = 0; i < _vFunc.size(); ++i)
	{
		_coroSched->createCoroutine(_vFunc[i]);
	}

	_coroSched->run();

	delete _coroSched;
	_coroSched = NULL;
}

////////////////////////////////////////////
//继承框架的协程类
class TestCoroutine : public Coroutine
{
public:
	TestCoroutine(int iNum, const string &sObj);

	~TestCoroutine() {}

	void handle();

private:
	int _num;
	string _sObj;
	Communicator _comm;
	BServantPrx _prx;
};

TestCoroutine::TestCoroutine(int iNum, const string &sObj)
: _num(iNum)
, _sObj(sObj)
{
	_comm.setProperty("locator", "tars.tarsregistry.QueryObj@tcp -h 10.208.139.242 -p 17890 -t 10000");
	_comm.stringToProxy(_sObj, _prx);
}

void TestCoroutine::handle()
{
	string sIn(32,'a');
	unsigned long sum = 0;

	for(int i = 0; i < _num; i++) 
	{
		try
		{
            CoroParallelBasePtr sharedPtr = new CoroParallelBase(2);

			BServantCoroCallbackPtr cb1 = new BServantCoroCallback();
			cb1->setCoroParallelBasePtr(sharedPtr);
			_prx->coro_testCoroSerial(cb1, sIn);

			BServantCoroCallbackPtr cb2 = new BServantCoroCallback();
			cb2->setCoroParallelBasePtr(sharedPtr);
			_prx->coro_testCoroParallel(cb2, sIn);

			coroWhenAll(sharedPtr);

			cout << "ret1:" << cb1->_sOut << "|ret2:" << cb2->_sOut << endl;
			;
			if(cb1->_iRet == 0 && cb2->_iRet == 0 && cb1->_iException == 0 && cb2->_iException == 0)
			{
				++sum;
			}
		}
		catch(TC_Exception &e)
		{
			cout << "i: " << i << "exception: " << e.what() << endl;
		}
		catch(...)
		{
			cout << "i: " << i << "unknown exception." << endl;
		}

	}
	cout << "succ:" << sum <<endl;
}

int main(int argc,char ** argv)
{
	if(argc != 3)
	{
		cout << "usage: " << argv[0] << " CallTimes sObj" << endl;
		return -1;
	}

	tars::Int32 iNum = TC_Common::strto<tars::Int32>(string(argv[1]));

	string sObj = string(argv[2]);

	TestCoroutine testCoro(iNum, sObj);

	testCoro.setCoroInfo(10, 128, 128*1024);

	testCoro.start();
		
	testCoro.getThreadControl().join();
	
    return 0;
}
