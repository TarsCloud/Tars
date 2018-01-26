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

package com.qq.tars.service.server;

import com.qq.tars.validate.*;
import org.hibernate.validator.constraints.NotBlank;
import org.hibernate.validator.constraints.NotEmpty;

import javax.validation.Valid;
import java.util.List;

public class DeployServer {

    @Application
    private String application;

    @ServerName
    private String serverName;

    @NotBlank
    private String nodeName;

    @NotBlank
    @Value(values = {"tars_cpp", "tars_java", "tars_nodejs", "tars_php", "not_tars"})
    private String serverType;

    @NotBlank
    private String templateName;

    private boolean enableSet;

    @SetName(groups = EnableSet.class)
    private String setName;

    @SetArea(groups = EnableSet.class)
    private String setArea;

    @SetGroup(groups = EnableSet.class)
    private String setGroup;

    @NotEmpty
    @Valid
    private List<DeployAdapter> adapters;

    public String getApplication() {
        return application;
    }

    public void setApplication(String application) {
        this.application = application;
    }

    public String getServerName() {
        return serverName;
    }

    public void setServerName(String serverName) {
        this.serverName = serverName;
    }

    public String getNodeName() {
        return nodeName;
    }

    public void setNodeName(String nodeName) {
        this.nodeName = nodeName;
    }

    public String getServerType() {
        return serverType;
    }

    public void setServerType(String serverType) {
        this.serverType = serverType;
    }

    public String getTemplateName() {
        return templateName;
    }

    public void setTemplateName(String templateName) {
        this.templateName = templateName;
    }

    public boolean isEnableSet() {
        return enableSet;
    }

    public void setEnableSet(boolean enableSet) {
        this.enableSet = enableSet;
    }

    public String getSetName() {
        return setName;
    }

    public void setSetName(String setName) {
        this.setName = setName;
    }

    public String getSetArea() {
        return setArea;
    }

    public void setSetArea(String setArea) {
        this.setArea = setArea;
    }

    public String getSetGroup() {
        return setGroup;
    }

    public void setSetGroup(String setGroup) {
        this.setGroup = setGroup;
    }

    public List<DeployAdapter> getAdapters() {
        return adapters;
    }

    public void setAdapters(List<DeployAdapter> adapters) {
        this.adapters = adapters;
    }
}
