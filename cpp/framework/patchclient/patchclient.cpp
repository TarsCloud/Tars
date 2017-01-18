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

#include "tars_patch.h"
#include "util/tc_option.h"
#include "servant/Application.h"

#include <iostream>

using namespace tars;

void usage(int argc, char *argv[])
{
    cout << "Usage:" << argv[0] << "  --server=[server] --remote=[server-dir] --local=[local] [--remove]" << endl;
    cout << "server: PatchObj@tcp -h ip -p port -t timeout" << endl;
    cout << "remote: directory or file sync from server" << endl;
    cout << "local:  directory sync to local" << endl;
    cout << "remove: remove local dir data" << endl;
    exit(0);
}

class PatchClientInterface : public TarsPatchNotifyInterface
{
public:
    virtual void onDownload(const FileInfo &fi)
    {
        cout << "------------------------------------" << endl;
        cout << "downloading [" << fi.path << "] [" << fi.size*1.0/1024 << "K]" << endl;
    }
    virtual void onDownloading(const FileInfo &fi, size_t pos, const string &dest)
    {
        cout << "downloading [" << fi.path << " ->] [" << dest << "] [" << pos*100/fi.size << "/100%]" << endl;
    }
    virtual void onDownloadOK(const FileInfo &fi, const string &dest)
    {
        cout << "download ok [" << fi.path << "] -> [" << dest << "]" << endl;
    }
    virtual void onListFile()
    {
        cout << "listing files" << endl;
    }
    virtual void onListFileOK(const vector<FileInfo> &vf)
    {
        cout << "list ok total [" << vf.size() << "] files" << endl;
    }
    virtual void onRemoveLocalDirectory(const string &sDir)
    {
        cout << "------------------------------------" << endl;
        cout << "remove local directory [" << sDir << "]" << endl;
    }
    virtual void onRemoveLocalDirectoryOK(const string &sDir)
    {
        cout << "remove local directory ok [" << sDir << "]"<< endl;
        cout << "------------------------------------" << endl;
    }
    virtual void onSetExecutable(const FileInfo &fi)
    {
        cout << "executable  [" << fi.path << "]" << endl;
    }
    virtual void onDownloadAllOK(const vector<FileInfo> &vf, time_t timeBegin, time_t timeEnd)
    {
        cout << "------------------------------------" << endl;
        cout << "download all files succ." << endl;
    }

    virtual void onReportTime(const string & sFile, const time_t timeBegin, const time_t timeEnd)
    {
        cout << "------------------------------------" << endl;
        cout << "download '" << sFile << "', begin:" << timeBegin << ", end:" << timeEnd << endl;
    }
};

typedef TC_AutoPtr<PatchClientInterface> PatchClientInterfacePtr;

int main(int argc, char *argv[])
{

    if(argc < 2)
    {
        usage(argc, argv);
    }

    try
    {

        TC_Option option;
        option.decode(argc, argv);

        //远程目录的相对路径
        string server = option.getValue("server");
        if(server.empty())
        {
            cout << "server must not be empty." << endl;
            return 0;
        }

        CommunicatorFactory::getInstance()->getCommunicator()->setProperty("sync-invoke-timeout", "60000");
        PatchPrx pPtr;
        CommunicatorFactory::getInstance()->getCommunicator()->stringToProxy(server, pPtr);

        //远程目录的相对路径
        string remote_dir = option.getValue("remote");
        if(remote_dir.empty())
        {
            cout << "remote_dir must not be empty." << endl;
            return 0;
        }

        //本地同步路径
        string local_dir  = option.getValue("local");
        if(local_dir.empty())
        {
            cout << "local_dir must not be empty." << endl;
            return 0;
        }

        //是否先清空本地目录
        bool bRemove = option.hasParam("remove");

        cout << "connecting to patch server" << endl;
        TarsPatch op;
        op.init(pPtr, remote_dir, local_dir);
        cout << "connect ok to patch server" << endl;
        op.setRemove(bRemove);

        PatchClientInterfacePtr p = new PatchClientInterface();
        op.download(p);
    }
    catch(exception &ex)
    {
        cout << ex.what() << endl;
    }

    return 0;
}


