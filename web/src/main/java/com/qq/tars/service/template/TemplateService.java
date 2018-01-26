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

package com.qq.tars.service.template;

import com.qq.tars.db.TemplateMapper;
import com.qq.tars.entity.ProfileTemplate;
import com.qq.tars.entity.ServerConf;
import com.qq.tars.exception.DBConsistencyException;
import com.qq.tars.exception.DBUpdateNothingException;
import com.qq.tars.service.server.ServerService;
import com.qq.tars.tools.DateTime;
import org.apache.commons.lang3.StringUtils;
import org.springframework.beans.BeanUtils;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.stereotype.Service;
import org.springframework.transaction.annotation.Transactional;

import java.util.List;

@Service
public class TemplateService {

    @Autowired
    private ServerService serverService;

    @Autowired
    private TemplateMapper templateMapper;

    @Transactional(rollbackFor = Exception.class)
    public ProfileTemplate addTemplate(AddTemplate addTemplate) throws DBConsistencyException {
        ProfileTemplate template = new ProfileTemplate();

        ProfileTemplate parent = loadTemplate(addTemplate.getParentsName());
        if (null == parent) {
            throw new DBConsistencyException("指定的父模板不存在");
        }

        BeanUtils.copyProperties(addTemplate, template);
        template.setPosttime(DateTime.now());
        templateMapper.insertTemplate(template);
        return template;
    }

    @Transactional(rollbackFor = Exception.class)
    public int deleteTemplate(long id) throws DBUpdateNothingException, DBConsistencyException {
        ProfileTemplate template = loadTemplate(id);
        if (null == template) {
            throw new DBUpdateNothingException("没有匹配到要删除的模板");
        }

        List<ServerConf> ref = serverService.getServerConfsByTemplate(template.getTemplateName());
        if (!ref.isEmpty()) {
            throw new DBConsistencyException("模板被服务引用，不能删除");
        }

        return templateMapper.deleteTemplate(template);
    }

    @Transactional(rollbackFor = Exception.class)
    public ProfileTemplate updateTemplate(UpdateTemplate updateTemplate) throws DBUpdateNothingException, DBConsistencyException {
        ProfileTemplate template = loadTemplate(updateTemplate.getId());
        if (null == template) {
            throw new DBUpdateNothingException("没有匹配到要修改的模板");
        }

        if (!template.getTemplateName().equalsIgnoreCase(updateTemplate.getTemplateName())) {
            List<ServerConf> ref = serverService.getServerConfsByTemplate(template.getTemplateName());
            if (!ref.isEmpty()) {
                throw new DBConsistencyException("模板被服务引用，不能更新模板名称");
            }
        }

        ProfileTemplate parent = loadTemplate(updateTemplate.getParentsName());
        if (null == parent) {
            throw new DBConsistencyException("指定的父模板不存在");
        }

        BeanUtils.copyProperties(updateTemplate, template);
        template.setPosttime(DateTime.now());
        templateMapper.updateTemplate(template);
        return template;
    }

    public ProfileTemplate loadTemplate(long id) {
        return templateMapper.loadTemplate(id);
    }

    public ProfileTemplate loadTemplate(String templateName) {
        return templateMapper.loadTemplateByName(templateName);
    }

    public List<ProfileTemplate> getTemplate() {
        return templateMapper.getTemplate();
    }

    public List<ProfileTemplate> queryTemplate(String templateName, String parentsName) {
        templateName = StringUtils.trimToEmpty(templateName);
        parentsName = StringUtils.trimToEmpty(parentsName);
        return templateMapper.queryTemplate(templateName, parentsName);
    }

}
