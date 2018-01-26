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

#include "servant/EndpointInfo.h"
#include "servant/TarsLogger.h"

namespace tars
{
EndpointInfo::EndpointInfo()
: _port(0)
, _grid(0)
, _qos(0)
, _type(TCP)
, _weight(-1)
, _weighttype(0)
, _authType(0)
{
    _setDivision.clear();
    memset(&_addr,0,sizeof(struct sockaddr_in));
}

EndpointInfo::EndpointInfo(const string& host, uint16_t port, EndpointInfo::EType type, int32_t grid, const string & setDivision, int qos, int weight, unsigned int weighttype, int authType)
: _host(host)
, _port(port)
, _grid(grid)
, _qos(qos)
, _type(type)
, _setDivision(setDivision)
, _weight(weight)
, _weighttype(weighttype)
, _authType(authType)
{
    try
    {
        if(_weighttype == 0)
        {
            _weight = -1;
        }
        else
        {
            if(_weight == -1)
            {    
                _weight = 100;
            }

            _weight = (_weight > 100 ? 100 : _weight);
        }

        NetworkUtil::getAddress(_host, _port, _addr);

        _cmpDesc = createCompareDesc();

        _desc = createDesc();
    }
    catch (...)
    {
        TLOGERROR("[ERROR:getAddress fail:" << _host << ":" << _port << "]" << endl);
    }
}

string EndpointInfo::createCompareDesc()
{
    ostringstream os;
    if (_type == EndpointInfo::UDP) { os << "udp:"; }
    if (_type == EndpointInfo::TCP) { os << "tcp:"; }
    if (_type == EndpointInfo::SSL) { os << "ssl:"; }
    os << _grid << ":" << _host << ":" << _port
       << ":" << _setDivision << ":" << _qos << ":" << _weight << ":" << _weighttype << ":" << _authType;

    return os.str();
}

string EndpointInfo::createDesc() const
{
    ostringstream os;

    if (_type == EndpointInfo::TCP)
        os << "tcp";
    else if (_type == EndpointInfo::UDP)
        os << "udp";
    else
        os << "ssl";

    os << " -h " << host();
    os << " -p " << port();
    if(0 != _grid)
        os << " -g " << _grid;

    if (!_setDivision.empty())
    {
        os << " -s " << _setDivision;
    }

    if(0 != _qos)
        os << " -q " << _qos;
    if(0 != _authType)
		os << " -e " << _authType;

    return os.str();
}

bool EndpointInfo::operator == (const EndpointInfo& r) const
{
    return (_cmpDesc == r._cmpDesc);
}

bool EndpointInfo::operator < (const EndpointInfo& r) const
{
    return (_cmpDesc < r._cmpDesc);
}

string EndpointInfo::host() const
{
    return string(_host);
}

int32_t EndpointInfo::grid() const
{
    return _grid;
}

uint16_t EndpointInfo::port() const
{
    return _port;
}

const struct sockaddr_in& EndpointInfo::addr() const
{
    return _addr;
}

EndpointInfo::EType EndpointInfo::type() const
{
    return _type;
}

const string& EndpointInfo::setDivision() const
{
    return _setDivision;
}
///////////////////////////////////////////////////////////
}
