#ifndef __TC_CALLBACK_DETAIL_H__
#define __TC_CALLBACK_DETAIL_H__

#include "tc_scoped_ptr.h"

namespace tars
{

    namespace detail 
    {

        class TC_BindStateBase
        {
        public:
            virtual ~TC_BindStateBase() { }
        };


        template <typename T>
        struct TC_CallbackParamTraits
        {
            typedef const T& ForwardType;
            typedef T StorageType;
        };

        template <typename T>
        struct TC_CallbackParamTraits<T&>
        {
            typedef T& ForwardType;
            typedef T StorageType;
        };

        template <typename T>
        struct TC_CallbackParamTraits<const T&>
        {
            typedef const T& ForwardType;
            typedef T StorageType;
        };

        template <typename T>
        struct TC_CallbackParamTraits<T[]>
        {
            typedef const T* ForwardType;
            typedef const T* StorageType;
        };

        template <typename T, size_t n>
        struct TC_CallbackParamTraits<T[n]>
        {
            typedef const T* ForwardType;
            typedef const T* StorageType;
        };


        template <typename T>
        struct TC_CallbackParamTraits<TC_ScopedPtr<T> >
        {
            typedef TC_ScopedPtr<T> ForwardType;
            typedef TC_ScopedPtr<T> StorageType;
        };

    }

}

#endif
