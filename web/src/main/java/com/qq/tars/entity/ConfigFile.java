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

package com.qq.tars.entity;

import com.fasterxml.jackson.annotation.JsonProperty;
import com.qq.common.TimestampString;

public class ConfigFile {

    private Long id;

    private String serverName = "";

    private String setName = "";

    private String setArea = "";

    private String setGroup = "";

    @JsonProperty("node_name")
    private String host = "";

    private String filename;

    private String config;

    private String posttime;

    private String lastuser;

    private Integer level = 2;

    private Integer configFlag = 0;

    public Long getId() {
        return id;
    }

    public void setId(Long id) {
        this.id = id;
    }

    public String getServerName() {
        return serverName;
    }

    public void setServerName(String serverName) {
        this.serverName = serverName;
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

    public String getHost() {
        return host;
    }

    public void setHost(String host) {
        this.host = host;
    }

    public String getFilename() {
        return filename;
    }

    public void setFilename(String filename) {
        this.filename = filename;
    }

    public String getConfig() {
        return config;
    }

    public void setConfig(String config) {
        this.config = config;
    }

    public String getPosttime() {
        return posttime;
    }

    public void setPosttime(String posttime) {
        this.posttime = new TimestampString(posttime).toString();
    }

    public String getLastuser() {
        return lastuser;
    }

    public void setLastuser(String lastuser) {
        this.lastuser = lastuser;
    }

    public Integer getLevel() {
        return level;
    }

    public void setLevel(Integer level) {
        this.level = level;
    }

    public Integer getConfigFlag() {
        return configFlag;
    }

    public void setConfigFlag(Integer configFlag) {
        this.configFlag = configFlag;
    }
}
