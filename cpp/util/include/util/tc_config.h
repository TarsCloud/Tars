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

#ifndef __TC_CONFIG_H_
#define __TC_CONFIG_H_

#include <map>
#include <list>
#include <stack>
#include <vector>
#include "util/tc_ex.h"

namespace tars
{
/////////////////////////////////////////////////
/** 
 * @file  tc_config.h 
 * @brief  配置文件读取类. 
 */
/////////////////////////////////////////////////

/**
* 域分隔符
*/
const char TC_CONFIG_DOMAIN_SEP = '/';

/**
* 参数开始符
*/
const char TC_CONFIG_PARAM_BEGIN = '<';

/**
* 参数结束符
*/
const char TC_CONFIG_PARAM_END = '>';

/**
* @brief 配置文件异常类.
*/
struct TC_Config_Exception : public TC_Exception
{
    TC_Config_Exception(const string &buffer) : TC_Exception(buffer){};
    TC_Config_Exception(const string &buffer, int err) : TC_Exception(buffer, err){};
    ~TC_Config_Exception() throw(){};
};

/**
 * @brief 配置没有文件错误
 */
struct TC_ConfigNoParam_Exception : public TC_Exception
{
    TC_ConfigNoParam_Exception(const string &buffer) : TC_Exception(buffer){};
    TC_ConfigNoParam_Exception(const string &buffer, int err) : TC_Exception(buffer, err){};
    ~TC_ConfigNoParam_Exception() throw(){};
};


/** 
* @brief 定义配置文件中的域的类. 
*/
class TC_ConfigDomain
{
public:
    friend class TC_Config;

    /**
    * @brief 构造函数. 
    *  
    * @param sLine  指配置文件中的一行，按行读取
    */
    TC_ConfigDomain(const string &sLine);

    /**
    *  @brief 析够函数.
    */
    ~TC_ConfigDomain();

    /**
     * @brief 拷贝复制.
     *  
     * @param tcd 一个TC_ConfigDomain对象，指配置文件的一个域
     */
    TC_ConfigDomain(const TC_ConfigDomain &tcd);

    /**
     * @brief 赋值. 
     * 
     * @param tcd 一个TC_ConfigDomain对象，指配置文件的一个域
     */
    TC_ConfigDomain& operator=(const TC_ConfigDomain &tcd);


    struct DomainPath
    {
        vector<string>  _domains;
        string          _param;
    };

    /**
     * @brief 解析一个domain 
     * 对形如"/Main/Domain<Name>"的path进行解析，解析的结果为一个 
     * DomainPath 类型，包括路径_domains和路径中对应的配置项_param.
     *  
     * @param path       一个经过处理的字符串，必须符合一定的要求
     * @param bWithParam "/Main/Domain<Name>"时bWithParam为ture 
     *                   "/Main/Domain"时bWithParam为false
     * @return DomainPath 一个DomainPath对象，解析出域中的域名和参数
     */
    static DomainPath parseDomainName(const string& path, bool bWithParam);

    /**
    * @brief 增加子域名称
    * 增加一个子域，name是子域的名字，比如"/Main/Domain" 
    * 再增加一个name为subDomain的子域,变成"/Main/Domain/subDomain" 
    *  
    * @param name               子域的名称 
    * @return TC_ConfigDomain*  指向子域的指针 
    * @throws TC_Config_Exception 
    */
    TC_ConfigDomain* addSubDomain(const string& name);

    /**
    * @brief 递归搜索子域 Sub Domain. 
    *  
    * @param itBegin 迭代器 ，指向_domains的顶部
    * @param itEnd  迭代器 ，指向_domains的底部
    * @return TC_ConfigDomain* 指向目标子域的指针
    */
    TC_ConfigDomain *getSubTcConfigDomain(vector<string>::const_iterator itBegin, vector<string>::const_iterator itEnd);
    const TC_ConfigDomain *getSubTcConfigDomain(vector<string>::const_iterator itBegin, vector<string>::const_iterator itEnd) const;

    /**
    * @brief Get Param Value 获取参数/值对. 
    *  
    * @param sName 配置项名称
    * @return      配置项对应的值
    */
    string getParamValue(const string &sName) const;

    /**
    * @brief Get Param Map 获取map 
    * map是参数/值对，是配置项和配置项的值对应的键值对, 
    * eg. SyncThreadNum = 2
    * @return 一个域中的参数和其对应的值的map 
    */
    const map<string, string>& getParamMap() const { return _param; }

    /**
    * @brief Get Domain Map 获取map. 
    *  
    * @return map
    */
    const map<string, TC_ConfigDomain*>& getDomainMap() const { return _subdomain; }

    /**
    * @brief Set Param Value 设置参数/值对. 
    * 
    * @param sLine 行
    * @return 
    */
    void setParamValue(const string &sLine);

