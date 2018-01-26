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

#include "BServantImp.h"
#include "BServer.h"
#include "servant/Application.h"
#include "servant/Communicator.h"

using namespace std;
using namespace tars;


//////////////////////////////////////////////////////
void BServantImp::initialize()
{
	//initialize servant here:
	//...
	_pPrx = Application::getCommunicator()->stringToProxy<AServantPrx>("Test.AServer.AServantObj");
}
//////////////////////////////////////////////////////
void BServantImp::destroy()
{
}

class AServantCoroCallback : public AServantCoroPrxCallback
{
public:
    virtual ~AServantCoroCallback(){}

    virtual void callback_testInt(tars::Int32 ret, tars::Int32 iOut)
    { 
		_iRet = ret;
		_iOut = iOut;
	}
    virtual void callback_testInt_exception(tars::Int32 ret)
    { 
		_iException = ret; 
	}

    virtual void callback_testStr(tars::Int32 ret,  const std::string& sOut)
    { 
		_iRet = ret;
		_sOut = sOut; 
	}
    virtual void callback_testStr_exception(tars::Int32 ret)
    { 
		_iException = ret;  
	}

public:
	int		_iException;
	int		_iRet;
	int		_iOut;
	string	_sOut;
};
typedef tars::TC_AutoPtr<AServantCoroCallback> AServantCoroCallbackPtr;

int BServantImp::test(tars::TarsCurrentPtr current) { return 0;}

tars::Int32 BServantImp::testCoroSerial(const std::string& sIn, std::string &sOut, tars::TarsCurrentPtr current)
{
    try
    {
        int	iRet = -1;

	    int	iIn  = 5;
	    int	iOut = 0;

	    iRet = _pPrx->testInt(iIn, iOut);

        if(iRet == 0)
        {
            string	sRet("");

	        iRet =	_pPrx->testStr(sIn, sRet);

            if(iRet == 0)
            {
                sOut = sRet;
            }
        }

        return iRet;
    }
    catch(exception &ex)
    {
        TLOGERROR("BServantImp::testCoroSerial exception:" << ex.what() << endl);
    }

    return -1;
}

tars::Int32 BServantImp::testCoroParallel(const std::string& sIn, std::string &sOut, tars::TarsCurrentPtr current)
{
    try
    {
        int iRet = -1;

        int iIn  = 5;

	    CoroParallelBasePtr sharedPtr = new CoroParallelBase(2);

	    AServantCoroCallbackPtr cb1 = new AServantCoroCallback();
	    cb1->setCoroParallelBasePtr(sharedPtr);
	    _pPrx->coro_testInt(cb1, iIn);

	    AServantCoroCallbackPtr cb2 = new AServantCoroCallback();
	    cb2->setCoroParallelBasePtr(sharedPtr);
	    _pPrx->coro_testStr(cb2, sIn);

	    coroWhenAll(sharedPtr);

	    if(cb1->_iRet == 0 && cb2->_iRet == 0)
	    {
		    sOut = cb2->_sOut;
            iRet = 0;
	    }

        return iRet;
    }
    catch(exception &ex)
    {
        TLOGERROR("BServantImp::testCoroParallel exception:" << ex.what() << endl);
    }

    return -1;
}
