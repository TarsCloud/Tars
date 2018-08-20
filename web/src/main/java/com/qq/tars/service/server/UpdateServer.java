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

import javax.validation.constraints.Min;
import javax.validation.constraints.NotNull;

public class UpdateServer {

    @NotNull
    @Min(0)
    private long id;

    private boolean isBak;

    private String templateName;

    private String serverType;

    private boolean enableSet;

    private String setName;

    private String setArea;

    private String setGroup;

    private int asyncThreadNum;

    private String basePath;

    private String exePath;

    private String startScriptPath;

    private String stopScriptPath;

    private String monitorScriptPath;

    private String profile;
    private String gridflag;

    public long getId() {
        return id;
    }

    public void setId(long id) {
        this.id = id;
    }

    public boolean isBak() {
        return isBak;
    }

    public void setBak(boolean bak) {
        isBak = bak;
    }

    public String getTemplateName() {
        return templateName;
    }

    public void setTemplateName(String templateName) {
        this.templateName = templateName;
    }

    public String getServerType() {
        return serverType;
    }

    public void setServerType(String serverType) {
        this.serverType = serverType;
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

    public int getAsyncThreadNum() {
        return asyncThreadNum;
    }

    public void setAsyncThreadNum(int asyncThreadNum) {
        this.asyncThreadNum = asyncThreadNum;
    }

    public String getBasePath() {
        return basePath;
    }

    public void setBasePath(String basePath) {
        this.basePath = basePath;
    }

    public String getExePath() {
        return exePath;
    }

    public void setExePath(String exePath) {
        this.exePath = exePath;
    }

    public String getStartScriptPath() {
        return startScriptPath;
    }

    public void setStartScriptPath(String startScriptPath) {
        this.startScriptPath = startScriptPath;
    }

    public String getStopScriptPath() {
        return stopScriptPath;
    }

    public void setStopScriptPath(String stopScriptPath) {
        this.stopScriptPath = stopScriptPath;
    }

    public String getMonitorScriptPath() {
        return monitorScriptPath;
    }

    public void setMonitorScriptPath(String monitorScriptPath) {
        this.monitorScriptPath = monitorScriptPath;
    }

    public String getProfile() {
        return profile;
    }

    public void setProfile(String profile) {
        this.profile = profile;
    }
    public String getGridflag() {
    	return gridflag;
    }
    public void setGridflag(String gridflag) {
    	this.gridflag = gridflag;
    }
}
