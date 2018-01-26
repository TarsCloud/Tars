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

#include "util/tc_cgi.h"
#include "util/tc_common.h"
#include "util/tc_http.h"
#include <string.h>

namespace tars
{

ostream &operator<<(ostream &os, const TC_Cgi_Upload &tcCgiUpload)
{
    os << tcCgiUpload.tostr();
    return os;
}

TC_Cgi_Upload::TC_Cgi_Upload(const TC_Cgi_Upload &tcCgiUpload)
{
    _sFileName      = tcCgiUpload._sFileName;
    _sRealFileName  = tcCgiUpload._sRealFileName;
    _sServerFileName= tcCgiUpload._sServerFileName;
    _iSize          = tcCgiUpload._iSize;
    _bOverSize      = tcCgiUpload._bOverSize;
}

TC_Cgi_Upload & TC_Cgi_Upload::operator=(const TC_Cgi_Upload &tcCgiUpload)
{
    if(this != &tcCgiUpload)
    {
        _sFileName      = tcCgiUpload._sFileName;
        _sRealFileName  = tcCgiUpload._sRealFileName;
        _sServerFileName= tcCgiUpload._sServerFileName;
        _iSize          = tcCgiUpload._iSize;
        _bOverSize      = tcCgiUpload._bOverSize;
    }

    return *this;
}

string TC_Cgi_Upload::tostr() const
{
    string sBuffer;

    sBuffer = "[file控件名称:" + _sFileName + "] ";
    sBuffer += " [真实文件名称:" + _sRealFileName + "] ";
    sBuffer += " [服务器端文件名称:" + _sServerFileName + "] ";
    sBuffer += " [大小(字节):" + TC_Common::tostr(_iSize) + "] ";
    sBuffer += " [超过大小限制:" + TC_Common::tostr(_bOverSize) + "] ";

    return sBuffer;
}

TC_Cgi::TC_Cgi()
: _is(NULL)
, _iMaxUploadFiles(1)
, _iUploadMaxSize(20971520)
, _bOverUploadFiles(false)
, _iMaxContentLength(20971520)
, _bUploadFileOverSize(false)
{
}

TC_Cgi::~TC_Cgi()
{
}

void TC_Cgi::setUpload(const string &sUploadFilePrefix, int iMaxUploadFiles, size_t iUploadMaxSize, size_t iMaxContentLength)
{
    _sUploadFilePrefix  = sUploadFilePrefix;
    _iMaxUploadFiles    = iMaxUploadFiles;
    _iUploadMaxSize     = iUploadMaxSize;
    _iMaxContentLength  = iMaxContentLength;
}

void TC_Cgi::parseCgi()
{
    char **env = environ;
    while(*env != NULL)
    {
        string s(*env);
        string::size_type pos = s.find('=');
        if(pos != string::npos)
        {
            _env[s.substr(0, pos)] = s.substr(pos + 1);
        }
        ++env;
    }

    _is = &cin;

    readCgiInput(_mmpParams, _mpCookies);
}

void TC_Cgi::parseCgi(const TC_HttpRequest &request)
{
    setCgiEnv("QUERY_STRING", request.getRequestParam());
    setCgiEnv("CONTENT_LENGTH", TC_Common::tostr(request.getContentLength()));
    setCgiEnv("HTTP_COOKIE", request.getHeader("Cookie"));
    setCgiEnv("CONTENT_TYPE", request.getHeader("Content-Type"));
    setCgiEnv("REQUEST_METHOD", request.isGET() ? "GET" : "POST");

    _buffer = request.getContent();
    _iss.str(_buffer);
    _is = &_iss;

    readCgiInput(_mmpParams, _mpCookies);
}

void TC_Cgi::getPOST(string &sBuffer)
{
    string::size_type iSize = atoi(getCgiEnv(TC_Cgi::ENM_CONTENT_LENGTH).c_str());

    if( iSize > _iMaxContentLength)
    {
        throw TC_Cgi_Exception("[TC_Cgi::getPOST] : CONTENT_LENGTH is too large!");
    }

    if(iSize <= 0)
    {
        return;
    }

    sBuffer = _buffer;
    if(sBuffer.length() < iSize)
    {
        throw TC_Cgi_Exception("[TC_Cgi::getPOST] : Read CONTENT error: content size " + TC_Common::tostr(sBuffer.length()) + "<" + TC_Common::tostr(iSize));
    }
}

void TC_Cgi::getGET(string &sBuffer)
{
    sBuffer = getCgiEnv(TC_Cgi::ENM_QUERY_STRING);
}

void TC_Cgi::readCgiInput(multimap<string, string> &mmpParams, map<string, string> &mpCooies)
{
    string sBuffer = TC_Cgi::getCgiEnv(TC_Cgi::ENM_HTTP_COOKIE);

    parseCookies(mpCooies, sBuffer);

    if (getCgiEnv(ENM_REQUEST_METHOD).find("POST") != string::npos)
    {
        //文件上传模式
        if (getCgiEnv(ENM_CONTENT_TYPE).find("multipart/form-data") != string::npos)
        {
            parseUpload(mmpParams);
        }
        else if (getCgiEnv(ENM_CONTENT_TYPE).find("application/x-www-form-urlencoded") != string::npos)
        {
            getPOST(sBuffer);
            parseNormal(mmpParams, sBuffer);
        }
        getGET(sBuffer);
    }
    else if (getCgiEnv(ENM_REQUEST_METHOD).find("GET") != string::npos)
    {
        getGET(sBuffer);
    }
    else
    {
        //default is GET
        getGET(sBuffer);
    }

    if (sBuffer.length() == 0)
    {
        return ;
    }

    parseNormal(mmpParams, sBuffer);
}

void TC_Cgi::parseNormal(multimap<string, string> &mmpParams, const string& sBuffer)
{
    int iFlag = 0;
    string sName;
    string sValue;
    string sTmp;
    string::size_type len = sBuffer.length();
    string::size_type pos = 0;

    while (pos < len)
    {
        sTmp = "";

        if(iFlag == 0)
        {
            while ( (sBuffer[pos] != '=') && (pos < len) )
            {
                sTmp += (sBuffer[pos] == '+') ? ' ' : sBuffer[pos];

                ++pos;
            }
        }
        else
        {
            while ( (sBuffer[pos] != '&') && (pos < len) )
            {
                sTmp += (sBuffer[pos] == '+') ? ' ' : sBuffer[pos];

                ++pos;
            }
        }

        if (iFlag == 0)                         //param name
        {
            sName = TC_Cgi::decodeURL(sTmp);

            if ( (sBuffer[pos] != '=') || (pos == len - 1) )
            {
                sValue = "";

                mmpParams.insert(multimap<string, string>::value_type(sName, sValue));
            }
            else
            {
                iFlag = 1;
            }
        }
        else
        {
            sValue = TC_Cgi::decodeURL(sTmp);

            mmpParams.insert(multimap<string, string>::value_type(sName, sValue));

            iFlag = 0;
        }

        ++pos;
    }
}

void TC_Cgi::parseCookies(map<string, string> &mpCookies, const string& sBuffer)
{
    string::size_type len = sBuffer.length();
    string::size_type pos = 0;

    if(len == 0)
    {
        return;
    }

    string sName;
    string sValue;
    int iFlag = 1;

    while(pos < len)
    {
        if(sBuffer[pos] == '=')
        {
            if( pos == len - 1)                     //最后一个参数
            {
                sName  = decodeURL(TC_Common::trimleft(sName, " "));
                sValue = decodeURL(sValue);

                mpCookies[sName] = sValue;
                sName = "";
                sValue= "";
            }

            iFlag = 0;                              //取值
        }
        else if ( (sBuffer[pos] == ';') || (pos == len - 1) )
        {
            if(iFlag == 0)
            {
                if(pos == len - 1)
                {
                    sValue += sBuffer[pos];
                }

                sName  = decodeURL(TC_Common::trimleft(sName, " "));
                sValue = decodeURL(sValue);

                mpCookies[sName] = sValue;
                sName = "";
                sValue= "";
                iFlag = 1;                          //取名称
            }
        }
        else if (iFlag)
        {
            sName += sBuffer[pos];
        }
        else
        {
            sValue += sBuffer[pos];
        }

        ++pos;
    }
}

void TC_Cgi::ignoreLine()
{
    string sBuffer;

    //读取直到有一个空行
    while(true)
    {
        if(!getline(*_is, sBuffer))
        {
            throw TC_Cgi_Exception("[TC_Cgi::parseFormData] 'multipart/form-data' Format is error");
        }
        sBuffer = TC_Common::trim(sBuffer, "\r\n");

        if(sBuffer.empty())
        {
            break;
        }
    }
}

bool TC_Cgi::writeFile(FILE*fp, const string &sFileName, const string &sBuffer, size_t &iTotalWrite)
{
    if(!fp)
    {
        return false;
    }

    if(_mpUpload[sFileName]._bOverSize
       || iTotalWrite > _iUploadMaxSize
       || (iTotalWrite + sBuffer.length()) > _iUploadMaxSize)
    {
        _mpUpload[sFileName]._bOverSize  = true;
        _bUploadFileOverSize             = true;
        return false;
    }

    //内容写入文件
    int ret = fwrite(sBuffer.c_str(), 1, sBuffer.length(), fp);
    if(ret != (int)sBuffer.length())
    {
        fclose(fp);
        throw TC_Cgi_Exception("[TC_Cgi::parseFormData] upload file '" + _mpUpload[sFileName]._sServerFileName + "' error:" + string(strerror(errno)));
    }
    iTotalWrite += sBuffer.length();
    _mpUpload[sFileName]._iSize = iTotalWrite;

    return true;
}

void TC_Cgi::parseFormData(multimap<string, string> &mmpParams, const string &sBoundary)
{
    string sBuffer;
    if(!getline(*_is, sBuffer))
    {
        throw TC_Cgi_Exception("[TC_Cgi::parseFormData] 'multipart/form-data' Format is error");
    }

    string sName;
    string sValue;

    string::size_type pos1 = string("Content-Disposition: form-data; name=\"").length();
    string::size_type pos2 = sBuffer.find('"', pos1);
    if(pos2 == string::npos)
    {
        throw TC_Cgi_Exception("[TC_Cgi::parseFormData] 'multipart/form-data' Format is error");
    }

    //控件名称
    sName = sBuffer.substr(pos1, pos2 - pos1);

    pos1 = sBuffer.find("filename=\"");
    if(pos1 != string::npos)
    {
        FILE *fp = NULL;
        string sTheFile;

        //上传文件的form data
        pos1 += 10;     //filename=

        pos2 = sBuffer.find('"', pos1);
        if(pos2 == string::npos)
        {
            throw TC_Cgi_Exception("[TC_Cgi::parseFormData] 'multipart/form-data' Format is error");
        }

        //获取文件名
        sValue = sBuffer.substr(pos1, pos2 - pos1);

        if( sValue.length() > 0)
        {
            if(_iMaxUploadFiles < 0 || _mpUpload.size() < (size_t)_iMaxUploadFiles)
            {
                mmpParams.insert(multimap<string, string>::value_type(sName, sValue));

                //组成文件名称
                string sUploadFileName = _sUploadFilePrefix + "_" + TC_Common::tostr(_mpUpload.size());

                //记录上传文件路径
                _mpUpload[sName]._sFileName         = sName;
                _mpUpload[sName]._sRealFileName     = sValue;
                _mpUpload[sName]._sServerFileName   = sUploadFileName;

                sTheFile = sName;

                //打开文件
                if ( (fp = fopen(sUploadFileName.c_str(),"w")) == NULL)
                {
                    mmpParams.clear();          //clear , exception safe
                    throw TC_Cgi_Exception("[TC_Cgi::parseFormData] Upload File '" + sValue + "' to '" + sUploadFileName +"' error! " + string(strerror(errno)));
                }
            }
            else
            {
                 _bOverUploadFiles = true;
            }
        }

        ignoreLine();

        string sLastBuffer;
        size_t iTotalWrite = 0;

        //读取内容到文件
        while(getline(*_is, sBuffer))
        {
            string sTmp = TC_Common::trimright(sBuffer);

            if(sTmp.find(sBoundary) != string::npos)
            {
                if(sLastBuffer.length() < 2)
                {
                    if(fp)
                    {
                        fclose(fp);
                    }
                    throw TC_Cgi_Exception("[TC_Cgi::parseFormData] 'multipart/form-data' Format is error");
                }

                sLastBuffer = sLastBuffer.substr(0, sLastBuffer.length() - 2);

                writeFile(fp, sTheFile, sLastBuffer, iTotalWrite);
                if(fp)
                {
                    fclose(fp);
                    fp = NULL;
                }

                //新的一个form data
                if(sTmp == sBoundary)
                {
                    parseFormData(mmpParams, sBoundary);
                    return;
                }
                else if(sTmp == sBoundary + "--")
                {
                    return;
                }
                {
                    throw TC_Cgi_Exception("[TC_Cgi::parseFormData] 'multipart/form-data' Format is error");
                }
            }

            if(!sLastBuffer.empty())
            {
                writeFile(fp, sTheFile, sLastBuffer, iTotalWrite);
            }

            sLastBuffer = sBuffer + "\n";
        }
        if(fp)
        {
            fclose(fp);
            fp = NULL;
        }
    }
    else
    {
        ignoreLine();

        string sLastBuffer;

        while(getline(*_is, sBuffer))
        {
            string sTmp = TC_Common::trimright(sBuffer);
            if(sTmp.find(sBoundary) != string::npos)
            {
                if(sLastBuffer.length() < 2)
                {
                    throw TC_Cgi_Exception("[TC_Cgi::parseFormData] 'multipart/form-data' Format is error");
                }
                sLastBuffer = sLastBuffer.substr(0, sLastBuffer.length() - 2);
                mmpParams.insert(multimap<string, string>::value_type(sName, sLastBuffer));

                //新的一个form data
                if(sTmp == sBoundary)
                {
                    parseFormData(mmpParams, sBoundary);
                    return;
                }
                else if(sTmp == sBoundary + "--")
                {
                    return;
                }
                throw TC_Cgi_Exception("[TC_Cgi::parseFormData] 'multipart/form-data' Format is error");
            }

            sLastBuffer += sBuffer + "\n";
        }
    }
}

void TC_Cgi::parseUpload(multimap<string, string> &mmpParams)
{
    string::size_type iSize = atoi(getCgiEnv(TC_Cgi::ENM_CONTENT_LENGTH).c_str());

    if( iSize > _iMaxContentLength)
    {
        throw TC_Cgi_Exception("[TC_Cgi::parseUpload] : CONTENT_LENGTH is too large!");
    }

    //get content type
    string sContentType = getCgiEnv(TC_Cgi::ENM_CONTENT_TYPE);

    string::size_type bPos = sContentType.find("boundary=");
    if(bPos == string::npos)
    {
        return;
    }

    //get boundary, boundary separete params
    string sBoundary(sContentType.substr(bPos + 9));
    sBoundary = "--" + sBoundary;

    string sBuffer;

    if(!getline(*_is, sBuffer))
    {
        throw TC_Cgi_Exception("[TC_Cgi::parseFormData] 'multipart/form-data' Format is error");
    }
    sBuffer = TC_Common::trim(sBuffer);
    if(sBuffer == sBoundary)
    {
        parseFormData(mmpParams, sBoundary);

    }
    else
    {
        throw TC_Cgi_Exception("[TC_Cgi::parseUpload] 'multipart/form-data' Format is error");
    }
}

string &TC_Cgi::operator[](const string &sName)
{
    multimap<string, string>::iterator it;

    if((it = _mmpParams.find(sName)) != _mmpParams.end())
    {
        return it->second;
    }

    return _mmpParams.insert(multimap<string, string>::value_type(sName, ""))->second;
}

string TC_Cgi::getValue(const string& sName) const
{
    multimap<string, string>::const_iterator it;

    if((it = _mmpParams.find(sName)) != _mmpParams.end())
    {
        return it->second;
    }

    return "";
}

const vector<string>& TC_Cgi::getMultiValue(const string& sName, vector<string> &vtValue) const
{
    vtValue.clear();

    multimap<string, string>::const_iterator itEnd = _mmpParams.end();

    for( multimap<string, string>::const_iterator it = _mmpParams.begin(); it != itEnd; ++it)
    {
        if(it->first == sName)
        {
            vtValue.push_back(it->second);
        }
    }

    return vtValue;
}

const multimap<string, string> &TC_Cgi::getParamMap() const
{
    return _mmpParams;
}

map<string, string> TC_Cgi::getParamMapEx() const
{
    map<string, string> mpCgiParam;
    multimap<string, string>::const_iterator it     = _mmpParams.begin();
    multimap<string, string>::const_iterator itEnd  = _mmpParams.end();

    while(it != itEnd)
    {
        mpCgiParam[it->first] = it->second;
        ++it;
    }

    return mpCgiParam;
}

const map<string, string> &TC_Cgi::getCookiesMap() const
{
    return _mpCookies;
}

bool  TC_Cgi::isUploadOverSize() const
{
    return _bUploadFileOverSize;
}

bool  TC_Cgi::isUploadOverSize(vector<TC_Cgi_Upload> &vtUploads) const
{
    vtUploads.clear();

    map<string, TC_Cgi_Upload>::const_iterator itEnd = _mpUpload.end();

    for( map<string, TC_Cgi_Upload>::const_iterator it = _mpUpload.begin(); it != itEnd; ++it)
    {
        if(it->second._bOverSize)
        {
            vtUploads.push_back(it->second);
        }
    }

    return vtUploads.size() > 0;
}

const map<string, TC_Cgi_Upload> &TC_Cgi::getUploadFilesMap() const
{
    return _mpUpload;
}

size_t TC_Cgi::getUploadFilesCount() const
{
    return _mpUpload.size();
}

string TC_Cgi::getCookie(const string &sName) const
{
    map<string, string>::const_iterator it = _mpCookies.find(sName);

    if(it != _mpCookies.end())
    {
        return it->second;
    }

    return "";
}

string TC_Cgi::setCookie(const string &sName, const string &sValue, const string &sExpires, const string &sPath, const string &sDomain, bool bSecure)
{
    if(sName.length() == 0)
    {
        return "";
    }

    ostringstream os;

    os << "Set-Cookie: %s=%s;",sName.c_str(),sValue.c_str();
    if (sExpires.length() > 0)
    {
        os << "EXPIRES=%s;",sExpires.c_str();
    }

    if (sPath.length() > 0)
    {
        os << "PATH=%s;", sPath.c_str();
    }

    if (sDomain.length() > 0)
    {
        os << "DOMAIN=%s;", sDomain.c_str();
    }

    if (bSecure)
    {
        os << "SECURE";
    }
    os << endl;

    _mpCookies[sName] = sValue;

    return os.str();
}

bool TC_Cgi::isParamEmpty() const
{
    return _mmpParams.empty();
}

bool TC_Cgi::isParamExist(const string& sName) const
{
    return _mmpParams.find(sName) != _mmpParams.end();
}

string TC_Cgi::getCgiEnv(int iEnv)
{
    switch(iEnv)
    {
    case ENM_SERVER_SOFTWARE:
        return getCgiEnv("SERVER_SOFTWARE");
    case ENM_SERVER_NAME:
        return getCgiEnv("SERVER_NAME");
    case ENM_GATEWAY_INTERFACE:
        return getCgiEnv("GATEWAY_INTERFACE");
    case ENM_SERVER_PROTOCOL:
        return getCgiEnv("SERVER_PROTOCOL");
    case ENM_SERVER_PORT:
        return getCgiEnv("SERVER_PORT");
    case ENM_REQUEST_METHOD:
        return getCgiEnv("REQUEST_METHOD");
    case ENM_PATH_INFO:
        return getCgiEnv("PATH_INFO");
    case ENM_PATH_TRANSLATED:
        return getCgiEnv("PATH_TRANSLATED");
    case ENM_SCRIPT_NAME:
        return getCgiEnv("SCRIPT_NAME");
    case ENM_HTTP_COOKIE:
        return getCgiEnv("HTTP_COOKIE");;
    case ENM_QUERY_STRING:
        return getCgiEnv("QUERY_STRING");
    case ENM_REMOTE_HOST:
        return getCgiEnv("REMOTE_HOST");
    case ENM_REMOTE_ADDR:
        return getCgiEnv("REMOTE_ADDR");
    case ENM_AUTH_TYPE:
        return getCgiEnv("AUTH_TYPE");
    case ENM_REMOTE_USER:
        return getCgiEnv("REMOTE_USER");
    case ENM_REMOTE_IDENT:
        return getCgiEnv("REMOTE_IDENT");
    case ENM_CONTENT_TYPE:
        return getCgiEnv("CONTENT_TYPE");
    case ENM_CONTENT_LENGTH:
        return getCgiEnv("CONTENT_LENGTH");
    case ENM_HTTP_USER_AGENT:
        return getCgiEnv("HTTP_USER_AGENT");
    default:
        return "";
    }
}

string TC_Cgi::getCgiEnv(const string &sEnv)
{
    if(_env.find(sEnv) != _env.end())
    {
        return _env[sEnv];
    }
    return "";
}

void TC_Cgi::setCgiEnv(const string &sName, const string &sValue)
{
    _env[sName] = sValue;
}

string TC_Cgi::htmlHeader(const string &sHeader)
{
    return "Content-type: " + sHeader + "\n\n";
}

string TC_Cgi::decodeURL(const string &sUrl)
{
    string sDecodeUrl;
    register string::size_type pos = 0;
    string::size_type len = sUrl.length();

    sDecodeUrl = "";

    while (pos < len)
    {
        if(sUrl[pos] == '+')
        {
            sDecodeUrl += ' ';
            ++pos;
        }
        else if(sUrl[pos] == '%')
        {
            sDecodeUrl += TC_Common::x2c(sUrl.substr(pos + 1));
            pos += 3;
        }
        else
        {
            sDecodeUrl += sUrl[pos];

            ++pos;
        }
    }

    return sDecodeUrl;
}

string TC_Cgi::encodeURL(const string &sUrl)
{
    static char HEX_TABLE[] = { '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F' };

    string result;
    for (size_t i = 0; i < sUrl.length(); i++)
    {
        char c = sUrl[i];
        if ((c >= '0' && c <= '9') || (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z'))
            result.append(1, c);
        else if(c == ' ')
        {
            result.append(1, '+');
        }
        else
        {
            result.append(1, '%');
            result.append(1, HEX_TABLE[(c >> 4) & 0x0f]);
            result.append(1, HEX_TABLE[c & 0x0f]);
        }
    }

    return result;
}

string TC_Cgi::encodeHTML(const string &src, bool blankEncode)
{
    if (src == "")
        return "";

    string result = src;
    result = TC_Common::replace(result, "&", "&amp;");
    result = TC_Common::replace(result, "<", "&lt;");
    result = TC_Common::replace(result, ">", "&gt;");
    result = TC_Common::replace(result, "\"", "&quot;");

    if (blankEncode)
    {
        result = TC_Common::replace(result, "\t", "    ");
        result = TC_Common::replace(result, "  ", "&nbsp; ");
        result = TC_Common::replace(result, "\r\n", "<br>");
        result = TC_Common::replace(result, "\n", "<br>");
    }
    return result;
}

string TC_Cgi::encodeXML(const string &src)
{
    if (src == "")
        return "";

    string result = src;
    result = TC_Common::replace(result, "&", "&amp;");
    result = TC_Common::replace(result, "<", "&lt;");
    result = TC_Common::replace(result, ">", "&gt;");
    result = TC_Common::replace(result, "\"", "&quot;");
    result = TC_Common::replace(result, "'", "&apos;");
    return result;
}

}

