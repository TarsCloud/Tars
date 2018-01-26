#ifndef _TC_CONTEXT_FCONTEXT_H
#define _TC_CONTEXT_FCONTEXT_H

// x86_64
// test x86_64 before i386 because icc might
// define __i686__ for x86_64 too
#if defined(__x86_64__) || defined(__x86_64) \
    || defined(__amd64__) || defined(__amd64) \
    || defined(_M_X64) || defined(_M_AMD64)
#  include "tc_fcontext_x86_64.h"
// i386
#elif defined(i386) || defined(__i386__) || defined(__i386) \
    || defined(__i486__) || defined(__i586__) || defined(__i686__) \
    || defined(__X86__) || defined(_X86_) || defined(__THW_INTEL__) \
    || defined(__I86__) || defined(__INTEL__) || defined(__IA32__) \
    || defined(_M_IX86) || defined(_I86_)
#  include "tc_fcontext_i386.h"
#else
# error "platform not supported"
#endif

namespace tars
{

extern "C" intptr_t jump_fcontext( fcontext_t * ofc, fcontext_t const* nfc, intptr_t vp, bool preserve_fpu = true);
extern "C" fcontext_t * make_fcontext( void * sp, std::size_t size, void (* fn)( intptr_t) );

}


#endif
