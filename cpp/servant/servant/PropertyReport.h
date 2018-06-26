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

#ifndef __TARS_PROPERTY_REPORT_H_
#define __TARS_PROPERTY_REPORT_H_

#include "util/tc_autoptr.h"
#include "util/tc_thread_mutex.h"

#include <tuple>
#include <vector>
#include <string>

using namespace std;

namespace tars
{

///////////////////////////////////////////////////////////////////////////////////
//
/**
 * 用户自定义属性统计基类
 */
class PropertyReport : public TC_HandleBase
{
public:
    /**
     * 设置该属性的服务名称，如果不设置则为当前服务
     */
    void setMasterName(const std::string & sMasterName) { _sMasterName = sMasterName; }
    
    /**
     * 获取该属性的服务名称
     */
    std::string getMasterName() { return _sMasterName; }
    
public:

    /**
     * 求和
     */
    class sum
    {
    public:
        sum() :_d(0)                { }
        string get();
        string desc()               { return "Sum"; }
        void   set(int o)           { _d += o; }
    protected:
        void   clear()              { _d  = 0; }
    private:
        int   _d;
    };

    /**
     * 求平均
     */
    class avg
    {
    public:
        avg():_sum(0), _count(0)    { }
        string desc()               { return "Avg"; }
        string get();
        void   set(int o)           { _sum += o;++_count; }
    protected:
        void   clear()                { _sum = 0; _count = 0; }
    private:
        int _sum;
        int _count;
    };

    /**
     * 分布
     */
    class distr
    {
    public:
        distr(){};
        distr(const vector<int>& range);
        string desc()               { return "Distr"; }
        void   set(int o);
        string get();
    protected:
        void clear()                { _result.clear();}
    private:
        vector<int>     _range;
        vector<size_t>  _result;
    };

    /**
     * 求最大值
     */
    class max
    {
    public:
        max() : _d(-9999999)        { }
        string desc()               { return "Max"; }
        string get();
        void   set(int o)           { _d < o?_d = o:1; }
    protected:
        void   clear()              { _d = 0; }
    private:
        int   _d;
    };

    /**
     * 求最小值
     */
    class min
    {
    public:
        min():_d(0)                 { }
        string desc()               { return "Min"; }
        string get();
        void   set(int o);
    protected:
        void   clear()              { _d = 0; }
    private:
        int   _d;
    };

    /**
     * 计数
     */
    class count
    {
    public:
        count():_d(0)               { }
        string desc()               { return "Count"; }
        string get();
        void   set(int o)           { _d++; }
    protected:
        void   clear()              { _d = 0; }
    private:
        int   _d;
    };

public:

    virtual void report(int iValue)             = 0;
    virtual vector<pair<string, string> > get() = 0;

protected:
    std::string _sMasterName;   //属性所属服务名称
};

typedef TC_AutoPtr<PropertyReport> PropertyReportPtr;

///////////////////////////////////////////////////////////////////////////////////
//
/**
 * 具体的属性策略管理
 */

template <typename... Params>
class PropertyReportImp : public PropertyReport, public TC_ThreadMutex
{
public:
    using PropertyReportData = std::tuple<Params...>;

    PropertyReportImp(Params&&... args) :
        _propertyReportData(std::forward<Params>(args)...)
    {
    }

    // do NOT copy
    PropertyReportImp(const PropertyReportImp& ) = delete;
    void operator = (const PropertyReportImp& ) = delete;

    // but CAN move
    PropertyReportImp(PropertyReportImp&& ) = default;
    PropertyReportImp& operator= (PropertyReportImp&& ) = default;


    /**
    * 设置调用数据
    * @param iValue,值
    */
    void report(int iValue) override
    {
        TC_LockT<TC_ThreadMutex> lock(*this);
        Helper<std::tuple_size<decltype(_propertyReportData)>::value>::Report(*this, iValue);
    }


    /**
     * 获取属性信息
     *
     * @return vector<pair<string, string>>
     */
    vector<pair<string, string> > get() override
    {
        TC_LockT<TC_ThreadMutex> lock(*this);
        return Helper<std::tuple_size<decltype(_propertyReportData)>::value>::Get(*this);
    }

private:
    // report helper
    template <int N, typename DUMMY = void>
    struct Helper
    {
        static void Report(PropertyReportImp<Params...>& pp, int iValue)
        {
            static_assert(N >= 1, "Obviously success");
            Helper<N - 1, DUMMY>::Report(pp, iValue);
            pp.template SetResult<N - 1>(iValue);
        }

        static std::vector<std::pair<std::string, std::string>> Get(PropertyReportImp<Params...>& pp)
        {
            static_assert(N >= 1, "Obviously success");
                
            std::vector<std::pair<std::string, std::string> > vs = Helper<N-1, DUMMY>::Get(pp);

            vs.push_back({std::get<N - 1>(pp._propertyReportData).desc(), std::get<N-1>(pp._propertyReportData).get()});
            return vs;
        }
    };

    template <typename DUMMY>
    struct Helper<0, DUMMY>
    {
        // base template
        static void Report(PropertyReportImp<Params...>&, int  )
        {
        }

        static 
        std::vector<std::pair<std::string, std::string>> Get(PropertyReportImp<Params...>& )
        {
            return std::vector<std::pair<std::string, std::string> >();
        }
    };

    template <int I>
    void SetResult(int iValue)
    {
        std::get<I>(_propertyReportData).set(iValue);
    }
    /**
     * 状态报告数据
     */
    PropertyReportData  _propertyReportData;
};

}

#endif
