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

#ifndef __TARS_ENDPOINT_INFO_H_
#define __TARS_ENDPOINT_INFO_H_

#include "servant/Global.h"
#include "servant/NetworkUtil.h"

using namespace std;

namespace tars
{
//////////////////////////////////////////////////////////////////////////////
/**
 * 地址信息IP:Port
 */
class EndpointInfo
{
public:
    enum EType { UDP = 0, TCP = 1, SSL = 2};

    /**
     * 构造函数
     */
    EndpointInfo();

    /**
     * 构造函数
     * @param host
     * @param port
     * @param type
     */
    EndpointInfo(const string& host, uint16_t port, EndpointInfo::EType type, int32_t grid, const string & setDivision, int qos, int weight = -1, unsigned int weighttype = 0, int authType = 0);

    /**
     * 地址的字符串描述,不带set信息
     *
     * @return string
     */
     const string& descNoSetInfo() const;

    /**
     * 地址的字符串描述
     *
     * @return string
     */
    const string & desc() const
    {
        return _desc;
    }

    /**
     * 比较的地址的字符串描述
     *
     * @return string
     */
    const string & compareDesc() const
    {
        return _cmpDesc;
    }

    /**
     * 详细地址字符串描述
     * 
     * @return string 
     */
    const string& fulldesc() const;

    /**
     * 获取主机名
     *
     * @return const string&
     */
    string host() const;

    /**
     * 获取端口号
     *
     * @return uint16_t
     */
    uint16_t port() const;

    /**
     * 获取路由状态
     * @return int32_t
     */
    int32_t grid() const;

    /*
     * 获取qos的descp值
     */
    int32_t qos() const {return _qos;}

    /*
     * 获取节点的静态权重值
     */
    int weight() const {return _weight;}

    /**
     * @brief 获取节点的权重使用方式
     */
    unsigned int getWeightType() const { return _weighttype; }

    /**
     * 获取主机地址
     *
     * @return const struct sockaddr_in&
     */
    const struct sockaddr_in& addr() const;

    /**
     * 返回端口类型
     *
     * @return EndpointInfo::EType
     */
    EndpointInfo::EType type() const;

    /**
    *设置set分组信息
    *
    *@return void
    */
    void setDivision(const string& sSetDivision);

    /**
    *返回set分组信息
    *
    *@return string
    */
    const string& setDivision() const;

    /*
     * 获取认证类型
     */
    int authType() const  { return _authType; }

    /**
     * 等于
     * @param r
     *
     * @return bool
     */
    bool operator == (const EndpointInfo& r) const;

    /**
    *等于,set信息不参与比较
    *@param r
    *
    *@return bool
    */
    bool equalNoSetInfo(const EndpointInfo& r) const;

    /**
     * 小于
     * @param r
     *
     * @return bool
     */
    bool operator < (const EndpointInfo& r) const;

protected:

    /**
     * 地址的字符串描述
     * @param bWithSetInfo,标识
     * @return string
     */
    string createDesc() const;

    /**
     * 详细地址字符串描述
     * 
     * @return string 
     */
    string createCompareDesc();

private:
    /**
     * 地址IP
     */
    string                   _host;

    /**
     * 端口号
     */
    uint16_t               _port;

    /**
     * 路由状态
     */
    int32_t                _grid;

    /*
     * qos的dscp值
     */

    int32_t                _qos;

    /**
     * 类型
     */
    EndpointInfo::EType    _type;

   /**
    *set分组信息
    */
    string                 _setDivision;

    /**
     *  节点的静态权重值
     */
    int                    _weight;

    /**
     *  节点的权重使用方式
     */
    unsigned int           _weighttype;

    /**
     * 地址
     */
    struct sockaddr_in     _addr;

    /**
     * 比较的地址字符串描述
     */
    string                 _cmpDesc;

    /**
     * 地址的字符串描述
     */
    string                 _desc;

    /**
     *  认证类型
     */
    int                    _authType;
};
/////////////////////////////////////////////////////////////////////////////
}
#endif
