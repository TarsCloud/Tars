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

package com.qq.tars.service.expand;

import java.util.List;

public class ExpandServerPreview {

    private String application;

    private String serverName;

    private String set;

    private String nodeName;

    private List<String> expandNodes;

    private boolean enableSet;

    private String setName;

    private String setArea;

    private String setGroup;

    private boolean copyNodeConfig;

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

    public String getSet() {
        return set;
    }

    public void setSet(String set) {
        this.set = set;
    }

    public String getNodeName() {
        return nodeName;
    }

    public void setNodeName(String nodeName) {
        this.nodeName = nodeName;
    }

    public List<String> getExpandNodes() {
        return expandNodes;
    }

    public void setExpandNodes(List<String> expandNodes) {
        this.expandNodes = expandNodes;
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

    public boolean isCopyNodeConfig() {
        return copyNodeConfig;
    }

    public void setCopyNodeConfig(boolean copyNodeConfig) {
        this.copyNodeConfig = copyNodeConfig;
    }
}
