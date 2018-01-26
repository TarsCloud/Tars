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

#ifndef __PATCH_IMP_H_
#define __PATCH_IMP_H_

#include "Patch.h"

using namespace tars;

class PatchImp : public Patch
{
public:
    /**
     *
     */
    PatchImp();

    /**
     * 初始化
     *
     * @return int
     */
    virtual void initialize();

    /**
     * 退出
     */
    virtual void destroy() {};

    /**
     * 获取路径下所有文件列表信息
     * @param path, 目录路径, 相对_directory的路径, 不能有..
     * @param vector<FileInfo>, 文件列表信息
     * @return int
     */
    int listFileInfo(const string &path, vector<FileInfo> &vf, TarsCurrentPtr current);

    /**
     * 下载文件
     * @param file, 文件完全路径
     * @param pos, 从什么位置开始下载
     * @return vector<byte>, 文件内容
     */
    int download(const string &file, int pos, vector<char> &vb, TarsCurrentPtr current);
    
    /**
     * 准备好需要patch的文件,将发布的文件从上传目录复制到发布目录
     * @param app, 应用名
     * @param serverName, 服务名
     * @param patchFile, 需要发布的文件名
     * @return int, 0: 成功, <0: 失败
     */
    int preparePatchFile(const string &app, const string &serverName, const string &patchFile, TarsCurrentPtr current);

protected:
    int __listFileInfo(const string &path, vector<FileInfo> &vf);
    
    int __downloadFromMem (const string & file, size_t pos, vector<char> & vb);
    
    int __downloadFromFile(const string & file, size_t pos, vector<char> & vb);

protected:
    /**
     * 目录
     */
    string _directory;

    /**
     * 上传的目录
     */
    string _uploadDirectory;

    /**
     * 每次同步大小
     */
    size_t _size;
};

#endif
