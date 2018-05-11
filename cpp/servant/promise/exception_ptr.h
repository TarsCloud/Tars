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

#ifndef __TARS_PROMISE_EXCEPTION_PTR_H_
#define __TARS_PROMISE_EXCEPTION_PTR_H_

#include <memory>
#include "promise_exception.h"
#include <stdexcept>
#include <new>
#include <typeinfo>
#include <string>

using namespace std;

namespace promise 
{
namespace detail 
{
    class ExceptionBase : public std::exception
    {
    public:
        ExceptionBase() {}

        virtual ~ExceptionBase() throw() {}

        virtual ExceptionBase* clone() const { return new ExceptionBase(*this); }

        virtual void rethrow() const { throw *this; }

        virtual const char* what() const throw() { return "ExceptionBase"; }
        
        virtual int getErrCode() const { return -1; }
    };

    class ExceptionBaseImpl : public ExceptionBase
    {
    public:
        ExceptionBaseImpl() {}
        
        ExceptionBaseImpl(const std::string& s)
        : _sInfo(s)
        , _error(-1)
        {}
        
        ExceptionBaseImpl(const std::string& s, int err)
        : _sInfo(s)
        , _error(err)
        {}

        virtual ~ExceptionBaseImpl() throw() {}

        virtual ExceptionBase* clone() const
        {
            return new ExceptionBaseImpl(*this);
        }

        virtual void rethrow() const
        {
            throw *this;
        }

        virtual const char* what() const throw()
        {
            return _sInfo.c_str();
        }
        
        virtual int getErrCode() const
        { 
            return _error; 
        }
    private:
        std::string _sInfo;
        int            _error;
    };

}  // namespace detail

typedef std::shared_ptr<detail::ExceptionBase> ExceptionPtr;

class CurrentExceptionUnknownException : PromiseException 
{
private:

    const char *name() const
    { 
        return "CurrentExceptionUnknownException"; 
    }
};

/**
 *
 */
inline ExceptionPtr currentException()
{
    try 
    {
        try 
        {
            throw;
        } 
        catch (detail::ExceptionBaseImpl& e) 
        {
            return ExceptionPtr(e.clone());
        } 
        catch (detail::ExceptionBase& e) 
        {
            return ExceptionPtr(e.clone());
        } 
        catch (std::domain_error& e) 
        {
            return ExceptionPtr(new detail::ExceptionBaseImpl(string(e.what(), strlen(e.what())) ));
        } 
        catch (std::invalid_argument& e) 
        {
            return ExceptionPtr(new detail::ExceptionBaseImpl(string(e.what(), strlen(e.what())) ));
        } 
        catch (std::length_error& e) 
        {
            return ExceptionPtr(new detail::ExceptionBaseImpl(string(e.what(), strlen(e.what())) ));
        } 
        catch (std::out_of_range& e) 
        {
            return ExceptionPtr(new detail::ExceptionBaseImpl(string(e.what(), strlen(e.what())) ));
        } 
        catch (std::range_error& e) 
        {
            return ExceptionPtr(new detail::ExceptionBaseImpl(string(e.what(), strlen(e.what())) ));                
        } 
        catch (std::overflow_error& e) 
        {
            return ExceptionPtr(new detail::ExceptionBaseImpl(string(e.what(), strlen(e.what())) ));
        } 
        catch (std::underflow_error& e) 
        {
            return ExceptionPtr(new detail::ExceptionBaseImpl(string(e.what(), strlen(e.what())) ));
        } 
        catch (std::logic_error& e) 
        {
            return ExceptionPtr(new detail::ExceptionBaseImpl(string(e.what(), strlen(e.what())) ));
        } 
        catch (std::bad_alloc& e) 
        {
            return ExceptionPtr(new detail::ExceptionBaseImpl(string(e.what(), strlen(e.what())) ));
        } 
        catch (std::bad_cast& e) 
        {
            return ExceptionPtr(new detail::ExceptionBaseImpl(string(e.what(), strlen(e.what())) ));
        } 
        catch (std::bad_typeid& e) 
        {
            return ExceptionPtr(new detail::ExceptionBaseImpl(string(e.what(), strlen(e.what())) ));
        } 
        catch (std::bad_exception& e) 
        {
            return ExceptionPtr(new detail::ExceptionBaseImpl(string(e.what(), strlen(e.what())) ));
        } 
        catch (PromiseException& e) 
        {
            return ExceptionPtr(new detail::ExceptionBaseImpl(string(e.what(), strlen(e.what())) ));
        } 
        catch (std::exception& e) 
        {
            return ExceptionPtr(new detail::ExceptionBaseImpl(string(e.what(), strlen(e.what())) ));
        }
    } 
    catch (std::bad_alloc& e) 
    {
        //return detail::ExceptionPtrStaticExceptionObject<std::bad_alloc>::e;
        return ExceptionPtr(new detail::ExceptionBaseImpl(string(e.what(), strlen(e.what())) ));
    } 
    catch (...) 
    {
        //return detail::ExceptionPtrStaticExceptionObject<CurrentExceptionUnknownException>::e;
        return ExceptionPtr(new detail::ExceptionBaseImpl(string("PromiseUnknownException") ));
    }
}


inline detail::ExceptionBaseImpl enableCurrentException(const string& t)
{
    return detail::ExceptionBaseImpl(t);
}

inline detail::ExceptionBaseImpl enableCurrentException(const string& t, int err)
{
    return detail::ExceptionBaseImpl(t, err);
}

template <typename E>
inline void throwException(const E& e)
{
    throw enableCurrentException(string(e.what(), strlen(e.what())));
}

inline ExceptionPtr copyException(const string& e)
{
    try 
    {
        throw enableCurrentException(e);
    } 
    catch (...) 
    {
        return currentException();
    }
}

inline ExceptionPtr copyException(const string& e, int err)
{
    try 
    {
        throw enableCurrentException(e, err);
    } 
    catch (...) 
    {
        return currentException();
    }
}
    
}  // namespace promise

#endif  // __EXCEPTION_PTR_H__
