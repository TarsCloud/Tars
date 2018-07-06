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

package com.qq.tars.register.eureka;

import javax.annotation.PostConstruct;

import org.springframework.beans.factory.annotation.Value;
import org.springframework.boot.context.properties.ConfigurationProperties;
import org.springframework.cloud.commons.util.InetUtils;
import org.springframework.cloud.netflix.eureka.EurekaInstanceConfigBean;

@ConfigurationProperties(prefix = TarsEurekaInstance.PREFIX)
public class TarsEurekaInstance extends EurekaInstanceConfigBean {

    public static final String PREFIX = "eureka.instance";

    @Value("${tars.server.application:unknown}")
    private String appname = "unknown";

    @Value("${tars.server.server-name:unkonwn}")
    private String servicename = "unknown";

    @Value("${tars.server.port:80}")
    private int tarsPort = 80;

    @Value("${tars.server.isTcp:1}")
    private int isTcp = 1;

    @Value("${tars.server.timeout:6000}")
    private int timeOut = 6000;

    @Value("${tars.server.weight:0}")
    private int weight = 0;

    @Value("${tars.server.weightType:0}")
    private int weightType = 0;

    public TarsEurekaInstance(InetUtils inetUtils) {
        super(inetUtils);
    }

    @PostConstruct
    public void init() {
        getMetadataMap().put("isTcp", String.valueOf(isTcp));
        getMetadataMap().put("timeOut", String.valueOf(timeOut));
        getMetadataMap().put("weight", String.valueOf(weight));
        getMetadataMap().put("weightType", String.valueOf(weightType));
        getMetadataMap().put("instanceId", getIpAddress() + ":" + (isNonSecurePortEnabled() ? tarsPort : getSecurePort()));
    }

    public String getAppname() {
        if (servicename != null) {
            return appname + "." + servicename;
        }
        return appname;
    }

    public int getNonSecurePort() {
        return tarsPort;
    }

    public void setNonSecurePort(int port) {
        tarsPort = port;
    }

    public void setAppname(String appname) {
        if (appname != null) {
            String[] parts = appname.split("\\.");
            if (parts.length >= 2) {
                this.appname = parts[0];
                this.servicename = parts[1];
                return;
            }
        }
        this.appname = appname;
    }
}
