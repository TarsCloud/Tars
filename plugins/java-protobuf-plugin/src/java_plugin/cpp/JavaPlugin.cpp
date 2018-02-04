// Generates Java tars service interface out of Protobuf IDL.
//
// This is a Proto2 compiler plugin.  See net/proto2/compiler/proto/plugin.proto
// and net/proto2/compiler/public/plugin.h for more information on plugins.

#include <memory>

#include "JavaServantGenerator.h"
#include "JavaPrxGenerator.h"
#include "JavaPrxCallbackGenerator.h"
#include <google/protobuf/compiler/code_generator.h>
#include <google/protobuf/compiler/plugin.h>
#include <google/protobuf/descriptor.h>
#include <google/protobuf/io/zero_copy_stream.h>

/**
 * 把java的包名转化成文件目录名
 * @param package_name java的包名
 * @return string 文件目录名
 */
static string JavaPackageToDir(const string &package_name)
{
    string package_dir = package_name;
    for (size_t i = 0; i < package_dir.size(); ++i)
    {
        if (package_dir[i] == '.')
        {
            package_dir[i] = '/';
        }
    }
    if (!package_dir.empty())
    {
        package_dir += "/";
    }
    return package_dir;
}

/**
 * PB tars 代码生成的实现类
 */
class JavaTarsGenerator : public google::protobuf::compiler::CodeGenerator
{
public:
    /**
     *
     */
    JavaTarsGenerator() {}

    /**
     *
     */
    virtual ~JavaTarsGenerator() {}

    /**
     * 代码生成
     */
    virtual bool Generate(const google::protobuf::FileDescriptor *file,
                          const string &parameter,
                          google::protobuf::compiler::GeneratorContext *context,
                          string *error) const
    {
        std::vector<std::pair<string, string> > options;
        google::protobuf::compiler::ParseGeneratorParameter(parameter, &options);

        // 定义生成的代码类型
        JavaTarsServantGenerator::ProtoFlavor flavor = JavaTarsServantGenerator::ProtoFlavor::NORMAL;
        JavaTarsPrxGenerator::ProtoFlavor flavorPrx = JavaTarsPrxGenerator::ProtoFlavor::NORMAL;
        JavaTarsPrxCbGenerator::ProtoFlavor flavorPrxCB = JavaTarsPrxCbGenerator::ProtoFlavor::NORMAL;

        bool enable_deprecated = false;
        for (size_t i = 0; i < options.size(); i++)
        {
            std::cout << options[i].first << std::endl;
            std::cout << options[i].second << std::endl;
            if (options[i].first == "nano")
            {
                flavor = JavaTarsServantGenerator::ProtoFlavor::NANO;
                flavorPrx = JavaTarsPrxGenerator::ProtoFlavor::NANO;
                flavorPrxCB = JavaTarsPrxCbGenerator::ProtoFlavor::NANO;
            }
            else if (options[i].first == "lite")
            {
                flavor = JavaTarsServantGenerator::ProtoFlavor::LITE;
                flavorPrx = JavaTarsPrxGenerator::ProtoFlavor::LITE;
                flavorPrxCB = JavaTarsPrxCbGenerator::ProtoFlavor::LITE;
            }
            else if (options[i].first == "enable_deprecated")
            {
                enable_deprecated = options[i].second == "true";
            }
        }

        // 服务端接口生成
        {
            string package_name = JavaTarsServantGenerator::ServiceJavaPackage(
                    file, flavor == JavaTarsServantGenerator::ProtoFlavor::NANO);
            string package_filename = JavaPackageToDir(package_name);
            for (int i = 0; i < file->service_count(); ++i)
            {
                const google::protobuf::ServiceDescriptor *service = file->service(i);
                string filename = package_filename + JavaTarsServantGenerator::ServiceClassName(service) + ".java";
                std::unique_ptr<google::protobuf::io::ZeroCopyOutputStream> output(context->Open(filename));
                JavaTarsServantGenerator::GenerateService(service, output.get(), flavor, enable_deprecated);
            }
        }
        // 客户端代理接口生成
        {
            string package_name = JavaTarsPrxGenerator::ServiceJavaPackage(
                    file, flavorPrx == JavaTarsPrxGenerator::ProtoFlavor::NANO);
            string package_filename = JavaPackageToDir(package_name);
            for (int i = 0; i < file->service_count(); ++i)
            {
                const google::protobuf::ServiceDescriptor *service = file->service(i);
                string filename = package_filename + JavaTarsPrxGenerator::ServiceClassName(service) + ".java";
                std::unique_ptr<google::protobuf::io::ZeroCopyOutputStream> output(context->Open(filename));
                JavaTarsPrxGenerator::GenerateService(service, output.get(), flavorPrx, enable_deprecated);
            }
        }
        // 回调类的生成
        {
            string package_name = JavaTarsPrxCbGenerator::ServiceJavaPackage(
                    file, flavorPrxCB == JavaTarsPrxCbGenerator::ProtoFlavor::NANO);
            string package_filename = JavaPackageToDir(package_name);
            for (int i = 0; i < file->service_count(); ++i)
            {
                const google::protobuf::ServiceDescriptor *service = file->service(i);
                string filename = package_filename + JavaTarsPrxCbGenerator::ServiceClassName(service) + ".java";
                std::unique_ptr<google::protobuf::io::ZeroCopyOutputStream> output(context->Open(filename));
                JavaTarsPrxCbGenerator::GenerateService(service, output.get(), flavorPrxCB, enable_deprecated);
            }
        }
        return true;
    }
};

int main(int argc, char *argv[])
{
    JavaTarsGenerator generator;
    return google::protobuf::compiler::PluginMain(argc, argv, &generator);
}
