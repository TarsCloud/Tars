// Generates C++ tars service interface out of Protobuf IDL.
//
// This is a Proto2 compiler plugin.  See net/proto2/compiler/proto/plugin.proto
// and net/proto2/compiler/public/plugin.h for more information on plugins.

#pragma once

#include <string>

std::string ProtoFileBaseName(const std::string& fullName);

extern const std::string kDeclaration;
extern const std::string kIndent;

std::string LineFeed(int indent = 0) ;

