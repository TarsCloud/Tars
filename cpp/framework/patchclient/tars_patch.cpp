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

#include "util/tc_file.h"
#include "util/tc_option.h"
#include "util/tc_md5.h"
#include "servant/TarsLogger.h"
#include "tars_patch.h"

#include <iostream>

namespace tars
{
TarsPatch::TarsPatch()
: _remove(false)
{
}

void TarsPatch::checkLocalDir()
{
    if (!tars::TC_File::isFileExistEx(_localDir, S_IFDIR))
    {
        throw TarsPatchException("local dir '" + _localDir + "' must be a directory");
    }
}

void TarsPatch::init(const PatchPrx &patchPrx, const string &remote_dir, const string &local_dir, const bool bCheck)
{
    _patchPrx   = patchPrx;
    _remoteDir  = remote_dir;
    _localDir   = local_dir;

    if (bCheck)
    {
        checkLocalDir();
    }
}

void TarsPatch::setRemove(bool bRemove)
{
    _remove = bRemove;
}

void TarsPatch::download(const TarsPatchNotifyInterfacePtr &pPtr)
{
    //记录下载本次服务的总的时间开始
    time_t timeBegin = TNOW;

    checkLocalDir();

    if (_remove)
    {
        string tmp = TC_File::simplifyDirectory(_localDir);// + "/" + _remoteDir);

        if (pPtr) pPtr->onRemoveLocalDirectory(tmp);

        int ret = TC_File::removeFile(tmp, true);
        if (ret != 0)
        {
            throw TarsPatchException("remove '" + tmp + "' error", errno);
        }

        if (pPtr)
        {
            pPtr->onRemoveLocalDirectoryOK(tmp);
        }
    }

    if (pPtr)
    {
        pPtr->onListFile();
    }

    //获取同步列表
    vector<FileInfo> vf;
    int ret = -1;

    int nRetryTime = 0;

RETRY:
    try
    {
        ret = _patchPrx->listFileInfo(_remoteDir, vf);

    }
    catch(TarsException& ex)
    {
        //最多重试三次
        if(nRetryTime < 3)
        {
            nRetryTime++;
            goto RETRY;
        }
        TLOGERROR("TarsPatch::download load error|" << __FILE__ << "," << __LINE__ << "|ex:" << ex.what() << endl);
        throw TarsPatchException(ex.what());
    }

    if (ret != 0 && ret != 1)
    {
        throw TarsPatchException("listFileInfo error");
    }

    if (pPtr)
    {
        pPtr->onListFileOK(vf);
    }

    if (ret == 0)
    {
        //path是路径, 对每个文件下载
        for (size_t i = 0; i < vf.size(); i++)
        {
            download(true, vf[i], pPtr);
        }
    }
    else if (ret == 1)
    {
        //path是文件
        download(false, vf[0], pPtr);
    }

    time_t timeEnd = TNOW;

    if (pPtr)
    {
        pPtr->onDownloadAllOK(vf, timeBegin, timeEnd);
    }
}

void TarsPatch::download(bool bDir, const FileInfo &fi, const TarsPatchNotifyInterfacePtr &pPtr)
{
    if (pPtr)
    {
        pPtr->onDownload(fi);
    }

    //获取本地文件目录
    string file_dir = _localDir;

    if (bDir)
    {
        file_dir = tars::TC_File::simplifyDirectory(_localDir + "/" + tars::TC_File::extractFilePath(fi.path));

        if (!file_dir.empty())
        {
            //创建本地目录
            if (!tars::TC_File::makeDirRecursive(file_dir))
            {
                throw TarsPatchException("create directory '" + file_dir + "' failed!");
            }
        }
        else
        {
            file_dir = ".";
        }
    }

    //本地文件名
    string file = tars::TC_File::simplifyDirectory(file_dir + "/" + tars::TC_File::extractFileName(fi.path));

    FILE *fp = fopen(file.c_str(), "wb");
    if (!fp)
    {
        throw TarsPatchException("fopen file '" + file + "' error", errno);
    }


    time_t timeBegin = TNOW;
    try
    {
        //循环下载文件到本地
        vector<char> v;
        int pos = 0;
        while (true)
        {
            v.clear();
            int ret;

            int nRetryTime = 0;

        RETRY:
            try
            {
                if (bDir)
                {
                    ret = _patchPrx->download(tars::TC_File::simplifyDirectory(_remoteDir + "/" + fi.path), pos, v);
                }
                else
                {
                    ret = _patchPrx->download(tars::TC_File::simplifyDirectory(_remoteDir), pos, v);
                }
            }
            catch(TarsException& ex)
            {
                //最多重试两次
                if(nRetryTime < 2)
                {
                    nRetryTime++;
                    goto RETRY;
                }
                TLOGERROR("TarsPatch::download load error|" << __FILE__ << "," << __LINE__ << "|"<<ex.what()<<endl);
                throw TarsPatchException(ex.what());
            }

            if (ret < 0)
            {
                throw TarsPatchException("download file '" + file + "' error!");
            }
            else if (ret == 0)
            {
                size_t r = fwrite((void*)&v[0], 1, v.size(), fp);
                if (r == 0)
                {
                    throw TarsPatchException("fwrite file '" + file + "' error!", errno);
                }
                pos += r;
                if (pPtr) pPtr->onDownloading(fi, pos, file);
            }
            else if (ret == 1)
            {
                TLOGERROR("TarsPatch::download load succ|" << __FILE__ << "," << __LINE__ << "|" << fi.path << "|" << fi.md5<< "|" << pos << endl);
                break;
            }
        }
    }
    catch (...)
    {
        fclose(fp);
        fp = NULL;
        TLOGERROR("TarsPatch::download error|" << __FILE__ << "," << __LINE__ << "|" << fi.path << "|" << fi.md5 << endl);
        throw;
    }

    fclose(fp);

    if (fi.canExec)
    {
        int ret = tars::TC_File::setExecutable(file, true);
        if (ret == 0)
        {
            if (pPtr)
            {
                pPtr->onSetExecutable(fi);
            }
        }
        else
        {
            throw TarsPatchException("set file '" + file + "' executable error!");
        }
    }

    //检查MD5值
    if (!fi.md5.empty())
    {
        std::string smd5 = tars::TC_MD5::md5file(file);
        if (smd5 != fi.md5)
        {
            TLOGERROR("TarsPatch::download " << __FILE__ << "," << __LINE__ << "|" << fi.path << "|" << fi.md5 << "|" << smd5 << endl);
            throw TarsPatchException(fi.path + "'s md5 is not equal to the file in patch server!");;
        }
    }

    time_t timeEnd = TNOW;
    if (pPtr)
    {
        pPtr->onReportTime(fi.path, timeBegin, timeEnd);
    }

    if (pPtr)
    {
        pPtr->onDownloadOK(fi, file);
    }
}


}


