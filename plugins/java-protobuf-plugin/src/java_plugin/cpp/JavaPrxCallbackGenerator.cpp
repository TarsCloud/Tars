#include "JavaPrxCallbackGenerator.h"

#include <algorithm>
#include <iostream>
#include <iterator>
#include <map>
#include <vector>
#include <google/protobuf/compiler/java/java_names.h>
#include <google/protobuf/descriptor.h>
#include <google/protobuf/io/printer.h>
#include <google/protobuf/io/zero_copy_stream.h>

// Stringify helpers used solely to cast GRPC_VERSION
#ifndef STR
#define STR(s) #s
#endif

#ifndef XSTR
#define XSTR(s) STR(s)
#endif

#ifndef FALLTHROUGH_INTENDED
#define FALLTHROUGH_INTENDED
#endif

namespace JavaTarsPrxCbGenerator
{

    using google::protobuf::FileDescriptor;
    using google::protobuf::ServiceDescriptor;
    using google::protobuf::MethodDescriptor;
    using google::protobuf::Descriptor;
    using google::protobuf::io::Printer;
    using google::protobuf::SourceLocation;
    using std::to_string;

    /**
     * Adjust a method name prefix identifier to follow the JavaBean spec:
     * decapitalize the first letter
     * remove embedded underscores & capitalize the following letter
     */
    static string MixedLower(const string &word)
    {
        string w;
        w += tolower(word[0]);
        bool after_underscore = false;
        for (size_t i = 1; i < word.length(); ++i)
        {
            if (word[i] == '_')
            {
                after_underscore = true;
            }
            else
            {
                w += after_underscore ? toupper(word[i]) : word[i];
                after_underscore = false;
            }
        }
        return w;
    }

    /**
     * Converts to the identifier to the ALL_UPPER_CASE format.
     * An underscore is inserted where a lower case letter is followed by an
     * upper case letter.
     * All letters are converted to upper case
     */
    static string ToAllUpperCase(const string &word)
    {
        string w;
        for (size_t i = 0; i < word.length(); ++i)
        {
            w += toupper(word[i]);
            if ((i < word.length() - 1) && islower(word[i]) && isupper(word[i + 1]))
            {
                w += '_';
            }
        }
        return w;
    }

    static inline string LowerMethodName(const MethodDescriptor *method)
    {
        return MixedLower(method->name());
    }

    static inline string MethodPropertiesFieldName(const MethodDescriptor *method)
    {
        return "METHOD_" + ToAllUpperCase(method->name());
    }

    static inline string MethodIdFieldName(const MethodDescriptor *method)
    {
        return "METHODID_" + ToAllUpperCase(method->name());
    }

    static inline string MessageFullJavaName(bool nano, const Descriptor *desc)
    {
        string name = google::protobuf::compiler::java::ClassName(desc);
        if (nano)
        {
            if (isupper(name[0]))
            {
                // No java package specified.
                return "nano." + name;
            }
            for (size_t i = 0; i < name.size(); ++i)
            {
                if ((name[i] == '.') && (i < (name.size() - 1)) && isupper(name[i + 1]))
                {
                    return name.substr(0, i + 1) + "nano." + name.substr(i + 1);
                }
            }
        }
        return name;
    }

    static void TarsWriteServiceDocComment(Printer *printer,
                                           const ServiceDescriptor *service)
    {
        printer->Print("/**\n");
        printer->Print(" */\n");
    }

    enum StubType
    {
        ASYNC_INTERFACE = 0,
        BLOCKING_CLIENT_INTERFACE = 1,
        FUTURE_CLIENT_INTERFACE = 2,
        BLOCKING_SERVER_INTERFACE = 3,
        ASYNC_CLIENT_IMPL = 4,
        BLOCKING_CLIENT_IMPL = 5,
        FUTURE_CLIENT_IMPL = 6,
        ABSTRACT_CLASS = 7,
    };

    enum CallType
    {
        ASYNC_CALL = 0,
        BLOCKING_CALL = 1,
        FUTURE_CALL = 2
    };

