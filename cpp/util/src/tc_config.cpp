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

#include <errno.h>
#include <fstream>
#include "util/tc_config.h"
#include "util/tc_common.h"

namespace tars
{

TC_ConfigDomain::TC_ConfigDomain(const string &sLine)
{
    _name = TC_Common::trim(sLine);
}

TC_ConfigDomain::~TC_ConfigDomain()
{
    destroy();
}

TC_ConfigDomain::TC_ConfigDomain(const TC_ConfigDomain &tcd)
{
    (*this) = tcd;
}

TC_ConfigDomain& TC_ConfigDomain::operator=(const TC_ConfigDomain &tcd)
{
    if(this != &tcd)
    {
        destroy();

        _name  = tcd._name;
        _param = tcd._param;
        _key   = tcd._key;
        _domain= tcd._domain;
        _line  = tcd._line;

        const map<string, TC_ConfigDomain*> & m = tcd.getDomainMap();
        map<string, TC_ConfigDomain*>::const_iterator it = m.begin();
        while(it != m.end())
        {
            _subdomain[it->first] = it->second->clone();
            ++it;
        }
    }
    return *this;
}

TC_ConfigDomain::DomainPath TC_ConfigDomain::parseDomainName(const string& path, bool bWithParam)
{
    TC_ConfigDomain::DomainPath dp;

    if(bWithParam)
    {
        string::size_type pos1 = path.find_first_of(TC_CONFIG_PARAM_BEGIN);
        if(pos1 == string::npos)
        {
            throw TC_Config_Exception("[TC_Config::parseDomainName] : param path '" + path + "' is invalid!" );
        }

        if(path[0] != TC_CONFIG_DOMAIN_SEP)
        {
            throw TC_Config_Exception("[TC_Config::parseDomainName] : param path '" + path + "' must start with '/'!" );
        }

        string::size_type pos2 = path.find_first_of(TC_CONFIG_PARAM_END);
        if(pos2 == string::npos)
        {
            throw TC_Config_Exception("[TC_Config::parseDomainName] : param path '" + path + "' is invalid!" );
        }

        dp._domains = TC_Common::sepstr<string>(path.substr(1, pos1-1), TC_Common::tostr(TC_CONFIG_DOMAIN_SEP));
        dp._param = path.substr(pos1+1, pos2 - pos1 - 1);
    }
    else
    {
//        if(path.length() <= 1 || path[0] != TC_CONFIG_DOMAIN_SEP)
        if(path[0] != TC_CONFIG_DOMAIN_SEP)
        {
            throw TC_Config_Exception("[TC_Config::parseDomainName] : param path '" + path + "' must start with '/'!" );
        }

        dp._domains = TC_Common::sepstr<string>(path.substr(1), TC_Common::tostr(TC_CONFIG_DOMAIN_SEP));
    }

    return dp;
}

TC_ConfigDomain* TC_ConfigDomain::addSubDomain(const string& name)
{
    if(_subdomain.find(name) == _subdomain.end())
    {
        _domain.push_back(name);

        _subdomain[name] = new TC_ConfigDomain(name);
    }
    return _subdomain[name];
}

string TC_ConfigDomain::getParamValue(const string &name) const
{
    map<string, string>::const_iterator it = _param.find(name);
    if( it == _param.end())
    {
        throw TC_ConfigNoParam_Exception("[TC_ConfigDomain::getParamValue] param '" + name + "' not exits!");
    }

    return it->second;
}

TC_ConfigDomain *TC_ConfigDomain::getSubTcConfigDomain(vector<string>::const_iterator itBegin, vector<string>::const_iterator itEnd)
{
    if(itBegin == itEnd)
    {
        return this;
    }

    map<string, TC_ConfigDomain*>::const_iterator it = _subdomain.find(*itBegin);

    //根据匹配规则找不到匹配的子域
    if(it == _subdomain.end())
    {
        return NULL;
    }

    //继续在子域下搜索
    return it->second->getSubTcConfigDomain(itBegin + 1, itEnd);
}

const TC_ConfigDomain *TC_ConfigDomain::getSubTcConfigDomain(vector<string>::const_iterator itBegin, vector<string>::const_iterator itEnd) const
{
    if(itBegin == itEnd)
    {
        return this;
    }

    map<string, TC_ConfigDomain*>::const_iterator it = _subdomain.find(*itBegin);

    //根据匹配规则找不到匹配的子域
    if(it == _subdomain.end())
    {
        return NULL;
    }

    //继续在子域下搜索
    return it->second->getSubTcConfigDomain(itBegin + 1, itEnd);
}

void TC_ConfigDomain::insertParamValue(const map<string, string> &m)
{
    _param.insert(m.begin(),  m.end());

    map<string, string>::const_iterator it = m.begin();
    while(it != m.end())
    {
        size_t i = 0;
        for(; i < _key.size(); i++)
        {
            if(_key[i] == it->first)
            {
                break;
            }
        }

        //没有该key, 则添加到最后
        if(i == _key.size())
        {
            _key.push_back(it->first);
        }

        ++it;
    }
}

void TC_ConfigDomain::setParamValue(const string &name, const string &value)
{
    _param[name] = value;

    //如果key已经存在,则删除
    for(vector<string>::iterator it = _key.begin(); it != _key.end(); ++it)
    {
        if(*it == name)
        {
            _key.erase(it);
            break;
        }
    }

    _key.push_back(name);
}

void TC_ConfigDomain::setParamValue(const string &line)
{
    if(line.empty())
    {
        return;
    }

    _line.push_back(line);

    string::size_type pos = 0;
    for(; pos <= line.length() - 1; pos++)
    {
        if (line[pos] == '=')
        {
            if(pos > 0 && line[pos-1] == '\\')
            {
                continue;
            }

            string name  = parse(TC_Common::trim(line.substr(0, pos), " \r\n\t"));

            string value;
            if(pos < line.length() - 1)
            {
                value = parse(TC_Common::trim(line.substr(pos + 1), " \r\n\t"));
            }

            setParamValue(name, value);
            return;
        }
    }

    setParamValue(line, "");
}

string TC_ConfigDomain::parse(const string& s)
{
    if(s.empty())
    {
        return "";
    }

    string param;
    string::size_type pos = 0;
    for(; pos <= s.length() - 1; pos++)
    {
        char c;
        if(s[pos] == '\\' && pos < s.length() - 1)
        {
            switch (s[pos+1])
            {
            case '\\':
                c = '\\';
                pos++;
                break;
            case 'r':
                c = '\r';
                pos++;
                break;
            case 'n':
                c = '\n';
                pos++;
                break;
            case 't':
                c = '\t';
                pos++;
                break;
            case '=':
                c = '=';
                pos++;
                break;
            default:
                throw TC_Config_Exception("[TC_ConfigDomain::parse] '" + s + "' is invalid, '" + TC_Common::tostr(s[pos]) + TC_Common::tostr(s[pos+1]) + "' couldn't be parse!" );
            }

            param += c;
        }
        else if (s[pos] == '\\')
        {
            throw TC_Config_Exception("[TC_ConfigDomain::parse] '" + s + "' is invalid, '" + TC_Common::tostr(s[pos]) + "' couldn't be parse!" );
        }
        else
        {
            param += s[pos];
        }
    }

    return param;
}

string TC_ConfigDomain::reverse_parse(const string &s)
{
    if(s.empty())
    {
        return "";
    }

    string param;
    string::size_type pos = 0;
    for(; pos <= s.length() - 1; pos++)
    {
        string c;
        switch (s[pos])
        {
        case '\\':
            param += "\\\\";
            break;
        case '\r':
            param += "\\r";
            break;
        case '\n':
            param += "\\n";
            break;
        case '\t':
            param += "\\t";
            break;
            break;
        case '=':
            param += "\\=";
            break;
        case '<':
        case '>':
            throw TC_Config_Exception("[TC_ConfigDomain::reverse_parse] '" + s + "' is invalid, couldn't be parse!" );
        default:
            param += s[pos];
        }
    }

    return param;
}

string TC_ConfigDomain::getName() const
{
    return _name;
}

void TC_ConfigDomain::setName(const string& name)
{
    _name = name;
}

vector<string> TC_ConfigDomain::getKey() const
{
    return _key;
}

vector<string> TC_ConfigDomain::getLine() const
{
    return _line;
}

vector<string> TC_ConfigDomain::getSubDomain() const
{
    return _domain;
}

void TC_ConfigDomain::destroy()
{
    _param.clear();
    _key.clear();
    _line.clear();
    _domain.clear();

    map<string, TC_ConfigDomain*>::iterator it = _subdomain.begin();
    while(it != _subdomain.end())
    {
        delete it->second;
        ++it;
    }

    _subdomain.clear();
}

string TC_ConfigDomain::tostr(int i) const
{
    string sTab;
    for(int k = 0; k < i; ++k)
    {
        sTab += "\t";
    }

    ostringstream buf;

    buf << sTab << "<" << reverse_parse(_name) << ">" << endl;;

    for(size_t n = 0; n < _key.size(); n++)
    {
        map<string, string>::const_iterator it = _param.find(_key[n]);

        assert(it != _param.end());

        //值为空, 则不打印出=
        if(it->second.empty())
        {
            buf << "\t" << sTab << reverse_parse(_key[n]) << endl;
        }
        else
        {
            buf << "\t" << sTab << reverse_parse(_key[n]) << "=" << reverse_parse(it->second) << endl;
        }
    }

    ++i;

    for(size_t n = 0; n < _domain.size(); n++)
    {
        map<string, TC_ConfigDomain*>::const_iterator itm = _subdomain.find(_domain[n]);

        assert(itm != _subdomain.end());

        buf << itm->second->tostr(i);
    }


    buf << sTab << "</" << reverse_parse(_name) << ">" << endl;

    return buf.str();
}

/********************************************************************/
/*        TC_Config implement                                            */
/********************************************************************/

TC_Config::TC_Config() : _root("")
{
}

TC_Config::TC_Config(const TC_Config &tc)
: _root(tc._root)
{

}

TC_Config& TC_Config::operator=(const TC_Config &tc)
{
    if(this != &tc)
    {
        _root = tc._root;
    }

    return *this;
}

void TC_Config::parse(istream &is)
{
    _root.destroy();

    stack<TC_ConfigDomain*> stkTcCnfDomain;
    stkTcCnfDomain.push(&_root);

    string line;
    while(getline(is, line))
    {
        line = TC_Common::trim(line, " \r\n\t");

        if(line.length() == 0)
        {
            continue;
        }

        if(line[0] == '#')
        {
            continue;
        }
        else if(line[0] == '<')
        {
            string::size_type posl = line.find_first_of('>');

            if(posl == string::npos)
            {
                throw TC_Config_Exception("[TC_Config::parse]:parse error! line : " + line);
            }

            if(line[1] == '/')
            {
                string sName(line.substr(2, (posl - 2)));

                if(stkTcCnfDomain.size() <= 0)
                {
                    throw TC_Config_Exception("[TC_Config::parse]:parse error! <" + sName + "> hasn't matched domain.");
                }

                if(stkTcCnfDomain.top()->getName() != sName)
                {
                    throw TC_Config_Exception("[TC_Config::parse]:parse error! <" + stkTcCnfDomain.top()->getName() + "> hasn't match <" + sName +">.");
                }

                //弹出
                stkTcCnfDomain.pop();
            }
            else
            {
                string name(line.substr(1, posl - 1));

                stkTcCnfDomain.push(stkTcCnfDomain.top()->addSubDomain(name));
            }
        }
        else
        {
            stkTcCnfDomain.top()->setParamValue(line);
        }
    }

    if(stkTcCnfDomain.size() != 1)
    {
        throw TC_Config_Exception("[TC_Config::parse]:parse error : hasn't match");
    }
}

void TC_Config::parseFile(const string &sFileName)
{
    if(sFileName.length() == 0)
    {
        throw TC_Config_Exception("[TC_Config::parseFile]:file name is empty");
    }

    ifstream ff;
    ff.open(sFileName.c_str());
    if (!ff)
    {
        throw TC_Config_Exception("[TC_Config::parseFile]:fopen fail: " + sFileName, errno);
    }

    parse(ff);
}

void TC_Config::parseString(const string& buffer)
{
    istringstream iss;
    iss.str(buffer);

    parse(iss);
}

string TC_Config::operator[](const string &path)
{
    TC_ConfigDomain::DomainPath dp = TC_ConfigDomain::parseDomainName(path, true);

    TC_ConfigDomain *pTcConfigDomain = searchTcConfigDomain(dp._domains);

    if(pTcConfigDomain == NULL)
    {
        throw TC_ConfigNoParam_Exception("[TC_Config::operator[]] path '" + path + "' not exits!");
    }

    return pTcConfigDomain->getParamValue(dp._param);
}

string TC_Config::get(const string &sName, const string &sDefault) const
{
    try
    {
        TC_ConfigDomain::DomainPath dp = TC_ConfigDomain::parseDomainName(sName, true);

        const TC_ConfigDomain *pTcConfigDomain = searchTcConfigDomain(dp._domains);

        if(pTcConfigDomain == NULL)
        {
            throw TC_ConfigNoParam_Exception("[TC_Config::get] path '" + sName + "' not exits!");
        }

        return pTcConfigDomain->getParamValue(dp._param);
    }
    catch ( TC_ConfigNoParam_Exception &ex )
    {
        return sDefault;
    }
}

bool TC_Config::getDomainMap(const string &path, map<string, string> &m) const
{
    TC_ConfigDomain::DomainPath dp = TC_ConfigDomain::parseDomainName(path, false);

    const TC_ConfigDomain *pTcConfigDomain = searchTcConfigDomain(dp._domains);

    if(pTcConfigDomain == NULL)
    {
        return false;
    }

    m = pTcConfigDomain->getParamMap();

    return true;
}

map<string, string> TC_Config::getDomainMap(const string &path) const
{
    map<string, string> m;

    TC_ConfigDomain::DomainPath dp = TC_ConfigDomain::parseDomainName(path, false);

    const TC_ConfigDomain *pTcConfigDomain = searchTcConfigDomain(dp._domains);

    if(pTcConfigDomain != NULL)
    {
        m = pTcConfigDomain->getParamMap();
    }

    return m;
}

vector<string> TC_Config::getDomainKey(const string &path) const
{
    vector<string> v;

    TC_ConfigDomain::DomainPath dp = TC_ConfigDomain::parseDomainName(path, false);

    const TC_ConfigDomain *pTcConfigDomain = searchTcConfigDomain(dp._domains);

    if(pTcConfigDomain != NULL)
    {
        v = pTcConfigDomain->getKey();
    }

    return v;
}

vector<string> TC_Config::getDomainLine(const string &path) const
{
    vector<string> v;

    TC_ConfigDomain::DomainPath dp = TC_ConfigDomain::parseDomainName(path, false);

    const TC_ConfigDomain *pTcConfigDomain = searchTcConfigDomain(dp._domains);

    if(pTcConfigDomain != NULL)
    {
        v = pTcConfigDomain->getLine();
    }

    return v;
}

bool TC_Config::getDomainVector(const string &path, vector<string> &vtDomains) const
{
    TC_ConfigDomain::DomainPath dp = TC_ConfigDomain::parseDomainName(path, false);

    //根域, 特殊处理
    if(dp._domains.empty())
    {
        vtDomains = _root.getSubDomain();
        return !vtDomains.empty();
    }

    const TC_ConfigDomain *pTcConfigDomain = searchTcConfigDomain(dp._domains);

    if(pTcConfigDomain == NULL)
    {
        return false;
    }

    vtDomains = pTcConfigDomain->getSubDomain();

    return true;
}

vector<string> TC_Config::getDomainVector(const string &path) const
{
    TC_ConfigDomain::DomainPath dp = TC_ConfigDomain::parseDomainName(path, false);

    //根域, 特殊处理
    if(dp._domains.empty())
    {
        return _root.getSubDomain();
    }

    const TC_ConfigDomain *pTcConfigDomain = searchTcConfigDomain(dp._domains);

    if(pTcConfigDomain == NULL)
    {
        return vector<string>();
    }

    return pTcConfigDomain->getSubDomain();
}


TC_ConfigDomain *TC_Config::newTcConfigDomain(const string &sName)
{
    return new TC_ConfigDomain(sName);
}

TC_ConfigDomain *TC_Config::searchTcConfigDomain(const vector<string>& domains)
{
    return _root.getSubTcConfigDomain(domains.begin(), domains.end());
}

const TC_ConfigDomain *TC_Config::searchTcConfigDomain(const vector<string>& domains) const
{
    return _root.getSubTcConfigDomain(domains.begin(), domains.end());
}

int TC_Config::insertDomain(const string &sCurDomain, const string &sAddDomain, bool bCreate)
{
    TC_ConfigDomain::DomainPath dp = TC_ConfigDomain::parseDomainName(sCurDomain, false);

    TC_ConfigDomain *pTcConfigDomain = searchTcConfigDomain(dp._domains);

    if(pTcConfigDomain == NULL)
    {
        if(bCreate)
        {
            pTcConfigDomain = &_root;

            for(size_t i = 0; i < dp._domains.size(); i++)
            {
                pTcConfigDomain = pTcConfigDomain->addSubDomain(dp._domains[i]);
            }
        }
        else
        {
            return -1;
        }
    }

    pTcConfigDomain->addSubDomain(sAddDomain);

    return 0;
}

int TC_Config::insertDomainParam(const string &sCurDomain, const map<string, string> &m, bool bCreate)
{
    TC_ConfigDomain::DomainPath dp = TC_ConfigDomain::parseDomainName(sCurDomain, false);

    TC_ConfigDomain *pTcConfigDomain = searchTcConfigDomain(dp._domains);

    if(pTcConfigDomain == NULL)
    {
        if(bCreate)
        {
            pTcConfigDomain = &_root;

            for(size_t i = 0; i < dp._domains.size(); i++)
            {
                pTcConfigDomain = pTcConfigDomain->addSubDomain(dp._domains[i]);
            }
        }
        else
        {
            return -1;
        }
    }

    pTcConfigDomain->insertParamValue(m);

    return 0;
}

string TC_Config::tostr() const
{
    string buffer;

    map<string, TC_ConfigDomain*> msd = _root.getDomainMap();
    map<string, TC_ConfigDomain*>::const_iterator it = msd.begin();
    while (it != msd.end())
    {
        buffer += it->second->tostr(0);
        ++it;
    }

    return buffer;
}

void TC_Config::joinConfig(const TC_Config &cf, bool bUpdate)
{
    string buffer;
    if(bUpdate)
    {
        buffer = tostr() + cf.tostr();
    }
    else
    {
        buffer = cf.tostr() + tostr();
    }
    parseString(buffer);
}

}

