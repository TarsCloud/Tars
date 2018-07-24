#ifndef __LOG_MACRO_H__
#define __LOG_MACRO_H__

#include "servant/TarsLogger.h"
//using namespace taf;
using namespace tars;

#define LOG_INFO		(LOG->info() << __FILE__ << ":" << __LINE__ << "[" << __FUNCTION__ << "]")
#define LOG_DEBUG		(LOG->debug() << __FILE__ << ":" << __LINE__ << "[" << __FUNCTION__ << "]")
#define LOG_WARN		(LOG->warn() << __FILE__ << ":" << __LINE__ << "[" << __FUNCTION__ << "]")
#define LOG_ERROR		(LOG->error() << __FILE__ << ":" << __LINE__ << "[" << __FUNCTION__ << "]")

#endif
