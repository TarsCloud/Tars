// Generates C++ tars service interface out of Protobuf IDL.
//
// This is a Proto2 compiler plugin.  See net/proto2/compiler/proto/plugin.proto
// and net/proto2/compiler/public/plugin.h for more information on plugins.

#include <iostream>

#include <google/protobuf/compiler/plugin.h>
#include <google/protobuf/compiler/plugin.pb.h>
#include <google/protobuf/descriptor.h>

#include "CppPlugin.h"
#include "CppGenCallback.h"
#include "CppGenProxy.h"
#include "CppGenServant.h"


bool CppTarsGenerator::Generate(const google::protobuf::FileDescriptor *file,
                                const std::string &parameter,
                                google::protobuf::compiler::GeneratorContext *context,
                                std::string *error) const {

    if (!_CheckFile(file, error)) {
        return false;
    }

    std::string content = _GenHeader(ProtoFileBaseName(file->name()));

    // namespace
    content += _GenNamespaceBegin(file->package());

    const int indent = 1;
    content += LineFeed(indent);
    for (int i = 0; i < file->service_count(); ++i) {
        content += GenPrxCallback(file->service(i), indent);
        content += GenPrx(file->service(i), indent);
        content += GenServant(file->service(i), indent);
    }

    content += _GenNamespaceEnd(file->package());

    // gen response to parent
    const std::string& outputFile = ProtoFileBaseName(file->name()) + ".tars.h";
    std::string output = _GenResponse(outputFile, content);
    std::cout << output;

    return true;
}
    
std::string
CppTarsGenerator::_GenResponse(const std::string& filename,
                               const std::string& content) {
    google::protobuf::compiler::CodeGeneratorResponse response;
    auto f = response.add_file();
    f->set_name(filename);
    f->set_content(content);

    // output to parent
    std::string outbytes;
    response.SerializeToString(&outbytes);
    return outbytes;
}

bool CppTarsGenerator::_CheckFile(const google::protobuf::FileDescriptor* file,
                                  std::string* error) const {
    if (file->package().empty()) {
        error->append("package name is missed.");
        return false;
    }
        
    return true;
}

std::string CppTarsGenerator::_GenHeader(const std::string& name) {
    std::string content;
    content.reserve(8 * 1024);

    content += kDeclaration;
    content += "#pragma once\n\n";
    content += "#include <map>\n";
    content += "#include <string>\n";
    content += "#include <vector>\n";
    content += "#include \"servant/ServantProxy.h\"\n";
    content += "#include \"servant/Servant.h\"\n";

    // include pb file
    content += "#include \"" + name + ".pb.h\"\n";

    return content;
}

std::string CppTarsGenerator::_GenNamespaceBegin(const std::string& ns) {
    std::string content;
    content += "\n\nnamespace ";
    content += ns + "\n{";

    return content;
}

std::string CppTarsGenerator::_GenNamespaceEnd(const std::string& ns) {
    return std::string("\n} // end namespace " + ns + "\n\n");
}

int main(int argc, char *argv[]) {
    CppTarsGenerator generator;
    return google::protobuf::compiler::PluginMain(argc, argv, &generator);
}

