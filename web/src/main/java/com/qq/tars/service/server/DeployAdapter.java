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

import com.qq.tars.validate.ObjName;
import com.qq.tars.validate.Value;
import org.hibernate.validator.constraints.NotBlank;

import javax.validation.constraints.Max;
import javax.validation.constraints.Min;
import javax.validation.constraints.NotNull;

public class DeployAdapter {

    @ObjName
    private String objName;

    @NotBlank
    private String bindIp;

    @NotNull
    @Min(0)
    @Max(65535)
    private Integer port;

    @Value(values = {"tcp", "udp", "ssl"})
    private String portType;

    @Value(values = {"tars", "not_tars"})
    private String protocol;

    @NotNull
    @Min(1)
    private Integer threadNum;

    @NotNull
    @Min(1)
    private Integer maxConnections;

    @NotNull
    @Min(0)
    private Integer queuecap;

    @NotNull
    @Min(0)
    private Integer queuetimeout;

    @Min(0)
    @Max(1)
    private Integer auth;

    public String getObjName() {
        return objName;
    }

    public void setObjName(String objName) {
        this.objName = objName;
    }

    public String getBindIp() {
        return bindIp;
    }

    public void setBindIp(String bindIp) {
        this.bindIp = bindIp;
    }

    public Integer getPort() {
        return port;
    }

    public void setPort(Integer port) {
        this.port = port;
    }

    public String getPortType() {
        return portType;
    }

    public void setPortType(String portType) {
        this.portType = portType;
    }

    public String getProtocol() {
        return protocol;
    }

    public void setProtocol(String protocol) {
        this.protocol = protocol;
    }

    public Integer getThreadNum() {
        return threadNum;
    }

    public void setThreadNum(Integer threadNum) {
        this.threadNum = threadNum;
    }

    public Integer getMaxConnections() {
        return maxConnections;
    }

    public void setMaxConnections(Integer maxConnections) {
        this.maxConnections = maxConnections;
    }

    public Integer getQueuecap() {
        return queuecap;
    }

    public void setQueuecap(Integer queuecap) {
        this.queuecap = queuecap;
    }

    public Integer getQueuetimeout() {
        return queuetimeout;
    }

    public void setQueuetimeout(Integer queuetimeout) {
        this.queuetimeout = queuetimeout;
    }

    public Integer getAuth() {
        return auth;
    }

    public void setAuth(Integer e) {
        this.auth = e;
    }
}
