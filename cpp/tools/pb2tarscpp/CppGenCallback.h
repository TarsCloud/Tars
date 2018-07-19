// Generates C++ tars service interface out of Protobuf IDL.
//
// This is a Proto2 compiler plugin.  See net/proto2/compiler/proto/plugin.proto
// and net/proto2/compiler/public/plugin.h for more information on plugins.

#pragma once

#include <string>
#include "CppPbUtils.h"

namespace google
{

namespace protobuf
{
class ServiceDescriptor;
}

}


// gen prx callback
std::string GenPrxCallback(const ::google::protobuf::ServiceDescriptor* desc, int indent);

