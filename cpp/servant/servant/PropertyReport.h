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

#include "util/tc_loki.h"
#include "util/tc_autoptr.h"
#include "util/tc_thread_mutex.h"

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
template<typename TList>
class PropertyReportImp : public PropertyReport, public TC_ThreadMutex
{
public:
    typedef TL::Tuple<TList> PropertyReportData;

    //定义类型列表中每个参数的原类型
    typedef typename TL::TypeTraits<typename TL::TypeAtNonStrict<TList, 0, TL::EmptyType>::Result>::ParameterType Param1;
    typedef typename TL::TypeTraits<typename TL::TypeAtNonStrict<TList, 1, TL::EmptyType>::Result>::ParameterType Param2;
    typedef typename TL::TypeTraits<typename TL::TypeAtNonStrict<TList, 2, TL::EmptyType>::Result>::ParameterType Param3;
    typedef typename TL::TypeTraits<typename TL::TypeAtNonStrict<TList, 3, TL::EmptyType>::Result>::ParameterType Param4;
    typedef typename TL::TypeTraits<typename TL::TypeAtNonStrict<TList, 4, TL::EmptyType>::Result>::ParameterType Param5;
    typedef typename TL::TypeTraits<typename TL::TypeAtNonStrict<TList, 5, TL::EmptyType>::Result>::ParameterType Param6;
    typedef typename TL::TypeTraits<typename TL::TypeAtNonStrict<TList, 6, TL::EmptyType>::Result>::ParameterType Param7;
    typedef typename TL::TypeTraits<typename TL::TypeAtNonStrict<TList, 7, TL::EmptyType>::Result>::ParameterType Param8;
    typedef typename TL::TypeTraits<typename TL::TypeAtNonStrict<TList, 8, TL::EmptyType>::Result>::ParameterType Param9;
    typedef typename TL::TypeTraits<typename TL::TypeAtNonStrict<TList, 9, TL::EmptyType>::Result>::ParameterType Param10;

    /**
     * 构造函数
     * @param p1
     */
    PropertyReportImp(Param1 p1)
    {
        TL::field<0>(_propertyReportData) = p1;
    }

    /**
     * 构造函数
     * @param p1
     * @param p2
     */
    PropertyReportImp(Param1 p1, Param2 p2)
    {
        TL::field<0>(_propertyReportData) = p1;
        TL::field<1>(_propertyReportData) = p2;
    }

    /**
     * 三参数构造函数
     * @param p1
     * @param p2
     * @param p3
     */
    PropertyReportImp(Param1 p1, Param2 p2, Param3 p3)
    {
        TL::field<0>(_propertyReportData) = p1;
        TL::field<1>(_propertyReportData) = p2;
        TL::field<2>(_propertyReportData) = p3;
    }

    /**
     * 四参数构造函数
     * @param p1
     * @param p2
     * @param p3
     * @param p4
     */
    PropertyReportImp(Param1 p1, Param2 p2, Param3 p3, Param4 p4)
    {
        TL::field<0>(_propertyReportData) = p1;
        TL::field<1>(_propertyReportData) = p2;
        TL::field<2>(_propertyReportData) = p3;
        TL::field<3>(_propertyReportData) = p4;
    }

    /**
     * 五参数构造函数
     * @param p1
     * @param p2
     * @param p3
     * @param p4
     * @param p5
     */
    PropertyReportImp(Param1 p1, Param2 p2, Param3 p3, Param4 p4, Param5 p5)
    {
        TL::field<0>(_propertyReportData) = p1;
        TL::field<1>(_propertyReportData) = p2;
        TL::field<2>(_propertyReportData) = p3;
        TL::field<3>(_propertyReportData) = p4;
        TL::field<4>(_propertyReportData) = p5;
    }

