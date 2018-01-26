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
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.stereotype.Controller;
import org.springframework.web.bind.annotation.RequestMapping;
import org.springframework.web.bind.annotation.RequestParam;
import org.springframework.web.bind.annotation.ResponseBody;

import java.util.List;
import java.util.stream.Collectors;

@Controller
public class PatchController extends WrappedController {

    @Autowired
    private PatchService patchService;

    @RequestMapping(
            value = "server/api/server_patch_list",
            produces = "application/json"
    )
    @ResponseBody
    public List<ServerPatchView> serverPatchList(@RequestParam String application,
                                                 @RequestParam("module_name") String moduleName,
                                                 @RequestParam(value = "cur_page", required = false, defaultValue = "0") int curPage,
                                                 @RequestParam(value = "page_size", required = false, defaultValue = "0") int pageSize) throws Exception {
        return patchService.getServerPatch(application, moduleName, curPage, pageSize).stream()
                .sorted((l, r) -> r.getPosttime().compareTo(l.getPosttime())) // 逆序
                .map(patch -> mapper.map(patch, ServerPatchView.class))
                .collect(Collectors.toList());
    }
}
