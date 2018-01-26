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

#include "tars2node.h"

void usage()
{
    cout << "Usage : tars2node [OPTION] tarsfile" << endl;
    cout << "  --tars-lib-path=DIRECTORY  specify the path of tars nodejs module." << endl;
    cout << "  --with-tars                allow you to use keyword 'tars' as a namespace." << endl;
    cout << "  --dir=DIRECTORY            generate source file to DIRECTORY." << endl;
    cout << "  --relative                 use current path." << endl;
    cout << "  --tarsBase=DIRECTORY       where to search tars files." << endl;
    cout << "  --r                        generate source all tars files." << endl;
    cout << "  --client                   just for client side source file." << endl;
    cout << "  --server                   just for server side source file." << endl << endl;
    cout << "  tars2node support type: boolean char short int long float double list map" << endl;

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
			usage();
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

	try
	{
        tars::TC_Option option;
        option.decode(argc, argv);
        vector<string> vTars = option.getSingle();
        check(vTars);

        if(option.hasParam("help"))
        {
            usage();
            return 0;
        }

        if(option.hasParam("tarsBase"))
        {
            if(::chdir(option.getValue("tarsBase").c_str()) != 0)
            {
                cerr << "changing working directory" << ::strerror(errno) << endl;
                return -1;
            }
        }

        g_parse->setTars(option.hasParam("with-tars"));
        g_parse->setUseCurrentPath(option.hasParam("relative"));

        Tars2Node tars2node;
        tars2node.setTarsLibPath(option.hasParam("tars-lib-path")?option.getValue("tars-lib-path"):"@tars/rpc");
        tars2node.setTarsStreamPath(option.hasParam("tars-stream-path")?option.getValue("tars-stream-path"):"@tars/stream");
        tars2node.setEnableClient(option.hasParam("client"));
        tars2node.setEnableServer(option.hasParam("server"));
        tars2node.setTargetPath(option.hasParam("dir")?option.getValue("dir"):"./");
        tars2node.setRecursive(option.hasParam("r"));
        tars2node.setUseSpecialPath(option.hasParam("relative"));

	    for (size_t i = 0; i < vTars.size(); i++)
	    {
	        tars2node.createFile(vTars[i]);
	    }
	}
    catch(exception& e)
	{
		cerr<<e.what()<<endl;
	}

    return 0;
}

