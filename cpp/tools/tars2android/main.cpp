/**
 * Tencent is pleased to support the open source community by making Tars available.
 *
 * Copyright (C) 2016THL A29 Limited, a Tencent company. All rights reserved.
 *
 * Licensed under the BSD 3-Clause License (the "License"); you may not use this file except 
 * in compliance with the License. You may obtain a copy of the License at
 *
 * https://opensource.org/licenses/BSD-3-Clause
 *
 * Unless required by applicable law or agreed to in writing, software distributed 
 * under the License is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR 
 * CONDITIONS OF ANY KIND, either express or implied. See the License for the 
 * specific language governing permissions and limitations under the License.
 */
#include "util/tc_option.h"
#include "util/tc_file.h"
#include "tars2android.h"

void usage()
{
    cout << "Usage : tars2android [OPTION] tarsfile" << endl;
    cout << "  tars2android support type: bool byte short int long float double vector map" << endl;
    cout << "supported [OPTION]:" << endl;
    cout << "  --help                           help,print this" << endl;
    cout << "  --dir=DIRECTORY                  generate java file to DIRECTORY(default to current)" << endl;
    cout << "  --base-package=NAME              package prefix, default(com.qq.)" << endl;
	cout << "  --not-force-array                default changed byte vector to byte[], use this for list" << endl;
    cout << "  --check-default=<true,false>     optional field not package(default package)" << endl;
    cout << "  --extends-package=NAME           set the extends package name"<< endl;
    cout << "  --with-charset                   set charset, default UTF8" << endl;
    cout << "  --with-JavaBeanRule              support javabeab, default not support" << endl;
    cout << "  --include=dir1;dir2;dir3         设置tars文件搜索路径" << endl;

    cout << endl;
    exit(0);
}

void check(vector<string> &vTars)
{
    for(size_t i  = 0; i < vTars.size(); i++)
    {
        string ext  = tars::TC_File::extractFileExt(vTars[i]);
        if(ext == "tars")
        {
            if(!tars::TC_File::isFileExist(vTars[i]))
            {
                cerr << "file '" << vTars[i] << "' not exists" << endl;
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

int main(int argc, char* argv[])
{
    if(argc < 2)
    {
        usage();
    }

    tars::TC_Option option;
    option.decode(argc, argv);
    vector<string> vTars = option.getSingle();

    check(vTars);

    if(option.hasParam("help"))
    {
        usage();
    }

    Tars2Java t2a;

    g_parse->setTars(option.hasParam("with-tars"));

    if(option.getValue("dir") != "")
    {
        t2a.setBaseDir(option.getValue("dir"));
    }
    else
    {
        t2a.setBaseDir(".");
    }

    if(option.hasParam("base-package"))
    {
        t2a.setBasePackage(option.getValue("base-package"));
    }
    else
    {
        t2a.setBasePackage("prx.");
    }

	if (option.hasParam("not-force-array"))
	{
		t2a.setForceArray(false);
	}
	else
	{
		t2a.setForceArray(true);
	}

    if (option.hasParam("extends-package"))
    {
        t2a.setTafPacket(option.getValue("extends-package"));
    }

    t2a.setCheckDefault(tars::TC_Common::lower(option.getValue("check-default")) == "true"?true:false);


    if (option.hasParam("with-charset"))
    {
        t2a.setCharset(option.getValue("with-charset"));
    }
	else
	{
		t2a.setCharset("UTF8");
	}

    if(option.hasParam("with-JavaBeanRule"))
    {
        t2a.setWithJbr(true);
    }
    else
    {
        t2a.setWithJbr(false);
    }

    t2a.setWithCompact(false);
    t2a.setEnumCompact(true);

    t2a.setWithGenerateInterfaceDependencies(true);
    t2a.setWithFilterRomTars(true);

	try
	{
        //增加include搜索路径
        g_parse->addIncludePath(option.getValue("include"));

	    for(size_t i = 0; i < vTars.size(); i++)
	    {
	        g_parse->parse(vTars[i]);
	        t2a.createFile(vTars[i]);
	    }
	}catch(exception& e)
	{
		cerr<<e.what()<<endl;
	}

    return 0;
}