    /**
     * 六参数构造函数
     * @param p1
     * @param p2
     * @param p3
     * @param p4
     * @param p5
     * @param p6
     */
    PropertyReportImp(Param1 p1, Param2 p2, Param3 p3, Param4 p4, Param5 p5, Param6 p6)
    {
        TL::field<0>(_propertyReportData) = p1;
        TL::field<1>(_propertyReportData) = p2;
        TL::field<2>(_propertyReportData) = p3;
        TL::field<3>(_propertyReportData) = p4;
        TL::field<4>(_propertyReportData) = p5;
        TL::field<5>(_propertyReportData) = p6;
    }

    /**
     * 七参数构造函数
     * @param p1
     * @param p2
     * @param p3
     * @param p4
     * @param p5
     * @param p6
     * @param p7
     */
    PropertyReportImp(Param1 p1, Param2 p2, Param3 p3, Param4 p4, Param5 p5, Param6 p6, Param7 p7)
    {
        TL::field<0>(_propertyReportData) = p1;
        TL::field<1>(_propertyReportData) = p2;
        TL::field<2>(_propertyReportData) = p3;
        TL::field<3>(_propertyReportData) = p4;
        TL::field<4>(_propertyReportData) = p5;
        TL::field<5>(_propertyReportData) = p6;
        TL::field<6>(_propertyReportData) = p7;
    }

    /**
     * 八参数构造函数
     * @param p1
     * @param p2
     * @param p3
     * @param p4
     * @param p5
     * @param p6
     * @param p7
     * @param p8
     */
    PropertyReportImp(Param1 p1, Param2 p2, Param3 p3, Param4 p4, Param5 p5, Param6 p6, Param7 p7, Param8 p8)
    {
        TL::field<0>(_propertyReportData) = p1;
        TL::field<1>(_propertyReportData) = p2;
        TL::field<2>(_propertyReportData) = p3;
        TL::field<3>(_propertyReportData) = p4;
        TL::field<4>(_propertyReportData) = p5;
        TL::field<5>(_propertyReportData) = p6;
        TL::field<6>(_propertyReportData) = p7;
        TL::field<7>(_propertyReportData) = p8;
    }

    /**
     * 九参数构造函数
     * @param p1
     * @param p2
     * @param p3
     * @param p4
     * @param p5
     * @param p6
     * @param p7
     * @param p8
     * @param p9
     */
    PropertyReportImp(Param1 p1, Param2 p2, Param3 p3, Param4 p4, Param5 p5, Param6 p6, Param7 p7, Param8 p8, Param9 p9)
    {
        TL::field<0>(_propertyReportData) = p1;
        TL::field<1>(_propertyReportData) = p2;
        TL::field<2>(_propertyReportData) = p3;
        TL::field<3>(_propertyReportData) = p4;
        TL::field<4>(_propertyReportData) = p5;
        TL::field<5>(_propertyReportData) = p6;
        TL::field<6>(_propertyReportData) = p7;
        TL::field<7>(_propertyReportData) = p8;
        TL::field<8>(_propertyReportData) = p9;
    }

    /**
     * 十参数构造函数
     * @param p1
     * @param p2
     * @param p3
     * @param p4
     * @param p5
     * @param p6
     * @param p7
     * @param p8
     * @param p9
     * @param p10
     */
    PropertyReportImp(Param1 p1, Param2 p2, Param3 p3, Param4 p4, Param5 p5, Param6 p6, Param7 p7, Param8 p8, Param9 p9, Param10 p10)
    {
        TL::field<0>(_propertyReportData) = p1;
        TL::field<1>(_propertyReportData) = p2;
        TL::field<2>(_propertyReportData) = p3;
        TL::field<3>(_propertyReportData) = p4;
        TL::field<4>(_propertyReportData) = p5;
        TL::field<5>(_propertyReportData) = p6;
        TL::field<6>(_propertyReportData) = p7;
        TL::field<7>(_propertyReportData) = p8;
        TL::field<8>(_propertyReportData) = p9;
        TL::field<9>(_propertyReportData) = p10;
    }

