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

#include "SingleFileDownloader.h"
#include "servant/TarsLogger.h"
#include "util.h"

DownloadTaskFactory* DownloadTaskFactory::_instance = new DownloadTaskFactory();

int SingleFileDownloader::download(const PatchPrx &patchPrx, const string &remoteFile, const string &localFile, const DownloadEventPtr &pPtr, std::string & sResult)
{

    vector<tars::FileInfo> vFiles;
    int ret = 0;
    
    for( int i = 0; i < 2; i ++)
    {
        try
        {
            ret = patchPrx->listFileInfo(remoteFile, vFiles);
            break;
        }
        catch(TarsException& ex)
        {
            TLOGERROR("SingleFileDownloader::download " << (remoteFile + " TarsException " + ex.what())<< endl);
        }
    }

    //1表示是文件
    if(ret != 1)
    {
        sResult = remoteFile + " is not an file";
        return -1;
    }

    if(vFiles.size() < 1)
    {
        sResult = remoteFile + " not exist";
        return -2;
    }

    tars::FileInfo fileInfo = vFiles[0];

    FILE *fp = fopen(localFile.c_str(), "wb");
    if (!fp)
    {
        sResult = localFile + " can not write";
        return -3;
    }

    //循环下载文件到本地
    vector<char> buffer;
    int pos         = 0;
    int downloadRet = 0;
    bool fileEnded  = false;
    while (true)
    {
        buffer.clear();

        //最多尝试两次
        for( int i = 0; i < 2; i ++)
        {
            try
            {
                downloadRet = patchPrx->download(remoteFile, pos, buffer);
                break;
            }
            catch(TarsException& ex)
            {
                TLOGERROR("SingleFileDownloader::download "<< (remoteFile + " TarsException " + ex.what()) << endl);
            }
        }

        if (downloadRet < 0)
        {
            TLOGERROR("SingleFileDownloader::download " << "|downloadRet:" << downloadRet << "|remoteFile:"  << remoteFile << endl);
            sResult = remoteFile + " download from tarspatch error " + TC_Common::tostr(downloadRet);
            ret = downloadRet - 100;
            break;
        }
        else if (downloadRet == 0)
        {
            size_t r = fwrite((void*)&buffer[0], 1, buffer.size(), fp);
            if (r == 0)
            {
                TLOGERROR("SingleFileDownloader::download fwrite file '" + localFile + "' error!" << endl);
                sResult = "fwrite file '" + localFile + "' error!";
                ret = -5;
                break;
            }
            pos += r;
            if(pPtr)
            {
                pPtr->onDownloading(fileInfo, pos);
            }
        }
        else if (downloadRet == 1)
        {
            TLOGDEBUG("SingleFileDownloader::download load succ " << remoteFile << "|pos:"<<pos<<endl);
            ret = 0;
            fileEnded = true;
            break;
        }
    }

    if(!fileEnded)
    {
        ret = -6;
        sResult = remoteFile + " not end";
    }

    fclose(fp);

    return ret;
}
