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
#include "tars2c.h"


void usage()
{
    cout << "Usage : tars2c [OPTION] tarsfile" << endl;
	cout << "  --dir=DIRECTORY       generate source file to DIRECTORY(�����ļ���Ŀ¼DIRECTORY,Ĭ��Ϊ��ǰĿ¼)" << endl;
    cout << "  --check-default<>     ���optional�ֶ�ֵΪĬ��ֵ�������(Ĭ�ϲ����)(value:true or false)" << endl;
    cout << "  tars2c support type: bool byte short int long float double vector map" << endl;
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



    Tars2C j2c;
	if (option.hasParam("dir"))
	{
		j2c.setBaseDir(option.getValue("dir"));
	}
	else
	{
		j2c.setBaseDir(".");
	}

    j2c.setCheckDefault(tars::TC_Common::lower(option.getValue("check-default")) == "false"?false:true);
    try
    {
        //�Ƿ������tars��ͷ
        g_parse->setTars(option.hasParam("with-tars"));
        g_parse->setHeader(option.getValue("header"));

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