    /**
     * @brief 插入参数，把key里没有的配置项添加到最后 
     *  
     * @param 存配置项和对应配置项值的map
     */
    void insertParamValue(const map<string, string> &m);

    /**
    * @brief Destroy 释放. 
    *  
    * @return 无
    */
    void destroy();

    /**
    * @brief Get Name 获取域名称
    * @return 域名称
    */
    string getName() const;

    /**
     * @brief 设置域名称 
     * 
     * @param 域名称
     */
    void setName(const string& name);

    /**
     * @brief 按照文件中的顺序获取key
     * 
     * @return vector<string>存放key的vector
     */
    vector<string> getKey() const;

    /**
     * @brief 按照文件中的顺序获取line
     * 
     * @return vector<string>存放key的vector
     */
    vector<string> getLine() const;

    /**
     * @brief 按照文件中的顺序获取子Domain
     * 
     * @return vector<string>存放域名字的vector
     */
    vector<string> getSubDomain() const;

    /**
    * @brief 转换成配置文件的字符串格式. 
    *  
    * @param i  tab的层数
    * @return   string类型配置字符串
    */
    string tostr(int i) const;

    /**
     * @brief 克隆.
     * 
     * @return TC_ConfigDomain*
     */
    TC_ConfigDomain* clone() const 
    { 
        return new TC_ConfigDomain(*this); 
    }

protected:
    /**
     * @brief 转义. 
     *  
     * @param name 
     * @return string
     */
    static string parse(const string& s);

    /**
     * @brief 方向转义. 
     *  
     * @param s 
     * @return string
     */
    static string reverse_parse(const string &s);

    /**
    * @brief 设置参数/值对 
    *  
    * @param sName  配置项名称
    * @param sValue 配置项的值
    * @return 无
    */
    void setParamValue(const string &sName, const string &sValue);

protected:

    /**
    * 域名称
    */
    string                  _name;

    /**
    * name/value对，配置项的名称和配置项的值
    */
    map<string, string>     _param;

    /**
     * key也就是配置项的插入顺序
     */
    vector<string>          _key;

    /**
    * 子域
    */
    map<string, TC_ConfigDomain*>        _subdomain;

    /**
     * 域的插入顺序
     */
    vector<string>          _domain;
 
    /**
     * 整行的配置列表
     */
    vector<string>          _line;
};

/**
 * @brief 定义配置文件的类（兼容wbl模式）.
 
 * 支持从string中解析配置文件；
 
 * 支持生成配置文件；
 
 * 解析出错抛出异常；
 
 * 采用[]获取配置，如果无配置则抛出异常；
 
 * 采用get获取配置，不存在则返回空；
 
 * 读取配置文件是线程安全的，insert域等函数非线程安全；
 
 * 例如：
 
 *  <Main>
 
 *  <Domain>
 
 *      Name = Value
 
 *  </Domain>
 
 *   <Domain1>
 
 *       Name = Value
 
 *   </Domain1>
 
 *  </Main>
 
 *  获取参数:conf["/Main/Domain<Name>"] 获取域Map:
 
 *   getDomainMap("/Main/Domain", m); m得到Name/Value对
 
 * 获取域Vector: getDomainVector("/Main", v); v得到Domain列表
 
   可以增加域或域下面的值对
 
 */
class TC_Config
{
public:

    /**
    * @brief 构造函数
    */
    TC_Config();

    /**
     * @brief 拷贝复制.
     *  
     * @param tc为TC_Config类型
     */
    TC_Config(const TC_Config &tc);

    /**
     * @brief 赋值. 
     *  
     * @param tcd 
     * @return TC_Config&
     */
    TC_Config& operator=(const TC_Config &tc);

    /**
    * @brief 解析文件. 
    *  
    * 把fileName对应的文件转化为输入流后调用parse对其进行解析 
    * @param sFileName : 文件名称 
    * @return 无 
    * @throws TC_Config_Exception 
    */
    void parseFile(const string& sFileName);

    /**
    * @brief 解析字符串. 
    *  
    * 把string 类型先转化为输入流后调用parse对其进行解析 
    * @return void 
    * @throws TC_Config_Exception 
    */
    void parseString(const string& buffer);

    /**
    * @brief 获取值, 如果没有则抛出异常， 
    * 对于型如/Main/Domain<Param>的字符串，获取域/Main/Domain下的配置项名字为Param的值
    * @param sName 参数名称,例如: /Main/Domain<Param>
    * @return      配置项对应的值 
    *  @throws     TC_Config_Exception 
    */
    string operator[](const string &sName);

