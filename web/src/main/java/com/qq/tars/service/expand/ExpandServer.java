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

public class ExpandServer {

    private String application;

    private String serverName;

    private String nodeName;

    private String set;

    private boolean copyNodeConfig;

    private List<ExpandPreviewServer> expandPreviewServers;

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

    public String getSet() {
        return set;
    }

    public void setSet(String set) {
        this.set = set;
    }

    public boolean isCopyNodeConfig() {
        return copyNodeConfig;
    }

    public void setCopyNodeConfig(boolean copyNodeConfig) {
        this.copyNodeConfig = copyNodeConfig;
    }

    public List<ExpandPreviewServer> getExpandPreviewServers() {
        return expandPreviewServers;
    }

    public void setExpandPreviewServers(List<ExpandPreviewServer> expandPreviewServers) {
        this.expandPreviewServers = expandPreviewServers;
    }
}
