//
// Created by scguo on 2017/10/16.
//

#include <stdlib.h>  // for abort()
#include <iostream>
#include <string>

#ifndef NET_TARS_COMPILER_JAVA_GENERATOR_LOG_

/**
 * 输出帮助类
 */
class LogHelper
{
    std::ostream *os;

public:
    LogHelper(std::ostream *os) : os(os) {}

    ~LogHelper()
    {
        *os << std::endl;
        ::abort();
    }

    std::ostream &get_os()
    {
        return *os;
    }
};

// Abort the program after logging the mesage if the given condition is not
// true. Otherwise, do nothing.
#define TARS_CODEGEN_CHECK(x) !(x) && LogHelper(&std::cerr).get_os() \
                             << "CHECK FAILED: " << __FILE__ << ":" \
                             << __LINE__ << ": "

// Abort the program after logging the mesage.
#define TARS_CODEGEN_FAIL TARS_CODEGEN_CHECK(false)
#define NET_TARS_COMPILER_JAVA_GENERATOR_LOG_
#endif //NET_TARS_COMPILER_JAVA_GENERATOR_LOG_
