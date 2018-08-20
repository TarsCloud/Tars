/**
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

package com.qq.tars.spring.config;

import com.qq.tars.server.config.ServerConfig;
import org.springframework.boot.context.properties.ConfigurationProperties;

@ConfigurationProperties(prefix = "tars.server")
public class TarsServerProperties extends ServerConfig {

    private int port;
    private int maxConns = 128;
    private int queueCap = 1024;
    private int queueTimeout = 10000;
    private String codecClass = null;
    private int threads = 1;
    private boolean isUdp = false;

    public int getPort() {
        return port;
    }

    public void setPort(int port) {
        this.port = port;
    }

    public int getMaxConns() {
        return maxConns;
    }

    public void setMaxConns(int maxConns) {
        this.maxConns = maxConns;
    }

    public int getQueueCap() {
        return queueCap;
    }

    public void setQueueCap(int queueCap) {
        this.queueCap = queueCap;
    }

    public int getQueueTimeout() {
        return queueTimeout;
    }

    public void setQueueTimeout(int queueTimeout) {
        this.queueTimeout = queueTimeout;
    }

    public String getCodecClass() {
        return codecClass;
    }

    public void setCodecClass(String codecClass) {
        this.codecClass = codecClass;
    }

    public int getThreads() {
        return threads;
    }

    public void setThreads(int threads) {
        this.threads = threads;
    }

    public boolean isUdp() {
        return isUdp;
    }

    public void setUdp(boolean udp) {
        isUdp = udp;
    }
}
