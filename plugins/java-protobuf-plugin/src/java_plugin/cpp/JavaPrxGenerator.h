#ifndef _NET_TARS_PRX_COMPILER_JAVA_GENERATOR_H_
#define _NET_TARS_PRX_COMPILER_JAVA_GENERATOR_H_


#include <google/protobuf/io/zero_copy_stream.h>
#include <google/protobuf/descriptor.h>

#include "LogHelper.h"

using namespace std;

namespace JavaTarsPrxGenerator
{

    enum ProtoFlavor
    {
        NORMAL, LITE, NANO
    };

    /**
     * Returns the package name of the tars prx services defined in the given file.
     */
    string ServiceJavaPackage(const google::protobuf::FileDescriptor *file, bool nano);

    /**
     * Returns the name of the outer class that wraps in all the generated code for the given service.
     */
    string ServiceClassName(const google::protobuf::ServiceDescriptor *service);

    /**
     * Writes the generated service interface into the given ZeroCopyOutputStream
     */
    void GenerateService(const google::protobuf::ServiceDescriptor *service,
                         google::protobuf::io::ZeroCopyOutputStream *out,
                         ProtoFlavor flavor,
                         bool enable_deprecated);

}  // namespace JavaTarsPrxGenerator

#endif  // _NET_TARS_PRX_COMPILER_JAVA_GENERATOR_H_
