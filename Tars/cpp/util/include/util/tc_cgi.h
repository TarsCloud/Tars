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

#ifndef __TC_CGI_H
#define __TC_CGI_H

#include <sstream>
#include <istream>
#include <map>
#include <vector>
#include "util/tc_ex.h"

namespace tars
{
/////////////////////////////////////////////////
/** 
* @file tc_cgi.h
* @brief CGI处理类
*/            
/////////////////////////////////////////////////
class TC_Cgi;
class TC_Cgi_Upload;
class TC_HttpRequest;

/**
* @brief 配置文件异常类
*/
struct TC_Cgi_Exception : public TC_Exception
{
    TC_Cgi_Exception(const string &buffer) : TC_Exception(buffer){};
    TC_Cgi_Exception(const string &buffer, int err) : TC_Exception(buffer, err){};
    ~TC_Cgi_Exception() throw(){};
};

/**
* @brief 全局的友元函数，定义该函数, 
*        则可以用TC_Common::tostr对 vector<TC_Cgi_Upload>进行输出操作
*/
ostream &operator<<(ostream &os, const TC_Cgi_Upload &tcCgiUpload);

/**
* @brief cgi上传文件操作，通过该类获取cgi上传的文件信息 
*/
class TC_Cgi_Upload
{
public:
    friend ostream &operator<<(ostream &os, const TC_Cgi_Upload &tcCgiUpload);

    /**
    * @brief 构造函数
    */
    TC_Cgi_Upload()
    :_sFileName("")
    , _sRealFileName("")
    , _sServerFileName("")
    , _iSize(0)
    , _bOverSize(false)
    {
    }

    /**
    * @brief 拷贝构造函数. 
    */

    TC_Cgi_Upload(const TC_Cgi_Upload &tcCgiUpload);

    /**
    * @brief 赋值构造函数
    */
    TC_Cgi_Upload & operator=(const TC_Cgi_Upload &tcCgiUpload);

    /**
    * @brief 获取上传的信息. 
    *  
    * return 上传文件的信息
    */
    string tostr() const;

    /**
    * @brief 获取客户端IE INPUT上传控件的名称. 
    *  
    * return INPUT上传控件名称
    */
    string getFormFileName() const
    {
        return _sFileName;
    }

    /**
    * @brief 返回INPUT控件用户输入的名称,及客户端真实的文件名称. 
    *  
    * return  客户端真实的文件名称
    */
    string retRealFileName() const
    {
        return _sRealFileName;
    }

    /**
    * @brief 上传到服务器后,服务器端文件名称. 
    *  
    * return 服务器端文件名称
    */
    string getSeverFileName() const
    {
        return _sServerFileName;
    }

    /**
    * @brief 获取上传的文件大小. 
    *  
    * return size_t类型，上传的文件大小
    */
    size_t getFileSize() const
    {
        return _iSize;
    }

    /**
    * @brief 上传的文件是否超过大小. 
    *  
    * return 超过大小返回true，否则返回false
    */
    bool isOverSize() const
    {
        return _bOverSize;
    }

protected:

    /**
    * 上传文件,浏览器file控件名称
    */
    string  _sFileName;

    /**
    * 上传文件真实名称,客户端的文件名称
    */
    string  _sRealFileName;

    /**
    * 上传文件服务器端名称
    */
    string  _sServerFileName;

    /**
    * 上传文件大小,字节数
     */
    size_t  _iSize;

    /**
    * 上传文件是否超过大小
    */
    bool    _bOverSize;

    friend class TC_Cgi;
};

/**
* @brief cgi操作相关类. 
*  
* 主要操作包括： 
*  
* 1 支持参数解析 
*  
* 2 支持cookies解析 
*  
* 3 支持文件上传,设置上传文件的最大个数,文件的最大大小 
*  
* 4 上传文件时, 需要检查文件是否超过最大大小 
*  
* 5 上传文件时, 需要检查上传文件个数是否限制
*  
* 说明:多个文件同时上传时,浏览器的file控件必须取不同name,否则将无法正确的上传文件 
*  
* 注意:调用parseCgi解析标准输入,
*  
* 如果有文件上传需要调用setUpload, 并且需要在parseCgi之前调用
*
*/
class TC_Cgi
{
public:

    /**
    * @brief TC_Cgi构造函数
    */
    TC_Cgi();

    /**
    * @brief 析构函数
    */
    virtual ~TC_Cgi();

    /**
    * @brief 定义环境变量
    */
    enum
    {
        ENM_SERVER_SOFTWARE,
        ENM_SERVER_NAME,
        ENM_GATEWAY_INTERFACE,
        ENM_SERVER_PROTOCOL,
        ENM_SERVER_PORT,
        ENM_REQUEST_METHOD,
        ENM_PATH_INFO,
        ENM_PATH_TRANSLATED,
        ENM_SCRIPT_NAME,
        ENM_HTTP_COOKIE,
        ENM_QUERY_STRING,
        ENM_REMOTE_HOST,
        ENM_REMOTE_ADDR,
        ENM_AUTH_TYPE,
        ENM_REMOTE_USER,
        ENM_REMOTE_IDENT,
        ENM_CONTENT_TYPE,
        ENM_CONTENT_LENGTH,
        ENM_HTTP_USER_AGENT
    };

