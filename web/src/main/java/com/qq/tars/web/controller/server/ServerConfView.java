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

package com.qq.tars.web.controller.server;

public class ServerConfView {

    private Long id;

    private String application;

    private String serverName;

    private String nodeName;

    private String serverType;

    private Boolean enableSet;

    private String setName;

    private String setArea;

    private String setGroup;

    private String settingState;

    private String presentState;

    private Boolean bakFlag;

    private String templateName;

    private String profile;

    private Integer asyncThreadNum;

    private String basePath;

    private String exePath;

    private String startScriptPath;

    private String stopScriptPath;

    private String monitorScriptPath;

    private String patchTime;

    private String patchVersion;

    private Integer processId;

    private String posttime;

    private String gridflag;
    
    public Long getId() {
        return id;
    }

    public void setId(Long id) {
        this.id = id;
    }

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

    public Boolean getEnableSet() {
        return enableSet;
    }

    public void setEnableSet(Boolean enableSet) {
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

    public String getSettingState() {
        return settingState;
    }

    public void setSettingState(String settingState) {
        this.settingState = settingState;
    }

    public String getPresentState() {
        return presentState;
    }

    public void setPresentState(String presentState) {
        this.presentState = presentState;
    }

    public Integer getProcessId() {
        return processId;
    }

    public void setProcessId(Integer processId) {
        this.processId = processId;
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

    public Boolean getBakFlag() {
        return bakFlag;
    }

    public void setBakFlag(Boolean bakFlag) {
        this.bakFlag = bakFlag;
    }

    public Integer getAsyncThreadNum() {
        return asyncThreadNum;
    }

    public void setAsyncThreadNum(Integer asyncThreadNum) {
        this.asyncThreadNum = asyncThreadNum;
    }

    public String getPatchVersion() {
        return patchVersion;
    }

    public void setPatchVersion(String patchVersion) {
        this.patchVersion = patchVersion;
    }

    public String getPatchTime() {
        return patchTime;
    }

    public void setPatchTime(String patchTime) {
        this.patchTime = patchTime;
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

    public String getTemplateName() {
        return templateName;
    }

    public void setTemplateName(String templateName) {
        this.templateName = templateName;
    }

    public String getProfile() {
        return profile;
    }

    public void setProfile(String profile) {
        this.profile = profile;
    }

    public String getPosttime() {
        return posttime;
    }

    public void setPosttime(String posttime) {
        this.posttime = posttime;
    }
    
    public String getGridflag() {
    	return gridflag;
    }
    
    public void setGridflag(String gridflag) {
    	this.gridflag = gridflag;
    }
}
