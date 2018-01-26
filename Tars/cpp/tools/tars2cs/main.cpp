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
#include "tars2cs.h"

void usage()
{
    cout << "Usage : tars2cs [OPTION] tarsfile" << endl;
    cout << "  tars2cs support type: bool byte short int long float double vector map" << endl;
    cout << "supported [OPTION]:" << endl;
    cout << "  --help                help,print this(帮助)" << endl;
    cout << "  --dir=DIRECTORY       generate java file to DIRECTORY(生成文件到目录DIRECTORY,默认为当前目录)" << endl;
    cout << "  --base-package=NAME   package prefix, default 'com.qq.'(package前缀，未指定则默认为com.qq.)" << endl;
    cout << "  --with-servant        also generate servant class(一并生成服务端代码，未指定则默认不生成)" << endl;
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

    Tars2Cs t2cs;


    //设置生成文件的根目录
    if(option.getValue("dir") != "")
    {
        t2cs.setBaseDir(option.getValue("dir"));
    }
    else
    {
        t2cs.setBaseDir(".");
    }

    //包名前缀
    if(option.hasParam("base-package"))
    {
        t2cs.setBasePackage(option.getValue("base-package"));
    }
    else
    {
        t2cs.setBasePackage("Com.QQ.");
    }

    //是否生成服务端类,默认不生成
    if(option.hasParam("with-servant"))
    {
        t2cs.setWithServant(true);
    }
    else
    {
        t2cs.setWithServant(false);
    }
	try
	{
        //是否可以以tars开头
        g_parse->setTars(option.hasParam("with-tars"));

	    for(size_t i = 0; i < vTars.size(); i++)
	    {
	        g_parse->parse(vTars[i]);
	        t2cs.createFile(vTars[i]);
	    }
	}catch(exception& e)
	{
		cerr<<e.what()<<endl;
	}

    return 0;
}

