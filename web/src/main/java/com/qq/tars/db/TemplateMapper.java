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

package com.qq.tars.db;

import com.qq.tars.entity.ProfileTemplate;
import org.apache.ibatis.annotations.Param;
import org.springframework.stereotype.Repository;

import java.util.List;

@Repository
public interface TemplateMapper {

    int insertTemplate(ProfileTemplate template);

    int deleteTemplate(ProfileTemplate template);

    int updateTemplate(ProfileTemplate template);

    ProfileTemplate loadTemplate(@Param("id") long id);

    ProfileTemplate loadTemplateByName(@Param("templateName") String templateName);

    List<String> getParentTemplateName();

    List<ProfileTemplate> getTemplate();

    List<ProfileTemplate> queryTemplate(@Param("templateName") String templateName,
                                        @Param("parentsName") String parentsName);

}