    /**
     * @brief 获取值, 注意如果没有不抛出异常,返回空字符串. 
     * 对于型如/Main/Domain<Param>的字符串，获取域/Main/Domain下的配置项名字为Param的值
     * @param sName 参数名称,  例如: /Main/Domain<Param>
     * @return     配置项对应的值
     */
    string get(const string &sName, const string &sDefault="") const;

    /**
    * @brief GetDomainParamMap获取域下面的参数值对. 
    *  
    * @param path 域名称, 域标识, 例如: /Main/Domain
    * @param m    map<string, string>类型，域内的map列表
    * @return     bool, 返回域下面的参数值对
    */
    bool getDomainMap(const string &path, map<string, string> &m) const;

    /**
     * @brief 获取域下面的参数值对,不存则返回空map. 
     *  
     * @param path 域名称, 域标识, 例如: /Main/Domain 
     * @return     map<string,string>,对应域下面的所有参数值对
     */
    map<string, string> getDomainMap(const string &path) const;

    /**
     * @brief 获取域下面的所有key, 按照文件的行顺序返回 
     * @param path 域名称, 域标识, 例如: /Main/Domain 
     * @return     vector<string>，某个标识下的所有key
     */
    vector<string> getDomainKey(const string &path) const;

    /**
     * @brief 获取域下面的所有非域行, 按照文件的行顺序返回 
     *        区别于getDomainKey只能获取到key,该接口对带"="的配置也整行返回
     * @param path 域名称, 域标识, 例如: /Main/Domain 
     * @return     vector<string>，某个标识下的所有非域行
     */
    vector<string> getDomainLine(const string &path) const;

    /**
    * @brief getDomainMap 获取域下面的子域. 
    *  
    * @param path  域名称, 域标识, 例如: /Main/Domain
    * @param v     要获取的子域名字
    * @param       vector<string>域下面的域名称
    * @return      成功获取返回true，否则返回false
    */
    bool getDomainVector(const string &path, vector<string> &v) const;

    /**
     * @brief 获取域下面的子域, 
     *        不存在则返回空vector按照在文件中的顺序返回
     * @param path 域名称, 域标识, 例如: /Main/Domain 
     * @return     vector<string>目标域下面的子域
     */
    vector<string> getDomainVector(const string &path) const;

    /**
     * @brief 是否存在域. 
     *  
     * @param path 域名称, 域标识, 例如: /Main/Domain 
     * @return     存在返回true，否则返回false
     */
    bool hasDomainVector(const string &path) const;

    /** 
     * @brief 增加域，在当前域下面增加域, 如果已经存在sAddDomain域, 
     *        则认为成功
     * @param sCurDomain 域标识符, 例如:/Main/Domain
     * @param sAddDomain 需要增加的域名称: 例如: sCurDomain
     * @param bCreate    sCurDomain域不存在的情况下, 是否自动创建
     * @return           0-成功增加, 1-sCurDomain不存在 
     */
    int insertDomain(const string &sCurDomain, const string &sAddDomain, bool bCreate);


    /** 
     * @brief 增加参数，即配置项，当前域下面增加配置项参数, 
     *        如果已经有相关参数, 则忽略(不替换)
     * @param sCurDomain  域标识符, 例如:/Main/Domain
     * @param m           map类型，存参数值对
     * @param bCreate     sCurDomain域不存在的情况下, 是否自动创建
     * @return            0: 成功, 1:sCurDomain不存在 
     */
    int insertDomainParam(const string &sCurDomain, const map<string, string> &m, bool bCreate);

    /**
     * @brief 合并配置文件到当前配置文件. 
     *  
     * @param cf
     * @param bUpdate true-冲突项更新本配置, false-冲突项不更新
     */
    void joinConfig(const TC_Config &cf, bool bUpdate);

    /**
    * @brief 转换成配置文件的字符串格式. 
    *  
    * @return 配置字符串
    */
    string tostr() const;

protected:
    /**
    * @brief Parse输入流， 
    * 最终把输入流解析成为一个TC_ConfigDomain装入stack中 
    * @param  要解析的输入流，按行解析 
    * @throws TC_Config_Exception
    * @return
    */
    void parse(istream &is);

    /**
    * @brief create New Domain 生成子域. 
    *  
    * @param sName 域名称
    * @return      指向新生成的子域的指针 
    * @throws TC_Config_Exception 
    */
    TC_ConfigDomain *newTcConfigDomain(const string& sName);

    /**
    * @brief Search Domain 搜索域. 
    *  
    * @param sDomainName 参数名称,支持子域搜索
    * @return value
    */
    TC_ConfigDomain *searchTcConfigDomain(const vector<string>& domains);
    const TC_ConfigDomain *searchTcConfigDomain(const vector<string>& domains) const;

protected:

    /**
    * 根domain
    */
    TC_ConfigDomain _root;
};

}
#endif //_TC_CONFIG_H_
