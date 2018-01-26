/*
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

package com.qq.tars.service;

import com.google.common.base.Preconditions;
import com.qq.tars.db.PatchMapper;
import com.qq.tars.entity.ServerPatch;
import com.qq.tars.tools.SystemUtils;
import org.apache.commons.io.FilenameUtils;
import org.apache.commons.lang3.StringUtils;
import org.apache.commons.lang3.tuple.Pair;
import org.apache.ibatis.session.RowBounds;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.core.io.ClassPathResource;
import org.springframework.stereotype.Service;
import org.springframework.transaction.annotation.Transactional;

import java.time.LocalDateTime;
import java.time.format.DateTimeFormatter;
import java.util.List;

@Service
public class PatchService {

    private final Logger log = LoggerFactory.getLogger(PatchService.class);

    @Autowired
    private PatchMapper patchMapper;

    @Transactional(rollbackFor = Exception.class)
    public ServerPatch addServerPatch(String application, String moduleName, String tgz, String comment) {
        String md5 = md5(tgz);
        Preconditions.checkNotNull(md5);

        ServerPatch patch = new ServerPatch();
        patch.setServer(String.format("%s.%s", application, moduleName));
        patch.setTgz(FilenameUtils.getName(tgz));
        patch.setMd5(md5);
        patch.setUpdateText(comment);
        patch.setPosttime(LocalDateTime.now().format(DateTimeFormatter.ofPattern("yyyy-MM-dd HH:mm:ss")));

        patchMapper.insertServerPatch(patch);
        log.info("id={}, server={}, tgz={}, md5={}, update_text={}",
                patch.getId(), patch.getServer(), patch.getTgz(), patch.getMd5(), patch.getUpdateText());
        return patch;
    }

    private String md5(String path) {
        String command = "md5sum " + path;
        Pair<Integer, Pair<String, String>> result = SystemUtils.exec(command);
        if (result.getLeft() == -1) {
            return null;
        }
        String stdout = result.getRight().getLeft();
        if (stdout.contains(" ")) {
            stdout = stdout.substring(0, stdout.indexOf(" "));
        }

        return StringUtils.trimToNull(stdout);
    }

    public String war2tgz(String packageWarPath, String serverName) throws Exception {
        String basePath = FilenameUtils.removeExtension(packageWarPath);
        String serverPath = basePath + "/" + serverName;
        String frameZipPath = new ClassPathResource("frame.zip").getFile().getCanonicalPath();

        StringBuilder script = new StringBuilder();
        script.append(String.format("rm -rf %s;", basePath));
        script.append(String.format("mkdir -p %s;", serverPath));
        script.append(String.format("unzip -q %s -d %s;", frameZipPath, serverPath));
        script.append(String.format("unzip -q %s -d %s/apps/ROOT/;", packageWarPath, serverPath));
        script.append(String.format("cd %s;tar -cvzf %s.tgz *;cd -;", basePath, basePath));
        script.append(String.format("rm -rf %s;", basePath));
        script.append(String.format("rm -rf %s;", packageWarPath));
        Pair<Integer, Pair<String, String>> result = SystemUtils.exec(script.toString());
        log.info("script={}, code={}, stdout={}, stderr={}", script, result.getLeft(), result.getRight().getLeft(), result.getRight().getRight());
        return basePath + ".tgz";
    }

    public List<ServerPatch> getServerPatch(String application, String moduleName, int curPage, int pageSize) {
        return patchMapper.getServerPatch(application, moduleName, new RowBounds(curPage, pageSize));
    }
}
