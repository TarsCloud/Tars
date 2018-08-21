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

package com.qq.tars.spring.bean;

import com.qq.tars.server.config.ConfigurationManager;
import com.qq.tars.server.config.ServantAdapterConfig;
import com.qq.tars.server.config.ServerConfig;
import com.qq.tars.spring.annotation.TarsHttpService;
import org.springframework.beans.BeansException;
import org.springframework.boot.web.server.WebServerFactoryCustomizer;
import org.springframework.boot.web.servlet.server.ConfigurableServletWebServerFactory;
import org.springframework.context.ApplicationContext;
import org.springframework.context.ApplicationContextAware;
import org.springframework.core.annotation.AnnotationUtils;

import java.net.InetAddress;
import java.net.UnknownHostException;
import java.util.Map;

public class ServletContainerCustomizer implements WebServerFactoryCustomizer<ConfigurableServletWebServerFactory>, ApplicationContextAware {
    private ApplicationContext applicationContext;

    @Override
    public void setApplicationContext(ApplicationContext applicationContext) throws BeansException {
        this.applicationContext = applicationContext;
    }

    @Override
    public void customize(ConfigurableServletWebServerFactory factory) {
        Map<String, Object> beans = applicationContext.getBeansWithAnnotation(TarsHttpService.class);
        int port = 8080;
        String host = null;
        ServerConfig serverCfg = ConfigurationManager.getInstance().getserverConfig();

        try {
            for (Object bean : beans.values()) {
                String objName = AnnotationUtils.getAnnotation(bean.getClass(), TarsHttpService.class).value();
                String homeName = String.format("%s.%s.%s", serverCfg.getApplication(), serverCfg.getServerName(), objName);

                ServantAdapterConfig adapterConfig = ConfigurationManager.getInstance()
                        .getServerConfig().getServantAdapterConfMap().get(homeName);

                port = adapterConfig.getEndpoint().port();
                host = adapterConfig.getEndpoint().host();
            }
        } catch (Exception e) {
            System.err.println("[TARS] load http servant failed");
            e.printStackTrace();
        }

        if (host != null) {
            try {
                factory.setAddress(InetAddress.getByName(host));
                factory.setPort(port);
            } catch (UnknownHostException e) {
                e.printStackTrace();
            }
        }

        System.out.println("[TARS] http server start at " + port);
    }
}