    static void PrintMethod(
            const ServiceDescriptor *service,
            std::map<string, string> *vars,
            Printer *p, StubType type, bool generate_nano,
            bool enableDeprecated)
    {
        const string service_name = service->name();
        (*vars)["service_name"] = service_name;
        (*vars)["abstract_name"] = service_name + "ImplBase";
        string stub_name = service_name;
        string client_name = service_name;
        CallType call_type;
        bool impl_base = false;
        bool interface = false;
        switch (type)
        {
            case ABSTRACT_CLASS:
                call_type = ASYNC_CALL;
                impl_base = true;
                break;
            case ASYNC_CLIENT_IMPL:
                call_type = ASYNC_CALL;
                stub_name += "Stub";
                break;
            case BLOCKING_CLIENT_INTERFACE:
                interface = true;
                FALLTHROUGH_INTENDED;
            case BLOCKING_CLIENT_IMPL:
                call_type = BLOCKING_CALL;
                stub_name += "BlockingStub";
                client_name += "BlockingClient";
                break;
            case FUTURE_CLIENT_INTERFACE:
                interface = true;
                FALLTHROUGH_INTENDED;
            case FUTURE_CLIENT_IMPL:
                call_type = FUTURE_CALL;
                stub_name += "FutureStub";
                client_name += "FutureClient";
                break;
            case ASYNC_INTERFACE:
                call_type = ASYNC_CALL;
                interface = true;
                break;
            default:
                TARS_CODEGEN_FAIL << "Cannot determine class name for StubType: " << type;
        }
        (*vars)["stub_name"] = stub_name;
        (*vars)["client_name"] = client_name;

        // RPC methods
        interface = true;
        for (int i = 0; i < service->method_count(); ++i)
        {
            if (i != 0)
            {
                p->Print("\n");
            }
            const MethodDescriptor *method = service->method(i);
            (*vars)["input_type"] = MessageFullJavaName(generate_nano,
                                                        method->input_type());
            (*vars)["output_type"] = MessageFullJavaName(generate_nano,
                                                         method->output_type());
            (*vars)["lower_method_name"] = LowerMethodName(method);
            (*vars)["method_field_name"] = MethodPropertiesFieldName(method);
            bool client_streaming = method->client_streaming();
            bool server_streaming = method->server_streaming();

            if (call_type == BLOCKING_CALL && client_streaming)
            {
                // Blocking client interface with client streaming is not available
                continue;
            }

            if (call_type == FUTURE_CALL && (client_streaming || server_streaming))
            {
                // Future interface doesn't support streaming.
                continue;
            }
            switch (call_type)
            {
                case BLOCKING_CALL:
                    TARS_CODEGEN_CHECK(!client_streaming)
                    << "Blocking client interface with client streaming is unavailable";
                    if (server_streaming)
                    {
                        // Server streaming
                        p->Print(
                                *vars,
                                "$Iterator$<$output_type$> $lower_method_name$(\n"
                                        "    $input_type$ request)");
                    }
                    else
                    {
                        // Simple RPC
                        p->Print(
                                *vars,
                                "public abstract void callback_$lower_method_name$($output_type$ ret)");
                    }
                    break;
                case ASYNC_CALL:
                    break;
                case FUTURE_CALL:
                    break;
            }

            if (interface)
            {
                p->Print(";\n");
                continue;
            }
        }
    }

    static void PrintService(const ServiceDescriptor *service,
                             std::map<string, string> *vars,
                             Printer *p,
                             ProtoFlavor flavor,
                             bool enable_deprecated)
    {
        (*vars)["service_name"] = service->name();
        (*vars)["file_name"] = service->file()->name();
        (*vars)["service_class_name"] = ServiceClassName(service);
        TarsWriteServiceDocComment(p, service);
        p->Print(
                *vars,
                "public abstract class $service_class_name$ extends TarsAbstractCallback{\n");
        p->Indent();

        bool generate_nano = flavor == ProtoFlavor::NANO;
        PrintMethod(service, vars, p, BLOCKING_CLIENT_IMPL, generate_nano, enable_deprecated);
        p->Outdent();
        p->Print("}\n");
    }

    void PrintImports(Printer *p, bool generate_nano)
    {
        p->Print("import com.qq.tars.rpc.protocol.tars.support.TarsAbstractCallback;\n\n");
        if (generate_nano)
        {
            p->Print("import java.io.IOException;\n\n");
        }
    }

    void GenerateService(const ServiceDescriptor *service,
                         google::protobuf::io::ZeroCopyOutputStream *out,
                         ProtoFlavor flavor,
                         bool enable_deprecated)
    {
        // All non-generated classes must be referred by fully qualified names to
        // avoid collision with generated classes.
        std::map<string, string> vars;
        vars["String"] = "java.lang.String";
        vars["Override"] = "java.lang.Override";

        Printer printer(out, '$');
        string package_name = ServiceJavaPackage(service->file(),
                                                 flavor == ProtoFlavor::NANO);
        if (!package_name.empty())
        {
            printer.Print(
                    "package $package_name$;\n\n",
                    "package_name", package_name);
        }
        PrintImports(&printer, flavor == ProtoFlavor::NANO);

        // Package string is used to fully qualify method names.
        vars["Package"] = service->file()->package();
        if (!vars["Package"].empty())
        {
            vars["Package"].append(".");
        }
        PrintService(service, &vars, &printer, flavor, enable_deprecated);
    }

    string ServiceJavaPackage(const FileDescriptor *file, bool nano)
    {
        string result = google::protobuf::compiler::java::ClassName(file);
        size_t last_dot_pos = result.find_last_of('.');
        if (last_dot_pos != string::npos)
        {
            result.resize(last_dot_pos);
        }
        else
        {
            result = "";
        }
        if (nano)
        {
            if (!result.empty())
            {
                result += ".";
            }
            result += "nano";
        }
        return result;
    }

    string ServiceClassName(const google::protobuf::ServiceDescriptor *service)
    {
        return service->name() + "PrxCallback";
    }

}  // namespace JavaTarsPrxGenerator
