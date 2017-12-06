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

package com.qq.tars.web.controller.patch;

import com.qq.common.WrappedController;
import com.qq.tars.service.PatchService;
import com.qq.tars.service.SystemConfigService;
import com.qq.tars.validate.Application;
import com.qq.tars.validate.ServerName;
import org.apache.commons.io.FileUtils;
import org.apache.commons.io.FilenameUtils;
import org.apache.commons.io.IOUtils;
import org.apache.commons.lang3.StringUtils;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.stereotype.Controller;
import org.springframework.ui.ModelMap;
import org.springframework.validation.annotation.Validated;
import org.springframework.web.bind.annotation.RequestMapping;
import org.springframework.web.bind.annotation.RequestParam;
import org.springframework.web.bind.annotation.ResponseBody;
import org.springframework.web.multipart.MultipartFile;
import org.springframework.web.multipart.MultipartHttpServletRequest;
import org.springframework.web.multipart.commons.CommonsMultipartResolver;

import javax.servlet.http.HttpServletRequest;
import java.io.File;
import java.io.FileOutputStream;
import java.io.IOException;
import java.util.Iterator;
import java.util.UUID;

@Controller
@Validated
public class UploadController extends WrappedController {

    private final Logger log = LoggerFactory.getLogger(UploadController.class);

    @Autowired
    private SystemConfigService systemConfigService;

    @Autowired
    private PatchService patchService;

    @RequestMapping(
            value = "server/api/upload_patch_package",
            produces = "application/json"
    )
    @ResponseBody
    public ServerPatchView upload(@Application @RequestParam String application,
                                  @ServerName @RequestParam("module_name") String moduleName,
                                  HttpServletRequest request, ModelMap modelMap) throws Exception {
        String comment = StringUtils.trimToEmpty(request.getParameter("comment"));
        String uploadTgzBasePath = systemConfigService.getUploadTgzPath();

        CommonsMultipartResolver multipartResolver = new CommonsMultipartResolver(request.getSession().getServletContext());
        if (multipartResolver.isMultipart(request)) {
            MultipartHttpServletRequest multiRequest = (MultipartHttpServletRequest) request;
            Iterator<String> it = multiRequest.getFileNames();
            if (it.hasNext()) {
                MultipartFile file = multiRequest.getFile(it.next());

                String originalName = file.getOriginalFilename();
                String extension = FilenameUtils.getExtension(originalName);
                String temporary = uploadTgzBasePath + "/" + UUID.randomUUID() + "." + extension;
                IOUtils.copy(file.getInputStream(), new FileOutputStream(temporary));

                String packageType = "suse";

                // 发布包上传目录
                String updateTgzPath = uploadTgzBasePath + "/" + application + "/" + moduleName;

                // 发布包上传目录中的发布包文件名
                String uploadTgzName = application + "." + moduleName + "_" + packageType + "_" + System.currentTimeMillis() + ".tgz";

                // 发布包上传目录中的发布包文件路径
                String uploadTgzFullPath = updateTgzPath + "/" + uploadTgzName;

                log.info("temporary path={}, upload path={}", temporary, uploadTgzFullPath);

                File uploadPathDir = new File(updateTgzPath);
                if (!uploadPathDir.exists()) {
                    if (!uploadPathDir.mkdirs()) {
                        throw new IOException(String.format("mkdirs error, path=%s", uploadPathDir.getCanonicalPath()));
                    }
                }

                FileUtils.moveFile(new File(temporary), new File(uploadTgzFullPath));

                return mapper.map(patchService.addServerPatch(application, moduleName, uploadTgzFullPath, comment), ServerPatchView.class);
            }
        }

        throw new Exception("请求中没有包含发布包数据");
    }
}