    /**
    * 设置调用数据
    * @param iValue,值
    */
    virtual void report(int iValue)
    {
        TC_LockT<TC_ThreadMutex> lock(*this);
        report(iValue, TL::Int2Type<TL::Length<TList>::value-1>());
    }

    /**
     * 获取属性信息
     *
     * @return vector<pair<string, string>>
     */
    virtual vector<pair<string, string> > get()
    {
        TC_LockT<TC_ThreadMutex> lock(*this);
        return get(TL::Int2Type<TL::Length<TList>::value-1>());
    }
protected:
    void report(int iValue, TL::Int2Type<0>)
    {
        TL::field<0>(_propertyReportData).set(iValue);
    }
    void report(int iValue, TL::Int2Type<1>)
    {
        TL::field<0>(_propertyReportData).set(iValue);
        TL::field<1>(_propertyReportData).set(iValue);
    }
    void report(int iValue, TL::Int2Type<2>)
    {
        TL::field<0>(_propertyReportData).set(iValue);
        TL::field<1>(_propertyReportData).set(iValue);
        TL::field<2>(_propertyReportData).set(iValue);
    }
    void report(int iValue, TL::Int2Type<3>)
    {
        TL::field<0>(_propertyReportData).set(iValue);
        TL::field<1>(_propertyReportData).set(iValue);
        TL::field<2>(_propertyReportData).set(iValue);
        TL::field<3>(_propertyReportData).set(iValue);
    }
    void report(int iValue, TL::Int2Type<4>)
    {
        TL::field<0>(_propertyReportData).set(iValue);
        TL::field<1>(_propertyReportData).set(iValue);
        TL::field<2>(_propertyReportData).set(iValue);
        TL::field<3>(_propertyReportData).set(iValue);
        TL::field<4>(_propertyReportData).set(iValue);
    }
    void report(int iValue, TL::Int2Type<5>)
    {
        TL::field<0>(_propertyReportData).set(iValue);
        TL::field<1>(_propertyReportData).set(iValue);
        TL::field<2>(_propertyReportData).set(iValue);
        TL::field<3>(_propertyReportData).set(iValue);
        TL::field<4>(_propertyReportData).set(iValue);
        TL::field<5>(_propertyReportData).set(iValue);
    }
    void report(int iValue, TL::Int2Type<6>)
    {
        TL::field<0>(_propertyReportData).set(iValue);
        TL::field<1>(_propertyReportData).set(iValue);
        TL::field<2>(_propertyReportData).set(iValue);
        TL::field<3>(_propertyReportData).set(iValue);
        TL::field<4>(_propertyReportData).set(iValue);
        TL::field<5>(_propertyReportData).set(iValue);
        TL::field<6>(_propertyReportData).set(iValue);
    }
    void report(int iValue, TL::Int2Type<7>)
    {
        TL::field<0>(_propertyReportData).set(iValue);
        TL::field<1>(_propertyReportData).set(iValue);
        TL::field<2>(_propertyReportData).set(iValue);
        TL::field<3>(_propertyReportData).set(iValue);
        TL::field<4>(_propertyReportData).set(iValue);
        TL::field<5>(_propertyReportData).set(iValue);
        TL::field<6>(_propertyReportData).set(iValue);
        TL::field<7>(_propertyReportData).set(iValue);
    }
    void report(int iValue, TL::Int2Type<8>)
    {
        TL::field<0>(_propertyReportData).set(iValue);
        TL::field<1>(_propertyReportData).set(iValue);
        TL::field<2>(_propertyReportData).set(iValue);
        TL::field<3>(_propertyReportData).set(iValue);
        TL::field<4>(_propertyReportData).set(iValue);
        TL::field<5>(_propertyReportData).set(iValue);
        TL::field<6>(_propertyReportData).set(iValue);
        TL::field<7>(_propertyReportData).set(iValue);
        TL::field<8>(_propertyReportData).set(iValue);
    }
    void report(int iValue, TL::Int2Type<9>)
    {
        TL::field<0>(_propertyReportData).set(iValue);
        TL::field<1>(_propertyReportData).set(iValue);
        TL::field<2>(_propertyReportData).set(iValue);
        TL::field<3>(_propertyReportData).set(iValue);
        TL::field<4>(_propertyReportData).set(iValue);
        TL::field<5>(_propertyReportData).set(iValue);
        TL::field<6>(_propertyReportData).set(iValue);
        TL::field<7>(_propertyReportData).set(iValue);
        TL::field<8>(_propertyReportData).set(iValue);
        TL::field<9>(_propertyReportData).set(iValue);
    }