    /**
     * @brief 设置上传文件. 
     *  
     * @param sUploadFilePrefix, 文件前缀(包含路径), 如果有文件上传,则文件保存在以该前缀为名称的路径下
     *                           如果有多个文件上传,则文件名称以次在后面加"_序号"
     * @param iMaxUploadFiles    最多上传文件个数,<0:没有限制
     * @param iUploadMaxSize     每个文件上传的最大大小(字节)
     */
    void setUpload(const string &sUploadFilePrefix, int iMaxUploadFiles = 5, size_t iUploadMaxSize = 1024*1024*10, size_t iMaxContentLength = 1024*1024*10);

    /**
     * @brief 从标准输入解析cgi. 
     */
    void parseCgi();

    /**
     * @brief 直接从http请求解析. 
     *  
     * @param request http请求
     */
    void parseCgi(const TC_HttpRequest &request);

    /**
    * @brief 获取cgi的url参数multimap. 
    *  
    * @return multimap<string, string>cgi的url参数
    */
    const multimap<string, string> &getParamMap() const;

    /**
     * @brief 获取cgi环境变量map.
     *
     * @return map<string,string>cgi的环境变量
     */
    map<string, string> getEnvMap() const { return _env; }

    /**
    * @brief 获取cgi的参数map, 将multimap转换成map返回 
    *        , 对于一个参数名称对应多个参数值的情况, 只取其中一个值.
    *  
    * @return map<string, string>
    */
    map<string, string> getParamMapEx() const;

    /**
    * @brief 获取cookies的参数map. 
    *  
    * @return map<string, string>
    */
    const map<string, string> &getCookiesMap() const;

    /**
    * @brief 获取cgi的某个参数. 
    *  
    * @param sName  参数名称
    * @return       
    */
    string &operator[](const string &sName);

    /**
    * @brief 获取cgi的某个参数. 
    *  
    * @param sName 参数名称
    * @return      参数的值
    */
    string getValue(const string& sName) const;

    /**
    * @brief 获取某一名称的参数的多个值. 
    *  
    * 用于解析checkbox这类控件的值( 一个参数名,多个参数值)
    * @param sName   参数名称
    * @param vtValue 该名称的参数值组成的vector
    * @return        vector<string>, 该名称的参数值组成的vector
    */
    const vector<string> &getMultiValue(const string& sName, vector<string> &vtValue) const;

    /**
    * @brief 获取cookie值. 
    *  
    * @param sName cookie名称
    * @return      string类型的cookie值
    */
    string getCookie(const string& sName) const;

    /**
    * @brief 设置cookie值. 
    *  
    * @param sName    cookie名称
    * @param sValue   cookie值
    * @param sExpires 过期日期
    * @param sPath    cookie有效路径
    * @param sDomain  cookie有效域
    * @param bSecure  是否安全(ssl时有效)
    * @return         返回字符串，代表cookie值
    */
    string setCookie(const string &sName, const string &sValue, const string &sExpires="", const string &sPath="/", const string &sDomain = "", bool bSecure = false);

    /**
    * @brief 参数链表是否为空. 
    *  
    * @return 参数链表为空返回true，否则返回false
    */
    bool isParamEmpty() const;

    /**
    * @brief 参数是否存在. 
    *  
    * @param sName 参数名称
    * @return      存在返回true，否则返回false
    */
    bool isParamExist(const string& sName) const;

    /**
    * @brief 上传文件是否超过大小，多个文件上传时, 
    *        只要有一个文件超过大小,则超过
    * @return 按照以上标准超过大小的返回true，否则返回false
    */
    bool  isUploadOverSize() const;

    /**
    * @brief 上传文件是否超过大小,多个文件上传时, 
    *        只要有一个文件超过大小,则超过
    * @param vtUploads 返回超过大小的文件名称(浏览器file控件的名称)
    * @return          按照以上标准超过大小的返回true，否则返回false
    */
    bool  isUploadOverSize(vector<TC_Cgi_Upload> &vtUploads) const;

    /**
     * @brief 是否超过上传文件个数.
     *
     * @return 超过上传个数返回true，否则返回false
     */
    bool isOverUploadFiles() const { return _bOverUploadFiles; }

    /**
    * @brief 获取上传文件个数. 
    *  
    * @return size_t上传文件的个数
    */
    size_t getUploadFilesCount() const;

    /**
    * @brief 获取上传文件的相关信息
    *  
    * @return map<string,TC_Cgi_Upload>结构中， 
    *         保存文件名和文件相关信息的map
    */
    const map<string, TC_Cgi_Upload> &getUploadFilesMap() const;

