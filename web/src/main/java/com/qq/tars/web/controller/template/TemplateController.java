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

package com.qq.tars.web.controller.template;

import com.qq.common.WrappedController;
import com.qq.tars.service.template.AddTemplate;
import com.qq.tars.service.template.TemplateService;
import com.qq.tars.service.template.UpdateTemplate;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.stereotype.Controller;
import org.springframework.web.bind.annotation.RequestBody;
import org.springframework.web.bind.annotation.RequestMapping;
import org.springframework.web.bind.annotation.RequestParam;
import org.springframework.web.bind.annotation.ResponseBody;

import javax.validation.Valid;
import java.util.LinkedList;
import java.util.List;
import java.util.stream.Collectors;

@Controller
public class TemplateController extends WrappedController {

    @Autowired
    private TemplateService templateService;

    @RequestMapping(
            value = "server/api/add_profile_template",
            produces = {"application/json"}
    )
    @ResponseBody
    public ProfileTemplateView addTemplate(@RequestBody @Valid AddTemplate addTemplate) throws Exception {
        return mapper.map(templateService.addTemplate(addTemplate), ProfileTemplateView.class);
    }

    @RequestMapping(
            value = "server/api/delete_profile_template",
            produces = {"application/json"}
    )
    @ResponseBody
    public List<Long> deleteTemplate(@RequestParam long id) throws Exception {
        templateService.deleteTemplate(id);
        return new LinkedList<Long>() {{
            add(id);
        }};
    }

    @RequestMapping(
            value = "server/api/update_profile_template",
            produces = {"application/json"}
    )
    @ResponseBody
    public ProfileTemplateView updateTemplate(@RequestBody UpdateTemplate updateTemplate) throws Exception {
        return mapper.map(templateService.updateTemplate(updateTemplate), ProfileTemplateView.class);
    }

    @RequestMapping(
            value = "server/api/query_profile_template",
            produces = {"application/json"}
    )
    @ResponseBody
    public List<ProfileTemplateView> queryTemplate(@RequestParam("template_name") String templateName,
                                                   @RequestParam("parents_name") String parentsName) throws Exception {
        return templateService.queryTemplate(templateName, parentsName).stream()
                .map(template -> mapper.map(template, ProfileTemplateView.class))
                .collect(Collectors.toList());
    }

    @RequestMapping(
            value = "server/api/profile_template",
            produces = {"application/json"}
    )
    @ResponseBody
    public ProfileTemplateView loadTemplate(@RequestParam("profile_template") String templateName) throws Exception {
        return mapper.map(templateService.loadTemplate(templateName), ProfileTemplateView.class);
    }

}
