#ifndef _TC_CONTEXT_FCONTEXT_I386H
#define _TC_CONTEXT_FCONTEXT_I386H

#include <stdint.h>

namespace tars
{

extern "C" 
{

struct stack_t
{
    void            *   sp;
    std::size_t         size;

    stack_t()
    : sp( 0)
    , size( 0)
    {}
};

struct fcontext_t
{
    uint32_t     fc_greg[6];
    stack_t      fc_stack;
    uint32_t     fc_freg[2];

    fcontext_t()
    : fc_greg()
    , fc_stack()
    , fc_freg()
    {}
};

}

}

#endif
