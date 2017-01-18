#ifndef _TC_CONTEXT_FCONTEXT_X86_64_H
#define _TC_CONTEXT_FCONTEXT_X86_64_H

#include <stdint.h>

namespace tars
{

extern "C" 
{

struct stack_t
{
    void    *   sp;
    std::size_t size;

    stack_t()
    : sp(0)
    , size(0)
    {}
};

struct fp_t
{
    uint32_t     fc_freg[2];

    fp_t()
    : fc_freg()
    {}
};

struct fcontext_t
{
    uint64_t     fc_greg[8];
    stack_t      fc_stack;
    fp_t         fc_fp;

    fcontext_t()
    : fc_greg()
    , fc_stack()
    , fc_fp()
    {}
};

}

}

#endif
