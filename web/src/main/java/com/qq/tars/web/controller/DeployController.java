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

package com.qq.tars.web.controller;

import com.qq.common.WrappedController;
import com.qq.tars.entity.ProfileTemplate;
import com.qq.tars.service.server.DeployServer;
import com.qq.tars.service.server.ServerService;
import com.qq.tars.service.template.TemplateService;
import com.qq.tars.validate.EnableSet;
import com.qq.tars.web.controller.server.ServerConfView;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.stereotype.Controller;
import org.springframework.transaction.annotation.Transactional;
import org.springframework.validation.annotation.Validated;
import org.springframework.web.bind.annotation.RequestBody;
import org.springframework.web.bind.annotation.RequestMapping;
import org.springframework.web.bind.annotation.RequestMethod;
import org.springframework.web.bind.annotation.ResponseBody;

import javax.validation.*;
import java.util.LinkedList;
import java.util.List;
import java.util.Set;
import java.util.stream.Collectors;

@Controller
public class DeployController extends WrappedController {

    @Autowired
    private ServerService serverService;

    @Autowired
    private TemplateService templateService;

    @RequestMapping(
            value = "op_manage",
            produces = {"text/html"}
    )
    public String pageOpManage() throws Exception {
        return "op_manage";
    }

    @RequestMapping(
            value = "server/api/server_type_list",
            produces = {"application/json"}
    )
    @ResponseBody
    public List<String> serverTypeList() throws Exception {
        return new LinkedList<String>() {{
            add("tars_cpp");
            add("tars_java");
            add("tars_nodejs");
            add("tars_php");
        }};
    }

    @RequestMapping(
            value = "server/api/template_name_list",
            produces = {"application/json"}
    )
    @ResponseBody
    public List<String> templateNameList() throws Exception {
        List<ProfileTemplate> templates = templateService.getTemplate();
        return templates.stream().map(ProfileTemplate::getTemplateName).collect(Collectors.toList());
    }

    @RequestMapping(
            value = "server/api/deploy_server",
            method = {RequestMethod.POST},
            produces = {"application/json"}
    )
    @ResponseBody
    @Transactional(rollbackFor = {Exception.class})
    public ServerConfView deployServer(@Valid @RequestBody DeployServer deployServer) throws Exception {
        if (deployServer.isEnableSet()) {
            validate(deployServer, EnableSet.class);
        }

        return mapper.map(serverService.addServerConf(deployServer), ServerConfView.class);
    }

}
