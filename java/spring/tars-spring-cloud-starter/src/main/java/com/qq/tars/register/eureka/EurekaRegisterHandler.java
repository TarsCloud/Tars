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

import java.util.ArrayList;
import java.util.List;
import java.util.concurrent.atomic.AtomicBoolean;

import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.context.event.ContextRefreshedEvent;
import org.springframework.context.event.ContextStoppedEvent;
import org.springframework.context.event.EventListener;

import com.netflix.appinfo.ApplicationInfoManager;
import com.netflix.appinfo.HealthCheckHandler;
import com.netflix.appinfo.InstanceInfo;
import com.netflix.appinfo.InstanceInfo.InstanceStatus;
import com.netflix.discovery.EurekaClient;
import com.netflix.discovery.shared.Application;
import com.qq.tars.client.config.TarsClientProperties;
import com.qq.tars.register.RegisterHandler;
import com.qq.tars.support.query.prx.EndpointF;

public class EurekaRegisterHandler implements RegisterHandler {

    private EurekaClient client;

    private TarsEurekaInstance instanceConfig;

    private HealthCheckHandler healthCheckHandler;

    private ApplicationInfoManager applicationInfoManager;

    @Autowired
    private TarsClientProperties clientProperties;

    private AtomicBoolean isRegisted = new AtomicBoolean(false);

    public EurekaRegisterHandler(EurekaClient client, TarsEurekaInstance instanceConfig,
                                 HealthCheckHandler healthCheckHandler, ApplicationInfoManager applicationInfoManager) {
        this.client = client;
        this.instanceConfig = instanceConfig;
        this.healthCheckHandler = healthCheckHandler;
        this.applicationInfoManager = applicationInfoManager;
    }

    @EventListener(ContextRefreshedEvent.class)
    public void onApplicationEvent(ContextRefreshedEvent ev) {
        start();
        clientProperties.setLocator("eureka instance");
    }

    @EventListener(ContextStoppedEvent.class)
    public void onApplicationEvent(ContextStoppedEvent ev) {
        close();
    }

    public void start() {
        if (isRegisted.compareAndSet(false, true)) {
            if (instanceConfig.getNonSecurePort() > 0) {
                client.getApplications();
                applicationInfoManager.setInstanceStatus(instanceConfig.getInitialStatus());

                if (healthCheckHandler != null) {
                    client.registerHealthCheck(healthCheckHandler);
                }
            }
        }
    }

    public void close() {
        if (applicationInfoManager.getInfo() != null) {
            applicationInfoManager.setInstanceStatus(InstanceStatus.DOWN);
        }
        if (client != null) {
            client.shutdown();
        }
        client = null;
    }

    @Override
    public void register(String appName, String serviceName) {
    }

    @Override
    public List<EndpointF> query(String name) {
        name = normalObjName(name);
        if (name == null) {
            return null;
        }
        Application application = client.getApplication(name);
        List<EndpointF> results = new ArrayList<EndpointF>();
        if (application == null) {
            return results;
        }
        for (InstanceInfo instanceInfo : application.getInstances()) {
            if (instanceInfo.getStatus() != InstanceStatus.UP) {
                continue;
            }
            EndpointF endpoint = convert2Endpoint(instanceInfo);
            if (endpoint != null) {
                results.add(endpoint);
            }
        }
        return results;
    }

    private EndpointF convert2Endpoint(InstanceInfo instanceInfo) {
        EndpointF endpoint = null;
        if (instanceInfo == null) {
            return endpoint;
        }
        int istcp = instanceInfo.getMetadata().get("isTcp") == null ? 1 : Integer.parseInt(instanceInfo.getMetadata().get("isTcp"));
        int timeOut = instanceInfo.getMetadata().get("timeOut") == null ? 6000 : Integer.parseInt(instanceInfo.getMetadata().get("timeOut"));
        int grid = instanceInfo.getMetadata().get("grid") == null ? 0 : Integer.parseInt(instanceInfo.getMetadata().get("grid"));
        int qos = instanceInfo.getMetadata().get("qos") == null ? 0 : Integer.parseInt(instanceInfo.getMetadata().get("qos"));
        int weight = instanceInfo.getMetadata().get("weight") == null ? 0 : Integer.parseInt(instanceInfo.getMetadata().get("weight"));
        int weightType = instanceInfo.getMetadata().get("weightType") == null ? 0 : Integer.parseInt(instanceInfo.getMetadata().get("weightType"));
        endpoint = new EndpointF(instanceInfo.getIPAddr(), instanceInfo.getPort(), timeOut, istcp, grid, 0, 0, null, qos, 0, weight, weightType);
        return endpoint;
    }

    private String normalObjName(String name) {
        if (name == null || name.length() == 0) {
            return null;
        }
        String[] parts = name.split("\\.");
        if (parts.length < 3) {
            return name;
        }
        StringBuffer buffer = new StringBuffer();
        buffer.append(parts[0]).append(".").append(parts[1]);
        return buffer.toString();
    }

}
