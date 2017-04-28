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
#include "util/tc_common.h"
#include "tars2oc.h"


void usage()
{
    cout << "Usage : tars2oc [OPTION] tarsfile" << endl;
	cout << "  --dir=DIRECTORY       generate source file to DIRECTORY(生成文件到目录DIRECTORY,默认为当前目录)" << endl;
    cout << "  --arc    是否支持arc版本" << endl;
	cout << "  --nonamespace  设置各个成员属性是否加上名字空间前缀,默认是加上" << endl;
	cout << "  --with-namespace=NAME 自定义变量名前缀的名字空间" << endl;
    cout << "  tars2oc support type: bool byte short int long float double vector map" << endl;
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

    //是否可以以tars开头
    g_parse->setTars(option.hasParam("with-tars"));
    g_parse->setHeader(option.getValue("header"));

    Tars2OC j2c;
	if (option.hasParam("dir"))
	{
		j2c.setBaseDir(option.getValue("dir"));
	}
	else
	{
		j2c.setBaseDir(".");
	}

	if (option.hasParam("arc"))
	{
		j2c.setArc(true);
	}
	else
	{
		j2c.setArc(false);
	}

	if(option.hasParam("nonamespace"))
	{
		j2c.setNeedNS(false);

	}
	else
	{
		j2c.setNeedNS(true);
		j2c.setNS(option.getValue("with-namespace"));
	}

	try
	{
	    for(size_t i = 0; i < vTars.size(); i++)
	    {

	        g_parse->parse(vTars[i]);
	        j2c.createFile(vTars[i]);
	    }
	}catch(exception& e)
	{
		cerr<<e.what()<<endl;
	}

    return 0;
}

