#include "tars2go.h"
#include "util/tc_option.h"
#include "util/tc_file.h"
#include "util/tc_common.h"
#include <set>

void usage()
{
    cout << "Usage : tars2go [OPTION] tarsfile" << endl;
    //cout << "  --coder=Demo::interface1;Demo::interface2   create interface encode and decode api" << endl;
    cout << "  --dir=DIRECTORY                             generate source file to DIRECTORY(default current dir)"
            << endl;
    cout << "  tars2go support type: bool byte short int long float double vector map" << endl;
    exit(0);
}

void check(vector<string> &vJce)
{
    for (size_t i = 0; i < vJce.size(); i++)
    {
        string ext = tars::TC_File::extractFileExt(vJce[i]);
        if (ext == "tars")
        {
            if (!tars::TC_File::isFileExist(vJce[i]))
            {
                cerr << "file '" << vJce[i] << "' not exists" << endl;
                usage();
                exit(0);
            }
        }
        else
        {
            cerr << "only support tars file." << endl;
            exit(0);
        }
    }
}

static void get_dependency_files(const std::string& file, std::set<std::string>& fs)
{
    if (!tars::TC_File::isFileExist(file))
    {
        cerr << "Tars file '" << file << "' not exists" << endl;
        return;
    }
    fs.insert(file);
    g_parse->parse(file);
    std::vector<ContextPtr> contexts = g_parse->getContexts();
    for (size_t i = 0; i < contexts.size(); i++)
    {
        vector<string> is = contexts[i]->getIncludes();
        for (size_t j = 0; j < is.size(); j++)
        {
            std::string include_file = is[j];
            include_file = include_file.substr(0, include_file.size() - 2);
            include_file.append(".tars");
            string base = tars::TC_File::extractFilePath(file);
            //printf("####%s\n", is[j].c_str());
            get_dependency_files(base + "/" + include_file, fs);
        }
    }
}

int main(int argc, char* argv[])
{
    if (argc < 2)
    {
        usage();
    }

    tars::TC_Option option;
    option.decode(argc, argv);
    vector<string> vJce = option.getSingle();

    check(vJce);

    if (option.hasParam("help"))
    {
        usage();
    }

    bool bCoder = option.hasParam("coder");
    vector<string> vCoder;
    if (bCoder)
    {
        vCoder = tars::TC_Common::sepstr<string>(option.getValue("coder"), ";", false);
        if (vCoder.size() == 0)
        {
            usage();
            return 0;
        }
    }

    Tars2Go t2go;

    if (option.hasParam("dir"))
    {
        t2go.setBaseDir(option.getValue("dir"));
    }
    else
    {
        t2go.setBaseDir(".");
    }
    t2go.setCheckDefault(tars::TC_Common::lower(option.getValue("check-default")) == "true" ? true : false);

    try
    {
        //g_parse->setTaf(option.hasParam("with-taf"));
        g_parse->setTars(true);
        g_parse->setHeader(option.getValue("header"));
        g_parse->setCurrentPriority(option.hasParam("currentPriority"));

        t2go.setUnknownField(option.hasParam("unknown"));
        std::set<std::string> all_files;
        for (size_t i = 0; i < vJce.size(); i++)
        {
            get_dependency_files(vJce[i], all_files);
        }
        std::set<std::string>::iterator sit = all_files.begin();
        while (sit != all_files.end())
        {
            const string& f = *sit;
            g_parse->parse(f);
            t2go.createFile(f, vCoder);
            sit++;
        }
//        for (size_t i = 0; i < vJce.size(); i++)
//        {
//            g_parse->parse(vJce[i]);
//            j2c.createFile(vJce[i], vCoder);
//        }
    }
    catch (exception& e)
    {
        cerr << e.what() << endl;
    }

    return 0;
}

