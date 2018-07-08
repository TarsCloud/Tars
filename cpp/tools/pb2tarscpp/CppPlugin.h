// Generates C++ tars service interface out of Protobuf IDL.
//
// This is a Proto2 compiler plugin.  See net/proto2/compiler/proto/plugin.proto
// and net/proto2/compiler/public/plugin.h for more information on plugins.

#include <string>
#include <google/protobuf/compiler/code_generator.h>

namespace google
{

namespace protobuf
{
class FileDescriptor;
}

}

class CppTarsGenerator : public google::protobuf::compiler::CodeGenerator {
public:
    CppTarsGenerator() {}

    virtual ~CppTarsGenerator() {}

    bool Generate(const google::protobuf::FileDescriptor *file,
                  const std::string &parameter,
                  google::protobuf::compiler::GeneratorContext *context,
                  std::string *error) const; //override final;
private:
    bool _CheckFile(const google::protobuf::FileDescriptor* file, std::string* error) const;
    static std::string _GenHeader(const std::string& file);
    static std::string _GenNamespaceBegin(const std::string& ns);
    static std::string _GenNamespaceEnd(const std::string& ns);

    static std::string _GenResponse(const std::string& filename, const std::string& content);
};

