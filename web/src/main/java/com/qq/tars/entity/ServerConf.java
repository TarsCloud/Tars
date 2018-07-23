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

import com.qq.common.TimestampString;

public class ServerConf {

    private Long id;

    private String application = "";

    private String serverName = "";

    private String nodeGroup = "";

    private String nodeName = "";

    private String registryTimestamp;

    private String basePath = "";

    private String exePath = "";

    private String templateName = "";

    private Integer bakFlag = 0;

    private String settingState = "inactive";

    private String presentState = "inactive";

    private Integer processId = 0;

    private String patchVersion = "";

    private String patchTime = "1970-01-01 00:00:00";

    private String patchUser = "";

    private String tarsVersion = "";

    private String posttime = "1970-01-01 00:00:00";

    private String lastuser;

    private String serverType = "tars_cpp";

    private String startScriptPath;

    private String stopScriptPath;

    private String monitorScriptPath;

    private String enableGroup = "N";

    private String enableSet = "N";

    private String setName;

    private String setArea;

    private String setGroup;

    private String ipGroupName;

    private String profile;
    
    private String gridflag="NORMAL";

    private Integer configCenterPort = 0;

    private Integer asyncThreadNum = 3;

    private String serverImportantType = "0";

    private String remoteLogReserveTime = "65";

    private String remoteLogCompressTime = "2";

    private Integer remoteLogType = 0;

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

    public String getNodeGroup() {
        return nodeGroup;
    }

    public void setNodeGroup(String nodeGroup) {
        this.nodeGroup = nodeGroup;
    }

    public String getNodeName() {
        return nodeName;
    }

    public void setNodeName(String nodeName) {
        this.nodeName = nodeName;
    }

    public String getRegistryTimestamp() {
        return registryTimestamp;
    }

    public void setRegistryTimestamp(String registryTimestamp) {
        this.registryTimestamp = new TimestampString(registryTimestamp).toString();
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

    public String getTemplateName() {
        return templateName;
    }

    public void setTemplateName(String templateName) {
        this.templateName = templateName;
    }

    public Integer getBakFlag() {
        return bakFlag;
    }

    public void setBakFlag(Integer bakFlag) {
        this.bakFlag = bakFlag;
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
        this.patchTime = new TimestampString(patchTime).toString();
    }

    public String getPatchUser() {
        return patchUser;
    }

    public void setPatchUser(String patchUser) {
        this.patchUser = patchUser;
    }

    public String getTarsVersion() {
        return tarsVersion;
    }

    public void setTarsVersion(String tarsVersion) {
        this.tarsVersion = tarsVersion;
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

    public String getServerType() {
        return serverType;
    }

    public void setServerType(String serverType) {
        this.serverType = serverType;
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

    public String getEnableGroup() {
        return enableGroup;
    }

    public void setEnableGroup(String enableGroup) {
        this.enableGroup = enableGroup;
    }

    public String getEnableSet() {
        return enableSet;
    }

    public void setEnableSet(String enableSet) {
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

    public String getIpGroupName() {
        return ipGroupName;
    }

    public void setIpGroupName(String ipGroupName) {
        this.ipGroupName = ipGroupName;
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
    
    public Integer getConfigCenterPort() {
        return configCenterPort;
    }

    public void setConfigCenterPort(Integer configCenterPort) {
        this.configCenterPort = configCenterPort;
    }

    public Integer getAsyncThreadNum() {
        return asyncThreadNum;
    }

    public void setAsyncThreadNum(Integer asyncThreadNum) {
        this.asyncThreadNum = asyncThreadNum;
    }

    public String getServerImportantType() {
        return serverImportantType;
    }

    public void setServerImportantType(String serverImportantType) {
        this.serverImportantType = serverImportantType;
    }

    public String getRemoteLogReserveTime() {
        return remoteLogReserveTime;
    }

    public void setRemoteLogReserveTime(String remoteLogReserveTime) {
        this.remoteLogReserveTime = remoteLogReserveTime;
    }

    public String getRemoteLogCompressTime() {
        return remoteLogCompressTime;
    }

    public void setRemoteLogCompressTime(String remoteLogCompressTime) {
        this.remoteLogCompressTime = remoteLogCompressTime;
    }

    public Integer getRemoteLogType() {
        return remoteLogType;
    }

    public void setRemoteLogType(Integer remoteLogType) {
        this.remoteLogType = remoteLogType;
    }
}
