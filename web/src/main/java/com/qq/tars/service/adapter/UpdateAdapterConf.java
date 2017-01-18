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

package com.qq.tars.service.adapter;

public class UpdateAdapterConf {

    private long id;
    private String servant;
    private String endpoint;
    private String protocol;
    private String handlegroup;
    private int threadNum;
    private int maxConnections;
    private String allowIp;
    private int queuecap;
    private int queuetimeout;

    public long getId() {
        return id;
    }

    public void setId(long id) {
        this.id = id;
    }

    public String getEndpoint() {
        return endpoint;
    }

    public void setEndpoint(String endpoint) {
        this.endpoint = endpoint;
    }

    public String getServant() {
        return servant;
    }

    public void setServant(String servant) {
        this.servant = servant;
    }

    public String getProtocol() {
        return protocol;
    }

    public void setProtocol(String protocol) {
        this.protocol = protocol;
    }

    public String getHandlegroup() {
        return handlegroup;
    }

    public void setHandlegroup(String handlegroup) {
        this.handlegroup = handlegroup;
    }

    public int getThreadNum() {
        return threadNum;
    }

    public void setThreadNum(int threadNum) {
        this.threadNum = threadNum;
    }

    public int getMaxConnections() {
        return maxConnections;
    }

    public void setMaxConnections(int maxConnections) {
        this.maxConnections = maxConnections;
    }

    public String getAllowIp() {
        return allowIp;
    }

    public void setAllowIp(String allowIp) {
        this.allowIp = allowIp;
    }

    public int getQueuecap() {
        return queuecap;
    }

    public void setQueuecap(int queuecap) {
        this.queuecap = queuecap;
    }

    public int getQueuetimeout() {
        return queuetimeout;
    }

    public void setQueuetimeout(int queuetimeout) {
        this.queuetimeout = queuetimeout;
    }
}
