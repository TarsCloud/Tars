/**
 * Tencent is pleased to support the open source community by making Tars available.
 *
 * Copyright (C) 2016 THL A29 Limited, a Tencent company. All rights reserved.
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
#include "tarsAnalyzer.h"
#include "tupRequestF.h"

void usage()
{
    cout << "Usage : ./tarsAnalyzer tarsfile --FileBuff=tarsBuffer.txt --structName=TestInfo  [--startPos=0] [--tupPutName=mystruct]" << endl;

    cout << "  tarsAnalyzer        工具名" << endl;
    cout << "  tarsfile            必须指定，该文件中包含待分析的tars结构名称" << endl;

    cout << "  --structName       必须指定，指定待分析的结构名，结构名必须在tars文件名定义" << endl;


    cout << "  --FileBuff         指定待分析的tars二进制编码文件名" << endl;
    cout << "  --startPos         指定从编码文件的第几个字节开始是结构的编码，缺省从第0字节开始" << endl;

    cout << "  --tupPutName       tup编码时的属性名称，没有该选项表示待分析的二进制数据不是tup编码" << endl;
    cout << "  --dumpTup          是否输出tup包中的sBuffer数据,此功能依赖于tupPutName项，默认是不输出"<<endl;
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

    tars::TC_Option option;
    option.decode(argc, argv);
    vector<string> vTarsFiles = option.getSingle();

    check(vTarsFiles);

    if(option.hasParam("help"))
    {
        usage();
    }

    string streamFile;

    TarsAnalyzer jAnalyzer;

    if(option.hasParam("FileBuff"))
    {
        streamFile = option.getValue("FileBuff");
        jAnalyzer.setStreamFileName(streamFile);
    }
    else
    {
        usage();
    }


    if(option.hasParam("structName"))
    {
        string sStructName = option.getValue("structName");
        if(!sStructName.empty())
        {
            jAnalyzer.setStructName(sStructName);
        }
        else
        {
            cout<<"structName shouldn't be empty"<<endl;
            exit(1);
        }
    }

    if(option.hasParam("startPos"))
    {
        string pos = option.getValue("startPos");
        if(tars::TC_Common::strto<int>(pos) >= 0)
        {
            jAnalyzer.setStartPos(tars::TC_Common::strto<int>(pos));
        }
        else
        {
            cout<<"startPos should be >=0 "<<endl;
            exit(1);

        }
    }

    string tupPutName = option.getValue("tupPutName");
    if( tupPutName != "")
    {
        jAnalyzer.setStartPos(4); //tup包头有4个字节的长度

        jAnalyzer.setPutName(tupPutName);

        jAnalyzer.setTup(true);
        if(option.hasParam("dumpTup"))
        {
            jAnalyzer.setDumpTup(true);
        }
        //
        tars::TC_File::save2file("./tmpTupRequestF.tars", g_sTupPacket);
        g_parse->parse("./tmpTupRequestF.tars");

        tars::TC_File::removeFile("./tmpTupRequestF.tars", true);

        //analyze tup tars
        jAnalyzer.analyzeFile("./tmpTupRequestF.tars", streamFile);
        jAnalyzer.setTup(false);


        if (vTarsFiles.size() > 0)
        {
            //需要考虑tup包中的内容在多个tars文件的情况
            g_parse->parse(vTarsFiles[0]);
            //分析tup数据包中的sBuffer数据,即structName对应的数据
            jAnalyzer.analyzeWithTupBuffer(vTarsFiles[0], "");
        }
        return 0;

    }
    try
    {
        for (size_t i = 0; i < vTarsFiles.size(); i++)
        {
            g_parse->parse(vTarsFiles[i]);
            jAnalyzer.analyzeFile(vTarsFiles[i], streamFile);
        }
    }catch(exception& e)
    {
        cerr<<e.what()<<endl;
    }

    return 0;
}

