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

#ifndef __TARS_PROMISE_EXCEPTION_H_
#define __TARS_PROMISE_EXCEPTION_H_

#include <string.h>
#include <exception>
#include <string>
#include <sstream>
#include <memory>
#include "util/detail/tc_template_util.h"

namespace promise 
{

class PromiseException : public std::exception 
{
public:

    PromiseException()
    : _filename(NULL)
    , _line(0)
    , _sep(':')
    {}

    PromiseException(const char *filename, int line)
    : _filename(filename)
    , _line(line)
    , _sep(':')
    {}

    virtual ~PromiseException() throw() {}
    
    virtual const char *what() const throw()
    {
        try 
        {
            if (!_what.unique())
                _what.reset(new std::string(*_what));

            if (!_what || _what->empty()) 
            {
                std::stringstream ss;
                ss << name() << " thrown";

                if (_filename && _line > 0)
                    ss << " at " << _filename << ':' << _line;

                if (!_what)
                    _what.reset(new std::string());

                const std::string& s = getErrorInfo();
                if (!s.empty()) 
                {
                    ss << _sep << ' ' << s;
                    _sep = ';';
                }
                ss.str().swap(*_what);
            }
                
            if (!_data.unique())
                _data.reset(new std::string(*_data));

            if (_data && !_data->empty()) 
            {
                *_what += _sep;
                _sep = ';';
                *_what += ' ';
                _what->append(*_data);
                _data.reset();
            }
            return _what->c_str();
        } 
        catch (...) 
        {

        }
        return "";
    }
    
private:

    virtual const char *name() const
    { 
        return "PromiseException"; 
    }

    virtual std::string getErrorInfo() const
    { 
        return ""; 
    }

    const char *_filename;
    int _line;

    mutable std::shared_ptr<std::string> _what;
    mutable std::shared_ptr<std::string> _data;
    mutable char _sep;

    template <typename T>
    void injectErrorInfo(const T& t) const;
    
    template <typename Exception, typename T>
    friend typename tc_enable_if<tc_is_derived_from<Exception, PromiseException>, const Exception&>::type
    operator<<(const Exception& e, const T& t)
    {
        e.injectErrorInfo(t);
        return e;
    }
};

template <typename T>
void PromiseException::injectErrorInfo(const T& t) const
{
    std::stringstream ss;
    ss << t;

    if (!_data.unique())
        _data.reset(new std::string(*_data));

    if (!_data) 
    {
        _data.reset(new std::string(ss.str()));
    } 
    else 
    {
        (*_data) += ss.str();
    }
}

namespace detail 
{
    inline std::string errnoToString(int errorno)
    {
        return strerror(errorno);
    }

}  // namespace detail

class SyscallException : public PromiseException 
{
public:

    SyscallException(const char *filename, int line, int err)
    : PromiseException(filename, line)
    , _errno(err)
    {}

    int error() const { return _errno; }

protected:
    
    virtual const char *name() const
    { 
        return "SyscallException"; 
    }

    virtual std::string getErrorInfo() const
    {
        return detail::errnoToString(_errno);
    }

private:

    int _errno;
};
    
class AssertionFailureException : public PromiseException 
{
public:
    AssertionFailureException(const char *filename, int line)
        : PromiseException(filename, line)
    {
    }

private:
    virtual const char *name() const
    {
        return "AssertionFailureException";
    }
};

}  // namespace promise

#endif  // __TARS_PROMISE_EXCEPTION_H__
