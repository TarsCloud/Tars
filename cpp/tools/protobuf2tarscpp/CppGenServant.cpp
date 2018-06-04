// Generates C++ tars service interface out of Protobuf IDL.
//
// This is a Proto2 compiler plugin.  See net/proto2/compiler/proto/plugin.proto
// and net/proto2/compiler/public/plugin.h for more information on plugins.

#include <google/protobuf/compiler/code_generator.h>
#include <google/protobuf/compiler/plugin.h>
#include <google/protobuf/compiler/plugin.pb.h>
#include <google/protobuf/descriptor.h>

#include "CppGenServant.h"

static std::string GenMethods(const ::google::protobuf::MethodDescriptor* method,
                              const std::string& pkg,
                              int indent) {
    std::string out;
    out.reserve(8 * 1024);

    out += "virtual " + pkg + "::" + method->output_type()->name() + " " + method->name() +
           "(const " + pkg + "::" + method->input_type()->name() + "& , tars::TarsCurrentPtr current) = 0;" + LineFeed(indent);
    out += "static void async_response_" + method->name() + "(tars::TarsCurrentPtr current, const TestApp::PbResponse &_ret)" + LineFeed(indent);
    out += "{" + LineFeed(++indent);
    out += "std::string _os;" + LineFeed(indent) +
           " _ret.SerializeToString(&_os);" + LineFeed(indent) + 
           "vector<char> _vc(_os.begin(), _os.end());" + LineFeed(indent) +
           "current->sendResponse(tars::TARSSERVERSUCCESS, _vc);";
    out += LineFeed(--indent) + "}";
    out += LineFeed(indent);
    out += LineFeed(indent);

    return out;
}

static std::string GenDispatchCase(const ::google::protobuf::MethodDescriptor* method,
                                   const std::string& pkg,
                                   int indent) {
    std::string out;
    out.reserve(8 * 1024);

    out += "{";
    out += LineFeed(++indent);
    out += "tars::TarsInputStream<tars::BufferReader> _is;" + LineFeed(indent) +
           "_is.setBuffer(_current->getRequestBuffer());" + LineFeed(indent);
    out += LineFeed(indent);

    out += pkg + "::" + method->input_type()->name() + " req;" + LineFeed(indent);
    out += "req.ParseFromArray(&_current->getRequestBuffer()[0], _current->getRequestBuffer().size());" + LineFeed(indent);
    out += LineFeed(indent);

    out += pkg + "::" + method->output_type()->name() + " _ret = " + method->name() + "(req, _current);" +  LineFeed(indent);
    out += "if (_current->isResponse())" + LineFeed(indent);
    out += "{" + LineFeed(++indent);
    out += "std::string _os;" + LineFeed(indent);
    out += "_ret.SerializeToString(&_os);" + LineFeed(indent);
    out += "std::vector<char> _vc(_os.begin(), _os.end());" + LineFeed(indent);
    out += "_sResponseBuffer.assign(_os.begin(), _os.end());";
    out += LineFeed(--indent) + "}";
    out += LineFeed(indent);
    out += "return tars::TARSSERVERSUCCESS;";

    out += LineFeed(--indent);
    out += "}";

    return out;
}

std::string GenServant(const ::google::protobuf::ServiceDescriptor* desc, int indent) {
    std::string out;
    out.reserve(8 * 1024);

    const auto& name = desc->name();
    const auto& pkg = desc->file()->package();
    const auto& servant = name;

    out += LineFeed(indent);
    out += "/* servant for server */";
    out += LineFeed(indent);
    out += "class " + servant + " : public tars::Servant";
    out += LineFeed(indent);
    out += "{";
    out += LineFeed(indent);
    out += "public:";
    out += LineFeed(++indent);
    out += "virtual ~" + servant + "() {}";
    out += LineFeed(indent);

    for (int i = 0; i < desc->method_count(); ++i) {
        out += GenMethods(desc->method(i), pkg, indent);
    }

    // gen onDispatch
    out += LineFeed(indent);
    out +=  "int onDispatch(tars::TarsCurrentPtr _current, std::vector<char>& _sResponseBuffer)";
    out += LineFeed(indent);
    out += "{";
    out += LineFeed(++indent);
    out += "static ::std::string __all[] = ";
    out += "{";
    out += LineFeed(++indent);
    for (int i = 0; i < desc->method_count(); ++i) {
        auto method = desc->method(i);
        out += "\"" + method->name() + "\",";
        out += LineFeed(indent);
    }
    out += LineFeed(--indent);
    out += "};";
    out += LineFeed(indent);
    out += "pair<string*, string*> r = equal_range(__all, __all + " + std::to_string((long long)desc->method_count()) + ", " + "_current->getFuncName());";
    out += LineFeed(indent);
    out += "if(r.first == r.second) return tars::TARSSERVERNOFUNCERR;";
    out += LineFeed(indent);
    out += "switch(r.first - __all)";
    out += LineFeed(indent);
    out += "{";
    out += LineFeed(++indent);
    for (int i = 0; i < desc->method_count(); ++i) {
        auto method = desc->method(i);
        out += LineFeed(indent);
        out += "case " + std::to_string((long long)i) + ":";
        out += LineFeed(indent);

        out += GenDispatchCase(method, pkg, indent);
    }

    // end switch
    out += LineFeed(--indent);
    out += "} // end switch";

    out += LineFeed(indent);
    out += "return tars::TARSSERVERNOFUNCERR;";
    out += LineFeed(--indent);
    out += "}"; // end of onDispatch
    out += LineFeed(indent);

    out += LineFeed(--indent) + "}; // end class " + servant;
    out += LineFeed(indent); // end class

    out += LineFeed(indent);

    return out;
}