    /**
    * @brief 获取环境变量. 
    *  
    * @param iEnv  枚举变量
    * @return      环境变量
    */
    string getCgiEnv(int iEnv);

    /**
    * @brief 获取环境变量. 
    *  
    * @param sEnv 环境变量名称
    * @return     环境变量的值
    */
    string getCgiEnv(const string& sEnv);

    /**
     * @brief 设置环境变量. 
     *  
     * @param sName 环境变量名称
     * @param sValue 环境变量的值
     */
    void setCgiEnv(const string &sName, const string &sValue);

    /**
    * @brief 返回html头，content-type .
    *  
    * @param sHeader 缺省值为"text/html"
    * @return 
    */
    static string htmlHeader(const string &sHeader = "text/html");

    /**
    * @brief http请求的url解码, %后面的换成字符. 
    *  
    * @param sUrl http请求url
    * @return    解码后的字符串
    */
    static string decodeURL(const string &sUrl);

    /**
     * @brief 对url进行编码, 非数字和字母用%XX代替. 
     *  
     * @param sUrl http请求url
     * @return     编码后的url
     */
    static string encodeURL(const string &sUrl);

    /**
     * @brief 对源字符串进行HTML编码(<>"&) 
     *  
     * @param src         源字符串
     * @param blankEncode 是否对空格也编码(空格, \t, \r\n, \n) 
     * @return            HTML编码后的字符串
     */
    static string encodeHTML(const string &src, bool blankEncode = false);

    /**
     * @brief 对源字符串进行XML编码(<>"&'). 
     *  
     * @param src  源字符串
     * @return     XML编码后的字符串
     */
    static string encodeXML(const string &src);

protected:

    /**
    * @brief 申明,但是不定义,保证这个函数不会被使用
    */
    TC_Cgi &operator=(const TC_Cgi &tcCgi);

    /**
    * @brief GET method. 
    *  
    * @param sBuffer GET的QueryString
    * return 
    */
    void getGET(string &sBuffer);

    /**
    * @brief POST method. 
    *  
    * @param sBuffer   POST的QueryString
    * return 
    */
    void getPOST(string &sBuffer);

    /**
    * @brief 解析文件上传. 
    *  
    * @param mmpParams  [out]输出参数multimap
    * return 
    */
    void parseUpload(multimap<string, string> &mmpParams);

    /**
     * @brief 解析form数据
     */
    void parseFormData(multimap<string, string> &mmpParams, const string &sBoundary);

    /**
     * @brief  忽略空行
     */
    void ignoreLine();

    /**
     * @brief 写文件. 
     *  
     * @param sFileName 文件名称
     * @param sBuffer   要写入的内容
     */
    bool writeFile(FILE*fp, const string &sFileName, const string &sBuffer, size_t &iTotalWrite);

    /**
    * @brief 非上传模式下解析. 
    *  
    * @param mmpParams  [out]输出参数multimap
    * @param sBuffer    [in]输入QueryString
    * return 
    */
    void parseNormal(multimap<string, string> &mmpParams, const string& sBuffer);

    /**
    * @brief 解析cookies. 
    *  
    * @param mpCooies [out]输出cookiesmap
    * @param sBuffer  [in]输入Cookies字符串
    * return
    */
    void parseCookies(map<string, string> &mpCooies, const string& sBuffer);

    /**
    * @brief 控制解析cgi input的基本流程. 
    *  
    * @param mmpParams [out]输出参数 multimap
    * @param mpCooies [out]输出cookies
    * return 
    */
    void readCgiInput(multimap<string, string> &mmpParams, map<string, string> &mpCooies);

protected:

    /**
     * buffer
     */
    string                      _buffer;

    /**
     * 流
     */
    istringstream               _iss;

    /**
     * 读入
     */
    istream                     *_is;

    /**
     * 环境变量
     */
    map<string, string>         _env;

    /**
    * cgi参数
    */
    multimap<string, string>    _mmpParams;

    /**
    * cookies
    */
    map<string, string>         _mpCookies;

    /**
    * 上传文件名称前缀
    */
    string                      _sUploadFilePrefix;

    /**
    * 上传文件的最大个数,<0:无限制
    */
    int                         _iMaxUploadFiles;

    /**
    * 上传文件的最大大小
    */
    size_t                      _iUploadMaxSize;

    /**
     * 是否超过上传文件个数
     */
    bool                        _bOverUploadFiles;

    /**
     * 最大的content-length
     */
    size_t                      _iMaxContentLength;

    /**
    * 是否超过大小,有一个文件超过则超过
    */
    bool                        _bUploadFileOverSize;

    /**
    * 上传文件相关信息保存在该map中
    */
    map<string, TC_Cgi_Upload>  _mpUpload;
};

}

#endif
