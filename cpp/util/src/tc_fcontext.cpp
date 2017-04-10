#include "util/tc_fcontext.h"

#ifdef __APPLE__

namespace tars
{

intptr_t jump_fcontext( fcontext_t * ofc, fcontext_t const* nfc, intptr_t vp, bool preserve_fpu)
{
    //TODO..
    return 0;
}

fcontext_t * make_fcontext( void * sp, size_t size, void (* fn)( intptr_t) )
{
    //TODO..
    return NULL;
}

}

#else //!
    //see *.s
#endif
