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
#include "tars2php.h"


void usage()
{
    cout << "Usage : tars2php [OPTION] tarsfile" << endl;
    cout << "  --coder=Demo::interface1;Demo::interface2   create interface encode and decode api" << endl;
	cout << "  --dir=DIRECTORY                             generate source file to DIRECTORY(生成文件到目录DIRECTORY,默认为当前目录)" << endl;
    cout << "  --check-default=<true,false>                如果optional字段值为默认值不打包(默认不打包)" << endl;
    cout << "  tars2php support type: bool byte short int long float double vector map" << endl;
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

    bool bCoder = option.hasParam("coder");
    vector<string> vCoder;
    if(bCoder)
    {
        vCoder = tars::TC_Common::sepstr<string>(option.getValue("coder"), ";", false);
        if(vCoder.size() == 0)
        {
            usage();
            return 0;
        }
    }

    Tars2Php j2p;

	if (option.hasParam("dir"))
	{
		j2p.setBaseDir(option.getValue("dir"));
	}
	else
	{
		j2p.setBaseDir(".");
	}

    j2p.setCheckDefault(tars::TC_Common::lower(option.getValue("check-default")) == "false"?false:true);
	try
	{
        //是否可以以tars开头
        g_parse->setTars(option.hasParam("with-tars"));
        g_parse->setHeader(option.getValue("header"));

	    for(size_t i = 0; i < vTars.size(); i++)
	    {

	        g_parse->parse(vTars[i]);
	        j2p.createFile(vTars[i], vCoder);
	    }
	}catch(exception& e)
	{
		cerr<<e.what()<<endl;
	}

    return 0;
}