    vector<pair<string, string> > get(TL::Int2Type<0>)
    {
        vector<pair<string, string> > vs;
        pair<string, string> sp;
        sp.first   = TL::field<0>(_propertyReportData).desc();
        sp.second  = TL::field<0>(_propertyReportData).get();
        vs.push_back(sp);

        return vs;
    }
    vector<pair<string, string> > get(TL::Int2Type<1>)
    {
        vector<pair<string, string> > vs;
        pair<string, string> sp;
        sp.first   = TL::field<0>(_propertyReportData).desc();
        sp.second  = TL::field<0>(_propertyReportData).get();
        vs.push_back(sp);
        sp.first   = TL::field<1>(_propertyReportData).desc();
        sp.second  = TL::field<1>(_propertyReportData).get();
        vs.push_back(sp);

        return vs;
    }
    vector<pair<string, string> > get(TL::Int2Type<2>)
    {
        vector<pair<string, string> > vs;
        pair<string, string> sp;
        sp.first   = TL::field<0>(_propertyReportData).desc();
        sp.second  = TL::field<0>(_propertyReportData).get();
        vs.push_back(sp);
        sp.first   = TL::field<1>(_propertyReportData).desc();
        sp.second  = TL::field<1>(_propertyReportData).get();
        vs.push_back(sp);
        sp.first   = TL::field<2>(_propertyReportData).desc();
        sp.second  = TL::field<2>(_propertyReportData).get();
        vs.push_back(sp);

        return vs;
    }
    vector<pair<string, string> > get(TL::Int2Type<3>)
    {
        vector<pair<string, string> > vs;
        pair<string, string> sp;
        sp.first   = TL::field<0>(_propertyReportData).desc();
        sp.second  = TL::field<0>(_propertyReportData).get();
        vs.push_back(sp);
        sp.first   = TL::field<1>(_propertyReportData).desc();
        sp.second  = TL::field<1>(_propertyReportData).get();
        vs.push_back(sp);
        sp.first   = TL::field<2>(_propertyReportData).desc();
        sp.second  = TL::field<2>(_propertyReportData).get();
        vs.push_back(sp);
        sp.first   = TL::field<3>(_propertyReportData).desc();
        sp.second  = TL::field<3>(_propertyReportData).get();
        vs.push_back(sp);

        return vs;
    }
    vector<pair<string, string> > get(TL::Int2Type<4>)
    {
        vector<pair<string, string> > vs;
        pair<string, string> sp;
        sp.first   = TL::field<0>(_propertyReportData).desc();
        sp.second  = TL::field<0>(_propertyReportData).get();
        vs.push_back(sp);
        sp.first   = TL::field<1>(_propertyReportData).desc();
        sp.second  = TL::field<1>(_propertyReportData).get();
        vs.push_back(sp);
        sp.first   = TL::field<2>(_propertyReportData).desc();
        sp.second  = TL::field<2>(_propertyReportData).get();
        vs.push_back(sp);
        sp.first   = TL::field<3>(_propertyReportData).desc();
        sp.second  = TL::field<3>(_propertyReportData).get();
        vs.push_back(sp);
        sp.first   = TL::field<4>(_propertyReportData).desc();
        sp.second  = TL::field<4>(_propertyReportData).get();
        vs.push_back(sp);
        return vs;
    }
    vector<pair<string, string> > get(TL::Int2Type<5>)
    {
        vector<pair<string, string> > vs;
        pair<string, string> sp;
        sp.first   = TL::field<0>(_propertyReportData).desc();
        sp.second  = TL::field<0>(_propertyReportData).get();
        vs.push_back(sp);
        sp.first   = TL::field<1>(_propertyReportData).desc();
        sp.second  = TL::field<1>(_propertyReportData).get();
        vs.push_back(sp);
        sp.first   = TL::field<2>(_propertyReportData).desc();
        sp.second  = TL::field<2>(_propertyReportData).get();
        vs.push_back(sp);
        sp.first   = TL::field<3>(_propertyReportData).desc();
        sp.second  = TL::field<3>(_propertyReportData).get();
        vs.push_back(sp);
        sp.first   = TL::field<4>(_propertyReportData).desc();
        sp.second  = TL::field<4>(_propertyReportData).get();
        vs.push_back(sp);
        sp.first   = TL::field<5>(_propertyReportData).desc();
        sp.second  = TL::field<5>(_propertyReportData).get();
        vs.push_back(sp);

        return vs;
    }
    vector<pair<string, string> > get(TL::Int2Type<6>)
    {
        vector<pair<string, string> > vs;
        pair<string, string> sp;
        sp.first   = TL::field<0>(_propertyReportData).desc();
        sp.second  = TL::field<0>(_propertyReportData).get();
        vs.push_back(sp);
        sp.first   = TL::field<1>(_propertyReportData).desc();
        sp.second  = TL::field<1>(_propertyReportData).get();
        vs.push_back(sp);
        sp.first   = TL::field<2>(_propertyReportData).desc();
        sp.second  = TL::field<2>(_propertyReportData).get();
        vs.push_back(sp);
        sp.first   = TL::field<3>(_propertyReportData).desc();
        sp.second  = TL::field<3>(_propertyReportData).get();
        vs.push_back(sp);
        sp.first   = TL::field<4>(_propertyReportData).desc();
        sp.second  = TL::field<4>(_propertyReportData).get();
        vs.push_back(sp);
        sp.first   = TL::field<5>(_propertyReportData).desc();
        sp.second  = TL::field<5>(_propertyReportData).get();
        vs.push_back(sp);
        sp.first   = TL::field<6>(_propertyReportData).desc();
        sp.second  = TL::field<6>(_propertyReportData).get();
        vs.push_back(sp);

        return vs;
    }
    vector<pair<string, string> > get(TL::Int2Type<7>)
    {
        vector<pair<string, string> > vs;
        pair<string, string> sp;
        sp.first   = TL::field<0>(_propertyReportData).desc();
        sp.second  = TL::field<0>(_propertyReportData).get();
        vs.push_back(sp);
        sp.first   = TL::field<1>(_propertyReportData).desc();
        sp.second  = TL::field<1>(_propertyReportData).get();
        vs.push_back(sp);
        sp.first   = TL::field<2>(_propertyReportData).desc();
        sp.second  = TL::field<2>(_propertyReportData).get();
        vs.push_back(sp);
        sp.first   = TL::field<3>(_propertyReportData).desc();
        sp.second  = TL::field<3>(_propertyReportData).get();
        vs.push_back(sp);
        sp.first   = TL::field<4>(_propertyReportData).desc();
        sp.second  = TL::field<4>(_propertyReportData).get();
        vs.push_back(sp);
        sp.first   = TL::field<5>(_propertyReportData).desc();
        sp.second  = TL::field<5>(_propertyReportData).get();
        vs.push_back(sp);
        sp.first   = TL::field<6>(_propertyReportData).desc();
        sp.second  = TL::field<6>(_propertyReportData).get();
        vs.push_back(sp);
        sp.first   = TL::field<7>(_propertyReportData).desc();
        sp.second  = TL::field<7>(_propertyReportData).get();
        vs.push_back(sp);

        return vs;
    }
    vector<pair<string, string> > get(TL::Int2Type<8>)
    {
        vector<pair<string, string> > vs;
        pair<string, string> sp;
        sp.first   = TL::field<0>(_propertyReportData).desc();
        sp.second  = TL::field<0>(_propertyReportData).get();
        vs.push_back(sp);
        sp.first   = TL::field<1>(_propertyReportData).desc();
        sp.second  = TL::field<1>(_propertyReportData).get();
        vs.push_back(sp);
        sp.first   = TL::field<2>(_propertyReportData).desc();
        sp.second  = TL::field<2>(_propertyReportData).get();
        vs.push_back(sp);
        sp.first   = TL::field<3>(_propertyReportData).desc();
        sp.second  = TL::field<3>(_propertyReportData).get();
        vs.push_back(sp);
        sp.first   = TL::field<4>(_propertyReportData).desc();
        sp.second  = TL::field<4>(_propertyReportData).get();
        vs.push_back(sp);
        sp.first   = TL::field<5>(_propertyReportData).desc();
        sp.second  = TL::field<5>(_propertyReportData).get();
        vs.push_back(sp);
        sp.first   = TL::field<6>(_propertyReportData).desc();
        sp.second  = TL::field<6>(_propertyReportData).get();
        vs.push_back(sp);
        sp.first   = TL::field<7>(_propertyReportData).desc();
        sp.second  = TL::field<7>(_propertyReportData).get();
        vs.push_back(sp);
        sp.first   = TL::field<8>(_propertyReportData).desc();
        sp.second  = TL::field<8>(_propertyReportData).get();
        vs.push_back(sp);

        return vs;
    }
    vector<pair<string, string> > get(TL::Int2Type<9>)
    {
        vector<pair<string, string> > vs;
        pair<string, string> sp;
        sp.first   = TL::field<0>(_propertyReportData).desc();
        sp.second  = TL::field<0>(_propertyReportData).get();
        vs.push_back(sp);
        sp.first   = TL::field<1>(_propertyReportData).desc();
        sp.second  = TL::field<1>(_propertyReportData).get();
        vs.push_back(sp);
        sp.first   = TL::field<2>(_propertyReportData).desc();
        sp.second  = TL::field<2>(_propertyReportData).get();
        vs.push_back(sp);
        sp.first   = TL::field<3>(_propertyReportData).desc();
        sp.second  = TL::field<3>(_propertyReportData).get();
        vs.push_back(sp);
        sp.first   = TL::field<4>(_propertyReportData).desc();
        sp.second  = TL::field<4>(_propertyReportData).get();
        vs.push_back(sp);
        sp.first   = TL::field<5>(_propertyReportData).desc();
        sp.second  = TL::field<5>(_propertyReportData).get();
        vs.push_back(sp);
        sp.first   = TL::field<6>(_propertyReportData).desc();
        sp.second  = TL::field<6>(_propertyReportData).get();
        vs.push_back(sp);
        sp.first   = TL::field<7>(_propertyReportData).desc();
        sp.second  = TL::field<7>(_propertyReportData).get();
        vs.push_back(sp);
        sp.first   = TL::field<8>(_propertyReportData).desc();
        sp.second  = TL::field<8>(_propertyReportData).get();
        vs.push_back(sp);
        sp.first   = TL::field<9>(_propertyReportData).desc();
        sp.second  = TL::field<9>(_propertyReportData).get();
        vs.push_back(sp);
        return vs;
    }


private:
    /**
     * 状态报告数据
     */
    PropertyReportData  _propertyReportData;
};

}

#endif
